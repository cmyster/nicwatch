#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char name[32];
    uint64_t rx_errors;
    uint64_t tx_errors;
    uint64_t rx_dropped;
    uint64_t tx_dropped;
    int carrier; // 1 = up, 0 = down
    uint32_t speed;
} InterfaceStat;

typedef struct {
    uint64_t timestamp;
    int iface_count;
    InterfaceStat stats[64];
} NicSnapshot;

NicSnapshot* snapshot_capture_all(void);
void snapshot_free(NicSnapshot *snap);

#endif
