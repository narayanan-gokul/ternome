#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int main(void) {
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	ioctl(0, TIOCGWINSZ, &w);
	printf("lines: %d\n", w.ws_row);
	printf("columns: %d\n", w.ws_col);

	return 0;
}
