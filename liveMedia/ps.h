#ifndef _VA_PS_H_
#define _VA_PS_H_
#include <stdint.h> // for uint64_t, uint32_t, uint8_t
#ifdef __cplusplus
extern "C"{
#endif
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
    int create_ps_package(uint8_t *pFrame, uint32_t iFrameLen, uint8_t **ppOutput, uint32_t *pOutputSize, uint64_t *pPts);
    int find_starter(uint8_t *buf, int size, int from);
#ifdef __cplusplus
}
#endif
#endif

