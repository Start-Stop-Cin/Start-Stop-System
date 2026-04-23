#include "co2.h"

int main(void) {
	Co2_Init();

	for (int i = 0; i < 100; i++) {
		Co2_Run10ms(true, true);
	}

	return 0;
}
