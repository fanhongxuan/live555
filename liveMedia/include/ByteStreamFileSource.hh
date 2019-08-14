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
// C++ header

#ifndef _BYTE_STREAM_FILE_SOURCE_HH
#define _BYTE_STREAM_FILE_SOURCE_HH

#ifndef _FRAMED_FILE_SOURCE_HH
#include "FramedFileSource.hh"
#endif

// add by fanhongxuan@gmail.com
#include <pthread.h>
#include <list>

void logBegin(const char *type, const char *file, const char *function, int line);
void logEnd();
// #define Logi(...) do{logBegin("info" , __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__); logEnd();}while(0)
#define Logi(...)
#define Loge(...) do{logBegin("error", __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__); logEnd();}while(0)

class H264FrameBuffer;

class ByteStreamFileSource: public FramedFileSource {
public:
  static ByteStreamFileSource* createNew(UsageEnvironment& env,
					 char const* fileName,
					 unsigned preferredFrameSize = 0,
					 unsigned playTimePerFrame = 0);
  // "preferredFrameSize" == 0 means 'no preference'
  // "playTimePerFrame" is in microseconds

  static ByteStreamFileSource* createNew(UsageEnvironment& env,
					 FILE* fid,
					 unsigned preferredFrameSize = 0,
					 unsigned playTimePerFrame = 0);
      // an alternative version of "createNew()" that's used if you already have
      // an open file.

  u_int64_t fileSize() const { return fFileSize; }
      // 0 means zero-length, unbounded, or unknown

  void seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream = 0);
    // if "numBytesToStream" is >0, then we limit the stream to that number of bytes, before treating it as EOF
  void seekToByteRelative(int64_t offset, u_int64_t numBytesToStream = 0);
  void seekToEnd(); // to force EOF handling on the next read
  void seekToRange(double &range);
  // add by fanhongxuan@gmail.com
  void setFileHandle(long fileHandle);
  void setFileName(const char *pFileName);
  void addFrameBuffer(H264FrameBuffer *pBuffer);
protected:
  ByteStreamFileSource(UsageEnvironment& env,
		       FILE* fid,
		       unsigned preferredFrameSize,
		       unsigned playTimePerFrame);
	// called only by createNew()

  virtual ~ByteStreamFileSource();

  static void fileReadableHandler(ByteStreamFileSource* source, int mask);
  void doReadFromFile();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

protected:
  u_int64_t fFileSize;

private:
  // add by fanhongxuan@gmail.com
  long mlFileHandle;
  std::list<H264FrameBuffer *> mFrameBufferList;
  pthread_mutex_t mFrameBufferReadMutex;
  pthread_cond_t  mFrameBufferReadCond;
  pthread_mutex_t mFrameBufferProcessMutex;
  pthread_cond_t mFrameBufferProcessCond;
  char *mpFileName;
  bool mbSeeked;

  unsigned fPreferredFrameSize;
  unsigned fPlayTimePerFrame;
  Boolean fFidIsSeekable;
  unsigned fLastPlayTime;
  Boolean fHaveStartedReading;
  Boolean fLimitNumBytesToStream;
  u_int64_t fNumBytesToStream; // used iff "fLimitNumBytesToStream" is True
};

#endif
