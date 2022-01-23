package com.decomp.rsdkv4;

import android.Manifest;
import android.content.Context;
import android.os.Environment;

import java.io.File;

import org.libsdl.app.*;


public class RSDKv4 extends SDLActivity {
    @Override
    protected void onStart() {
        super.onStart();
        getBasePath();
    }

    //Idk what the hell "has multi window" is, but I do NOT have multiple windows and therefore DO wanna pause/resume these threads
    @Override
    protected void onPause() {
        super.onPause();
        if (mHasMultiWindow) {
            pauseNativeThread();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (mHasMultiWindow) {
            resumeNativeThread();
        }
    }

    public String getBasePath() {
        Context c = getApplicationContext();
        requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
        String p = Environment.getExternalStorageDirectory().getAbsolutePath() + "/RSDK/v4";
        //getExternalStorageDirectory is deprecated. I do not care.
        new File(p).mkdirs();
        return p + "/";
    }
}
