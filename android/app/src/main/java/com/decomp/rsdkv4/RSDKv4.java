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

    public String getBasePath() {
        Context c = getApplicationContext();
        requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
        String p = Environment.getExternalStorageDirectory().getAbsolutePath() + "/RSDK/v4";
        //getExternalStorageDirectory is deprecated. I do not care.
        new File(p).mkdirs();
        return p + "/";
    }
}
