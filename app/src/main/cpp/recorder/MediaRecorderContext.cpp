/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/LearnFFmpeg
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */

#include <LogUtil.h>
#include <ImageDef.h>
#include "MediaRecorderContext.h"

jfieldID MediaRecorderContext::s_ContextHandle = 0L;

MediaRecorderContext::MediaRecorderContext() {
	GLCameraRender::GetInstance();
}

MediaRecorderContext::~MediaRecorderContext()
{
	GLCameraRender::ReleaseInstance();
}

void MediaRecorderContext::CreateContext(JNIEnv *env, jobject instance)
{
	LOGCATE("MediaRecorderContext::CreateContext");
	MediaRecorderContext *pContext = new MediaRecorderContext();
	StoreContext(env, instance, pContext);
}

void MediaRecorderContext::StoreContext(JNIEnv *env, jobject instance, MediaRecorderContext *pContext)
{
	LOGCATE("MediaRecorderContext::StoreContext");
	jclass cls = env->GetObjectClass(instance);
	if (cls == NULL)
	{
		LOGCATE("MediaRecorderContext::StoreContext cls == NULL");
		return;
	}

	s_ContextHandle = env->GetFieldID(cls, "mNativeContextHandle", "J");
	if (s_ContextHandle == NULL)
	{
		LOGCATE("MediaRecorderContext::StoreContext s_ContextHandle == NULL");
		return;
	}

	env->SetLongField(instance, s_ContextHandle, reinterpret_cast<jlong>(pContext));

}


void MediaRecorderContext::DeleteContext(JNIEnv *env, jobject instance)
{
	LOGCATE("MediaRecorderContext::DeleteContext");
	if (s_ContextHandle == NULL)
	{
		LOGCATE("MediaRecorderContext::DeleteContext Could not find render context.");
		return;
	}

	MediaRecorderContext *pContext = reinterpret_cast<MediaRecorderContext *>(env->GetLongField(
			instance, s_ContextHandle));
	if (pContext)
	{
		delete pContext;
	}
	env->SetLongField(instance, s_ContextHandle, 0L);
}

MediaRecorderContext *MediaRecorderContext::GetContext(JNIEnv *env, jobject instance)
{
	LOGCATE("MediaRecorderContext::GetContext");

	if (s_ContextHandle == NULL)
	{
		LOGCATE("MediaRecorderContext::GetContext Could not find render context.");
		return NULL;
	}

	MediaRecorderContext *pContext = reinterpret_cast<MediaRecorderContext *>(env->GetLongField(
			instance, s_ContextHandle));
	return pContext;
}

int MediaRecorderContext::Init()
{
	GLCameraRender::GetInstance()->Init(0, 0, nullptr);
	GLCameraRender::GetInstance()->SetRenderCallback(this, OnGLRenderFrame);//nullptr
	return 0;
}

int MediaRecorderContext::UnInit()
{
	GLCameraRender::GetInstance()->UnInit();

	return 0;
}

int
MediaRecorderContext::StartRecord(int recorderType, const char *outUrl, int frameWidth, int frameHeight, long videoBitRate,
                                  int fps) {
	LOGCATE("MediaRecorderContext::StartRecord recorderType=%d, outUrl=%s, [w,h]=[%d,%d], videoBitRate=%ld, fps=%d", recorderType, outUrl, frameWidth, frameHeight, videoBitRate, fps);
	std::unique_lock<std::mutex> lock(m_mutex);

    if(m_pAVRecorder == nullptr) {
        RecorderParam param = {0};
        param.frameWidth      = frameHeight;
        param.frameHeight     = frameWidth;
        param.videoBitRate    = videoBitRate;
        param.fps             = fps;
        param.audioSampleRate = DEFAULT_SAMPLE_RATE;
        param.channelLayout   = AV_CH_LAYOUT_STEREO;
        param.sampleFormat    = AV_SAMPLE_FMT_S16;
        m_pAVRecorder = new MediaRecorder(outUrl, &param);
        m_pAVRecorder->StartRecord();
    }

    return 0;
}

int MediaRecorderContext::StopRecord() {
	std::unique_lock<std::mutex> lock(m_mutex);

	if(m_pAVRecorder != nullptr) {
		m_pAVRecorder->StopRecord();
		delete m_pAVRecorder;
		m_pAVRecorder = nullptr;
	}
    return 0;
}

void MediaRecorderContext::OnAudioData(uint8_t *pData, int size) {
    LOGCATE("MediaRecorderContext::OnAudioData pData=%p, dataSize=%d", pData, size);
    AudioFrame audioFrame(pData, size, false);

    if(m_pAVRecorder != nullptr)
    	m_pAVRecorder->OnFrame2Encode(&audioFrame);
}

