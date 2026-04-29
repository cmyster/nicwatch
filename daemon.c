#include "daemon.h"
#include "snapshot.h"
#include "journal.h"
#include <unistd.h>
#include <syslog.h>

int daemon_start(int interval) {
    if (fork() != 0) return 0;
    setsid();
    openlog("nicwatch", LOG_PID, LOG_DAEMON);

    while (1) {
        NicSnapshot *snap = snapshot_capture_all();
        if (snap) {
            journal_save(snap);
            snapshot_free(snap);
        }
        sleep(interval);
    }
    return 0;
}
