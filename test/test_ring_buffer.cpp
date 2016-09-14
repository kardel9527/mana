#include <stdio.h>
#include <stdlib.h>
#include "udt.h"
#include "ringbuffer.h"

int32 main(int32 argc, char *argv[]) {
	RingBuffer buffer;
	for (int i = 0; i < 2000; ++i) {
		if (buffer.space() == 0) {
			printf("space full.\n");
			buffer.rd_move(buffer.length() / 2);
		}
		buffer.wr_move(1);
		printf("write left :%d, read left:%d\n", buffer.space(), buffer.avail());
	}

	return 0;
}
