#include "stdmansos.h"
#include <string.h>

#include "../pd6_packet.h"

// Relay node configuration
// Each relay should have a unique ID for debugging
#ifndef RELAY_ID
#define RELAY_ID 100
#endif

// Cache for tracking recently seen packets to avoid duplicates
#define PACKET_CACHE_SIZE 20

typedef struct {
    uint16_t sourceId;
    uint32_t seq;
} PacketCache_t;

static PacketCache_t packetCache[PACKET_CACHE_SIZE];
static uint8_t cacheIndex = 0;
static uint8_t radioBuffer[RADIO_MAX_PACKET];

// Check if packet was already seen (duplicate detection)
static bool isPacketSeen(uint16_t sourceId, uint32_t seq)
{
    uint8_t i;
    for (i = 0; i < PACKET_CACHE_SIZE; i++) {
        if (packetCache[i].sourceId == sourceId &&
            packetCache[i].seq == seq) {
            return true;  // Duplicate found
        }
    }
    return false;
}

// Add packet to cache
static void addPacketToCache(uint16_t sourceId, uint32_t seq)
{
    packetCache[cacheIndex].sourceId = sourceId;
    packetCache[cacheIndex].seq = seq;
    cacheIndex = (cacheIndex + 1) % PACKET_CACHE_SIZE;
}

// Radio receive callback
static void recvRadio(void)
{
    int16_t len = radioRecv(radioBuffer, sizeof(radioBuffer));
    if (len < 0) {
        return;  // Error
    }

    if (len != (int16_t)sizeof(Pd6Packet_t)) {
        return;  // Wrong packet size
    }

    Pd6Packet_t packet;
    memcpy(&packet, radioBuffer, sizeof(packet));

    // Validate packet key
    if (memcmp(packet.key, PD6_KEY_STR, PD6_KEY_LEN) != 0) {
        return;  // Not a PD6 packet
    }

    // Check if this packet was already relayed
    if (isPacketSeen(packet.sourceId, packet.seq)) {
        yellowLedToggle();  // Visual indicator for duplicate
        PRINTF("Relay: Duplicate ignored - ID=%u seq=%lu\n",
               packet.sourceId,
               (unsigned long)packet.seq);
        return;
    }

    // Add to cache to prevent future rebroadcasting
    addPacketToCache(packet.sourceId, packet.seq);

    // Increment hop count
    packet.hopCount++;

    // Forward the packet
    radioSend(&packet, sizeof(packet));

    // Visual feedback
    greenLedToggle();

    PRINTF("Relay: Forwarded - ID=%u seq=%lu light=%u hops=%u\n",
           packet.sourceId,
           (unsigned long)packet.seq,
           packet.lightValue,
           packet.hopCount);
}

void appMain(void)
{
    uint8_t i;

    // Initialize packet cache
    for (i = 0; i < PACKET_CACHE_SIZE; i++) {
        packetCache[i].sourceId = 0;
        packetCache[i].seq = 0;
    }

    // Set up radio receive handler
    radioSetReceiveHandle(recvRadio);
    radioOn();

    PRINTF("Relay Node ID=%u started\n", RELAY_ID);

    while (1) {
        // Heartbeat LED to show relay is alive
        blueLedToggle();
        mdelay(2000);
    }
}
