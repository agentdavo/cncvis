#include "lockstepthread.h"
#include <stdlib.h>

#if TGL_ENABLE_THREADS

static void barrier_wait(c11_lsthread* t) {
	mtx_lock(&t->barrier.m);
	t->barrier.waiting++;
	if (t->barrier.waiting < t->barrier.count) {
		cnd_wait(&t->barrier.c, &t->barrier.m);
	} else {
		t->barrier.waiting = 0;
		cnd_broadcast(&t->barrier.c);
	}
	mtx_unlock(&t->barrier.m);
}

void init_c11_lsthread(c11_lsthread* t) {
	mtx_init(&t->mutex, mtx_plain);
	mtx_init(&t->barrier.m, mtx_plain);
	cnd_init(&t->barrier.c);
	t->barrier.count = 2;
	t->barrier.waiting = 0;
	t->isThreadLive = false;
	t->shouldKillThread = false;
	t->state = 0;
	t->execute = NULL;
	t->argument = NULL;
}

void destroy_c11_lsthread(c11_lsthread* t) {
	mtx_destroy(&t->mutex);
	mtx_destroy(&t->barrier.m);
	cnd_destroy(&t->barrier.c);
}

void lock_c11_lsthread(c11_lsthread* t) {
	if (t->state == 1 || !t->isThreadLive)
		return;
	barrier_wait(t);
	if (mtx_lock(&t->mutex))
		exit(1);
	t->state = 1;
}

void step_c11_lsthread(c11_lsthread* t) {
	if (t->state == -1 || !t->isThreadLive)
		return;
	if (mtx_unlock(&t->mutex))
		exit(1);
	barrier_wait(t);
	t->state = -1;
}

static int lsthread_func(void* arg) {
	c11_lsthread* me = arg;
	if (!me)
		return 0;
	for (;;) {
		barrier_wait(me);
		mtx_lock(&me->mutex);
		if (!me->shouldKillThread && me->execute) {
			me->execute(me->argument);
		} else if (me->shouldKillThread) {
			mtx_unlock(&me->mutex);
			return 0;
		}
		mtx_unlock(&me->mutex);
		barrier_wait(me);
	}
	return 0;
}

void start_c11_lsthread(c11_lsthread* t) {
	if (t->isThreadLive)
		return;
	t->isThreadLive = true;
	t->shouldKillThread = false;
	mtx_lock(&t->mutex);
	t->state = 1; /* locked */
	thrd_create(&t->thread, lsthread_func, t);
}

void kill_c11_lsthread(c11_lsthread* t) {
	if (!t->isThreadLive)
		return;
	if (t->state != 1)
		lock_c11_lsthread(t);
	t->shouldKillThread = true;
	step_c11_lsthread(t);
	thrd_join(t->thread, NULL);
	t->isThreadLive = false;
	t->shouldKillThread = false;
}

#endif /* TGL_ENABLE_THREADS */
