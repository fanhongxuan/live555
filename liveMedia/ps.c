#include <string.h>
#include "ps.h"
#include "Log.h"


static int g_frame_count = 0;

static int create_ps_header(unsigned char *pOutput, int iOutputLen, uint64_t pts)
{
    int iIndex = 0;
    uint32_t mux_rate = 0xA3D8;
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x01;
    pOutput[iIndex++] = 0xBA;
    
    pOutput[iIndex++] = 0x40 | (((uint8_t)(pts >> 30) & 0x07) << 3) | 0x04 | ((uint8_t)(pts >> 28) & 0x03);
	pOutput[iIndex++] = (uint8_t)((pts >> 20) & 0xff);
	pOutput[iIndex++] = (((uint8_t)(pts >> 15) & 0x1f) << 3) | 0x04 | ((uint8_t)(pts >> 13) & 0x03);
	pOutput[iIndex++] = (uint8_t)((pts >> 5) & 0xff);
	pOutput[iIndex++] = (((uint8_t)pts & 0x1f) << 3) | 0x04;
	pOutput[iIndex++] = 1;
    
	pOutput[iIndex++] = ( mux_rate >> 14 ) & 0xff;
	pOutput[iIndex++] = ( mux_rate >> 6 ) & 0xff;
	pOutput[iIndex++] = ((( mux_rate<<2 ) & 0xfc ) | 0x03);
    pOutput[iIndex++] = 0xFE;
    pOutput[iIndex++] = 0xFF;
    pOutput[iIndex++] = 0xFF;
    pOutput[iIndex++] = 0xFF;
    pOutput[iIndex++] = 0xFF;
    pOutput[iIndex++] = 0xFF;
    pOutput[iIndex++] = 0xFF;
    return iIndex;
}

static int create_sys_header(unsigned char *pOutput, int iOutputLen, uint64_t pts)
{
    int iIndex = 0;
    int iLen = 12;
    uint32_t rate_bound = 0xf7f;
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x01;
    pOutput[iIndex++] = 0xBB;
    pOutput[iIndex++] = (iLen >> 8) & 0xFF;
    pOutput[iIndex++] = (iLen & 0xFF);
    pOutput[iIndex++] = (( rate_bound >> 15 ) & 0xff ) | 0x80;
	pOutput[iIndex++] = ( rate_bound >> 7 ) & 0xff;
	pOutput[iIndex++] = ((( rate_bound << 1) & 0xfe) | 0x01 );
    // todo:fanhongxuan@gmail.com
    // maybe need to set fixed_flag
    pOutput[iIndex++] = 0xFE; // flags, one audio stream, fixed flag
    pOutput[iIndex++] = 0xE1; // flags, one video stream
    pOutput[iIndex++] = 0x7F;
    pOutput[iIndex++] = 0xE0; // video
    pOutput[iIndex++] = 0xE0;
    pOutput[iIndex++] = 0xE8;
    pOutput[iIndex++] = 0xC0; // audio
    pOutput[iIndex++] = 0xC0;
    pOutput[iIndex++] = 0x20;
    return iIndex;
}