void MediaRecorderContext::OnPreviewFrame(int format, uint8_t *pBuffer, int width, int height)
{
	LOGCATE("MediaRecorderContext::UpdateFrame format=%d, width=%d, height=%d, pData=%p",
			format, width, height, pBuffer);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pBuffer;

    switch (format)
	{
		case IMAGE_FORMAT_NV12:
		case IMAGE_FORMAT_NV21:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.pLineSize[0] = width;
            nativeImage.pLineSize[1] = width;
			break;
		case IMAGE_FORMAT_I420:
			nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
			nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            nativeImage.pLineSize[0] = width;
            nativeImage.pLineSize[1] = width / 2;
            nativeImage.pLineSize[2] = width / 2;
			break;
		default:
			break;
	}

//	std::unique_lock<std::mutex> lock(m_mutex);
//	if(m_pVideoRecorder!= nullptr) {
//        m_pVideoRecorder->OnFrame2Encode(&nativeImage);
//    }
//	lock.unlock();

    //NativeImageUtil::DumpNativeImage(&nativeImage, "/sdcard", "camera");
    GLCameraRender::GetInstance()->RenderVideoFrame(&nativeImage);
/*
if(false) {
	////GLCameraRender::RenderVideoFrame
	if (pBuffer == nullptr)
		return;
	NativeImage m_RenderImage;
	//std::unique_lock<std::mutex> lock(m_Mutex);
	NativeImage *pImage = &nativeImage;
	if (pImage->width != m_RenderImage.width || pImage->height != m_RenderImage.height) {
		if (m_RenderImage.ppPlane[0] != nullptr) {
			NativeImageUtil::FreeNativeImage(&m_RenderImage);
		}
		memset(&m_RenderImage, 0, sizeof(NativeImage));
		m_RenderImage.format = pImage->format;
		m_RenderImage.width = pImage->width;
		m_RenderImage.height = pImage->height;
		NativeImageUtil::AllocNativeImage(&m_RenderImage);
	}

	NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);

	////GLCameraRender::GetRenderFrameFromFBO()

	uint8_t *pBuf = new uint8_t[m_RenderImage.width * m_RenderImage.height * 4];
	NativeImage nImage = m_RenderImage;
	nImage.format = IMAGE_FORMAT_RGBA;
	nImage.width = m_RenderImage.height;
	nImage.height = m_RenderImage.width;
	nImage.pLineSize[0] = nImage.width * 4;
	nImage.ppPlane[0] = pBuf;
	glReadPixels(0, 0, nImage.width, nImage.height, GL_RGBA, GL_UNSIGNED_BYTE, pBuf);
	OnGLRenderFrame(this, &nImage);
	delete[]pBuf;
}else{
	OnGLRenderFrame(this, &nativeImage);
}
*/

}

void MediaRecorderContext::SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror)
{
	m_transformMatrix.translateX = translateX;
	m_transformMatrix.translateY = translateY;
	m_transformMatrix.scaleX = scaleX;
	m_transformMatrix.scaleY = scaleY;
	m_transformMatrix.degree = degree;
	m_transformMatrix.mirror = mirror;
	GLCameraRender::GetInstance()->UpdateMVPMatrix(&m_transformMatrix);
}

void MediaRecorderContext::OnSurfaceCreated()
{
	GLCameraRender::GetInstance()->OnSurfaceCreated();
}

void MediaRecorderContext::OnSurfaceChanged(int width, int height)
{
	GLCameraRender::GetInstance()->OnSurfaceChanged(width, height);
}

void MediaRecorderContext::OnDrawFrame()
{
	GLCameraRender::GetInstance()->OnDrawFrame();
}

void MediaRecorderContext::OnGLRenderFrame(void *ctx, NativeImage *pImage) {
	LOGCATE("MediaRecorderContext::OnGLRenderFrame ctx=%p, pImage=%p", ctx, pImage);
	MediaRecorderContext *context = static_cast<MediaRecorderContext *>(ctx);
	std::unique_lock<std::mutex> lock(context->m_mutex);

	if(context->m_pAVRecorder != nullptr)
		context->m_pAVRecorder->OnFrame2Encode(pImage);
}

void
MediaRecorderContext::SetLUTImage(int index, int format, int width, int height, uint8_t *pData) {
	LOGCATE("MediaRecorderContext::SetLUTImage index=%d, format=%d, width=%d, height=%d, pData=%p",
			index, format, width, height, pData);
	NativeImage nativeImage;
	nativeImage.format = format;
	nativeImage.width = width;
	nativeImage.height = height;
	nativeImage.ppPlane[0] = pData;
	nativeImage.pLineSize[0] = width * 4; //RGBA

	GLCameraRender::GetInstance()->SetLUTImage(index, &nativeImage);
}

void MediaRecorderContext::SetFragShader(int index, char *pShaderStr, int strSize) {
	GLCameraRender::GetInstance()->SetFragShaderStr(index, pShaderStr, strSize);
}


