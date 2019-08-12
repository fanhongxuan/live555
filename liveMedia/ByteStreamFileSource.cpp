/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2019 Live Networks, Inc.  All rights reserved.
// A file source that is a plain byte stream (rather than frames)
// Implementation
#include <map>
#include "ByteStreamFileSource.hh"
#include "InputFile.hh"
#include "GroupsockHelper.hh"

////////// ByteStreamFileSource //////////
#include "LocalSdk.h"
#define READ_FROM_FILES_SYNCHRONOUSLY
void logBegin(const char *type, const char *file, const char *function, int line);
void logEnd();

#define Logi(...) do{logBegin("info" , __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__); logEnd();}while(0)
#define Loge(...) do{logBegin("error", __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__); logEnd();}while(0)

class H264FrameBuffer{
public:
    unsigned char *pHeader;
    unsigned char *pContent;
    unsigned long nLength;
    unsigned long nFrameLength;
    unsigned long nOffset;
    unsigned int FrameType;
    unsigned int nSubType;
    unsigned int nEncodeType;
    unsigned int nFrameRate;
    unsigned int nWidth;
    unsigned int nHeight;
};

static pthread_mutex_t gLogMutex = PTHREAD_MUTEX_INITIALIZER;

void logBegin(const char *type, const char *file, const char *function, int line)
{
    pthread_mutex_lock(&gLogMutex);
    time_t timevalue=time(NULL);
    struct tm t;
    memset(&t, 0, sizeof(t));
	localtime_r(&timevalue,&t);
    struct timeval now;
    gettimeofday(&now, NULL);
	printf("[%04d-%02d-%02d %02d:%02d:%02d-%03d][%s][%s:%s:%d]",
        t.tm_year + 1900, 
        t.tm_mon + 1, 
        t.tm_mday,
        t.tm_hour, 
        t.tm_min, 
        t.tm_sec, 
        (int)now.tv_usec/1000,
        type,
        file,
        function, 
        line);
}

void logEnd()
{
    printf("\n");
    pthread_mutex_unlock(&gLogMutex);
}

static int buildFileInfoByName(LOCALSDK_FILE_DATA *pFileInfo, const char *pFileName){
    // todo:fanhongxan@gmail.com
    // parse the filename, channel, startTime/stopTime, size info from the pFileName
    // format is channel/size/starttime/stoptime/name.264
    int i = 0;
    int channel = 0, size = 0;
    int ret = sscanf(pFileName, "localsdk/%d/%d/%04d-%02d-%02d-%02d-%02d-%02d/%04d-%02d-%02d-%02d-%02d-%02d/%s", 
                     &channel, &size, 
                     &pFileInfo->stBeginTime.year,
                     &pFileInfo->stBeginTime.month,
                     &pFileInfo->stBeginTime.day,
                     &pFileInfo->stBeginTime.hour,
                     &pFileInfo->stBeginTime.minute,
                     &pFileInfo->stBeginTime.second,
                     &pFileInfo->stEndTime.year,
                     &pFileInfo->stEndTime.month,
                     &pFileInfo->stEndTime.day,
                     &pFileInfo->stEndTime.hour,
                     &pFileInfo->stEndTime.minute,
                     &pFileInfo->stEndTime.second,
                     pFileInfo->sFileName);
    Logi("pFileName:%s(%s)", pFileName, pFileInfo->sFileName);
    char *ext = strstr(pFileInfo->sFileName, ".h264.264");
    if (NULL != ext){
        //pFileInfo->sFileName[strlen(pFileInfo->sFileName)- strlen(".264")] = 0;
        pFileInfo->sFileName[ext - pFileInfo->sFileName + strlen(".h264")] = 0;
    }
    Logi("filename:%s", pFileInfo->sFileName);
    Logi("channel:%d, size:%d kb, starttime:%04d-%02d-%02dT%02d:%02d:%02dZ, stoptime:%04d-%02d-%02dT%02d:%02d:%02dZ", 
         channel, size, 
         pFileInfo->stBeginTime.year,
         pFileInfo->stBeginTime.month,
         pFileInfo->stBeginTime.day,
         pFileInfo->stBeginTime.hour,
         pFileInfo->stBeginTime.minute,
         pFileInfo->stBeginTime.second,
         pFileInfo->stEndTime.year,
         pFileInfo->stEndTime.month,
         pFileInfo->stEndTime.day,
         pFileInfo->stEndTime.hour,
         pFileInfo->stEndTime.minute,
         pFileInfo->stEndTime.second);
    pFileInfo->nChannel = channel-1;
    pFileInfo->nSize = size;
    // pFileInfo->nBeginTime;
    //pFileInfo->nEndTime;
    return 0;
}


