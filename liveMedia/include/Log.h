#ifndef _LIVE_555_LOG_H_
#define _LIVE_555_LOG_H_
void logBegin(const char *type, const char *file, const char *function, int line);
void logEnd();
// #define Logi(...) do{logBegin("info" , __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__); logEnd();}while(0)
#define Logi(...)
#define Loge(...) do{logBegin("error", __FILE__, __FUNCTION__, __LINE__);printf(__VA_ARGS__); logEnd();}while(0)
#endif

