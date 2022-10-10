//
// Created by Shocker on 2022/10/5.
//

#include "Draw.h"

int screen_width;
int screen_height;




extern int g_orientation;
struct motion_event m_event;
static bool g_Initialized = false;

static EGLDisplay g_EglDisplay = EGL_NO_DISPLAY;
static EGLSurface g_EglSurface = EGL_NO_SURFACE;
static EGLContext g_EglContext = EGL_NO_CONTEXT;


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Shocker", __VA_ARGS__))

int32_t handleInputEvent(motion_event *inputEvent)
{
    return ImGui_ImplAndroid_HandleInputEvent(inputEvent, g_orientation);
}


void init(ANativeWindow *g_EglNativeWindowType)
{
    if (g_Initialized)
        return;

    //window = windowSurface.getSurface();
    ANativeWindow_acquire(g_EglNativeWindowType);

    // Initialize EGL
    // This is mostly boilerplate code for EGL...
    {
        g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (g_EglDisplay == EGL_NO_DISPLAY)
            //__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");
            printf("eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY\n");

        if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
            //__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglInitialize() returned with an error");
            printf("eglInitialize() returned with an error\n");

        const EGLint egl_attributes[] = {EGL_BLUE_SIZE, 8,
                                         EGL_GREEN_SIZE, 8,
                                         EGL_RED_SIZE, 8,
                                         EGL_ALPHA_SIZE, 8,
                                         EGL_DEPTH_SIZE, 24,
                                         EGL_SURFACE_TYPE,
                                         EGL_WINDOW_BIT,
                                         EGL_NONE};
        EGLint num_configs = 0;
        if (eglChooseConfig(g_EglDisplay, egl_attributes, nullptr, 0, &num_configs) != EGL_TRUE)
            //__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglChooseConfig() returned with an error");
            printf("eglChooseConfig() returned with an error\n");
        if (num_configs == 0)
            //__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglChooseConfig() returned 0 matching config");
            printf("eglChooseConfig() returned 0 matching config\n");
        // Get the first matching config
        EGLConfig egl_config;
        eglChooseConfig(g_EglDisplay, egl_attributes, &egl_config, 1, &num_configs);
        EGLint egl_format;
        eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
        ANativeWindow_setBuffersGeometry(g_EglNativeWindowType, 0, 0, egl_format);

        const EGLint egl_context_attributes[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
        g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, egl_context_attributes);

        if (g_EglContext == EGL_NO_CONTEXT)
            //__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s", "eglCreateContext() returned EGL_NO_CONTEXT");
            printf("eglCreateContext() returned EGL_NO_CONTEXT\n");

        g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, g_EglNativeWindowType, NULL);
        eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);

        printf("init success\n");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    // Disable loading/saving of .ini file from disk.
    // FIXME: Consider using LoadIniSettingsFromMemory() / SaveIniSettingsToMemory() to save in appropriate location for Android.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    io.Fonts->AddFontFromMemoryTTF((void *)font_data, font_size, 24.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());


    // Setup Platform/Renderer backends
    ImGui_ImplAndroid_Init(g_EglNativeWindowType);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    g_Initialized = true;
}


void tick()
{
    static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImGuiIO& io = ImGui::GetIO();

    glViewport(0.0f, 0.0f, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT); // GL_DEPTH_BUFFER_BIT
    glFlush();

    if (g_EglDisplay == EGL_NO_DISPLAY){
        return;
    }

    // Our state
    static bool show_demo_window = true;
    static bool show_another_window = false;
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    // ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window){
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    { // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit4("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        if (ImGui::Button("Button")){
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    if (show_another_window){ // 3. Show another simple window.
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")){
            show_another_window = false;
        }
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(g_EglDisplay, g_EglSurface);
}