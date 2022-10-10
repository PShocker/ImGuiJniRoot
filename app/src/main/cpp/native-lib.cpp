#include <jni.h>

#include <android/log.h>
#include <android/native_window_jni.h>

#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "imgui/imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"

#include "Draw.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Shocker", __VA_ARGS__))

extern int screen_width;
extern int screen_height;

extern int touch_width;
extern int touch_height;

int g_orientation;

extern "C"
[[noreturn]] JNIEXPORT void JNICALL
Java_com_shocker_imguijniroot_AIDLService_native_1init_1ImGui(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement native_init_ImGui()
    LOGI("native_init_ImGui");
    ANativeWindow* w = ANativeWindow_fromSurface(env, (jobject)surface);
    init(w);
    screen_width=ANativeWindow_getWidth(w);
    screen_height=ANativeWindow_getHeight(w);
    ANativeWindow_release(w);

    int fd = getTouchDevice();
    int mEpollFd = createEpoll(fd);

    if (mEpollFd == 0)
    {
        printf("createEpoll error");
    }

    while (true){
        struct epoll_event events[1];
        struct input_event input_event;
        while (hasEvent(mEpollFd, events, 1)>0){
            read(fd, &input_event, sizeof(struct input_event));
            motion_event *m_event = process(input_event);
            if (m_event == nullptr)
            {
                continue;
            }
            m_event->x=m_event->x/touch_width*screen_width;
            m_event->y=m_event->y/touch_height*screen_height;
            handleInputEvent(m_event);
            delete m_event;
        }
        tick();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_shocker_imguijniroot_AIDLService_native_1update_1ImGui(JNIEnv *env, jobject thiz,
                                                                 jint width, jint height) {
    // TODO: implement update_ImGui()
    LOGI("update_ImGui");

    glViewport(0, 0, width, height);
    ImGui::GetIO().DisplaySize = ImVec2((float)width, (float)height);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_shocker_imguijniroot_AIDLService_native_1set_1ImGui_1Orientation(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jint orientation) {
    // TODO: implement native_set_ImGui_Orientation()
    LOGI("native_set_ImGui_Orientation");
    g_orientation=orientation;
}

