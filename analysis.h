#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "snapshot.h"

typedef struct {
    char iface[32];
    long rx_errors_delta;
    long tx_errors_delta;
    long rx_dropped_delta;
    long tx_dropped_delta;
    int carrier_changed;
} NicDiff;

int analysis_compare(const NicSnapshot *old_snap, const NicSnapshot *new_snap, NicDiff *diffs, int *count);
const char* analysis_get_health_color(const NicDiff *diff);

#endif
