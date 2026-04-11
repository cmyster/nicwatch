#include "snapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

static uint64_t read_sysfs_uint64(const char *iface, const char *stat) {
    char path[256];
    char buf[64];
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", iface, stat);
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    if (fgets(buf, sizeof(buf), f)) {
        fclose(f);
        return strtoull(buf, NULL, 10);
    }
    fclose(f);
    return 0;
}

static int read_carrier(const char *iface) {
    char path[256];
    char buf[16];
    snprintf(path, sizeof(path), "/sys/class/net/%s/carrier", iface);
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    int val = -1;
    if (fgets(buf, sizeof(buf), f)) {
        val = atoi(buf);
    }
    fclose(f);
    return val;
}

NicSnapshot* snapshot_capture_all(void) {
    NicSnapshot *snap = calloc(1, sizeof(NicSnapshot));
    if (!snap) return NULL;

    snap->timestamp = (uint64_t)time(NULL);
    DIR *d = opendir("/sys/class/net");
    if (!d) return snap;

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL && snap->iface_count < 64) {
        if (dir->d_name[0] == '.') continue;

        InterfaceStat *is = &snap->stats[snap->iface_count];
        strncpy(is->name, dir->d_name, sizeof(is->name) - 1);
        is->name[sizeof(is->name) - 1] = '\0';
        
        is->rx_errors = read_sysfs_uint64(is->name, "rx_errors");
        is->tx_errors = read_sysfs_uint64(is->name, "tx_errors");
        is->rx_dropped = read_sysfs_uint64(is->name, "rx_dropped");
        is->tx_dropped = read_sysfs_uint64(is->name, "tx_dropped");
        is->carrier = read_carrier(is->name);
        
        snap->iface_count++;
    }
    closedir(d);
    return snap;
}

void snapshot_free(NicSnapshot *snap) {
    free(snap);
}
