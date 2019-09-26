#ifndef _VA_PS_H_
#define _VA_PS_H_
#include <stdint.h> // for uint64_t, uint32_t, uint8_t
#ifdef __cplusplus
extern "C"{
#endif

#define PS_BUFF_SIZE (1<<20)
#define PES_BUFF_SIZE (65535)
#define PES_LENGTH (65000)
#define S_FRAME_MAX_COUNT (10)
#define S_FRAME_SIZE 300
typedef struct{
    unsigned char ucBuff[S_FRAME_SIZE+1];
    unsigned int uiLen;
}s_frame_buff_t;

typedef struct ps_handle{
    uint8_t scPSBuff[PS_BUFF_SIZE+1];
    uint32_t siPSBuffOffset;
    s_frame_buff_t stSFrameBuff[S_FRAME_MAX_COUNT];
    uint32_t siSFrameBuffOffset;
    uint32_t siFrameCount;
}ps_handle_t;

    /**
     * @brief create a ps package
     * @param pFrame the input h264/h264 nalu
     * @param iFrameLen the size of the input frame
     * @param ppOutput the output buffer, when has some data need to be send, will set the ppOutput, the output Must NOT freed.
     * @param pOutputSize the output data size
     * @param pPts if pPts is NULL or *pPts is zero, will analysis the frame to setup the PTS and set the *pPts to the analysis result,
     *             if *pPts is no zero, will use this value as output.
     * @return when no data need to send, return 0
     *         when has some data need to be send, return the data size.
     *         when error, return -1
     * @note   current this function is used a global buffer, so it is not thread-safe, and can not be re-entered.
     */
    int create_ps_package(ps_handle_t *pHandle, uint8_t *pFrame, uint32_t iFrameLen, uint8_t **ppOutput, uint32_t *pOutputSize, uint64_t *pPts);
    int find_starter(uint8_t *buf, int size, int from);
#ifdef __cplusplus
}
#endif
#endif

