package com.mg.axechen.changevoice;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void mFix(View view) {
        String path = Environment.getExternalStorageDirectory().getAbsolutePath()+File.separator+"one.wav";
        Log.i("getPath", path);
        switch (view.getId()) {
            // 普通
            case R.id.btn_normal:
                VoiceTools.changeVoice(path,0);
                break;
            // 大叔
            case R.id.btn_dashu:
                VoiceTools.changeVoice(path,2);
                break;
            // 搞怪
            case R.id.btn_gaoguai:
                VoiceTools.changeVoice(path,4);
                break;

             // 惊悚
            case R.id.btn_jingsong:
                VoiceTools.changeVoice(path,3);
                break;

            // 空灵
            case R.id.btn_kongling:
                VoiceTools.changeVoice(path,5);
                break;

            // 萝莉
            case R.id.btn_luoli:
                VoiceTools.changeVoice(path,1);
                break;
        }

    }

}
