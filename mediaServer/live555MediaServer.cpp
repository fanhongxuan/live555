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
// Copyright (c) 1996-2019, Live Networks, Inc.  All rights reserved
// LIVE555 Media Server
// main program

#include <BasicUsageEnvironment.hh>
#include "DynamicRTSPServer.hh"
#include "version.hh"
#ifdef DUMP_STACK
// add by fanhongxuan@gmail.com for debug the segment fault:
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>

/* Obtain a backtrace and print it to stdout. */
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
void dump_stack(void)
{
	void *array[30] = { 0 };
	size_t size = backtrace(array, ARRAY_SIZE(array));
	char **strings = backtrace_symbols(array, size);
	size_t i;

	if (strings == NULL)
	{
		perror("backtrace_symbols.");
		exit(EXIT_FAILURE);
	}

	printf("Obtained %zd stack frames.\n", size);

	for (i = 0; i < size; i++)
		printf("%s\n", strings[i]);

	free(strings);
	strings = NULL;

	exit(EXIT_SUCCESS);
}

void sighandler_dump_stack(int sig)
{
	psignal(sig, "handler");
	dump_stack();
	signal(sig, SIG_DFL);
	raise(sig);
}
#endif 
//

extern Boolean g_support_vlc;
int main(int argc, char** argv) {
#ifdef DUMP_STACK    
    if (signal(SIGSEGV, sighandler_dump_stack) == SIG_ERR)
		perror("can't catch SIGSEGV");
#endif    
  if (argc == 2 && strcmp(argv[1], "vlc") == 0){
    g_support_vlc = True;
  }
  
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
  // changed by fanhongxuan@gmail.com
  // first try 8554, then 18854
  RTSPServer* rtspServer;
  portNumBits rtspServerPortNum = 8554;
  rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  if (rtspServer == NULL) {
    rtspServerPortNum = 18554;
    rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  }
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "LIVE555 Media Server\n";
  *env << "\tversion " << MEDIA_SERVER_VERSION_STRING
       << " (LIVE555 Streaming Media library version "
       << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

  char* urlPrefix = rtspServer->rtspURLPrefix();
  *env << "Play streams from this server using the URL\n\t"
       << urlPrefix << "<filename>\nwhere <filename> is a file present in the current directory.\n";
  *env << "Each file's type is inferred from its name suffix:\n";
  *env << "\t\".264\" => a H.264 Video Elementary Stream file\n";
  *env << "\t\".265\" => a H.265 Video Elementary Stream file\n";
  *env << "\t\".aac\" => an AAC Audio (ADTS format) file\n";
  *env << "\t\".ac3\" => an AC-3 Audio file\n";
  *env << "\t\".amr\" => an AMR Audio file\n";
  *env << "\t\".dv\" => a DV Video file\n";
  *env << "\t\".m4e\" => a MPEG-4 Video Elementary Stream file\n";
  *env << "\t\".mkv\" => a Matroska audio+video+(optional)subtitles file\n";
  *env << "\t\".mp3\" => a MPEG-1 or 2 Audio file\n";
  *env << "\t\".mpg\" => a MPEG-1 or 2 Program Stream (audio+video) file\n";
  *env << "\t\".ogg\" or \".ogv\" or \".opus\" => an Ogg audio and/or video file\n";
  *env << "\t\".ts\" => a MPEG Transport Stream file\n";
  *env << "\t\t(a \".tsx\" index file - if present - provides server 'trick play' support)\n";
  *env << "\t\".vob\" => a VOB (MPEG-2 video with AC-3 audio) file\n";
  *env << "\t\".wav\" => a WAV Audio file\n";
  *env << "\t\".webm\" => a WebM audio(Vorbis)+video(VP8) file\n";
  *env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  // note:fanhongxuan@gmail.com
  // first try 10080, then 18000, then 18080

  if (rtspServer->setUpTunnelingOverHTTP(10080) || rtspServer->setUpTunnelingOverHTTP(18000) || rtspServer->setUpTunnelingOverHTTP(18080)) {
    *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
  } else {
    *env << "(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

  return 0; // only to prevent compiler warning
}
