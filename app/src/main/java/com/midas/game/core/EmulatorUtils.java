package com.midas.game.core;

import android.content.Context;
import android.os.Environment;

import com.midas.game.emulator.EmulatorException;

import java.io.File;

public class EmulatorUtils {

    public static String getBaseDir(Context context) {
        File dir = null;
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
            dir = context.getExternalFilesDir(null);
        }
        if (dir == null) {
            dir = context.getFilesDir();
        }
        if (dir == null || !dir.exists()) {
            throw new EmulatorException("No working directory");
        }
        return dir.getAbsolutePath();
    }
}
