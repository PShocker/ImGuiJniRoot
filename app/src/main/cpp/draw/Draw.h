//
// Created by Shocker on 2022/10/5.
//

#include <jni.h>
#include <pthread.h>
#include <string>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <queue>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/un.h>


#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "imgui/imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"

#include "Font.h"
#include "Common.h"
#include "Event.h"


#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <EGL/egl.h>
#include <GLES/gl.h>


void init(ANativeWindow *g_EglNativeWindowType);
void tick();
int32_t handleInputEvent(motion_event *inputEvent);