static  int PlayBackCallBackV2(long lRealHandle, 
                               SDK_H264_FRAME_INFO *pFrameInfo,
                               unsigned long dwUser)
{
    Logi("handle:%d, dwUser:%p", (int)lRealHandle, (void*)dwUser);
    ByteStreamFileSource *pSource = (ByteStreamFileSource *)(dwUser);
    H264FrameBuffer *pBuffer = new H264FrameBuffer();
    pBuffer->pHeader = pFrameInfo->pHeader;
    pBuffer->pContent = pFrameInfo->pContent;
    pBuffer->nLength = pFrameInfo->nLength;
    pBuffer->nFrameLength = pFrameInfo->nFrameLength;
    pBuffer->FrameType = pFrameInfo->FrameType;
    pBuffer->nSubType = pFrameInfo->nSubType;
    pBuffer->nFrameRate = pFrameInfo->nFrameRate;
    pBuffer->nWidth = pFrameInfo->nWidth;
    pBuffer->nHeight = pFrameInfo->nHeight;
    pBuffer->nOffset = 0;
    pSource->addFrameBuffer(pBuffer);
    Logi("Leave");
    return 0;
}

static int EndCallBack(long lRealHandle, unsigned long dwUser)
{
    Logi("handle:%d,dwUser:%p", (int)lRealHandle, (void*)dwUser);
    ByteStreamFileSource *pSource = (ByteStreamFileSource*)(dwUser);
#ifdef USE_LOCALSDK    
    LOCALSDK_StopGetFile(lRealHandle);
#endif    
    pSource->setFileHandle(0);
    return 0;
}

static LOCALSDK_DEVICE_INFO_V2 theDeviceInfo;

static std::map<ByteStreamFileSource *, long> theHandleMaps;

