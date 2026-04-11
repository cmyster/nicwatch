#include "analysis.h"
#include <string.h>
#include <stdio.h>

int analysis_compare(const NicSnapshot *old_snap, const NicSnapshot *new_snap, NicDiff *diffs, int *count) {
    if (!old_snap || !new_snap) return -1;

    int found = 0;
    for (int i = 0; i < new_snap->iface_count; i++) {
        const InterfaceStat *new_if = &new_snap->stats[i];
        for (int j = 0; j < old_snap->iface_count; j++) {
            const InterfaceStat *old_if = &old_snap->stats[j];
            if (strcmp(new_if->name, old_if->name) == 0) {
                strncpy(diffs[found].iface, new_if->name, 31);
                diffs[found].rx_errors_delta = new_if->rx_errors - old_if->rx_errors;
                diffs[found].tx_errors_delta = new_if->tx_errors - old_if->tx_errors;
                diffs[found].rx_dropped_delta = new_if->rx_dropped - old_if->rx_dropped;
                diffs[found].tx_dropped_delta = new_if->tx_dropped - old_if->tx_dropped;
                diffs[found].carrier_changed = (new_if->carrier != old_if->carrier);
                found++;
                break;
            }
        }
    }
    *count = found;
    return 0;
}

const char* analysis_get_health_color(const NicDiff *diff) {
    if (diff->rx_errors_delta > 10 || diff->tx_errors_delta > 10) return "\033[0;31mRED\033[0m";
    if (diff->rx_errors_delta > 0 || diff->tx_errors_delta > 0 || diff->carrier_changed) return "\033[0;33mYELLOW\033[0m";
    return "\033[0;32mGREEN\033[0m";
}
