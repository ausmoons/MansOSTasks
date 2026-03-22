#ifndef PD6_PACKET_H
#define PD6_PACKET_H

#include <stdint.h>

// Multihop network packet structure for PD6
// Used by Sensor, Relay, and Gateway nodes

#define PD6_KEY_LEN 8
#define PD6_KEY_STR "PD6MHOP\0"  // Multihop network identifier

// Packet structure
typedef struct __attribute__((packed)) {
    char key[PD6_KEY_LEN];     // Network identifier "PD6MHOP"
    uint16_t sourceId;          // Unique ID of the sensor that created this packet
    uint32_t seq;               // Sequence number from source sensor
    uint16_t lightValue;        // Light sensor reading (0-65535)
    uint8_t hopCount;           // Number of hops from source (incremented by each relay)
} Pd6Packet_t;

#endif
