#include "bsp.h"

void copyArray(unsigned char source[], unsigned char destination[], unsigned char length) {
	memcpy(destination, source, length * sizeof(int));
}