ByteStreamFileSource*
ByteStreamFileSource::createNew(UsageEnvironment& env, char const* fileName,
				unsigned preferredFrameSize,
				unsigned playTimePerFrame) 
{
    // open the localinput file
    Logi("CreateNew fileName:%s", fileName);
    if (strstr(fileName, "localsdk") == fileName){
        int ret = 0;
        
        if (theHandleMaps.empty()){
#ifdef USE_LOCALSDK            
            Logi("Call LOCALSDK_StartUp to bringup the device");
            ret = LOCALSDK_StartUp();
            if (ret != 0){
                Loge("LOCALSDK_StartUp failed:%d", ret);
            }
            Logi("Call LOCALSDK_GetDevInfo");
            ret = LOCALSDK_GetDevInfo(&theDeviceInfo);
            if (ret != 0){
                Loge("LOCALSDK_GetDevInfo failed:%d", ret);
            }
            ret = LOCALSDK_PlayBackInit(2);
            if (ret != 0){
                Loge("LOCALSDK_PlayBackInit failed:%d",ret);
            }
#endif            
        }
        else{
            std::map<ByteStreamFileSource *, long>::iterator it = theHandleMaps.begin();
            while(it != theHandleMaps.end()){
                if ((it->first) != NULL && it->second != 0){
                    Logi("Stop the previous opend file:%d", it->second);
#ifdef USE_LOCALSDK
                    LOCALSDK_StopGetFile(it->second);
#endif    
                    (it->first)->setFileHandle(0);
                }
                it++;
            }
            theHandleMaps.clear();
        }
        
        // call localsdk api to get the filehandle
        LOCALSDK_FILE_DATA fileInfo;
        memset(&fileInfo, 0, sizeof(fileInfo));
        buildFileInfoByName(&fileInfo, fileName);
        ByteStreamFileSource* newSource
        = new ByteStreamFileSource(env, NULL, preferredFrameSize, playTimePerFrame);
        
        long handle = 0;
        int streamtype = 0;
        unsigned long dwUser = (unsigned long)newSource;
#ifdef USE_LOCALSDK
        Logi("Call LOCALSDK_GetFileByNameV2");
        LOCALSDK_FINDINFOV2 findInfo;
        LOCALSDK_FIND_FILE_RET findRet;
        LOCALSDK_FILE_DATA fileData[1];
        memset(&findRet, 0, sizeof(findRet));
        memset(fileData, 0, sizeof(fileData[0]) * 1);
        findRet.nMaxSize = 1;
        findRet.nRetSize = 0;
        findRet.pFileData = fileData;
        memset(&findInfo, 0, sizeof(findInfo));
        findInfo.nChannel = fileInfo.nChannel;
        // todo:fanhongxan@gmail.com
        // convert the type here.
        findInfo.nFileType = 0x0f;
        findInfo.nDriverTypes = 3; // record.
        findInfo.StreamType = SDK_CAPTURE_CHN_MAIN;
        findInfo.MaxFileNum = findRet.nMaxSize;
        findInfo.stBeginTime = fileInfo.stBeginTime;
        findInfo.stEndTime = fileInfo.stEndTime;
        ret = LOCALSDK_FindFileV2(&findInfo, &findRet, 30);
        if (ret != 0){
            Loge("LOCALSDK_FindFileV2 faild:%d", ret);
            return NULL;
        }
        Logi("LOCALSDK_FindFileV2: ret:%d", findRet.nRetSize);
        ret = LOCALSDK_GetFileByNameV2(&findRet.pFileData[0], 
                                       PlayBackCallBackV2, 
                                       dwUser, 	
                                       EndCallBack, 
                                       dwUser, 
                                       &handle,
                                       streamtype);
#else
        handle = 2;
#endif        
        if (ret != 0){
            Loge("LOCALSDK_GetFileByNameV2 failed(%d):(%s)", ret, fileName);
            return NULL;
        }
        Logi("LocalSDK_GetFileByNameV2 ok, handle:%d, source:%p", (int)handle, newSource);
        // newSource->fFileSize = GetFileSize(fileName, fid);
        newSource->setFileHandle(handle);
        newSource->fFileSize = fileInfo.nSize * 1024;
        theHandleMaps[newSource] = handle;
        return newSource;
    }
    FILE* fid = OpenInputFile(env, fileName);
    if (fid == NULL) return NULL;

    ByteStreamFileSource* newSource
    = new ByteStreamFileSource(env, fid, preferredFrameSize, playTimePerFrame);
    newSource->fFileSize = GetFileSize(fileName, fid);
    
    return newSource;
}

ByteStreamFileSource*
ByteStreamFileSource::createNew(UsageEnvironment& env, FILE* fid,
				unsigned preferredFrameSize,
				unsigned playTimePerFrame) {
  if (fid == NULL) return NULL;

  ByteStreamFileSource* newSource = new ByteStreamFileSource(env, fid, preferredFrameSize, playTimePerFrame);
  newSource->fFileSize = GetFileSize(NULL, fid);

  return newSource;
}

