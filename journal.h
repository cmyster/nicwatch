#ifndef JOURNAL_H
#define JOURNAL_H

#include "snapshot.h"

int journal_init(void);
int journal_save(const NicSnapshot *snap);
NicSnapshot* journal_get_latest(void);
NicSnapshot* journal_get_by_timestamp(uint64_t ts);

#endif
