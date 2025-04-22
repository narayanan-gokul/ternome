#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>

#define BPM 216
#define SECONDS_IN_A_MINUTE 60
#define MICROSECOND_RESOLUTION 1000000
#define INTERVALS_IN_4TH_DIVISION 1
#define INTERVALS_IN_8TH_DIVISION 2
#define INTERVALS_IN_16TH_DIVISION 4
#define INTERVALS_IN_32ND_DIVISION 8
#define ERROR_MARGIN 0.001

uint64_t get_posix_clock_time() {
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
		return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
	} else
		return 0;
}

int main(int argc, char *argv[]) {
	int bpm = atoi(argv[1]);
	int subdivision = atoi(argv[2]);
	int intervalsforsubdivision = subdivision / 4;
	uint64_t interval = (SECONDS_IN_A_MINUTE * MICROSECOND_RESOLUTION) / (bpm * intervalsforsubdivision);
	uint64_t interval_low = interval - (ERROR_MARGIN * interval);
	uint64_t interval_high = interval + (ERROR_MARGIN * interval);
	printf("interval: %ld \ninterval_low %ld \ninterval_high: %ld \n", interval, interval_low, interval_high);
	printf("Tick!\n");
	uint64_t prevTime = get_posix_clock_time();
	uint64_t prevTickTime = prevTime;
	int ticked = 1;
	int count = 1;
	for(;;) {
		uint64_t currTime = get_posix_clock_time();
		uint64_t diff = currTime - prevTime;
		if(!ticked && ((diff <= interval_high) && (diff >= interval_low))) {
			if (count == 4){
				printf("----------------\n");
				count = 0;
			}
			printf("Tick!\n");
			if ((currTime - prevTickTime) != interval_low) {
				fprintf(stderr, "{\n\tinterval: %ld\n\tinterval_low: %ld\n\tinterval_high: %ld\n\tdiff: %ld\n\tcurrTime: %ld\n\tprevTime: %ld\n}\n", interval, interval_low, interval_high, diff, currTime, prevTime);
			}
			ticked = 1;
			++count;
			prevTime = currTime;
		} else {
			ticked = 0;
		}
	}
	return 0;
}
