#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define MAJOR_VERSION 3
#define MINOR_VERSION 1

using namespace glm;

int running = 1, restless_iterations = 1000000;
glm::vec3 camera_position = vec3(0.0, 0.0, 0.11);

void input();
float map(const float value, const float min1, const float max1, const float min2, const float max2);
void render_dream(const float &a, const float &b, const float &c, const float &d, const bool &smooth_a, const bool &smooth_b, const bool &smooth_c, const bool &smooth_d);
void render_dream_var(const float &a, const float &b, const float &c, const float &d, const bool &smooth_a, const bool &smooth_b, const bool &smooth_c, const bool &smooth_d);
void render_ginger(const float &b);
void render_gumowisk_mira(const float &a, const float &b);
void render_henon_attractor();
void render_hopalong_attractor(const float a, const float b, const float c);

int main(int argc, char *argv[])
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = NULL;
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayMode resolution;
    if(SDL_GetDesktopDisplayMode(0, &resolution) != 0)
    {
        SDL_Log("Desktop resolution failed: %s \n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        "Ray March", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        resolution.w,
        resolution.h,
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1.0f);

    glViewport(0, 0, resolution.w, resolution.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2.0, 2.0, -2.0, 2.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int selection = 0;
    float a = -0.9, b = 2.8, c = 0.7, d = 0.7;
    bool smooth_a = 0, smooth_b = 0, smooth_c = 0, smooth_d = 0, slow_speed = 0;

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

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(value_ptr(lookAt(camera_position, vec3(0.0), vec3(0.0, 1.0, 0.0))));

        glBegin(GL_POINTS);

        if(selection == 0)
            render_dream(a, b, c, d, smooth_a, smooth_b, smooth_c, smooth_d);
        else if(selection == 1)
            render_dream_var(a, b, c, d, smooth_a, smooth_b, smooth_c, smooth_d);
        else if(selection == 2)
            render_ginger(b);
        else if(selection == 3)
            render_gumowisk_mira(a, b);
        else if(selection == 4)
            render_henon_attractor();
        else if(selection == 5)
            render_hopalong_attractor(a, b, c);

        glEnd();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::Begin("Vita Manga");
        ImGui::SliderInt("Attractor #", &selection, 0, 5);
        ImGui::DragInt("Restless Iterations", &restless_iterations, 1000, 10000, 10000000);
        if(selection == 0 || selection == 1)
        {
            ImGui::DragFloat("a", &a, 0.02, -3.0, 3.0);
            ImGui::Checkbox("Smooth a", &smooth_a);
            ImGui::DragFloat("b", &b, 0.02, -3.0, 3.0);
            ImGui::Checkbox("Smooth b", &smooth_b);
            ImGui::DragFloat("c", &c, 0.02, -0.5, 1.5);
            ImGui::Checkbox("Smooth c", &smooth_c);
            ImGui::DragFloat("d", &d, 0.02, -0.5, 1.5);
            ImGui::Checkbox("Smooth d", &smooth_d);
            if(smooth_a)
                if(a < 3.0)
                    a += 0.02;
                else
                    a = -3.0;
            if(smooth_b)
                    if(b < 3.0)
                        b += 0.02;
                    else
                        b = -3.0;
            if(smooth_c)
                    if(c < 1.5)
                        c += 0.02;
                    else
                        c = -1.5;
            if(smooth_d)
                    if(d < 1.5)
                        d += 0.02;
                    else
                        d = -1.5;
        }
        else if(selection == 2)
        {
            float diff = 0.01;
            ImGui::DragFloat("b", &b, 0.001, -3.0, 3.0);
        }
        else if(selection == 3)
        {
            ImGui::DragFloat("a", &a, 0.001, -0.999, 0.999);
            ImGui::DragFloat("b", &b, 0.0001, 0.9, 1.1);
        }
        else if(selection == 5)
        {
            ImGui::DragFloat("a", &a, 0.1, 0, 10);
            ImGui::DragFloat("b", &b, 0.1, 0, 10);
            ImGui::DragFloat("c", &c, 0.1, 0, 10);
        }
        
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
    const float camera_speed = 0.01f;
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
                    case SDLK_q: camera_position.z += camera_speed; break;
                    case SDLK_e: camera_position.z -= camera_speed; break;
                    case SDLK_w: camera_position.y += camera_speed; break;
                    case SDLK_s: camera_position.y -= camera_speed; break;
                    case SDLK_a: camera_position.x -= camera_speed; break;
                    case SDLK_d: camera_position.x += camera_speed; break;
                    default: break;
                }
            default: break;
        }
    }
}

