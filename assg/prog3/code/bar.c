/*
2. A foolproof progress bar.
     [16] Using an ``ioctl()`` call to find the exact width of the terminal,
     write a program with function ``void bar(float percent)`` which can print
     out a progress bar given ``percent`` as an argument.  It should work for a
     terminal of any width; and in fact should update correctly if the width of
     the terminal is changed mid-execution.  An example executable ``bar`` is
     provided.  Call this ``bar.c``.  See the ``ioctl`` and ``ioctl_list``
     manual pages for clues on how to call ``ioctl()``.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define TEXTSPACE 9

struct winsize size;

void handle_resize(int sig) {
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	printf("\r");
	fflush(stdout);
	return;
}

void bar(float percent) {
	unsigned short sz = size.ws_col;
	unsigned short leftover = sz - TEXTSPACE;
	unsigned short barsz = leftover - 2;
	unsigned short n = (unsigned short) (percent * leftover);
	

//	printf("percent is %.7f, sz is %d, n is %d, leftover is %d\n", percent, sz, n, leftover);
	// allocate char of size sz
	// 0 = [
	// leftover-1 ]
	char buffer[sz];
	memset(buffer, 0, sz);
	memset(buffer, 0x20, sz-1);
	buffer[0] = '[';
	for(unsigned short i = 1; i < n; i++) {
		buffer[i] = '=';
		if (i < barsz-1) {
			buffer[i+1] = '>';
		}
	}
	buffer[leftover - 1] = ']';

	if (percent < 1.0f) {
		snprintf(&buffer[leftover], TEXTSPACE, "  %.1f%%", percent*100.0f);
		printf("%s\r", buffer);
		fflush(stdout);
	} else {
		memset(&buffer[leftover], 0x20, TEXTSPACE);
		char *str = "Done! ";
		snprintf(&buffer[leftover], TEXTSPACE, "  %s", str);
		printf("%s\n", buffer);
	}

}

int main(int argc, char *argv[]) {
	signal(SIGWINCH, handle_resize);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	const float limit = 1000000.0f;
	for(unsigned int i = 0; i <= limit; i++) {
		bar(i/limit);
	}

	return 0;
}
