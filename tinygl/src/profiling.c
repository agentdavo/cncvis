#include "gl_utils.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#if TGL_FEATURE_PROFILING

int tgl_profile_enabled = 0;

typedef struct {
	uint64_t calls;
	uint64_t cycles;
	uint64_t time_ns;
} ProfEntry;

static ProfEntry prof_data[OP_COUNT];
static uint64_t total_time_ns = 0;

static inline uint64_t tgl_rdtsc(void) {
#if defined(__i386__) || defined(__x86_64__)
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
#endif
}

static inline uint64_t tgl_time_ns(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}

void tgl_enable_profiling(int enable) { tgl_profile_enabled = enable; }

void tgl_profile_call(int op, void (*func)(GLParam*), GLParam* p) {
	uint64_t start_c = tgl_rdtsc();
	uint64_t start_t = tgl_time_ns();
	func(p);
	uint64_t end_c = tgl_rdtsc();
	uint64_t end_t = tgl_time_ns();
	ProfEntry* e = &prof_data[op];
	e->calls++;
	e->cycles += end_c - start_c;
	e->time_ns += end_t - start_t;
	total_time_ns += end_t - start_t;
}

static const char* op_names[] = {
#define ADD_OP(a, b, c) "gl" #a,
#include "opinfo.h"
};

void tgl_profile_report(void) {
	if (!tgl_profile_enabled)
		return;
	printf("\n-- TinyGL profiling results --\n");
	for (int i = 0; i < OP_COUNT; ++i) {
		if (!prof_data[i].calls)
			continue;
		double ms = prof_data[i].time_ns / 1e6;
		double pct = total_time_ns ? (double)prof_data[i].time_ns * 100.0 / total_time_ns : 0.0;
		printf("%-20s %8llu calls %10.2f ms %12llu cycles %6.2f%%\n", op_names[i], (unsigned long long)prof_data[i].calls, ms,
			   (unsigned long long)prof_data[i].cycles, pct);
	}
	double total_ms = total_time_ns / 1e6;
	printf("Total time: %.2f ms\n", total_ms);
}

#else

void tgl_enable_profiling(int enable) { (void)enable; }
void tgl_profile_call(int op, void (*func)(GLParam*), GLParam* p) {
	func(p);
	(void)op;
}
void tgl_profile_report(void) {}

#endif
