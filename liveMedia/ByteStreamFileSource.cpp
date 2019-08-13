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
    H264FrameBuffer(){
        pContent = NULL;
    }
    ~H264FrameBuffer(){
        if (NULL != pContent){
            delete []pContent;
        }
    }
    unsigned char *pContent;
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
    int channel = 0, size = 0;
    sscanf(pFileName, "localsdk/%d/%d/%04d-%02d-%02d-%02d-%02d-%02d/%04d-%02d-%02d-%02d-%02d-%02d/%s", 
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

static void ConvertLocalSDKTime2TM(LOCALSDK_TIME *pSrc, struct tm *pDst)
{
    if (NULL == pSrc || NULL == pDst){
        return;
    }
    memset(pDst, 0, sizeof(struct tm));
    pDst->tm_year = pSrc->year - 1900;
    pDst->tm_mon = pSrc->month - 1;
    pDst->tm_mday = pSrc->day - 1;
    pDst->tm_hour = pSrc->hour;
    pDst->tm_min = pSrc->minute;
    pDst->tm_sec = pSrc->second;
}

double GetFileDuration(const char *filename){
    double ret = .0;
    if (NULL == filename){
        return ret;
    }

    LOCALSDK_FILE_DATA fileInfo;
    memset(&fileInfo, 0, sizeof(fileInfo));
    buildFileInfoByName(&fileInfo, filename);
    struct tm start, end;
    ConvertLocalSDKTime2TM(&fileInfo.stBeginTime, &start);
    ConvertLocalSDKTime2TM(&fileInfo.stEndTime, &end);
    
    time_t tStart = mktime(&start);
    time_t tEnd = mktime(&end);
    ret = (double)(tEnd - tStart);
    Logi("ret:%lf, timeRange:%d<-->%d, filename:%s", ret, (int)tStart, (int)tEnd, filename);
    return ret;
}

static  int PlayBackCallBackV2(long lRealHandle, 
                               SDK_H264_FRAME_INFO *pFrameInfo,
                               unsigned long dwUser)
{
    Logi("handle:%d, dwUser:%p", (int)lRealHandle, (void*)dwUser);
    ByteStreamFileSource *pSource = (ByteStreamFileSource *)(dwUser);
    H264FrameBuffer *pBuffer = new H264FrameBuffer();
    pBuffer->pContent = new unsigned char[pFrameInfo->nFrameLength];
    memcpy(pBuffer->pContent, pFrameInfo->pContent, pFrameInfo->nFrameLength);
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

ByteStreamFileSource*
ByteStreamFileSource::createNew(UsageEnvironment& env, char const* fileName,
				unsigned preferredFrameSize,
				unsigned playTimePerFrame) 
{
    // open the localinput file
    Logi("CreateNew fileName:%s", fileName);
    if (strstr(fileName, "localsdk") == fileName){
        int ret = 0;
#ifdef USE_LOCALSDK
        static int gbInit = 0;
        if (gbInit == 0){
            LOCALSDK_DEVICE_INFO_V2 theDeviceInfo;
            Logi("Call LOCALSDK_StartUp to bringup the device");
            ret = LOCALSDK_StartUp();
            if (ret != 0){
                Loge("LOCALSDK_StartUp failed:%d", ret);
                return NULL;
            }
            Logi("Call LOCALSDK_GetDevInfo");
            ret = LOCALSDK_GetDevInfo(&theDeviceInfo);
            if (ret != 0){
                Loge("LOCALSDK_GetDevInfo failed:%d", ret);
                return NULL;
            }
            ret = LOCALSDK_PlayBackInit(4);
            if (ret != 0){
                Loge("LOCALSDK_PlayBackInit failed:%d",ret);
                return NULL;
            }
            Logi("LOCALSDK_PlaybackInit ok");
            gbInit = 1;
        }
#endif
        // call localsdk api to get the filehandle
        LOCALSDK_FILE_DATA fileInfo;
        memset(&fileInfo, 0, sizeof(fileInfo));
        buildFileInfoByName(&fileInfo, fileName);
        ByteStreamFileSource* newSource = new ByteStreamFileSource(env, NULL, preferredFrameSize, playTimePerFrame);
        long handle = 0;
#ifdef USE_LOCALSDK
        int streamtype = 0;
        unsigned long dwUser = (unsigned long)newSource;
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
            delete newSource;
            return NULL;
        }
        Logi("LOCALSDK_FindFileV2 nRetSize:%d, call LOCALSDK_GetFileByNameV2", findRet.nRetSize);
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
            delete newSource;
            return NULL;
        }
        Logi("LocalSDK_GetFileByNameV2 ok, handle:%d, source:%p", (int)handle, newSource);
        newSource->setFileName(fileName);
        newSource->setFileHandle(handle);
        newSource->fFileSize = fileInfo.nSize * 1024;
        return newSource;
    }
    FILE* fid = OpenInputFile(env, fileName);
    if (fid == NULL) return NULL;

    ByteStreamFileSource* newSource
    = new ByteStreamFileSource(env, fid, preferredFrameSize, playTimePerFrame);
    newSource->fFileSize = GetFileSize(fileName, fid);
    Logi("*************** source:%p", newSource);
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

void ByteStreamFileSource::seekToRange(double &rangeStart)
{
    Logi("seekToRangeStart:%lf", rangeStart);
    if (NULL == mpFileName){
        Logi("No filename ignore the seek request");
        return;
    }
    if (rangeStart <= 0.01){
        Logi("Invalid rangeStart:%lf", rangeStart);
        return;
    }
    // build the file info from the filename;
    LOCALSDK_FILE_DATA fileInfo;
    memset(&fileInfo, 0, sizeof(fileInfo));
    buildFileInfoByName(&fileInfo, mpFileName);
    struct tm begin, end;
    ConvertLocalSDKTime2TM(&fileInfo.stBeginTime, &begin);
    ConvertLocalSDKTime2TM(&fileInfo.stEndTime, &end);
    
    time_t tEnd = mktime(&end);
    time_t start = mktime(&begin);
    start += (int)rangeStart;
    if (start >= tEnd){
        Logi("Invalid target:%d, end(%d)", (int)start, (int)tEnd);
        rangeStart = .0;
        return;
    }
    gmtime_r(&start, &begin);
    fileInfo.stBeginTime.year = begin.tm_year + 1900;
    fileInfo.stBeginTime.month = begin.tm_mon + 1;
    fileInfo.stBeginTime.day = begin.tm_mday + 1;
    fileInfo.stBeginTime.hour = begin.tm_hour;
    fileInfo.stBeginTime.minute = begin.tm_min;
    fileInfo.stBeginTime.second = begin.tm_sec;
    Logi("LOCALSDK_SetSeekPlayBack:%04d-%02d-%02d:%02d:%02d:%02d", 
         fileInfo.stBeginTime.year,
         fileInfo.stBeginTime.month,
         fileInfo.stBeginTime.day,
         fileInfo.stBeginTime.hour,
         fileInfo.stBeginTime.minute,
         fileInfo.stBeginTime.second);
#ifdef USE_LOCALSDK
    int ret = LOCALSDK_SetSeekPlayBack(&fileInfo);
    if (ret != 0){
        Loge("LOCALSDK_SetSeekPlayBack failed:%d", ret);
        return;
    }
    Logi("LoCALSDK_SetSeekPlayBack ok");
#endif    
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
    fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0),
    fHaveStartedReading(False), fLimitNumBytesToStream(False), fNumBytesToStream(0) {
    
    mpFileName = NULL;
    mlFileHandle = 0;
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
    Logi("Enter");
    if (0 != mlFileHandle){
        long handle = mlFileHandle;
        // set the mlFileHandle to 0 to make sure the callback will not waiting on the signal anymore.
        mlFileHandle = 0; 
        Logi("Notify the callback return if they are waiting");
        pthread_cond_signal(&mFrameBufferReadCond);
        int ret = 0;
#ifdef USE_LOCALSDK
        ret = LOCALSDK_StopGetFile(handle);
#endif
        Logi("LOCALSDK_StopGetFile ret:%d", ret);
    }
    if (NULL != mpFileName){
        free(mpFileName);
        mpFileName = NULL;
    }
    if (fFid == NULL) return;
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  envir().taskScheduler().turnOffBackgroundReadHandling(fileno(fFid));
#endif
  CloseInputFile(fFid);
}

void ByteStreamFileSource::doGetNextFrame() {
    // Logi("Enter:%d", mlFileHandle);
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
    if (0 != mlFileHandle){
        long handle = mlFileHandle;
        // set the mlFileHandle to 0 to make sure the callback will not waiting on the signal anymore.
        mlFileHandle = 0; 
        Logi("Notify the callback return if they are waiting");
        pthread_cond_signal(&mFrameBufferReadCond);
        int ret = 0;
#ifdef USE_LOCALSDK
        ret = LOCALSDK_StopGetFile(handle);
#endif
        Logi("LOCALSDK_StopGetFile ret:%d", ret);
    }
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

void ByteStreamFileSource::setFileName(const char *pFileName)
{
    if (NULL != mpFileName){
        free(mpFileName);
        mpFileName = NULL;
    }
    if (pFileName != NULL){
        mpFileName = strdup(pFileName);
    }
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
        return;
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
    // Logi("enter");
    // Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
    if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fMaxSize) {
        fMaxSize = (unsigned)fNumBytesToStream;
    }
    if (fPreferredFrameSize > 0 && fPreferredFrameSize < fMaxSize) {
        fMaxSize = fPreferredFrameSize;
    }
    if (fMaxSize >= 10240){
        fMaxSize = 10240;
    }
#ifdef READ_FROM_FILES_SYNCHRONOUSLY
#ifdef USE_LOCALSDK
    while(1){
        if (mFrameBufferList.size() != 0){
            H264FrameBuffer *pBuffer = mFrameBufferList.front();
            fFrameSize = pBuffer->nFrameLength -pBuffer->nOffset;
            if (fFrameSize > fMaxSize){
                fFrameSize = fMaxSize;
            }
            Logi("Copy content to fTo, frameSize:%d, offset:%d, frameLength:%d", 
                 fFrameSize, 
                 pBuffer->nOffset,
                 pBuffer->nFrameLength);
            memcpy(fTo, pBuffer->pContent + pBuffer->nOffset, fFrameSize);
            Logi("After memcpy");
            pBuffer->nOffset += fFrameSize;
            if (pBuffer->nOffset >= pBuffer->nFrameLength){
                Logi("Before lock");
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
        Logi("fill a temp frame:%d", fFrameSize);
        fFrameSize = fMaxSize;
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
    // Logi("process data");
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
