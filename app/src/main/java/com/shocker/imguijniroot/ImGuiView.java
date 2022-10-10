package com.shocker.imguijniroot;

import android.content.Context;
import android.graphics.PixelFormat;
import android.os.RemoteException;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

class ImGuiView extends SurfaceView implements SurfaceHolder.Callback{
   public ImGuiView(Context context) {
      super(context);
      setZOrderOnTop(true);
      SurfaceHolder holder = getHolder();
      holder.setFormat(PixelFormat.TRANSLUCENT);
      getHolder().addCallback(this);
      setFocusable(true);
      setFocusableInTouchMode(true);
      requestFocus();
      requestFocusFromTouch();
   }

   @Override
   public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
      Log.d("Shocker","surfaceCreated");
         new Thread(new Runnable() {
            @Override
            public void run() {
               try {
                  MainActivity.ImGui.init_ImGui(surfaceHolder.getSurface());
               } catch (RemoteException e) {
                  e.printStackTrace();
               }
            }
         }).start();
   }

   @Override
   public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
      Log.d("Shocker","surfaceChanged");
      try {
         MainActivity.ImGui.set_ImGui_Orientation(MainActivity.windowManager.getDefaultDisplay().getOrientation());
      } catch (RemoteException e) {
         e.printStackTrace();
      }
   }

   @Override
   public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

   }
}
