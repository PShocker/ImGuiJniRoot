package com.shocker.imguijniroot;


import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.PixelFormat;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.provider.Settings;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

import com.shocker.imguijniroot.R;
import com.shocker.imguijniroot.AIDLService;
import com.topjohnwu.superuser.ipc.RootService;


public class MainActivity extends AppCompatActivity {

    public static WindowManager windowManager;
    public static Activity mainActivity;
    public static WindowManager.LayoutParams params;
    public static boolean start=false;

    public static IImGuiService ImGui;

    class AIDLConnection implements ServiceConnection {

        private final boolean isDaemon;

        AIDLConnection(boolean b) {
            isDaemon = b;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            ImGui= IImGuiService.Stub.asInterface(service);
            Log.d("Shocker","Connected");
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            Log.d("Shocker","Disconnected");
        }
    }

    void getPermission() {
        if (android.os.Build.VERSION.SDK_INT >= 23 && !Settings.canDrawOverlays(this)) {   //Android M Or Over
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 0);
        }
    }

    public static int getLayoutType() {
        int LAYOUT_FLAG;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_PHONE;
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_PHONE;
        } else {
            LAYOUT_FLAG = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
        }
        return LAYOUT_FLAG;
    }

    protected void addFullView() {
//        gl2JNIView = new GL2JNIView(this);
        ImGuiView imGuiView=new ImGuiView(this);
        params = new WindowManager.LayoutParams(
                getLayoutType(),
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE
                        | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL | WindowManager.LayoutParams.FLAG_FULLSCREEN,
                PixelFormat.TRANSLUCENT);
        params.gravity = Gravity.LEFT | Gravity.TOP;		// 调整悬浮窗显示的停靠位置为左侧置顶

        params.systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION|View.SYSTEM_UI_FLAG_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE|View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

        // 以屏幕左上角为原点，设置x、y初始值（10,10），相对于gravity
        params.x = 0;
        params.y = 0;
        // 设置悬浮窗口长宽数据
        params.width  = WindowManager.LayoutParams.MATCH_PARENT;
        params.height = WindowManager.LayoutParams.MATCH_PARENT;
        if (Build.VERSION.SDK_INT >= 28) {
            params.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES ;
        }
        windowManager.addView(imGuiView, params);
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        //
        RootService.bind(new Intent(this, AIDLService.class), new AIDLConnection(false));
        Button t=findViewById(R.id.sample_button);
        t.setOnClickListener(view -> {
            if (!start){
                addFullView();
                start=true;
            }
        });
        getPermission();
        mainActivity = this;
        windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
    }

    @Override
    public void onBackPressed() {
//        super.onBackPressed();
        Intent i = new Intent(Intent.ACTION_MAIN);
        i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        i.addCategory(Intent.CATEGORY_HOME);
        startActivity(i);
    }

    /**
     * A native method that is implemented by the 'imguikeyboard' native library,
     * which is packaged with this application.
     */
}