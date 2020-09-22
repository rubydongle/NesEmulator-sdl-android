package com.midas.game;

import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class GameActivity extends SDLActivity {

    private static String sGameName = "zhadanrenwudibannocopyright.nes";
    Vibrator vibrator;
    private int vibrationDuration = 30;
    boolean mVibrationEnable = true;

    void copyGameToDataFiles() {
        InputStream in = null;
        FileOutputStream out = null;
        String path = this.getApplicationContext().getFilesDir()
                .getAbsolutePath() + "/" + sGameName; // data/data目录
        File file = new File(path);
        if(!file.exists()) {
            try
            {
                in = this.getAssets().open(sGameName); // 从assets目录下复制
                out = new FileOutputStream(file);
                int length = -1;
                byte[] buf = new byte[1024];
                while ((length = in.read(buf)) != -1)
                {
                    out.write(buf, 0, length);
                }
                out.flush();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                if(in != null) {
                    try {
                        in.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                if(out != null) {
                    try {
                        out.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    protected String[] getArguments() {
        String[] argS = new String[1];
        String gamePath = this.getApplicationContext().getFilesDir()
                .getAbsolutePath() + "/" + sGameName; // data/data目录
        argS[0] = gamePath;
        return argS;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        copyGameToDataFiles();
        super.onCreate(savedInstanceState);
        vibrator = (Vibrator) getContext().getSystemService(Context.VIBRATOR_SERVICE);
        View controlPad = LayoutInflater.from(this).inflate(R.layout.control_pad, null);
        Button buttonUp = controlPad.findViewById(R.id.buttonup);
        buttonUp.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_UP);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
//                        onNativeKeyDown(273);//KeyEvent.KEYCODE_DPAD_UP);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_UP);
                        view.setScaleX(1);
                        view.setScaleY(1);
//                        onNativeKeyUp(273);//KeyEvent.KEYCODE_DPAD_UP);
                        break;
                }
                return true;
            }
        });
        Button buttonDown = controlPad.findViewById(R.id.buttondown);
        buttonDown.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_DOWN);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
//                        onNativeKeyDown(274);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_DOWN);
                        view.setScaleX(1);
                        view.setScaleY(1);
//                        onNativeKeyUp(274);
                        break;
                }
                return true;
            }
        });
        Button buttonLeft = controlPad.findViewById(R.id.buttonleft);
        buttonLeft.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_LEFT);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
//                        onNativeKeyDown(105);//276);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_LEFT);
                        view.setScaleX(1);
                        view.setScaleY(1);
//                        onNativeKeyUp(105);//276);
                        break;
                }
                return true;
            }
        });
        Button buttonRight = controlPad.findViewById(R.id.buttonright);
        buttonRight.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_RIGHT);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
//                        onNativeKeyDown(106);//275);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_RIGHT);
                        view.setScaleX(1);
                        view.setScaleY(1);
//                        onNativeKeyUp(106);//275);
                        break;
                }
                return true;
            }
        });

        Button buttonUpLeft = controlPad.findViewById(R.id.button_upleft);
        buttonUpLeft.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_UP);
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_LEFT);
                        buttonUp.setScaleX((float)0.9);
                        buttonUp.setScaleY((float)0.9);
                        buttonLeft.setScaleX((float)0.9);
                        buttonLeft.setScaleY((float)0.9);
//                        onNativeKeyDown(106);//275);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_UP);
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_LEFT);
                        buttonUp.setScaleX(1);
                        buttonUp.setScaleY(1);
                        buttonLeft.setScaleX(1);
                        buttonLeft.setScaleY(1);
//                        onNativeKeyUp(106);//275);
                        break;
                }
                return true;
            }
        });
        Button buttonUpRight = controlPad.findViewById(R.id.button_upright);
        buttonUpRight.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_UP);
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_RIGHT);
                        buttonUp.setScaleX((float)0.9);
                        buttonUp.setScaleY((float)0.9);
                        buttonRight.setScaleX((float)0.9);
                        buttonRight.setScaleY((float)0.9);
