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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <float.h>

#ifndef N
#   define N    10000000
#endif
#ifndef OFFSET
#   define OFFSET       0
#endif

static double bwData[N+OFFSET];

//#ifdef _OPENMP
extern int omp_get_num_threads();
//#endif

int main (int argc, char **argv) {
//#ifdef _OPENMP
//#pragma omp parallel
//	{
//#pragma omp master
//	{
//		register int k = omp_get_num_threads();
//	    	printf ("Number of Threads requested = %i\n",k);
//	}
//	}
//#endif

	double scalar = 3.0;
	if (argc < 3) {
		printf("Usage: ./memBw <duration in sec> <intensity>\n");
		exit(0);
	}

	unsigned int usr_timer = atoi(argv[1]);
	double intensity = atoi(argv[2]) / 100.0;
	if (intensity < 0) {
		intensity = 0.01;
	}
	if (intensity > 1.0) {
		intensity = 1.0;
	}

	unsigned int bwStreamSize = N * intensity;
	unsigned int numChunks = N / bwStreamSize;
	double doubleType;
	printf("For intensity = %f, stream size = %ld Bytes\n", intensity, bwStreamSize * sizeof(doubleType));

	double time_spent = 0.0;

	while (time_spent < usr_timer) {
		clock_t begin = clock();

		for (int l = 1; l <= numChunks; l++) {
			#pragma omp parallel for
			for (int i = (l-1) * bwStreamSize; i < l * bwStreamSize; i++) {
				bwData[i] = scalar * bwData[i];
			}
		}

		#pragma omp parallel for
		for (int i = numChunks * bwStreamSize; i < N; i++) {
			bwData[i] = scalar * bwData[i];
		}

		clock_t end = clock();
  		time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

	}
	return 0;
}