static int create_sys_map(unsigned char *pOutput, int iOutputLen, uint64_t pts){
    int iIndex = 0, iLen = 0;
    int iEsLen = 4, iPsLen = 0;
    
    iEsLen = 60;
    iPsLen = 36;
    
    iLen = 6 + 4 + iEsLen + iPsLen; // 6 byte head, 4 byte crc, 4 byte es info
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x01;
    pOutput[iIndex++] = 0xBC;
    pOutput[iIndex++] = (iLen >> 8) & 0xFF;
    pOutput[iIndex++] = (iLen & 0xFF);
    
    pOutput[iIndex++] = 0xE0;
    pOutput[iIndex++] = 0xFF;
    pOutput[iIndex++] = (iPsLen >> 8) & 0xFF;
    pOutput[iIndex++] = (iPsLen & 0xFF);
    pOutput[iIndex++]= 0x40;
	pOutput[iIndex++]= 0x0e;
	pOutput[iIndex++]= 0x53;
	pOutput[iIndex++]= 0x54;
	pOutput[iIndex++]= 0x00;
	pOutput[iIndex++]= 0x01;
	pOutput[iIndex++]= 0x13;
	pOutput[iIndex++]= 0x98;
	pOutput[iIndex++]= 0xa3;
	pOutput[iIndex++]= 0xbb;
	pOutput[iIndex++]=  0xe0;
	pOutput[iIndex++]=  0xf5;
	pOutput[iIndex++]=  0xff;
	pOutput[iIndex++]=  0xff;
	pOutput[iIndex++]=  0xff;
	pOutput[iIndex++]=  0xff;
	pOutput[iIndex++]=  0x41;
	pOutput[iIndex++]=  0x12;
	pOutput[iIndex++]=  0x53;
	pOutput[iIndex++]=  0x54;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
	pOutput[iIndex++]=  0x00;
    
    // es hdr
    pOutput[iIndex++]= (iEsLen >> 8) & 0xFF;
    pOutput[iIndex++]= (iEsLen & 0xff);
	pOutput [iIndex++]=0x1b;
	pOutput [iIndex++]=0xe0;
	pOutput [iIndex++]=0x00;
	pOutput [iIndex++]=0x28;
	pOutput [iIndex++]=0x42;
	pOutput [iIndex++]=0x0e;
	pOutput [iIndex++]=0x00;
	pOutput [iIndex++]=0x00;
	pOutput [iIndex++]=0x60;
	pOutput [iIndex++]=0x00;
	pOutput [iIndex++]= 0x02;
	pOutput [iIndex++]= 0xc0;
	pOutput [iIndex++]= 0x02;
	pOutput [iIndex++]= 0x40;
	pOutput [iIndex++]= 0x11;
	pOutput [iIndex++]= 0x0f;
	pOutput [iIndex++]= 0xff;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x0e;
	pOutput [iIndex++]= 0x10;
	pOutput [iIndex++]= 0x44;
	pOutput [iIndex++]= 0x0a;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x80;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0xff;
	pOutput [iIndex++]= 0xff;
	pOutput [iIndex++]= 0x2a;
	pOutput [iIndex++]= 0x0a;
	pOutput [iIndex++]= 0x7f;
	pOutput [iIndex++]= 0xff;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x03;
	pOutput [iIndex++]= 0x84;
	pOutput [iIndex++]= 0x1f;
	pOutput [iIndex++]= 0xfe;
	pOutput [iIndex++]= 0xa0;
	pOutput [iIndex++]= 0x5a;
	pOutput [iIndex++]= 0x90;
	pOutput [iIndex++]= 0xc0;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x0c;
	pOutput [iIndex++]= 0x43;
	pOutput [iIndex++]= 0x0a;
	pOutput [iIndex++]= 0x01;
	pOutput [iIndex++]= 0x40;
	pOutput [iIndex++]= 0xff;
	pOutput [iIndex++]= 0x00;
	pOutput [iIndex++]= 0x7d;
	pOutput [iIndex++]= 0x03;
	pOutput [iIndex++]= 0x01;
	pOutput [iIndex++]= 0xf4;
	pOutput [iIndex++]= 0x03;
	pOutput [iIndex++]= 0xff;
    
#if 0    
    pOutput[iIndex++] = 0;
    pOutput[iIndex++] = 0; // ps_info_len
    pOutput[iIndex++] = 0;
    pOutput[iIndex++] = 4; // es_info_len
    pOutput[iIndex++] = 0x1B; //H264
    pOutput[iIndex++] = 0xE0; // Video
    pOutput[iIndex++] = 0;
    pOutput[iIndex++] = 0; // es_item_len
#endif    
    pOutput[iIndex++] = 0; 
    pOutput[iIndex++] = 0;
    pOutput[iIndex++] = 0;
    pOutput[iIndex++] = 0; // crc
    return iIndex;
}

static int create_pes_package(unsigned char *pOutput, int iOutputLen, unsigned char *pInput, int iInputLen, uint64_t pts)
{
    int iIndex = 0;
    unsigned int iLen = iInputLen + 2 + 1; // 2 byte flags, 1 byte len
    if (pts != 0){
        iLen += 5; // 5 byte pts
    }
    
    int stuffing = (4-(iLen+6)%4)%4;
    iLen += stuffing;
    
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x00;
    pOutput[iIndex++] = 0x01;
    pOutput[iIndex++] = 0xE0;
    pOutput[iIndex++] = (iLen >> 8) & 0xFF;
    pOutput[iIndex++] = (iLen & 0xFF);
    int i = 0;
    
    pOutput[iIndex++] = 0x8C; // flags
    if (pts != 0){
        pOutput[iIndex++] = 0x80; // PTS type is 2
        pOutput[iIndex++] = 0x05 + stuffing; 
        pOutput[iIndex++] = (uint8_t)(((pts >> 30) & 0x07) << 1) | 0x30 | 0x01;
        pOutput[iIndex++] = (uint8_t)((pts >> 22) & 0xff);
        pOutput[iIndex++] = (uint8_t)(((pts >> 15) & 0xff) << 1) | 0x01;
        pOutput[iIndex++] = (uint8_t)((pts >> 7) & 0xff);
        pOutput[iIndex++] = (uint8_t)((pts & 0xff) << 1) | 0x01;
    }
    else{
        pOutput[iIndex++] = 0x00;
        pOutput[iIndex++] = stuffing;
    }
    for (i = 0; i < stuffing; i++){
        pOutput[iIndex++] = 0xff;
    }
    memcpy(pOutput+iIndex, pInput, iInputLen);
    return iLen + 4 + 2; // 4 byte start code, 2 byte len
}

