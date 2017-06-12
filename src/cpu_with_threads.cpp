/** $lic$
 * Copyright (C) 2016-2017 by The Board of Trustees of Cornell University
 * Copyright (C) 2013-2016 by The Board of Trustees of Stanford University
 *
 * This file is part of iBench.
 *
 * iBench is free software; you can redistribute it and/or modify it under the
 * terms of the Modified BSD-3 License as published by the Open Source Initiative.
 *
 * If you use this software in your research, we request that you reference
 * the iBench paper ("iBench: Quantifying Interference for Datacenter Applications",
 * Delimitrou and Kozyrakis, IISWC'13, September 2013) as the source of the benchmark
 * suite in any publications that use this software, and that
 * you send us a citation of your work.
 *
 * iBench is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the BSD-3 License for more details.
 *
 * You should have received a copy of the Modified BSD-3 License along with
 * this program. If not, see <https://opensource.org/licenses/BSD-3-Clause>.
 **/

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define NS_PER_S (1000000000L)

uint64_t getNs() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*NS_PER_S + ts.tv_nsec;
}

/**
 *  CPU microbenchmark that runs while loop in number of threads
 *  based on intensity.
 *  Each additional thread added is based on number of threads available.
 *  For example, with 4 threads available, 0-25 will use one thread,
 *  26-50 uses 2, etc.
 */
int main(int argc, const char** argv) {
	uint32_t maxThreads = omp_get_num_procs();
	//Usage: "./cpu <duration in sec> <intensity>"
	if (argc < 3) {
		printf("Usage: ./cpu <duration in sec> <intensity>\n");
		exit(0);
	}
        float intensity = atoi(argv[2]) * 1.0;
        uint32_t perThreadIntensity = 100 / maxThreads;
	uint64_t nsPerRun = NS_PER_S*atoi(argv[1]);
        uint32_t threads = ceil(intensity / perThreadIntensity);

	printf("Running with %d threads with %d total available\n",
               threads, maxThreads);

        omp_set_num_threads(threads);
		//uint64_t endNs = pi*getNs() + nsPerRun;
	uint64_t endNs = getNs() + nsPerRun;
#pragma omp parallel
	while (getNs() < endNs);

	return 0;
}
