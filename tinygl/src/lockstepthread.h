/*
 * Lock-step threading helper using C11 <threads.h>
 * Public Domain / CC0
 */
#ifndef LOCKSTEPTHREAD_H
#define LOCKSTEPTHREAD_H
#include <stdbool.h>
#include <threads.h>

typedef struct {
	mtx_t mutex;
	struct {
		mtx_t m;
		cnd_t c;
		int count;
		int waiting;
	} barrier;
	thrd_t thread;
	bool isThreadLive;
	bool shouldKillThread;
	int state;
	void (*execute)(void*);
	void* argument;
} c11_lsthread;

void init_c11_lsthread(c11_lsthread* t);
void start_c11_lsthread(c11_lsthread* t);
void kill_c11_lsthread(c11_lsthread* t);
void destroy_c11_lsthread(c11_lsthread* t);
void lock_c11_lsthread(c11_lsthread* t);
void step_c11_lsthread(c11_lsthread* t);

#endif