void ByteStreamFileSource::seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream) {
  SeekFile64(fFid, (int64_t)byteNumber, SEEK_SET);
    Logi("Enter:%d,%d", (int)byteNumber, (int)numBytesToStream);
  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

void ByteStreamFileSource::seekToByteRelative(int64_t offset, u_int64_t numBytesToStream) {
  SeekFile64(fFid, offset, SEEK_CUR);
    Logi("Enter:%d,%d", (int)offset, (int)numBytesToStream);
  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

void ByteStreamFileSource::seekToEnd() {
    Logi("Enter");
  SeekFile64(fFid, 0, SEEK_END);
}

ByteStreamFileSource::ByteStreamFileSource(UsageEnvironment& env, FILE* fid,
					   unsigned preferredFrameSize,
					   unsigned playTimePerFrame)
  : FramedFileSource(env, fid), fFileSize(0), fPreferredFrameSize(preferredFrameSize),
    fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0), mlFileHandle(0),
    fHaveStartedReading(False), fLimitNumBytesToStream(False), fNumBytesToStream(0) {
    
    pthread_cond_init(&mFrameBufferReadCond, NULL);
    pthread_mutex_init(&mFrameBufferReadMutex, NULL);
    pthread_cond_init(&mFrameBufferProcessCond, NULL);
    pthread_mutex_init(&mFrameBufferProcessMutex, NULL);
    
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  makeSocketNonBlocking(fileno(fFid));
#endif

  // Test whether the file is seekable
  fFidIsSeekable = FileIsSeekable(fFid);
}

ByteStreamFileSource::~ByteStreamFileSource() {
  if (fFid == NULL) return;
    Logi("Enter");
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  envir().taskScheduler().turnOffBackgroundReadHandling(fileno(fFid));
#endif
    if (0 != mlFileHandle){
        std::map<ByteStreamFileSource *, long>::iterator it = theHandleMaps.find(this);
        if (it != theHandleMaps.end()){
            theHandleMaps.erase(it);
        }
#ifdef USE_LOCALSDK
        LOCALSDK_StopGetFile(mlFileHandle);
#endif        
    }
  CloseInputFile(fFid);
}

void ByteStreamFileSource::doGetNextFrame() {
    Logi("Enter:%d", mlFileHandle);
    if (0 == mlFileHandle){
        if (feof(fFid) || ferror(fFid) || (fLimitNumBytesToStream && fNumBytesToStream == 0)) {
            handleClosure();
            return;
        }
    }
    
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
  doReadFromFile();
#else
  if (!fHaveStartedReading) {
    // Await readable data from the file:
    envir().taskScheduler().turnOnBackgroundReadHandling(fileno(fFid),
	       (TaskScheduler::BackgroundHandlerProc*)&fileReadableHandler, this);
    fHaveStartedReading = True;
  }
#endif
}

void ByteStreamFileSource::doStopGettingFrames() {
    Logi("Enter");
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  envir().taskScheduler().turnOffBackgroundReadHandling(fileno(fFid));
  fHaveStartedReading = False;
#endif
}

void ByteStreamFileSource::fileReadableHandler(ByteStreamFileSource* source, int /*mask*/) {
  if (!source->isCurrentlyAwaitingData()) {
    source->doStopGettingFrames(); // we're not ready for the data yet
    return;
  }
  source->doReadFromFile();
}

void ByteStreamFileSource::setFileHandle(long fileHandle)
{
    Logi("fileHandle:%d", (int)fileHandle);
    mlFileHandle = fileHandle;
}

void ByteStreamFileSource::addFrameBuffer(H264FrameBuffer *pBuffer)
{
    Logi("Enter");
    if (0 == mlFileHandle){
        Loge("Invalid fileHandle");
    }
    Logi("lock to push the frame");
    pthread_mutex_lock(&mFrameBufferReadMutex);
    mFrameBufferList.push_back(pBuffer);
    pthread_mutex_unlock(&mFrameBufferReadMutex);
    Logi("frame is ok, notify the process to take it");
    pthread_cond_signal(&mFrameBufferProcessCond);
    if (mFrameBufferList.size() > 2){
        // wait on the event
        Logi("localsdk wait for the next peek event");
        pthread_mutex_lock(&mFrameBufferReadMutex);
        pthread_cond_wait(&mFrameBufferReadCond, &mFrameBufferReadMutex);
        pthread_mutex_unlock(&mFrameBufferReadMutex);
        Logi("getting the next frame");
    }
}

void ByteStreamFileSource::doReadFromFile() {
    Logi("enter");
  // Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
    if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fMaxSize) {
        fMaxSize = (unsigned)fNumBytesToStream;
    }
    if (fPreferredFrameSize > 0 && fPreferredFrameSize < fMaxSize) {
        fMaxSize = fPreferredFrameSize;
    }
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
#ifdef USE_LOCALSDK
    while(1){
        if (mFrameBufferList.size() != 0){
            H264FrameBuffer *pBuffer = mFrameBufferList.front();
            fFrameSize = pBuffer->nLength -pBuffer->nOffset;
            if (fFrameSize > fMaxSize){
                fFrameSize = fMaxSize;
            }
            memcpy(fTo, pBuffer->pContent + pBuffer->nOffset, fFrameSize);
            pBuffer->nOffset += fFrameSize;
            if (pBuffer->nOffset >= pBuffer->nLength){
                pthread_mutex_lock(&mFrameBufferReadMutex);
                mFrameBufferList.pop_front();
                pthread_mutex_unlock(&mFrameBufferReadMutex);
                Logi("Get one frame, notify the callback to get next");
                pthread_cond_signal(&mFrameBufferReadCond);
                Logi("localsdk send the peek signal");
                delete pBuffer;
            }
            else{
                Logi("Has left some data for the next time process.");
            }
            break;
        }
        else{
            Logi("No frame valid, wait for the signal");
            pthread_mutex_lock(&mFrameBufferProcessMutex);
            pthread_cond_wait(&mFrameBufferProcessCond, &mFrameBufferProcessMutex);
            pthread_mutex_unlock(&mFrameBufferProcessMutex);
        }
    }
#else
    if (NULL != fFid){
        fFrameSize = fread(fTo, 1, fMaxSize, fFid);
    }
    else{
        Logi("fill a temp frame");
        fFrameSize = 10240;
    }
#endif    
#else
  if (fFidIsSeekable) {
    fFrameSize = fread(fTo, 1, fMaxSize, fFid);
  } else {
    // For non-seekable files (e.g., pipes), call "read()" rather than "fread()", to ensure that the read doesn't block:
    fFrameSize = read(fileno(fFid), fTo, fMaxSize);
  }
#endif
    Logi("process data");
  if (fFrameSize == 0) {
    handleClosure();
    return;
  }
  fNumBytesToStream -= fFrameSize;

  // Set the 'presentation time':
  if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0) {
    if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
      // This is the first frame, so use the current time:
      gettimeofday(&fPresentationTime, NULL);
    } else {
      // Increment by the play time of the previous data:
      unsigned uSeconds	= fPresentationTime.tv_usec + fLastPlayTime;
      fPresentationTime.tv_sec += uSeconds/1000000;
      fPresentationTime.tv_usec = uSeconds%1000000;
    }

    // Remember the play time of this data:
    fLastPlayTime = (fPlayTimePerFrame*fFrameSize)/fPreferredFrameSize;
    fDurationInMicroseconds = fLastPlayTime;
  } else {
    // We don't know a specific play time duration for this data,
    // so just record the current time as being the 'presentation time':
    gettimeofday(&fPresentationTime, NULL);
  }

  // Inform the reader that he has data:
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
  // To avoid possible infinite recursion, we need to return to the event loop to do this:
  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
				(TaskFunc*)FramedSource::afterGetting, this);
#else
  // Because the file read was done from the event loop, we can call the
  // 'after getting' function directly, without risk of infinite recursion:
  FramedSource::afterGetting(this);
#endif
}
