#include "fw/Framework.h"
#include "fw/Input.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl_gl3.h"
#include <iostream>

namespace
{
void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

    std::cerr << "OpenGL debug callback function\n";
    std::cerr << "    Message: " << message << "\n";

    std::cerr << "    Source: ";
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cerr << "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cerr << "WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cerr << "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cerr << "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cerr << "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cerr << "OTHER";
        break;
    }
    std::cerr << "\n";

    std::cerr << "    Type: ";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cerr << "TYPE_ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cerr << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cerr << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cerr << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cerr << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cerr << "MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cerr << "PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cerr << "POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cerr << "OTHER";
        break;
    default:
        std::cerr << "UNKNOWN";
        break;
    }
    std::cerr << "\n";

    std::cerr << "    ID: " << id << "\n";
    std::cerr << "    Severity: ";
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        std::cerr << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << "HIGH";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cerr << "NOTIFICATION";
        break;
    default:
        std::cerr << "UNKNOWN";
        break;
    }
    std::cerr << "\n\n";
}
} // namespace

namespace fw
{
float Framework::timeSinceStart = 0.0f;
float Framework::frameTime = 0.0f;

float Framework::getTimeSinceStart()
{
    return timeSinceStart;
}

float Framework::getFrameTime()
{
    return frameTime;
}

Framework::Framework()
{
}

Framework::~Framework()
{
}

bool Framework::initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "ERROR: Failed to initialize SDL2\n";
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("OpenGL", 400, 100, 1024, 768, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "ERROR: Failed to create SDL2 window\n";
        return false;
    }

    context = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "ERROR: " << glewGetErrorString(err) << "\n";
        return false;
    }

    ImGui_ImplSdlGL3_Init(window);

    int major = 0;
    int minor = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);

    if (glDebugMessageCallback)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
        glDebugMessageCallback(glDebugCallback, nullptr);
        std::cout << "Debug context enabled\n\n";
    }
    else
    {
        std::cout << "Debug context NOT enabled\n\n";
    }

    return true;
}

bool Framework::setApplication(Application* application)
{
    if (!application)
    {
        std::cerr << "ERROR: Invalid application pointer\n";
        return false;
    }

    app = application;
    if (!app->initialize())
    {
        return false;
    }
    return true;
}

void Framework::execute()
{
    while (running)
    {
        handleEvents();

        float currentTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
        frameTime = currentTime - timeSinceStart;
        timeSinceStart = currentTime;

        app->update();
        app->render();

        ImGui_ImplSdlGL3_NewFrame(window);
        app->gui();
        ImGui::Render();
        SDL_GL_SwapWindow(window);

        Input::clearPressed();
        Input::clearReleased();
    }
}

void Framework::uninitialize()
{
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Framework::handleEvents()
{
    Input::setMouseDeltaX(0);
    Input::setMouseDeltaY(0);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSdlGL3_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
        {
            running = false;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                running = false;
                break;
            }
            Input::setKeyDown(event.key.keysym.sym);
        }
        if (event.type == SDL_KEYUP)
        {
            Input::setKeyUp(event.key.keysym.sym);
        }
        if (event.type == SDL_MOUSEMOTION)
        {
            Input::setMouseDeltaX(event.motion.xrel);
            Input::setMouseDeltaY(event.motion.yrel);
        }
    }
}
} // namespace fw