static int pack_h264_nalu(unsigned char *pOutput, int iOutputLen, unsigned char *pInput, int iInputLen, uint64_t pts)
{
    int iRet = 0, iLeft = iInputLen, iStart = 0;
    while(iLeft != 0){
        int iLen = 0;
        if (iLeft >= PES_LENGTH){
            iLen = PES_LENGTH;
            iLeft -= PES_LENGTH;
        }
        else{
            iLen = iLeft;
            iLeft = 0;
        }

        int iOutLen = create_pes_package(pOutput + iRet,  iOutputLen - iRet, pInput + iStart,  iLen, pts);
        iRet += iOutLen;
        iStart += iLen;
    }
    return iRet;
}

int create_ps_package(ps_handle_t *pHandle, uint8_t *pFrame, uint32_t iFrameLen, uint8_t **ppOutput, uint32_t *pOutputSize, uint64_t *pPts)
{
    // static uint8_t pHandle->scPSBuff[PS_BUFF_SIZE+1] = {0};
    // static uint32_t pHandle->siPSBuffOffset = 0;
    // static s_frame_buff_t pHandle->stSFrameBuff[S_FRAME_MAX_COUNT] = {0};
    // static uint32_t pHandle->siSFrameBuffOffset = 0;
    if (NULL == pHandle){
        Loge("Invalid input");
        return -1;
    }
    
    int i = 0;
    char type = pFrame[4] & 0x1F;
    if (type == 0x05 || type == 0x01){
        pHandle->siFrameCount++;
        // Logi("[%d]P-FRAME type[0x%02x] len=%d", pHandle->siFrameCount, type, iFrameLen);
    }
    else{
        if (iFrameLen >= S_FRAME_SIZE){
            Loge("Invalid s_frame_size:%d, type:%d", iFrameLen, type);
            return 0;
        }
        if (type == 7){
            Logi("SPS len:%d\n", iFrameLen);
        }
        else if (type == 8){
            Logi("PPS len:%d\n", iFrameLen);
        }
        memcpy(pHandle->stSFrameBuff[pHandle->siSFrameBuffOffset].ucBuff, pFrame, iFrameLen);
        pHandle->stSFrameBuff[pHandle->siSFrameBuffOffset].uiLen = iFrameLen;
        pHandle->siSFrameBuffOffset++;
        if (pHandle->siSFrameBuffOffset >= S_FRAME_MAX_COUNT){
            Loge("Two many S-FRAME");
            pHandle->siSFrameBuffOffset = 0;
        }
        return 0;
    }    
    
    uint64_t pts =  (pHandle->siFrameCount) * 3600;
    if (NULL != pPts){
        if((*pPts) != 0){
            pts = (*pPts);
        }
        else{
            (*pPts) = pts;
        }
    }
    // 1, preare the ps header
    int len = create_ps_header(pHandle->scPSBuff,  PS_BUFF_SIZE+1, pts);
    pHandle->siPSBuffOffset += len;
    
    if (type == 0x05){ // i-frame
        // 2, prepare the sys header
        len = create_sys_header(pHandle->scPSBuff + pHandle->siPSBuffOffset, PS_BUFF_SIZE+1 - pHandle->siPSBuffOffset, pts);
        pHandle->siPSBuffOffset += len;
        
        // 3, prepare the sys map
        len = create_sys_map(pHandle->scPSBuff + pHandle->siPSBuffOffset, PS_BUFF_SIZE+1 - pHandle->siPSBuffOffset, pts);
        pHandle->siPSBuffOffset += len;
    }
    
    // 4, pack all the s-frame
    for (i = 0; i < pHandle->siSFrameBuffOffset; i++){
        len = pack_h264_nalu(pHandle->scPSBuff + pHandle->siPSBuffOffset, PS_BUFF_SIZE+1 - pHandle->siPSBuffOffset, pHandle->stSFrameBuff[i].ucBuff, pHandle->stSFrameBuff[i].uiLen, pts);
        pHandle->siPSBuffOffset += len;
        pts = 0;
    }
    pHandle->siSFrameBuffOffset = 0;
    
    // 5, pack the i-frame/p-frame here
    len = pack_h264_nalu(pHandle->scPSBuff + pHandle->siPSBuffOffset, PS_BUFF_SIZE+1 - pHandle->siPSBuffOffset, pFrame, iFrameLen, pts);
    pHandle->siPSBuffOffset += len;
    
    // 7, send out the ps packet
    // Logi("Send (%d)", pHandle->siPSBuffOffset);
    // RtpSend(theRtp, (char*)pHandle->scPSBuff, pHandle->siPSBuffOffset, pHandle->siFrameCount * 3600);
    // pHandle->siPSBuffOffset = 0;
    // return 0;
    if (NULL != ppOutput){
        *ppOutput = pHandle->scPSBuff;
    }
    if (NULL != pOutputSize){
        *pOutputSize = pHandle->siPSBuffOffset;
    }
    int ret = pHandle->siPSBuffOffset;
    pHandle->siPSBuffOffset = 0;
    return ret;
}

int find_starter(uint8_t *buf, int size, int from)
{
	int i = 0;
	for (i = from; i < size - 4; i++)
	{
		if (buf[i] == 0 && buf[i+1] == 0 && buf[i+2] == 0 && buf[i+3] == 1)
		{
			return i;
		}
	}
	return -1;
}
