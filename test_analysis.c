#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "analysis.h"
#include "snapshot.h"

void test_analysis_compare_deltas() {
    printf("Running test_analysis_compare_deltas...\n");
    NicSnapshot old_snap = {0};
    NicSnapshot new_snap = {0};
    
    old_snap.iface_count = 1;
    strcpy(old_snap.stats[0].name, "eth0");
    old_snap.stats[0].rx_errors = 100;
    old_snap.stats[0].tx_errors = 50;
    old_snap.stats[0].rx_dropped = 10;
    old_snap.stats[0].tx_dropped = 5;
    old_snap.stats[0].carrier = 1;

    new_snap.iface_count = 1;
    strcpy(new_snap.stats[0].name, "eth0");
    new_snap.stats[0].rx_errors = 110; // Delta +10
    new_snap.stats[0].tx_errors = 65;  // Delta +15
    new_snap.stats[0].rx_dropped = 12; // Delta +2
    new_snap.stats[0].tx_dropped = 5;  // Delta 0
    new_snap.stats[0].carrier = 0;     // Changed

    NicDiff diffs[64];
    int count = 0;
    int result = analysis_compare(&old_snap, &new_snap, diffs, &count);

    assert(result == 0);
    assert(count == 1);
    assert(strcmp(diffs[0].iface, "eth0") == 0);
    assert(diffs[0].rx_errors_delta == 10);
    assert(diffs[0].tx_errors_delta == 15);
    assert(diffs[0].rx_dropped_delta == 2);
    assert(diffs[0].tx_dropped_delta == 0);
    assert(diffs[0].carrier_changed == 1);
    printf("Passed!\n");
}

void test_analysis_compare_missing_iface() {
    printf("Running test_analysis_compare_missing_iface...\n");
    NicSnapshot old_snap = {0};
    NicSnapshot new_snap = {0};
    
    old_snap.iface_count = 1;
    strcpy(old_snap.stats[0].name, "eth0");

    new_snap.iface_count = 1;
    strcpy(new_snap.stats[0].name, "wlan0"); // Different name

    NicDiff diffs[64];
    int count = 0;
    analysis_compare(&old_snap, &new_snap, diffs, &count);

    // New snapshot has wlan0, but old doesn't. 
    // Code logic: only increments 'found' if strcmp == 0.
    assert(count == 0);
    printf("Passed!\n");
}

void test_analysis_compare_nulls() {
    printf("Running test_analysis_compare_nulls...\n");
    NicDiff diffs[64];
    int count = 0;
    assert(analysis_compare(NULL, NULL, diffs, &count) == -1);
    printf("Passed!\n");
}

void test_analysis_health_colors() {
    printf("Running test_analysis_health_colors...\n");
    NicDiff diff = {0};

    // GREEN: everything 0
    assert(strcmp(analysis_get_health_color(&diff), "\033[0;32mGREEN\033[0m") == 0);

    // YELLOW: 1-10 errors
    diff.rx_errors_delta = 5;
    assert(strcmp(analysis_get_health_color(&diff), "\033[0;33mYELLOW\033[0m") == 0);
    
    diff.rx_errors_delta = 0;
    diff.carrier_changed = 1;
    assert(strcmp(analysis_get_health_color(&diff), "\033[0;33mYELLOW\033[0m") == 0);

    // RED: > 10 errors
    diff.carrier_changed = 0;
    diff.tx_errors_delta = 11;
    assert(strcmp(analysis_get_health_color(&diff), "\033[0;31mRED\033[0m") == 0);
    printf("Passed!\n");
}

int main() {
    test_analysis_compare_deltas();
    test_analysis_compare_missing_iface();
    test_analysis_compare_nulls();
    test_analysis_health_colors();
    printf("All analysis tests passed!\n");
    return 0;
}
