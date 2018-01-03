
![布局](https://note.youdao.com/yws/public/resource/f5121682d839355f293abfd5dec29e9d/xmlnote/78FBEF408F7B41D4BE93F06120015739/9040)
#### NDK实战
为了学NDK编程，学了C/C++。学了cmakelists的配置。学了如何调用第三方的so和自己如何生成so库。那么第一个NDK实战就做起来吧。把前面的知识做一个综合。

#### 1、项目需求
做出类似QQ变声器的效果，使自己的声音产生多种变化。

#### 2、项目准备
1、下载依赖的三方类库：Fmod。（国际公认的音频处理工具）  
2、新建新项目，添加c++ support。 


#### 3、导入三方依赖
将下载的fmod中的api中的api/lowlevel/lib/中的文件全部copy到项目中。然后配置cmakeList。
```
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

find_library( log-lib
              log )

set(my_lib_path ${CMAKE_SOURCE_DIR}/libs)

# 添加三方的so库
add_library(libfmod
            SHARED
            IMPORTED )
# 指名第三方库的绝对路径
set_target_properties( libfmod
                       PROPERTIES IMPORTED_LOCATION
                       ${my_lib_path}/${ANDROID_ABI}/libfmod.so )

add_library(fmodL
            SHARED
            IMPORTED )

set_target_properties( fmodL
                       PROPERTIES IMPORTED_LOCATION
                       ${my_lib_path}/${ANDROID_ABI}/libfmodL.so )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 添加我们需要写代码的路径
add_library(changeVoice
            SHARED
            src/main/cpp/changeVoice.cpp )

# 导入路径，为了让编译时能够寻找到这个文件夹
include_directories(src/main/cpp/inc)

# 链接好三个路径
target_link_libraries( changeVoice
                       fmod
                       fmodL

                       ${log-lib} )


```
虽然以前学过cmakelists的配置，但是这里确实配置了很久才配置成功。  
遇到的问题：
* 第三方的so库配置的路径应为：**IMPORTED** 
* 自己定义的链接库应该写在前面：  

  ```
    target_link_libraries( changeVoice
                           fmod
                           fmodL
                       ${log-lib} )
  ```
  changeVoice为自己写的链接cpp文件的类库。
* 注意文件夹的导入：
  ```
    include_directories(src/main/cpp/inc)
  ```
  inc为头文件路径，为了是编译器能找到，所以include
* 动态库的命名规范
如果你的动态库为libxxx.so , 那么你的加载的名字为xxx就行了，系统会去加上lib,然后加上xxx加上.so。不然就会报找不到so的异常。


配置完以上之后记得先编译是否能通过。

#### 布局文件
布局文件应该是比较简单的东西。
![布局](https://note.youdao.com/yws/public/resource/f5121682d839355f293abfd5dec29e9d/xmlnote/78FBEF408F7B41D4BE93F06120015739/9040)

#### 代码
##### 1、建立一个java文件，添加加载动态库代码，添加native方法。
```
public class VoiceTools {

    public static native void  changeVoice(String path,int mode);

    static {
        System.loadLibrary("changeVoice");
        System.loadLibrary("fmod");
        System.loadLibrary("fmodL");
    }

}
```
##### 2、根据native方法生产c头文件。
用javah命名生产.h头文件。

然后将头文件copy到cpp文件夹下。

##### 3、将自己建的cpp文件include头文件并实现方法

##### 4、方法的具体实现
```
JNICALL
JNIEXPORT void JNICALL Java_com_mg_axechen_changevoice_VoiceTools_changeVoice
        (JNIEnv *jniEnv, jclass jclass, jstring jstring, jint mode) {

    // 初始化fmod
    FMOD::System *system;
    FMOD::System_Create(&system);
    
    Sound *sound;
    // 通道（声音是由多种音效组成）
    Channel *channel;
    // 音频
    DSP *pDSP;
    // 速度
    float frequency;

    system->init(32, FMOD_INIT_NORMAL, NULL);

    // 将 jstring转为 char
    const char *path = jniEnv->GetStringUTFChars(jstring, NULL);
    system->createSound(path, FMOD_DEFAULT, NULL, &sound);

    try {
        switch (mode) {
            case MODE_NORMAL:
                LOGI("%s", "正常");
                system->playSound(sound, NULL, false, &channel);
                break;
            case MODE_DASHU:
                LOGI("%s", "大叔");
                // 设置音调，调低音调
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pDSP);
                pDSP->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.7);
                system->playSound(sound, NULL, false, &channel);
                channel->addDSP(0, pDSP);
                break;
            case MODE_LUOLI:
                LOGI("%s", "萝莉");
                // 设置音调，调高音调
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pDSP);
                pDSP->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 3);
                system->playSound(sound, NULL, false, &channel);
                channel->addDSP(0, pDSP);
                break;
            case MODE_GAOGUAI:
                LOGI("%s", "搞怪");
                system->createDSPByType(FMOD_DSP_TYPE_NORMALIZE, &pDSP);
                system->playSound(sound, NULL, false, &channel);
                channel->addDSP(0, pDSP);
                // 获取速度并加速
                channel->getFrequency(&frequency);
                frequency = frequency * 1.6;
                channel->setFrequency(frequency);
                break;
            case MODE_JINGSONG:
                // FMOD_DSP_TYPE_TREMOLO 颤抖
                LOGI("%s", "惊悚");
                // 设置颤抖
                system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &pDSP);
                // 设置颤抖的频率
                pDSP->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.5);
                system->playSound(sound, NULL, false, &channel);
                channel->addDSP(0, pDSP);
                break;
            case MODE_KONGLING:
                LOGI("%s", "空灵");
                // 设置重复
                system->createDSPByType(FMOD_DSP_TYPE_ECHO, &pDSP);
                // 设置重复的重复延迟
                pDSP->setParameterFloat(FMOD_DSP_ECHO_DELAY, 300);
                pDSP->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20);
                system->playSound(sound, NULL, false, &channel);
                channel->addDSP(0, pDSP);
                break;
        }
        system->update();
        bool isPlaying = true;
        // 需要等待，等声音全部播放完成
        // 检查是否播放完成
        while (isPlaying) {
            channel->isPlaying(&isPlaying);
            usleep(1000);
        }
    } catch (...) {
        LOGE("%s", "error");
        goto end;
    }

    // 回收内存
    end:
    system->close();
    system->release();
    jniEnv->ReleaseStringUTFChars(jstring, path);

}

```

##### 5、界面的具体调用
```
public void mFix(View view) {
        String path = Environment.getExternalStorageDirectory().getAbsolutePath()+File.separator+"one.wav";
        Log.i("getPath", path);
        switch (view.getId()) {
            // 普通
            case R.id.btn_normal:
                VoiceTools.changeVoice(path,0);
                break;
                 // 萝莉
            case R.id.btn_luoli:
                VoiceTools.changeVoice(path,1);
                break;
            // 大叔
            case R.id.btn_dashu:
                VoiceTools.changeVoice(path,2);
                break;
                 // 惊悚
            case R.id.btn_jingsong:
                VoiceTools.changeVoice(path,3);
                break;
            // 搞怪
            case R.id.btn_gaoguai:
                VoiceTools.changeVoice(path,4);
                break;
            // 空灵
            case R.id.btn_kongling:
                VoiceTools.changeVoice(path,5);
                break;
           
        }

    }

```

##### 6、一些有问题的地方
1、我是默认添加一个语音文件在res/raw下。然后复制到sdCard中的。因为我获取res/raw的相对路径失败了。所以有一下比较奇怪的代码。
```
new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    InputStream inputStream =  getResources().openRawResource(R.raw.voice);
                    File file = new File(Environment.getExternalStorageDirectory().getPath(),"voice.wav");
                    if(!file.exists()){
                        file.createNewFile();
                    }
                    FileOutputStream fileOutputStream = new FileOutputStream(file);
                    byte[] buffer = new byte[10];
                    ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                    int len = 0;
                    while ((len=inputStream.read(buffer))!=-1){
                        outputStream.write(buffer,0,len);
                    }
                    byte[] bs = outputStream.toByteArray();
                    fileOutputStream.write(bs);
                    outputStream.close();
                    inputStream.close();
                    fileOutputStream.flush();
                    fileOutputStream.close();
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.i("getPath",e.getMessage());
                }
            }
        }).start();
```
2、可更换的路径：
```
 public void startChange(View view) {
        String path = Environment.getExternalStorageDirectory().getPath()+File.separator+"voice.wav";
        Log.i("getPath",path);
        switch (view.getId()) {

        //...    
        }
```
这个path应该是sdcard保存音频文件的路径。可自由跟换。

3、合成的效果并不好  
这个只是改了几个默认的效果，如果要做的更加准确需要自定义(音频)dsp。fmod中带的方法。
```
system->createDSP
```
