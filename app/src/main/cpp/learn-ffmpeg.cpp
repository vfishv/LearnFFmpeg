/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/LearnFFmpeg
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include <cstdio>
#include <cstring>
#include <MediaRecorderContext.h>
#include "util/LogUtil.h"
#include "jni.h"
#include "ASanTestCase.h"

extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
};


extern "C"
JNIEXPORT jstring JNICALL Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1GetFFmpegVersion
        (JNIEnv *env, jclass cls)
{
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGCATE("GetFFmpegVersion\n%s", strBuffer);

    //ASanTestCase::MainTest();

    return env->NewStringUTF(strBuffer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1CreateContext(JNIEnv *env,
                                                                                jobject thiz) {
    MediaRecorderContext::CreateContext(env, thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1DestroyContext(JNIEnv *env,
                                                                                 jobject thiz) {
    MediaRecorderContext::DeleteContext(env, thiz);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1Init(JNIEnv *env, jobject thiz) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) return pContext->Init();
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1UnInit(JNIEnv *env, jobject thiz) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) return pContext->UnInit();
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1StartRecord(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jint recorder_type,
                                                                             jstring out_url,
                                                                             jint frame_width,
                                                                             jint frame_height,
                                                                             jlong video_bit_rate,
                                                                             jint fps) {
    const char* url = env->GetStringUTFChars(out_url, nullptr);
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    env->ReleaseStringUTFChars(out_url, url);
    if(pContext) return pContext->StartRecord(recorder_type, url, frame_width, frame_height, video_bit_rate, fps);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1OnAudioData(JNIEnv *env,
                                                                             jobject thiz,
                                                                             jbyteArray data,
                                                                             jint size) {
    int len = env->GetArrayLength (data);
    unsigned char* buf = new unsigned char[len];
    env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->OnAudioData(buf, len);
    delete[] buf;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1StopRecord(JNIEnv *env,
                                                                            jobject thiz) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) return pContext->StopRecord();
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1OnPreviewFrame(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jint format,
                                                                                 jbyteArray data,
                                                                                 jint width,
                                                                                 jint height) {
    int len = env->GetArrayLength (data);
    unsigned char* buf = new unsigned char[len];
    env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->OnPreviewFrame(format, buf, width, height);
    delete[] buf;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1SetTransformMatrix(JNIEnv *env,
                                                                                     jobject thiz,
                                                                                     jfloat translate_x,
                                                                                     jfloat translate_y,
                                                                                     jfloat scale_x,
                                                                                     jfloat scale_y,
                                                                                     jint degree,
                                                                                     jint mirror) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->SetTransformMatrix(translate_x, translate_y, scale_x, scale_y, degree, mirror);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1OnSurfaceCreated(JNIEnv *env,
                                                                                   jobject thiz) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->OnSurfaceCreated();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1OnSurfaceChanged(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jint width,
                                                                                   jint height) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->OnSurfaceChanged(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1OnDrawFrame(JNIEnv *env, jobject thiz) {
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->OnDrawFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1SetFilterData(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint index,
                                                                               jint format,
                                                                               jint width,
                                                                               jint height,
                                                                               jbyteArray bytes) {
    int len = env->GetArrayLength (bytes);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(bytes, 0, len, reinterpret_cast<jbyte*>(buf));
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->SetLUTImage(index, format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(bytes);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_byteflow_learnffmpeg_media_MediaRecorderContext_native_1SetFragShader(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint index,
                                                                               jstring str) {
    int length = env->GetStringUTFLength(str);
    const char* cStr = env->GetStringUTFChars(str, JNI_FALSE);
    char *buf = static_cast<char *>(malloc(length + 1));
    memcpy(buf, cStr, length + 1);
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, thiz);
    if(pContext) pContext->SetFragShader(index, buf, length + 1);
    free(buf);
    env->ReleaseStringUTFChars(str, cStr);
}