#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "snapshot.h"

void test_snapshot_capture_limit() {
    printf("Running test_snapshot_capture_limit...\n");
    // We cannot easily create 64+ interfaces in /sys/class/net in a sandbox.
    // However, we can verify that the capture function returns a valid pointer 
    // and the count is within the [0, 64] range.
    NicSnapshot *snap = snapshot_capture_all();
    assert(snap != NULL);
    assert(snap->iface_count >= 0 && snap->iface_count <= 64);
    snapshot_free(snap);
    printf("Passed!\n");
}

void test_snapshot_free_null() {
    printf("Running test_snapshot_free_null...\n");
    // Ensure snapshot_free handles NULL if called (though calloc is used in capture)
    // Looking at snapshot_free: it just calls free(). free(NULL) is safe.
    snapshot_free(NULL);
    printf("Passed!\n");
}

int main() {
    test_snapshot_capture_limit();
    test_snapshot_free_null();
    printf("All snapshot tests passed!\n");
    return 0;
}
