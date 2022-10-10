// IImGuiService.aidl
package com.shocker.imguijniroot;

// Declare any non-default types here with import statements

interface IImGuiService {
    void init_ImGui(inout Surface surface);
    void update_ImGui(int width, int height);
    void set_ImGui_Orientation(int orientation);
}