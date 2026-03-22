#ifndef PD5_PACKET_H
#define PD5_PACKET_H

#include <stdint.h>

#define PD5_KEY_LEN 8
#define PD5_KEY_STR "PD5RANGE"

typedef struct __attribute__((packed)) {
    char key[PD5_KEY_LEN];
    uint32_t seq;
} Pd5Packet_t;

#endif
