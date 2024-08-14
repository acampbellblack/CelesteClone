#include "gl_renderer.h"

// To Load PNG Files
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "render_interface.h"

// #############################################################################
// MARK:                     OpenGL Constants
// #############################################################################
const char* TEXTURE_PATH = "assets/textures/TEXTURE_ATLAS.png";

// #############################################################################
// MARK:                     OpenGL Structs
// #############################################################################
struct GLContext
{
    GLuint programID;
    GLuint textureID;
    GLuint transformSBOID;
    GLuint screenSizeID;
    GLuint orthoProjectionID;

    long long textureTimestamp;
    long long shaderTimestamp;
};

// #############################################################################
// MARK:                     OpenGL Globals
// #############################################################################
static GLContext glContext;

// #############################################################################
// MARK:                     OpenGL Functions
// #############################################################################
static void APIENTRY gl_debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* user)
{
    if (severity == GL_DEBUG_SEVERITY_LOW ||
        severity == GL_DEBUG_SEVERITY_MEDIUM ||
        severity == GL_DEBUG_SEVERITY_HIGH)
    {
        SM_ASSERT(0, "OpenGL Error: %s", message);
    }
    else
    {
        SM_TRACE((char*)message);
    }
}

bool gl_init(BumpAllocator* transientStorage)
{
    load_gl_functions();

    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);

    if (!vertShader || !fragShader)
    {
        SM_ASSERT(0, "Failed to load shaders");
        return false;
    }

    glShaderSource(vertexShaderID, 1, &vertShader, nullptr);
    glShaderSource(fragmentShaderID, 1, &fragShader, nullptr);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);

    // Test if vertex shader compiled successfully
    {
        int success;
        char shaderLog[2048] = {};
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShaderID, 2048, nullptr, shaderLog);
            SM_ASSERT(false, "Vertex Shader Compilation Error: %s", shaderLog);
        }
    }

    // Test if fragment shader compiled successfully
    {
        int success;
        char shaderLog[2048] = {};
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShaderID, 2048, nullptr, shaderLog);
            SM_ASSERT(false, "Vertex Shader Compilation Error: %s", shaderLog);
        }
    }

    glContext.programID = glCreateProgram();
    glAttachShader(glContext.programID, vertexShaderID);
    glAttachShader(glContext.programID, fragmentShaderID);
    glLinkProgram(glContext.programID);

    glDetachShader(glContext.programID, vertexShaderID);
    glDetachShader(glContext.programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // This has to be done, otherwise OpenGL will not draw anything
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Texture Loading using STBI
    {
        int width, height, channels;
        unsigned char* data = stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);
        if (!data)
        {
            SM_ASSERT(0, "Failed to load texture");
            return false;
        }

        glGenTextures(1, &glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    // Transform Storage Buffer
    {
        glGenBuffers(1, &glContext.transformSBOID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, nullptr, GL_DYNAMIC_DRAW);
    }

    // Uniforms
    {
        glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");
        glContext.orthoProjectionID = glGetUniformLocation(glContext.programID, "orthoProjection");
    }

    // sRGB even if the texture is non-sRGB
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); // disable multisampling

    // Depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID);

    return true;
}

void gl_render()
{
    // Texture hot reloading
    {
        long long currentTimestamp = get_timestamp(TEXTURE_PATH);

        if (currentTimestamp > glContext.textureTimestamp)
        {
            glActiveTexture(GL_TEXTURE0);
            int width, height, nChannels;
            char* data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &nChannels, 4);
            if (data)
            {
                glContext.textureTimestamp = currentTimestamp;
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
        }
    }

    glClearColor(119.0f / 255.0f, 33.0f / 255.0f, 111.0f / 255.0f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, input->screenSize.x, input->screenSize.y);

    // Copy screen size to the GPU
    Vec2 screenSize = {(float)input->screenSize.x, (float)input->screenSize.y};
    glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

    // Orthographic projection
    {
        OrthographicCamera2D camera = renderData->gameCamera;
        Mat4 orthoProjection = orthographic_projection(
            camera.position.x - camera.dimensions.x / 2.0f,
            camera.position.x + camera.dimensions.x / 2.0f,
            camera.position.y - camera.dimensions.y / 2.0f,
            camera.position.y + camera.dimensions.y / 2.0f
        );
        glUniformMatrix4fv(glContext.orthoProjectionID, 1, GL_FALSE, &orthoProjection.ax);
    }

    // Opaque Objects
    {
        // Copy transforms to the GPU
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData->transformCount, renderData->transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transformCount);

        // Reset for next frame
        renderData->transformCount = 0;
    }
}