#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snapshot.h"
#include "journal.h"
#include "analysis.h"
#include "daemon.h"

void print_usage() {
    printf("Usage: nicwatch <command> [args]\n");
    printf("Commands:\n");
    printf("  snap              - Take a snapshot of NIC health\n");
    printf("  status            - Show current health summary\n");
    printf("  history <iface>   - Show history for an interface\n");
    printf("  diff [timestamp]  - Diff current state with a snapshot\n");
    printf("  watch <interval>  - Start monitoring daemon\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 0;
    }

    journal_init();

    if (strcmp(argv[1], "snap") == 0) {
        NicSnapshot *snap = snapshot_capture_all();
        if (snap) {
            journal_save(snap);
            printf("Snapshot captured at %lu\n", snap->timestamp);
            snapshot_free(snap);
        }
    } else if (strcmp(argv[1], "status") == 0) {
        NicSnapshot *curr = snapshot_capture_all();
        NicSnapshot *prev = journal_get_latest();
        if (!curr) return 1;

        printf("Interface\tStatus\tRX-Err\tTX-Err\n");
        for (int i = 0; i < curr->iface_count; i++) {
            NicDiff diff = {0};
            strncpy(diff.iface, curr->stats[i].name, 31);
            if (prev) {
                for (int j = 0; j < prev->iface_count; j++) {
                    if (strcmp(prev->stats[j].name, curr->stats[i].name) == 0) {
                        diff.rx_errors_delta = curr->stats[i].rx_errors - prev->stats[j].rx_errors;
                        diff.tx_errors_delta = curr->stats[i].tx_errors - prev->stats[j].tx_errors;
                        diff.carrier_changed = (curr->stats[i].carrier != prev->stats[j].carrier);
                        break;
                    }
                }
            }
            printf("%s\t%s\t%lu\t%lu\n", curr->stats[i].name, 
                   analysis_get_health_color(&diff), 
                   diff.rx_errors_delta, diff.tx_errors_delta);
        }
        snapshot_free(curr);
        if (prev) snapshot_free(prev);
    } else if (strcmp(argv[1], "diff") == 0) {
        NicSnapshot *curr = snapshot_capture_all();
        NicSnapshot *prev = NULL;

        if (argc > 2) {
            prev = journal_get_by_timestamp(strtoull(argv[2], NULL, 10));
        } else {
            prev = journal_get_latest();
        }

        if (!curr || !prev) {
            printf("Error: Could not perform diff. Ensure a snapshot exists.\n");
            if (curr) snapshot_free(curr);
            if (prev) snapshot_free(prev);
            return 1;
        }

        NicDiff diffs[64];
        int count = 0;
        analysis_compare(prev, curr, diffs, &count);

        printf("Diffing current state vs snapshot %lu\n", prev->timestamp);
        for (int i = 0; i < count; i++) {
            printf("Interface: %s | RX-Err: +%ld | TX-Err: +%ld | Dropped: %ld/%ld | Carrier Change: %s\n",
                   diffs[i].iface, diffs[i].rx_errors_delta, diffs[i].tx_errors_delta,
                   diffs[i].rx_dropped_delta, diffs[i].tx_dropped_delta,
                   diffs[i].carrier_changed ? "Yes" : "No");
        }
        snapshot_free(curr);
        snapshot_free(prev);
    } else if (strcmp(argv[1], "history") == 0) {
        if (argc < 3) {
            printf("Usage: nicwatch history <iface>\n");
            return 1;
        }
        printf("History for %s (Not fully implemented, showing latest)\n", argv[2]);
        // Implementation would involve scanning all .snap files
    } else if (strcmp(argv[1], "watch") == 0) {
        int interval = (argc > 2) ? atoi(argv[2]) : 60;
        daemon_start(interval);
    } else {
        print_usage();
    }

    return 0;
}
