#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "journal.h"
#include "snapshot.h"

void test_journal_init_no_home() {
    printf("Running test_journal_init_no_home...\n");
    char *old_home = getenv("HOME");
    setenv("HOME", "", 1); // Simulating missing/empty HOME
    // The code checks if (!home), but getenv returns a pointer. 
    // To truly test !home, we might need to use unsetenv.
    unsetenv("HOME");
    int res = journal_init();
    assert(res == -1);
    if (old_home) setenv("HOME", old_home, 1);
    printf("Passed!\n");
}

void test_journal_save_load() {
    printf("Running test_journal_save_load...\n");
    journal_init();
    
    NicSnapshot snap = {0};
    snap.timestamp = 123456789;
    snap.iface_count = 1;
    strcpy(snap.stats[0].name, "test0");
    snap.stats[0].rx_errors = 500;

    int res = journal_save(&snap);
    assert(res == 0);

    NicSnapshot *loaded = journal_get_by_timestamp(123456789);
    assert(loaded != NULL);
    assert(loaded->timestamp == 123456789);
    assert(loaded->iface_count == 1);
    assert(strcmp(loaded->stats[0].name, "test0") == 0);
    assert(loaded->stats[0].rx_errors == 500);
    
    snapshot_free(loaded);
    printf("Passed!\n");
}

void test_journal_get_latest() {
    printf("Running test_journal_get_latest...\n");
    journal_init();

    NicSnapshot s1 = {0}; s1.timestamp = 1000;
    NicSnapshot s2 = {0}; s2.timestamp = 3000;
    NicSnapshot s3 = {0}; s3.timestamp = 2000;

    journal_save(&s1);
    journal_save(&s2);
    journal_save(&s3);

    NicSnapshot *latest = journal_get_latest();
    assert(latest != NULL);
    assert(latest->timestamp == 3000);
    
    snapshot_free(latest);
    printf("Passed!\n");
}

void test_journal_get_by_timestamp_missing() {
    printf("Running test_journal_get_by_timestamp_missing...\n");
    NicSnapshot *snap = journal_get_by_timestamp(999999999);
    assert(snap == NULL);
    printf("Passed!\n");
}

int main() {
    // Setup a temporary home directory for testing to avoid cluttering real home
    char tmp_dir[] = "/tmp/nicwatch_test_XXXXXX";
    if (mkdtemp(tmp_dir) == NULL) {
        perror("mkdtemp");
        return 1;
    }
    setenv("HOME", tmp_dir, 1);

    test_journal_init_no_home();
    test_journal_save_load();
    test_journal_get_latest();
    test_journal_get_by_timestamp_missing();
    
    printf("All journal tests passed!\n");
    return 0;
}
