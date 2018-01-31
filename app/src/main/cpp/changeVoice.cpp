#include "com_mg_axechen_changevoice_VoiceTools.h"
#include "fmod.hpp"
#include <unistd.h>
#include <android/log.h>

using namespace FMOD;

// 定义五种不同的模式
#define MODE_NORMAL 0
#define MODE_LUOLI 1
#define MODE_DASHU 2
#define MODE_JINGSONG 3
#define MODE_GAOGUAI 4
#define MODE_KONGLING 5

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"axe",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"axe",FORMAT,##__VA_ARGS__);
extern "C"

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
                pDSP->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.8);
                system->playSound(sound, NULL, false, &channel);
                channel->addDSP(0, pDSP);
                channel->getFrequency(&frequency);
                frequency = frequency * 0.5;
                channel->setFrequency(frequency);
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



