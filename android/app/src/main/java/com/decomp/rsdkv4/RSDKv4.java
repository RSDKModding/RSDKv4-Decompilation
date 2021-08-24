package com.decomp.rsdkv4;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;

import org.libsdl.app.SDLActivity;

public class RSDKv4 extends SDLActivity {
    private static Boolean asked = false;

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (permissions.length == 0) return;
        asked = true;
        int acquired = 0;
        for (int a : grantResults) acquired += a;
        if (acquired != 0) finishActivity(1);
    }

    public String getBasePath() {
        Context c = getApplicationContext();
        if (!((c.checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) || asked)) {
            requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
        }
        return c.getExternalFilesDir(null).getAbsolutePath() + "/";
    }
}
