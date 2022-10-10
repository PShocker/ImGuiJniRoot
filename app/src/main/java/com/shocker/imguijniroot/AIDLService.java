/*
 * Copyright 2021 John "topjohnwu" Wu
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.shocker.imguijniroot;


import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.os.RemoteException;
import android.view.Surface;

import androidx.annotation.NonNull;

import com.topjohnwu.superuser.ipc.RootService;


// Demonstrate RootService using AIDL (daemon mode)
class AIDLService extends RootService {

    static {
        // Only load the library when this class is loaded in a root process.
        // The classloader will load this class (and call this static block) in the non-root
        // process because we accessed it when constructing the Intent to send.
        // Add this check so we don't unnecessarily load native code that'll never be used.
        if (Process.myUid() == 0)
            System.loadLibrary("imguijniroot");
    }

    public  native void native_init_ImGui(Surface surface);
    public  native void native_update_ImGui(int width, int height);
    public  native void native_set_ImGui_Orientation(int orientation);


    class ImGui extends IImGuiService.Stub {

        @Override
        public void init_ImGui(Surface surface) throws RemoteException {
            native_init_ImGui(surface);
        }

        @Override
        public void update_ImGui(int width, int height) throws RemoteException {
            native_update_ImGui(width,height);
        }


        @Override
        public void set_ImGui_Orientation(int orientation) throws RemoteException {
            native_set_ImGui_Orientation(orientation);
        }
    }

    @Override
    public IBinder onBind(@NonNull Intent intent) {
        return new ImGui();
    }
}