void render_dream(const float &a, const float &b, const float &c, const float &d, const bool &smooth_a, const bool &smooth_b, const bool &smooth_c, const bool &smooth_d)
{ 
    float x = 0.1, y = 0.1;
    for(int i = 0; i < restless_iterations; i++)
    {
        float oldx = x;
        x = sin(y * b) + c * sin(x * b);
        y = sin(oldx * a) + d * sin(y * a);
        float cx = map(x, 0.0, 1.0, 0.0, 1.0);
        float cy = map(y, 0.0, 1.0, 0.0, 1.0);
        glColor4f(cx, 0.5, cy, 0.2);
        glVertex2f(x, y);
    }
}

void render_dream_var(const float &a, const float &b, const float &c, const float &d, const bool &smooth_a, const bool &smooth_b, const bool &smooth_c, const bool &smooth_d)
{ 
    float x = 0.1, y = 0.1;
    for(int i = 0; i < restless_iterations; i++)
    {
        x = sin(y * b) + c * sin(x * b);
        y = sin(x * a) + d * sin(y * a);
        float cx = map(x, 0.0, 1.0, 0.0, 1.0);
        float cy = map(y, 0.0, 1.0, 0.0, 1.0);
        glColor4f(cx, 0.5, cy, 0.2);
        glVertex2f(x, y);
    }
}

void render_ginger(const float &b)
{
    float x = 0.1, y = 0.1, newx, newy;
    glColor4f(0.7, 0.2, 0.8, 1.0);
    for(int i = 0; i < restless_iterations; i++)
    {
        newx = 1 - y + b * abs(x);
        newy = x;
        x = newx;
        y = newy;
        glVertex2f(x, y);
    }
}

void render_gumowisk_mira(const float &a, const float &b)
{
    float x = -6.4, y = 2.7, t, w, xnew, ynew;
    for(int i = 0;  i < restless_iterations; i++)
    {
        t = x;
        xnew = b * y + w;
        w = a * x + (1 - a) * 2 * x * x / (1 + x * x);
        ynew = w - t;
        x = xnew;
        y = ynew;
        glColor4f(map(x, 0, 1.0, 0, 1.0), map(y, 0, 1.0, 0, 1.0), map(w, 0, 1.0, 0, 1.0), 0.2);
        glVertex2f(x, y);
    }
}

void render_henon_attractor()
{
    float x = 1, y = 1, xnew, ynew;
    for(int i = 0; i < restless_iterations; i++)
    {
        xnew = y + 1 - (1.4 * x * x);
        ynew = 0.3 * x;
        x = xnew;
        y = ynew;
        glColor4f(map(x, 0.0, 1.0, 0.0, 1.0), map(x, 0.0, 1.0, 0.0, 1.0),
            map(x+y, 0.0, 1.0, 0.0, 1.0), 0.2);
        glVertex2f(x, y);
    }
}

void render_hopalong_attractor(const float a, const float b, const float c)
{
    float x = 0, y = 0, xnew, ynew;
    for(int i = 0; i < restless_iterations; i++)
    {
        xnew = y - 1 - sqrt(abs(b * x - 1 - c)) * sign(x - 1);
        ynew = a - x - 1;
        x = xnew;
        y = ynew;
        glColor4f(map(i, 0, 1.0, 0.0, 1.0) * 0.2, 0.2, 0.4, 0.4);
        glVertex2f(x, y);
    }
}

float map(const float value, const float min1, const float max1, const float min2, const float max2)
{
    float percent = (value - min1) / (max1 - min1);
    return percent * (max2 - min2) + min2;
}