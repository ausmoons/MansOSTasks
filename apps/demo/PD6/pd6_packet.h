#ifndef PD6_PACKET_H
#define PD6_PACKET_H

#include <stdint.h>

#define PD6_KEY_LEN 8
#define PD6_KEY_STR "PD6MHOP"

typedef struct __attribute__((packed))
{
    char key[PD6_KEY_LEN];
    uint16_t sourceId;
    uint32_t seq;
    uint16_t lightValue;
    uint8_t hopCount;
} Pd6Packet_t;

#endif
