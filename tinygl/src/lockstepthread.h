/*
 * Lock-step threading helper using C11 <threads.h>
 * Public Domain / CC0
 * The worker runs in lock-step with the main thread using a barrier and mutex.
 * This avoids heavy use of C11 atomics so platforms lacking full `stdatomic`
 * support can still run multithreaded TinyGL builds.
 */
#ifndef LOCKSTEPTHREAD_H
#define LOCKSTEPTHREAD_H
#include <stdbool.h>

#if TGL_ENABLE_THREADS
#include <threads.h>
#endif

typedef struct {
#if TGL_ENABLE_THREADS
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
#endif
	void (*execute)(void*);
	void* argument;
} c11_lsthread;

#if TGL_ENABLE_THREADS
void init_c11_lsthread(c11_lsthread* t);
void start_c11_lsthread(c11_lsthread* t);
void kill_c11_lsthread(c11_lsthread* t);
void destroy_c11_lsthread(c11_lsthread* t);
void lock_c11_lsthread(c11_lsthread* t);
void step_c11_lsthread(c11_lsthread* t);
#else
static inline void init_c11_lsthread(c11_lsthread* t) { (void)t; }
static inline void start_c11_lsthread(c11_lsthread* t) { (void)t; }
static inline void kill_c11_lsthread(c11_lsthread* t) { (void)t; }
static inline void destroy_c11_lsthread(c11_lsthread* t) { (void)t; }
static inline void lock_c11_lsthread(c11_lsthread* t) { (void)t; }
static inline void step_c11_lsthread(c11_lsthread* t) { (void)t; }
#endif

#endif