//                        onNativeKeyDown(106);//275);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_UP);
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_RIGHT);
                        buttonUp.setScaleX(1);
                        buttonUp.setScaleY(1);
                        buttonRight.setScaleX(1);
                        buttonRight.setScaleY(1);
//                        onNativeKeyUp(106);//275);
                        break;
                }
                return true;
            }
        });
        Button buttonDownRight = controlPad.findViewById(R.id.button_downright);
        buttonDownRight.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_DOWN);
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_RIGHT);
                        buttonDown.setScaleX((float)0.9);
                        buttonDown.setScaleY((float)0.9);
                        buttonRight.setScaleX((float)0.9);
                        buttonRight.setScaleY((float)0.9);
//                        onNativeKeyDown(106);//275);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_DOWN);
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_RIGHT);
                        buttonDown.setScaleX(1);
                        buttonDown.setScaleY(1);
                        buttonRight.setScaleX(1);
                        buttonRight.setScaleY(1);
//                        onNativeKeyUp(106);//275);
                        break;
                }
                return true;
            }
        });
        Button buttonDownLeft = controlPad.findViewById(R.id.button_downleft);
        buttonDownLeft.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_DOWN);
                        onNativeKeyDown(KeyEvent.KEYCODE_DPAD_LEFT);
                        buttonDown.setScaleX((float)0.9);
                        buttonDown.setScaleY((float)0.9);
                        buttonLeft.setScaleX((float)0.9);
                        buttonLeft.setScaleY((float)0.9);
//                        onNativeKeyDown(106);//275);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_DOWN);
                        onNativeKeyUp(KeyEvent.KEYCODE_DPAD_LEFT);
                        buttonDown.setScaleX(1);
                        buttonDown.setScaleY(1);
                        buttonLeft.setScaleX(1);
                        buttonLeft.setScaleY(1);
//                        onNativeKeyUp(106);//275);
                        break;
                }
                return true;
            }
        });
        Button buttonReset = controlPad.findViewById(R.id.reset);
        buttonReset.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_F11);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_F11);
                        view.setScaleX(1);
                        view.setScaleY(1);
                        break;
                }
                return true;
            }
        });

        Button buttonS = controlPad.findViewById(R.id.buttons);
        buttonS.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_S);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_S);
                        view.setScaleX(1);
                        view.setScaleY(1);
                        break;
                }
                return true;
            }
        });
        Button buttonEnter = controlPad.findViewById(R.id.buttonenter);
        buttonEnter.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_ENTER);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_ENTER);
                        view.setScaleX(1);
                        view.setScaleY(1);
                        break;
                }
                return true;
            }
        });

        Button buttonD = controlPad.findViewById(R.id.buttond);
        buttonD.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_D);
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_D);
                        view.setScaleX(1);
                        view.setScaleY(1);
                        break;
                }
                return true;
            }
        });
        Button buttonF = controlPad.findViewById(R.id.buttonf);
        buttonF.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        onNativeKeyDown(KeyEvent.KEYCODE_F);
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        view.setScaleX((float)0.9);
                        view.setScaleY((float)0.9);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_F);
                        view.setScaleX(1);
                        view.setScaleY(1);
                        break;
                }
                return true;
            }
        });
        Button buttonBA = controlPad.findViewById(R.id.buttonba);
        buttonBA.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        if(mVibrationEnable) {vibrator.vibrate(vibrationDuration);}
                        onNativeKeyDown(KeyEvent.KEYCODE_D);
                        onNativeKeyDown(KeyEvent.KEYCODE_F);
                        buttonD.setScaleX((float)0.9);
                        buttonD.setScaleY((float)0.9);
                        buttonF.setScaleX((float)0.9);
                        buttonF.setScaleY((float)0.9);
                        break;
                    case MotionEvent.ACTION_UP:
                        onNativeKeyUp(KeyEvent.KEYCODE_D);
                        onNativeKeyUp(KeyEvent.KEYCODE_F);
                        buttonD.setScaleX(1);
                        buttonD.setScaleY(1);
                        buttonF.setScaleX(1);
                        buttonF.setScaleY(1);
                        break;
                }
                return true;
            }
        });
        mLayout.addView(controlPad);
    }
}
