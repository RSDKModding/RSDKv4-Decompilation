package com.decomp.rsdkv4;

import android.Manifest;
import android.content.Context;
import android.provider.MediaStore;
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
        requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
        MediaStore.Files.getContentUri(MediaStore.VOLUME_EXTERNAL); //just force it if its not there
        return getExternalFilesDir(null).getAbsolutePath().replace("/data/", "/media/").replace("/files", "/");
    }
}
