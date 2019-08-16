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
// RTP sink for H.264 or H.265 video
// Implementation

#include "H264or5VideoRTPSink.hh"
#include "H264or5VideoStreamFramer.hh"
#define PACK_NALU_IN_ONE_PS_PACKAGE
#ifdef PACK_NALU_IN_ONE_PS_PACKAGE
// add by fanhongxuan@gmail.com
#include "litets.h"
#define BUF_SIZE (1<<20)
#endif

////////// H264or5Fragmenter definition //////////

// Because of the ideosyncracies of the H.264 RTP payload format, we implement
// "H264or5VideoRTPSink" using a separate "H264or5Fragmenter" class that delivers,
// to the "H264or5VideoRTPSink", only fragments that will fit within an outgoing
// RTP packet.  I.e., we implement fragmentation in this separate "H264or5Fragmenter"
// class, rather than in "H264or5VideoRTPSink".
// (Note: This class should be used only by "H264or5VideoRTPSink", or a subclass.)

class H264or5Fragmenter: public FramedFilter {
public:
  H264or5Fragmenter(int hNumber, UsageEnvironment& env, FramedSource* inputSource,
		    unsigned inputBufferMax, unsigned maxOutputPacketSize);
  virtual ~H264or5Fragmenter();

  Boolean lastFragmentCompletedNALUnit() const { return fLastFragmentCompletedNALUnit; }

private:
#ifdef PACK_NALU_IN_ONE_PS_PACKAGE
    TsProgramInfo g_prog_info;
    int g_frame_count;
    uint8_t *g_outbuf;
    int convert_nalu_to_ps(uint8_t *pData, size_t data_len, char is_key);
    int init_ps();
#endif

private: // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

private:
  static void afterGettingFrame(void* clientData, unsigned frameSize,
				unsigned numTruncatedBytes,
                                struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame1(unsigned frameSize,
                          unsigned numTruncatedBytes,
                          struct timeval presentationTime,
                          unsigned durationInMicroseconds);
  void reset();

private:
  int fHNumber;
  unsigned fInputBufferSize;
  unsigned fMaxOutputPacketSize;
  unsigned char* fInputBuffer;
  unsigned fNumValidDataBytes;
  unsigned fCurDataOffset;
  unsigned fSaveNumTruncatedBytes;
  Boolean fLastFragmentCompletedNALUnit;
};


////////// H264or5VideoRTPSink implementation //////////

H264or5VideoRTPSink
::H264or5VideoRTPSink(int hNumber,
		      UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		      u_int8_t const* vps, unsigned vpsSize,
		      u_int8_t const* sps, unsigned spsSize,
		      u_int8_t const* pps, unsigned ppsSize)
  : VideoRTPSink(env, RTPgs, rtpPayloadFormat, 90000, hNumber == 264 ? "H264" : "H265"),
    fHNumber(hNumber), fOurFragmenter(NULL), fFmtpSDPLine(NULL) {

    if (vps != NULL) {
    fVPSSize = vpsSize;
    fVPS = new u_int8_t[fVPSSize];
    memmove(fVPS, vps, fVPSSize);
  } else {
    fVPSSize = 0;
    fVPS = NULL;
  }
  if (sps != NULL) {
    fSPSSize = spsSize;
    fSPS = new u_int8_t[fSPSSize];
    memmove(fSPS, sps, fSPSSize);
  } else {
    fSPSSize = 0;
    fSPS = NULL;
  }
  if (pps != NULL) {
    fPPSSize = ppsSize;
    fPPS = new u_int8_t[fPPSSize];
    memmove(fPPS, pps, fPPSSize);
  } else {
    fPPSSize = 0;
    fPPS = NULL;
  }
}

H264or5VideoRTPSink::~H264or5VideoRTPSink() {
  fSource = fOurFragmenter; // hack: in case "fSource" had gotten set to NULL before we were called
  delete[] fFmtpSDPLine;
  delete[] fVPS; delete[] fSPS; delete[] fPPS;
  stopPlaying(); // call this now, because we won't have our 'fragmenter' when the base class destructor calls it later.

  // Close our 'fragmenter' as well:
  Medium::close(fOurFragmenter);
  fSource = NULL; // for the base class destructor, which gets called next
}

Boolean H264or5VideoRTPSink::continuePlaying() {
  // First, check whether we have a 'fragmenter' class set up yet.
  // If not, create it now:
  if (fOurFragmenter == NULL) {
    fOurFragmenter = new H264or5Fragmenter(fHNumber, envir(), fSource, OutPacketBuffer::maxSize,
					   ourMaxPacketSize() - 12/*RTP hdr size*/);
  } else {
    fOurFragmenter->reassignInputSource(fSource);
  }
  fSource = fOurFragmenter;

  // Then call the parent class's implementation:
  return MultiFramedRTPSink::continuePlaying();
}

void H264or5VideoRTPSink::doSpecialFrameHandling(unsigned /*fragmentationOffset*/,
						 unsigned char* /*frameStart*/,
						 unsigned /*numBytesInFrame*/,
						 struct timeval framePresentationTime,
						 unsigned /*numRemainingBytes*/) {
  // Set the RTP 'M' (marker) bit iff
  // 1/ The most recently delivered fragment was the end of (or the only fragment of) an NAL unit, and
  // 2/ This NAL unit was the last NAL unit of an 'access unit' (i.e. video frame).
  if (fOurFragmenter != NULL) {
    H264or5VideoStreamFramer* framerSource
      = (H264or5VideoStreamFramer*)(fOurFragmenter->inputSource());
    // This relies on our fragmenter's source being a "H264or5VideoStreamFramer".
    if (((H264or5Fragmenter*)fOurFragmenter)->lastFragmentCompletedNALUnit()) {
	  // fprintf(stderr, "(%d) setMarkerBit\n", g_frame_count);
      // modified by fanhongxuan@gmail.com
	  setMarkerBit();
      if (framerSource != NULL && framerSource->pictureEndMarker()){
        framerSource->pictureEndMarker() = False;
      }
    }
  }

  setTimestamp(framePresentationTime);
}

Boolean H264or5VideoRTPSink
::frameCanAppearAfterPacketStart(unsigned char const* /*frameStart*/,
				 unsigned /*numBytesInFrame*/) const {
  return False;
}


////////// H264or5Fragmenter implementation //////////

H264or5Fragmenter::H264or5Fragmenter(int hNumber,
				     UsageEnvironment& env, FramedSource* inputSource,
				     unsigned inputBufferMax, unsigned maxOutputPacketSize)
  : FramedFilter(env, inputSource),
    fHNumber(hNumber),
    fInputBufferSize(inputBufferMax+1), fMaxOutputPacketSize(maxOutputPacketSize) {
  fInputBuffer = new unsigned char[fInputBufferSize];
  reset();
#ifdef  PACK_NALU_IN_ONE_PS_PACKAGE
    g_outbuf = NULL;
    g_frame_count = 0;
#endif
}

H264or5Fragmenter::~H264or5Fragmenter() {
#ifdef PACK_NALU_IN_ONE_PS_PACKAGE
    if (NULL != g_outbuf){
        delete []g_outbuf;
    }
#endif
    delete[] fInputBuffer;
  detachInputSource(); // so that the subsequent ~FramedFilter() doesn't delete it
}

// #define PACK_NALU_IN_ONE_PS_PACKAGE
#ifdef PACK_NALU_IN_ONE_PS_PACKAGE
// add by fanhongxuan@gmail.com
// #include "litets.h"
// #define BUF_SIZE (1<<20)


// convert the nalu to ps.
int H264or5Fragmenter::init_ps()
{
    if (NULL == g_outbuf){
        g_outbuf = new uint8_t[BUF_SIZE];
    }
    memset(&g_prog_info, 0, sizeof(g_prog_info));                                                                         
    g_prog_info.program_num = 1;                                                                                          
    g_prog_info.prog[0].stream_num = 1;                                                                                   
    g_prog_info.prog[0].stream[0].type = STREAM_TYPE_VIDEO_H264; 
    return 0;
}

int H264or5Fragmenter::convert_nalu_to_ps(uint8_t *pData, size_t data_len, char is_key)
{
   init_ps();
   TEsFrame es={0};
   pData[0] = 0x00;
   pData[1] = 0x00;
   pData[2] = 0x00;
   pData[3] = 0x01;
   es.program_number = 0;
   es.stream_number = 0;
   es.frame = pData;
   es.length = data_len;
   es.is_key = is_key;
   struct timeval tv;
   gettimeofday(&tv,NULL);
   u_int32_t timestampIncrement = (90000*tv.tv_sec);
   timestampIncrement += (u_int32_t)(90000*(tv.tv_usec/1000000.0) + 0.5); // note: rounding
   es.pts = timestampIncrement;
   es.ps_pes_length = 8000;
   int outlen = 0;
   outlen = lts_ps_stream(&es, g_outbuf, BUF_SIZE, &g_prog_info);
   g_frame_count++;
   return outlen;
}
#endif

// #define PACK_NALU_BY_NANRI
#ifdef PACK_NALU_BY_NANRI
// add by fanhongxuan@gmail.com
#include "streamdef_le.h"

#define PS_HDR_LEN 14   //psÍ·³¤¶È
#define SYS_HDR_LEN 18  //sysÍ·³¤¶È PS system header:18×Ö½Ú
#define PSM_HDR_LEN 24  
#define PES_HDR_LEN 19  

#define RTP_MAX_PACKET_BUFF 1460
#define PS_PES_PAYLOAD_SIZE 1300
#define PAYLOAD_TYPE 96  //H.264

typedef struct bitsBuffer
{
	int i_size;  
	int i_data;  
	int i_mask;  
	unsigned char *p_data;  
}bits_buffer_s;

//½«´«ÈëµÄÊý¾Ý°´µØÎ»Ò»¸öÒ»¸öµÄÑ¹ÈëÊý¾Ý
#define bits_write(buffer, count, bits)\
{\
	bits_buffer_s *p_buffer = (buffer);\
	int i_count = (count);\
	uint64_t i_bits = (bits);\
	while( i_count > 0 )\
{\
	i_count--;\
	if( ( i_bits >> i_count )&0x01 )\
{\
	p_buffer->p_data[p_buffer->i_data] |= p_buffer->i_mask;\
}\
		else\
{\
	p_buffer->p_data[p_buffer->i_data] &= ~p_buffer->i_mask;\
}\
	p_buffer->i_mask >>= 1;         /*²Ù×÷ÍêÒ»¸ö×Ö½ÚµÚÒ»Î»ºó£¬²Ù×÷µÚ¶þÎ»*/\
	if( p_buffer->i_mask == 0 )     /*Ñ­»·ÍêÒ»¸ö×Ö½ÚµÄ8Î»ºó£¬ÖØÐÂ¿ªÊ¼ÏÂÒ»Î»*/\
{\
	p_buffer->i_data++;\
	p_buffer->i_mask = 0x80;\
}\
}\
}


#include <vector>
class ps_package{
public:
    unsigned char *mpData;
    unsigned int mLen;
    ps_package(unsigned char *pData, unsigned int len){
        mpData = new unsigned char [len];
        memcpy(mpData, pData, len);
        mLen = len;
    }
    ~ps_package(){
        if (NULL != mpData){
            delete [] mpData;
            mpData = NULL;
            mLen = 0;
        }
    }
};


static unsigned int thePackageOffset;
static std::vector<ps_package *> thePackages;
static int rtp_session_send_with_ts(unsigned char *pBuff, unsigned int buff_len)
{
    ps_package *pPackage = new ps_package(pBuff, buff_len);
    thePackages.push_back(pPackage);
    return buff_len;
}

//psÍ·µÄ·â×°,ÀïÃæµÄ¾ßÌåÊý¾ÝµÄÌîÐ´ÒÑ¾­Õ¼Î»£¬¿ÉÒÔ²Î¿¼±ê×¼
static int make_ps_header(unsigned char *p_data, unsigned long long pts)         
{  
	ps_pack_header *hdr = (ps_pack_header *)p_data;
	char *scr_buf = (char *)hdr->scr;

	hdr->start_code[0] = 0;
	hdr->start_code[1] = 0;
	hdr->start_code[2] = 1;
	hdr->start_code[3] = 0xBA;
	hdr->program_mux_rate[0] = 0;
	hdr->program_mux_rate[1] = 0;
	hdr->program_mux_rate[2] = 0x03;
	hdr->pack_stuffing_length = 0xF8;
	
	scr_buf[0] = 0x40 | (((uint8_t)(pts >> 30) & 0x07) << 3) | 0x04 | ((uint8_t)(pts >> 28) & 0x03);
	scr_buf[1] = (uint8_t)((pts >> 20) & 0xff);
	scr_buf[2] = (((uint8_t)(pts >> 15) & 0x1f) << 3) | 0x04 | ((uint8_t)(pts >> 13) & 0x03);
	scr_buf[3] = (uint8_t)((pts >> 5) & 0xff);
	scr_buf[4] = (((uint8_t)pts & 0x1f) << 3) | 0x04;
	scr_buf[5] = 1;

	return 0;  
}  

// sysÍ·µÄ·â×°,ÀïÃæµÄ¾ßÌåÊý¾ÝµÄÌîÐ´ÒÑ¾­Õ¼Î»£¬¿ÉÒÔ²Î¿¼±ê×¼ 
static int make_sys_header(unsigned char *p_data)  
{  
	bits_buffer_s   bitsBuffer;
	if (p_data == NULL)  
		return -1;
	bitsBuffer.i_size = SYS_HDR_LEN;  
	bitsBuffer.i_data = 0;  
	bitsBuffer.i_mask = 0x80;  
	bitsBuffer.p_data = (unsigned char *)(p_data);  
	memset(bitsBuffer.p_data, 0, SYS_HDR_LEN);  
	/*system header*/  
	bits_write( &bitsBuffer, 32, 0x000001BB);   /*start code*/  
	bits_write( &bitsBuffer, 16, SYS_HDR_LEN-6);/*header_length ±íÊ¾´Î×Ö½ÚºóÃæµÄ³¤¶È£¬ºóÃæµÄÏà¹ØÍ·Ò²ÊÇ´ÎÒâË¼*/ 
	bits_write( &bitsBuffer, 1,  1);            /*marker_bit*/  
	bits_write( &bitsBuffer, 22, 50000);        /*rate_bound*/  
	bits_write( &bitsBuffer, 1,  1);            /*marker_bit*/  
	bits_write( &bitsBuffer, 6,  1);            /*audio_bound*/  
	bits_write( &bitsBuffer, 1,  0);            /*fixed_flag */  
	bits_write( &bitsBuffer, 1,  1);            /*CSPS_flag */  
	bits_write( &bitsBuffer, 1,  1);            /*system_audio_lock_flag*/  
	bits_write( &bitsBuffer, 1,  1);            /*system_video_lock_flag*/  
	bits_write( &bitsBuffer, 1,  1);            /*marker_bit*/  
	bits_write( &bitsBuffer, 5,  1);            /*video_bound*/  
	bits_write( &bitsBuffer, 1,  0);            /*dif from mpeg1*/  
	bits_write( &bitsBuffer, 7,  0x7F);         /*reserver*/  
	/*audio stream bound*/  
	bits_write( &bitsBuffer, 8,  0xC0);         /*stream_id*/  
	bits_write( &bitsBuffer, 2,  3);            /*marker_bit */  
	bits_write( &bitsBuffer, 1,  0);            /*PSTD_buffer_bound_scale*/  
	bits_write( &bitsBuffer, 13, 512);          /*PSTD_buffer_size_bound*/  
	/*video stream bound*/  
	bits_write( &bitsBuffer, 8,  0xE0);         /*stream_id*/  
	bits_write( &bitsBuffer, 2,  3);            /*marker_bit */  
	bits_write( &bitsBuffer, 1,  1);            /*PSTD_buffer_bound_scale*/  
	bits_write( &bitsBuffer, 13, 2048);         /*PSTD_buffer_size_bound*/  
	return 0;  
}  


//psmÍ·µÄ·â×°,ÀïÃæµÄ¾ßÌåÊý¾ÝµÄÌîÐ´ÒÑ¾­Õ¼Î»£¬¿ÉÒÔ²Î¿¼±ê×¼
static int make_psm_header(unsigned char *p_data)  
{  
	bits_buffer_s   bitsBuffer;
	if (p_data == NULL)  
		return -1;
	bitsBuffer.i_size = PSM_HDR_LEN;   
	bitsBuffer.i_data = 0;  
	bitsBuffer.i_mask = 0x80;  
	bitsBuffer.p_data = (unsigned char *)(p_data);  
	memset(bitsBuffer.p_data, 0, PSM_HDR_LEN);  
	bits_write(&bitsBuffer, 24,0x000001);   /*start code*/  
	bits_write(&bitsBuffer, 8, 0xBC);       /*map stream id*/  
	bits_write(&bitsBuffer, 16,18);         /*program stream map length*/   
	bits_write(&bitsBuffer, 1, 1);          /*current next indicator */  
	bits_write(&bitsBuffer, 2, 3);          /*reserved*/  
	bits_write(&bitsBuffer, 5, 0);          /*program stream map version*/  
	bits_write(&bitsBuffer, 7, 0x7F);       /*reserved */  
	bits_write(&bitsBuffer, 1, 1);          /*marker bit */  
	bits_write(&bitsBuffer, 16,0);          /*programe stream info length*/  
	bits_write(&bitsBuffer, 16, 8);         /*elementary stream map length  is*/  
	/*audio*/  
	bits_write(&bitsBuffer, 8, 0x90);       /*stream_type*/  
	bits_write(&bitsBuffer, 8, 0xC0);       /*elementary_stream_id*/  
	bits_write(&bitsBuffer, 16, 0);         /*elementary_stream_info_length is*/  
	/*video*/  
	bits_write(&bitsBuffer, 8, 0x1B);       /*stream_type*/  
	bits_write(&bitsBuffer, 8, 0xE0);       /*elementary_stream_id*/  
	bits_write(&bitsBuffer, 16, 0);         /*elementary_stream_info_length */  
	/*crc (2e b9 0f 3d)*/  
	bits_write(&bitsBuffer, 8, 0x45);       /*crc (24~31) bits*/  
	bits_write(&bitsBuffer, 8, 0xBD);       /*crc (16~23) bits*/  
	bits_write(&bitsBuffer, 8, 0xDC);       /*crc (8~15) bits*/  
	bits_write(&bitsBuffer, 8, 0xF4);       /*crc (0~7) bits*/  
	return 0;  
}  

//pesÍ·µÄ·â×°,ÀïÃæµÄ¾ßÌåÊý¾ÝµÄÌîÐ´ÒÑ¾­Õ¼Î»£¬¿ÉÒÔ²Î¿¼±ê×¼
int make_pes_header(unsigned char *p_data, int stream_id, int payload_len, unsigned long long pts, unsigned long long dts)  
{
	if (p_data == NULL)  
		return -1;
	bits_buffer_s   bitsBuffer;  
	bitsBuffer.i_size = PES_HDR_LEN;  
	bitsBuffer.i_data = 0;  
	bitsBuffer.i_mask = 0x80;  
	bitsBuffer.p_data = (unsigned char *)(p_data);  
	memset(bitsBuffer.p_data, 0, PES_HDR_LEN);  
	/*system header*/  
	bits_write( &bitsBuffer, 24,0x000001);  /*start code*/  
	bits_write( &bitsBuffer, 8, (stream_id));   /*streamID*/  
	bits_write( &bitsBuffer, 16,(payload_len)+13);  /*packet_len*/ //Ö¸³öpes·Ö×éÖÐÊý¾Ý³¤¶ÈºÍ¸Ã×Ö½ÚºóµÄ³¤¶ÈºÍ  
	bits_write( &bitsBuffer, 2, 2 );        /*'10'*/  
	bits_write( &bitsBuffer, 2, 0 );        /*scrambling_control*/  
	bits_write( &bitsBuffer, 1, 0 );        /*priority*/  
	bits_write( &bitsBuffer, 1, 0 );        /*data_alignment_indicator*/  
	bits_write( &bitsBuffer, 1, 0 );        /*copyright*/  
	bits_write( &bitsBuffer, 1, 0 );        /*original_or_copy*/  
	bits_write( &bitsBuffer, 1, 1 );        /*PTS_flag*/  
	bits_write( &bitsBuffer, 1, 1 );        /*DTS_flag*/  
	bits_write( &bitsBuffer, 1, 0 );        /*ESCR_flag*/  
	bits_write( &bitsBuffer, 1, 0 );        /*ES_rate_flag*/  
	bits_write( &bitsBuffer, 1, 0 );        /*DSM_trick_mode_flag*/  
	bits_write( &bitsBuffer, 1, 0 );        /*additional_copy_info_flag*/  
	bits_write( &bitsBuffer, 1, 0 );        /*PES_CRC_flag*/  
	bits_write( &bitsBuffer, 1, 0 );        /*PES_extension_flag*/  
	bits_write( &bitsBuffer, 8, 10);        /*header_data_length*/   
	// Ö¸³ö°üº¬ÔÚ PES ·Ö×é±êÌâÖÐµÄ¿ÉÑ¡×Ö¶ÎºÍÈÎºÎÌî³ä×Ö½ÚËùÕ¼ÓÃµÄ×Ü×Ö½ÚÊý¡£¸Ã×Ö¶ÎÖ®Ç°  
	//µÄ×Ö½ÚÖ¸³öÁËÓÐÎÞ¿ÉÑ¡×Ö¶Î¡£  

	/*PTS,DTS*/   
	bits_write( &bitsBuffer, 4, 3 );                    /*'0011'*/  
	bits_write( &bitsBuffer, 3, ((pts)>>30)&0x07 );     /*PTS[32..30]*/  
	bits_write( &bitsBuffer, 1, 1 );  
	bits_write( &bitsBuffer, 15,((pts)>>15)&0x7FFF);    /*PTS[29..15]*/  
	bits_write( &bitsBuffer, 1, 1 );  
	bits_write( &bitsBuffer, 15,(pts)&0x7FFF);          /*PTS[14..0]*/  
	bits_write( &bitsBuffer, 1, 1 );  
	bits_write( &bitsBuffer, 4, 1 );                    /*'0001'*/  
	bits_write( &bitsBuffer, 3, ((dts)>>30)&0x07 );     /*DTS[32..30]*/  
	bits_write( &bitsBuffer, 1, 1 );  
	bits_write( &bitsBuffer, 15,((dts)>>15)&0x7FFF);    /*DTS[29..15]*/  
	bits_write( &bitsBuffer, 1, 1 );  
	bits_write( &bitsBuffer, 15,(dts)&0x7FFF);          /*DTS[14..0]*/  
	bits_write( &bitsBuffer, 1, 1 );  
	return 0;  
}


static void build_package_from_frame(unsigned char *pFrame, unsigned int frameSize)
{
    // 1, clear the previous packages
    static int iFrameCount = 0;
    unsigned int i = 0;
    for (i = 0; i < thePackages.size(); i++){
        if (thePackages[i] != NULL){
            delete thePackages[i];
            thePackages[i] = NULL;
        }
    }
    thePackages.clear();
    thePackageOffset = 0;

    // 2, set the default nalu header, which is not include in the frame.
    pFrame[0] = 0x00;
    pFrame[1] = 0x00;
    pFrame[2] = 0x00;
    pFrame[3] = 0x01;
    bool isIFrame = false;
    unsigned char type = pFrame[4];
    if ((type & 0x1F)  == 5){
        // printf("*****************IFrame, len:%d, iFrameCount:%d\n", frameSize, iFrameCount);
        isIFrame = true;
    }
    //else{
    //    printf("Type:%d, len:%d\n", type, frameSize);
    //}
    iFrameCount++;
    
    // 3, build the packages from the frame
    struct timeval tv;
    gettimeofday(&tv,NULL);
    u_int32_t nTimeStamp = (90000*tv.tv_sec);
    nTimeStamp += (u_int32_t)(90000*(tv.tv_usec/1000000.0) + 0.5); // note: rounding
    
	int  nSendBytes = 0; //Í³¼Æ±¾Ö¡RTP·µ»ØµÄ·¢ËÍ×Ö½ÚÊý
	int nSendRet = 0;   //Ò»´Îrtp·µ»ØµÄ·¢ËÍ×Ö½ÚÊý

	//ÒÔÏÂÖÆ×÷psÍ·
	unsigned char sTempPackHeader[256]; //³ýrtpÍ·¶¼ÔÚÕâÀïÃæ³õÊ¼»¯ 				
	unsigned char sTempPack[1400];
	int  nSizePos = 0;  
	int  size = 0;       

	memset(sTempPackHeader, 0, 256);  
	make_ps_header(sTempPackHeader + nSizePos, nTimeStamp); //ÖÆ×÷psÍ·  
	nSizePos += PS_HDR_LEN;   
	if (isIFrame) // ÅÐ¶Ïkeyframe 
	{
		//IÖ¡·âsysÍ·
		make_sys_header(sTempPackHeader + nSizePos);  
		nSizePos += SYS_HDR_LEN; 
	}  
	make_psm_header(sTempPackHeader + nSizePos); //sizepos 14 ÖÆ×÷psmÍ· 
	nSizePos += PSM_HDR_LEN; 
    //¼ÓÉÏrtp·¢ËÍ³öÈ¥£¬ÕâÑùµÄ»°£¬ºóÃæµÄÊý¾Ý¾ÍÖ»Òª·ÖÆ¬·Ö°ü¾ÍÖ»ÓÐ¼ÓÉÏpesÍ·ºÍrtpÍ·ÁË
	nSendRet= rtp_session_send_with_ts((unsigned char*)sTempPackHeader, nSizePos); 
    
	int nFrameLen= frameSize;
	char* pBuffer = (char*)pFrame;
	while(nFrameLen > 0)  
	{  
		if(nFrameLen>PS_PES_PAYLOAD_SIZE)
		{ 
			size = PS_PES_PAYLOAD_SIZE;
			memset(sTempPack,0,1400);
			make_pes_header(sTempPack, 0xE0, size, nTimeStamp, nTimeStamp); 
			memcpy(sTempPack+PES_HDR_LEN, pBuffer, size);

			nSendRet = rtp_session_send_with_ts((unsigned char*)sTempPack, size + PES_HDR_LEN); 
			nSendBytes+= nSendRet;
			//·ÖÆ¬ºóÒÆ¶¯Ö¸Õë
			nFrameLen -= size;  
			pBuffer += size;
		}
		else //×îºóÒ»´Î·¢ËÍ
		{
			size = nFrameLen;
			memset(sTempPack, 0, 1400);
			make_pes_header(sTempPack, 0xE0, size, nTimeStamp, nTimeStamp);
			memcpy(sTempPack+PES_HDR_LEN, pBuffer, size);

			//×îºóÒ»¸ö°üÌí¼ÓrtpÍ··¢ËÍ
			nSendRet =  rtp_session_send_with_ts((unsigned char*)sTempPack, size + PES_HDR_LEN);		
			break;
		}
	}
}

#endif 

void H264or5Fragmenter::doGetNextFrame() {
  if (fNumValidDataBytes == 1) {
    // We have no NAL unit data currently in the buffer.  Read a new one:
    // modified by fanhongxuan@gmail.com
    // change offset from 1 to 4
    fInputSource->getNextFrame(&fInputBuffer[4], fInputBufferSize - 4,
			       afterGettingFrame, this,
			       FramedSource::handleClosure, this);
  } else {
    // We have NAL unit data in the buffer.  There are three cases to consider:
    // 1. There is a new NAL unit in the buffer, and it's small enough to deliver
    //    to the RTP sink (as is).
    // 2. There is a new NAL unit in the buffer, but it's too large to deliver to
    //    the RTP sink in its entirety.  Deliver the first fragment of this data,
    //    as a FU packet, with one extra preceding header byte (for the "FU header").
    // 3. There is a NAL unit in the buffer, and we've already delivered some
    //    fragment(s) of this.  Deliver the next fragment of this data,
    //    as a FU packet, with two (H.264) or three (H.265) extra preceding header bytes
    //    (for the "NAL header" and the "FU header").
    if (fMaxSize < fMaxOutputPacketSize) { // shouldn't happen
      envir() << "H264or5Fragmenter::doGetNextFrame(): fMaxSize ("
	      << fMaxSize << ") is smaller than expected\n";
    } else {
      fMaxSize = fMaxOutputPacketSize;
    }
    
#if 0
    fLastFragmentCompletedNALUnit = True; // by default
    if (fCurDataOffset == 1) { // case 1 or 2
      if (fNumValidDataBytes - 1 <= fMaxSize) { // case 1
	    memmove(fTo, &fInputBuffer[1], fNumValidDataBytes - 1);
	    fFrameSize = fNumValidDataBytes - 1;
	    fCurDataOffset = fNumValidDataBytes;
      } else { // case 2
	    // We need to send the NAL unit data as FU packets.  Deliver the first
	    // packet now.  Note that we add "NAL header" and "FU header" bytes to the front
	    // of the packet (overwriting the existing "NAL header").
	    if (fHNumber == 264) {
	      fInputBuffer[0] = (fInputBuffer[1] & 0xE0) | 28; // FU indicator
	      fInputBuffer[1] = 0x80 | (fInputBuffer[1] & 0x1F); // FU header (with S bit)
	    } else { // 265
	      u_int8_t nal_unit_type = (fInputBuffer[1]&0x7E)>>1;
	      fInputBuffer[0] = (fInputBuffer[1] & 0x81) | (49<<1); // Payload header (1st byte)
	      fInputBuffer[1] = fInputBuffer[2]; // Payload header (2nd byte)
	      fInputBuffer[2] = 0x80 | nal_unit_type; // FU header (with S bit)
	    }
	    memmove(fTo, fInputBuffer, fMaxSize);
	    fFrameSize = fMaxSize;
	    fCurDataOffset += fMaxSize - 1;
	    fLastFragmentCompletedNALUnit = False;
      }
    } else { // case 3
      // We are sending this NAL unit data as FU packets.  We've already sent the
      // first packet (fragment).  Now, send the next fragment.  Note that we add
      // "NAL header" and "FU header" bytes to the front.  (We reuse these bytes that
      // we already sent for the first fragment, but clear the S bit, and add the E
      // bit if this is the last fragment.)
      unsigned numExtraHeaderBytes;
      if (fHNumber == 264) {
	    fInputBuffer[fCurDataOffset-2] = fInputBuffer[0]; // FU indicator
	    fInputBuffer[fCurDataOffset-1] = fInputBuffer[1]&~0x80; // FU header (no S bit)
	    numExtraHeaderBytes = 2;
      } else { // 265
	    fInputBuffer[fCurDataOffset-3] = fInputBuffer[0]; // Payload header (1st byte)
	    fInputBuffer[fCurDataOffset-2] = fInputBuffer[1]; // Payload header (2nd byte)
	    fInputBuffer[fCurDataOffset-1] = fInputBuffer[2]&~0x80; // FU header (no S bit)
	    numExtraHeaderBytes = 3;
      }
      unsigned numBytesToSend = numExtraHeaderBytes + (fNumValidDataBytes - fCurDataOffset);
      if (numBytesToSend > fMaxSize) {
	    // We can't send all of the remaining data this time:
	    numBytesToSend = fMaxSize;
	    fLastFragmentCompletedNALUnit = False;
      } else {
	    // This is the last fragment:
	    fInputBuffer[fCurDataOffset-1] |= 0x40; // set the E bit in the FU header
	    fNumTruncatedBytes = fSaveNumTruncatedBytes;
      }
      memmove(fTo, &fInputBuffer[fCurDataOffset-numExtraHeaderBytes], numBytesToSend);
      fFrameSize = numBytesToSend;
      fCurDataOffset += numBytesToSend - numExtraHeaderBytes;
    }
#endif

#ifdef PACK_NALU_BY_NANRI
    // add by fanhongxuan@gmail.com
    // pack the rtp package according the nanri spec.
    fLastFragmentCompletedNALUnit = false;
    if (fCurDataOffset == 1){
        int frameSize = fNumValidDataBytes - 1 + 4;
        
        build_package_from_frame(fInputBuffer, frameSize);
        fCurDataOffset = 0;
    }
    if (thePackageOffset < thePackages.size()){
        ps_package *pPackage = thePackages[thePackageOffset];
        if (NULL != pPackage && NULL != pPackage->mpData){
            memcpy(fTo, pPackage->mpData, pPackage->mLen);
            fFrameSize = pPackage->mLen;
        }
        thePackageOffset++;
    }
    else {
        // We're done with this data.  Reset the pointers for receiving new data:
        fNumValidDataBytes = fCurDataOffset = 1;
    }
#endif    
#ifdef PACK_NALU_IN_ONE_PS_PACKAGE   
    // note:fanhongxuan@gmail.com
    // pack the nalu to ps format.
    // raw data is in fInputBuffer,
    if (fCurDataOffset == 1){
        // first conver the frame to ps and store it in g_outbuf.
        int frameSize = fNumValidDataBytes - 1 + 4; // 1 is the old offset, 4 is the nalu header.
        fNumValidDataBytes = convert_nalu_to_ps(fInputBuffer, frameSize, frameSize <= 100);
        fCurDataOffset = 0;
    }
    // fNumValidDataBytes is the real ps frame size.
    fLastFragmentCompletedNALUnit = True;
    if (fNumValidDataBytes< fMaxSize){
        memmove(fTo, &g_outbuf[0], fNumValidDataBytes);
        fFrameSize = fNumValidDataBytes;
        fCurDataOffset = fNumValidDataBytes;
    }
    else{
        fFrameSize = fNumValidDataBytes - fCurDataOffset;
        if (fFrameSize > fMaxSize){
            fFrameSize = fMaxSize;
        }
        fLastFragmentCompletedNALUnit = False;
        memmove(fTo, &g_outbuf[fCurDataOffset], fFrameSize);
        fCurDataOffset += fFrameSize;
    }
    
    // fprintf(stderr, "(%d)frameSize:%d, fMaxSize:%d\n", g_frame_count, fNumValidDataBytes, fMaxSize);
    // fprintf(stderr, "(%d)packet size:%d\n", g_frame_count, fFrameSize);
    // fprintf(stderr, "(%d)fCurDataOffset:%d\n", g_frame_count, fCurDataOffset);
    if (fCurDataOffset >= fNumValidDataBytes) {
      fLastFragmentCompletedNALUnit = true;
      // We're done with this data.  Reset the pointers for receiving new data:
      fNumValidDataBytes = fCurDataOffset = 1;
    }
#endif
    
    // Complete delivery to the client:
    FramedSource::afterGetting(this);
  }
}

void H264or5Fragmenter::doStopGettingFrames() {
  // Make sure that we don't have any stale data fragments lying around, should we later resume:
  reset();
  FramedFilter::doStopGettingFrames();
}

extern float gRTSPScale;

void H264or5Fragmenter::afterGettingFrame(void* clientData, unsigned frameSize,
					  unsigned numTruncatedBytes,
					  struct timeval presentationTime,
					  unsigned durationInMicroseconds) {
  H264or5Fragmenter* fragmenter = (H264or5Fragmenter*)clientData;
  /* printf("afterGettingFrame, durationInMs:%d\n", durationInMicroseconds);
  printf("change durationInMicroseconds\n");
  */
  if (gRTSPScale >= 0.01){
      durationInMicroseconds = durationInMicroseconds / gRTSPScale;
  }
  fragmenter->afterGettingFrame1(frameSize, numTruncatedBytes, presentationTime,
				 durationInMicroseconds);
}

void H264or5Fragmenter::afterGettingFrame1(unsigned frameSize,
					   unsigned numTruncatedBytes,
					   struct timeval presentationTime,
					   unsigned durationInMicroseconds) {
  fNumValidDataBytes += frameSize;
  fSaveNumTruncatedBytes = numTruncatedBytes;
  fPresentationTime = presentationTime;
  fDurationInMicroseconds = durationInMicroseconds;

  // Deliver data to the client:
  doGetNextFrame();
}

void H264or5Fragmenter::reset() {
  fNumValidDataBytes = fCurDataOffset = 1;
  fSaveNumTruncatedBytes = 0;
  fLastFragmentCompletedNALUnit = True;
}
