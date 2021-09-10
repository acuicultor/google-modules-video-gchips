// SPDX-License-Identifier: GPL-2.0-only
/*
 * Priority queue impletation with BigOcean
 *
 * Copyright 2021 Google LLC.
 *
 * Author: Ruofei Ma <ruofeim@google.com>
 */

#include <linux/kthread.h>
#include <linux/module.h>

#include "bigo_prioq.h"

int enqueue_prioq(struct bigo_core *core, struct bigo_inst *inst)
{
	struct bigo_job *job = &inst->job;

	if(!core || !inst)
		return -EINVAL;

	mutex_lock(&core->prioq.lock);
	list_add_tail(&job->list, &core->prioq.queue[inst->priority]);
	set_bit(inst->priority, &core->prioq.bitmap);
	mutex_unlock(&core->prioq.lock);

	kref_get(&inst->refcount);
	wake_up(&core->worker);
	return 0;
}

bool dequeue_prioq(struct bigo_core *core, struct bigo_job **job,
			bool *should_stop)
{
	int high_prio;
	struct bigo_job *j = NULL;
	struct list_head *queue;
	if (!core || !job || !should_stop)
		return false;

	*should_stop = false;
	if(kthread_should_stop()) {
		*should_stop = true;
		return true;
	}

	mutex_lock(&core->prioq.lock);
	high_prio = ffs(core->prioq.bitmap) - 1;
	if (high_prio < 0)
		goto exit;

	queue = &core->prioq.queue[high_prio];
	j = list_first_entry_or_null(queue, struct bigo_job, list);
	if (j) {
		list_del(&j->list);
		if (list_empty(queue))
			clear_bit(high_prio, &core->prioq.bitmap);
	}

exit:
	mutex_unlock(&core->prioq.lock);
	*job = j;
	return *job != NULL;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ruofei Ma <ruofeim@google.com>");
