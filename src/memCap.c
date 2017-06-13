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

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//#define CACHE_SIZE 2*1024*1024
#define NS_PER_S (1000000000L)
#define MAX_SIZE_RATIO 0.9

unsigned long int get_ns() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec*NS_PER_S + ts.tv_nsec;
}

// Removes all occurances of char c in the input string
void remove_all_chars(char* str, char c) {
	char *pr = str, *pw = str;
	while (*pr) {
          *pw = *pr++;
          pw += (*pw != c);
	}
	*pw = '\0';
}

long long int get_memory_size_kb(void) {
	char line[512], buffer[32];
	long long int column;
	FILE *meminfo;

	if (!(meminfo = fopen("/proc/meminfo", "r"))) {
          perror("/proc/meminfo: fopen");
	  return -1;
	}

	while (fgets(line, sizeof(line), meminfo)) {
		if (strstr(line, "MemTotal")) {
			char* colStr;
			colStr = strstr(line, ":");
			remove_all_chars(colStr, ':');
			remove_all_chars(colStr, 'k');
			remove_all_chars(colStr, 'B');
			remove_all_chars(colStr, ' ');
			column = atoi(colStr);
		        column = 1000*column;
			fclose(meminfo);
			return column;
		}
	}

	fclose(meminfo);
        perror("Unable to find MemTotal in proc/meminfo");
	return -1;
}

int main(int argc, char **argv) {
	timespec sleepValue = {0};

	if (argc < 3) {
          printf("Usage: ./memCap <duration in sec> <intensity>\n");
	  exit(0);
	}

	char* volatile block;
	long long int memory_size = get_memory_size_kb();
        if (memory_size == -1) {
          exit(1);
        }
	printf("Total Memory Size: %llu\n", memory_size);

	int usr_timer = atoi(argv[1]);
        double intensity = atoi(argv[2]) / 100.0;
	if (intensity < 0) {
	  intensity = 0.01;
	}
        if (intensity > 1.0) {
          intensity = 1.0;
        }
        long long int write_size = memory_size * intensity * MAX_SIZE_RATIO;
        printf("Size: %llu\n", write_size);

	block = (char*)mmap(NULL, write_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
        if (block == MAP_FAILED) {
          perror("Unable to mmap");
          exit(1);
        }

	double time_spent = 0.0;
  	clock_t begin, end;

	while (time_spent < usr_timer) {
          begin = clock();
          memcpy(block, block, write_size);
          //sleepValue.tv_nsec = (usr_timer-get_ns())/usr_timer;
          //nanosleep(&sleepValue, NULL);
          end = clock();
          time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
	}
	return 0;
}
