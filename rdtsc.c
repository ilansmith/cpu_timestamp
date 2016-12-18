#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>

/**
 * Cache Line Prefetch - Arch specific!
 */
#ifndef L1_CACHE_BYTES
#define L1_CACHE_BYTES		64
#endif

/* Read time stamp couter */
static void set_cpu_affinity(int cpu)
{
	cpu_set_t cpu_set;
	int ret;

	CPU_ZERO(&cpu_set);
	CPU_SET(cpu, &cpu_set);
	ret = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t),
		&cpu_set);

	printf("pthread_setaffinity_np to cpu %d %s\n", cpu,
		ret ? "failed" : "succeeded");
}

static inline void gettimeoftsc(unsigned long long *tscval)
{
	register unsigned int upper_32, lower_32;

	__asm__ __volatile__("rdtsc" : "=a" (lower_32), "=d" (upper_32));
	*tscval = (((unsigned long long)upper_32) << 32) | lower_32;
}

static inline void prefetch(void *x)
{
#if defined __i386__ || defined __x86_64__
	asm volatile("prefetcht0 %0" :: "m" (*(unsigned long *)x));
#else
	/* Use simple memcpy to get data into cache */
	char temp_prefetch_block[L1_CACHE_BYTES];
	memcpy(temp_prefetch_block, x, L1_CACHE_BYTES);
#endif
}

int main(int argc, char **argv)
{
	unsigned long long start = 0, end = 0;
	volatile unsigned long x;

#if !defined __i386__ && !defined __x86_64__
	printf("not supported\n");
	return -1;
#endif

	set_cpu_affinity(0);

	prefetch(&set_cpu_affinity);
	prefetch(&gettimeoftsc);
	x = (unsigned long)&gettimeoftsc - (unsigned long)&set_cpu_affinity;

	gettimeoftsc(&start);
	if (x)
		gettimeoftsc(&end);

	printf("if statement cycles: %llu\n", end - start);

	return 0;
}

