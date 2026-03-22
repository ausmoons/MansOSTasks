#include "stdmansos.h"
#include <string.h>

#include "../pd6_packet.h"

// Gateway node - receives radio packets and outputs to serial (USB)

// Cache for tracking received packets to avoid duplicates
#define PACKET_CACHE_SIZE 30

typedef struct {
    uint16_t sourceId;
    uint32_t seq;
} PacketCache_t;

static PacketCache_t packetCache[PACKET_CACHE_SIZE];
static uint8_t cacheIndex = 0;
static uint8_t radioBuffer[RADIO_MAX_PACKET];
static uint32_t totalReceived = 0;
static uint32_t duplicatesIgnored = 0;

// Check if packet was already received (duplicate detection)
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
    int8_t rssi;
    uint8_t lqi;

    memcpy(&packet, radioBuffer, sizeof(packet));

    // Validate packet key
    if (memcmp(packet.key, PD6_KEY_STR, PD6_KEY_LEN) != 0) {
        return;  // Not a PD6 packet
    }

    // Check if this packet was already received
    if (isPacketSeen(packet.sourceId, packet.seq)) {
        duplicatesIgnored++;
        redLedToggle();  // Visual indicator for duplicate
        return;
    }

    // Add to cache to prevent future duplicate output
    addPacketToCache(packet.sourceId, packet.seq);

    // Get radio quality metrics
    rssi = radioGetLastRSSI();
    lqi = radioGetLastLQI();

    totalReceived++;

    // Output packet data to serial port in CSV format
    // Format: sourceId,seq,lightValue,hopCount,rssi,lqi,totalReceived
    PRINTF("%u,%lu,%u,%u,%d,%u,%lu\n",
           packet.sourceId,
           (unsigned long)packet.seq,
           packet.lightValue,
           packet.hopCount,
           (int)rssi,
           (unsigned)lqi,
           (unsigned long)totalReceived);

    // Visual feedback
    greenLedToggle();
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

    // Print CSV header once
    PRINTF("sourceId,seq,lightValue,hopCount,rssi,lqi,totalReceived\n");
    PRINTF("# Gateway started - waiting for packets...\n");

    while (1) {
        // Heartbeat LED to show gateway is alive
        blueLedToggle();
        mdelay(1000);
    }
}
