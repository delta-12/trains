#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void DisplayInit(const uint32_t sda, const uint32_t scl);
void DisplayWriteString(const char *const str);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */