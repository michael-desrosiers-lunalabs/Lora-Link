#ifdef ROLE_PING
#include "ping.h"
#define main_fcn main_ping
#endif

#ifdef ROLE_PONG
#include "pong.h"
#define main_fcn main_pong
#endif
// #include <zephyr/sleep.h>
int main(void)
{
	main_fcn();
	// while (1) {
	// 	k_msleep(1000);
	// }
}
