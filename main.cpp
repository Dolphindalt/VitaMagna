#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>

#define MAJOR_VERSION 3
#define MINOR_VERSION 1

using namespace glm;

int running = 1;
vec3 resolution = glm::vec3(1000, 1000, 0.0);

void input();

int main(int argc, char *argv[])
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = NULL;

    window = SDL_CreateWindow(
        "Ray March", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        resolution.x,
        resolution.y,
        SDL_WINDOW_OPENGL
    );

    assert(window != NULL);

    SDL_GLContext glcon = SDL_GL_CreateContext(window);
    assert(glcon);

    GLenum err = glewInit();
    assert(err == GLEW_OK);

    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(window, glcon);
    ImGui_ImplOpenGL3_Init("#version 400");
    ImGui::StyleColorsClassic();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1.0f);

    glViewport(0, 0, resolution.x, resolution.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -30, 30);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float x = 0.1, y = 0.1;
    float a = -0.96632, b = 2.879534, c = 0.7654321, d = 0.744788;
    int settling_iterations = 100, restless_iterations = 100000;

    for(int i = 0; i < settling_iterations; i++)
    {
        x = sin(y * b) + c * sin(x * b);
        y = sin(x * a) + d * sin(y * a);
    }

    Uint64 now;
    Uint64 last_time = SDL_GetPerformanceCounter();
    double delta = 0;
    const double ns = 1000000000.0 / 60.0;
    while(running)
    {
        now = SDL_GetPerformanceCounter();
        delta += (now - last_time) / ns;
        while(delta >= 1.0)
        {
            input();
            delta--;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glBegin(GL_POINTS);
        
        for(int i = 0; i < restless_iterations; i++)
        {
            x = sin(y * b) + c * sin(x * b);
            y = sin(x * a) + d * sin(y * a);
            glVertex2f(x, y);
        }

        glEnd();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::Begin("Vita Manga");
        ImGui::DragFloat("Orbit a", &a, 0.1, -3.0, 3.0);
        ImGui::DragFloat("Orbit b", &b, 0.1, -3.0, 3.0);
        ImGui::DragFloat("Orbit c", &c, 0.1, -0.5, 1.5);
        ImGui::DragFloat("Orbit d", &d, 0.1, -0.5, 1.5);
        ImGui::End();

        ImGui::Render();

        SDL_GL_MakeCurrent(window, glcon);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glcon);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void input()
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        switch(e.type)
        {
            case SDL_QUIT: running = 0; break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE: running = 0;
                    default: break;
                }
            default: break;
        }
    }
}