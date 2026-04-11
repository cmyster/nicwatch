#include "journal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

static char journal_dir[256];

int journal_init(void) {
    const char *home = getenv("HOME");
    if (!home) return -1;
    snprintf(journal_dir, sizeof(journal_dir), "%s/.nicwatch", home);
    mkdir(journal_dir, 0700);
    return 0;
}

int journal_save(const NicSnapshot *snap) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%lu.snap", journal_dir, (unsigned long)snap->timestamp);
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fwrite(snap, sizeof(NicSnapshot), 1, f);
    fclose(f);
    return 0;
}

NicSnapshot* journal_get_latest(void) {
    uint64_t latest_ts = 0;
    
    const char *home = getenv("HOME");
    char dir_path[256];
    if (!home) return NULL;
    snprintf(dir_path, sizeof(dir_path), "%s/.nicwatch", home);
    
    DIR *d = opendir(dir_path);
    if (!d) return NULL;
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] == '.') continue;
        uint64_t ts = strtoull(dir->d_name, NULL, 10);
        if (ts > latest_ts) latest_ts = ts;
    }
    closedir(d);

    if (latest_ts == 0) return NULL;
    return journal_get_by_timestamp(latest_ts);
}

NicSnapshot* journal_get_by_timestamp(uint64_t ts) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%lu.snap", journal_dir, (unsigned long)ts);
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    NicSnapshot *snap = malloc(sizeof(NicSnapshot));
    if (!snap) {
        fclose(f);
        return NULL;
    }
    if (fread(snap, sizeof(NicSnapshot), 1, f) != 1) {
        free(snap);
        fclose(f);
        return NULL;
    }
    fclose(f);
    return snap;
}
