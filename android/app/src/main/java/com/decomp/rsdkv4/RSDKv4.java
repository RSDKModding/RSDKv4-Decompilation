package com.decomp.rsdkv4;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;

import org.libsdl.app.*;

public class RSDKv4 extends SDLActivity {
    @Override
    protected void onStart() {
        super.onStart();
        getBasePath();

    }

    public String getBasePath() {
        Context c = getApplicationContext();
        requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.INTERNET}, 0);
        return c.getExternalFilesDir(null).getAbsolutePath() + "/";
    }
}
