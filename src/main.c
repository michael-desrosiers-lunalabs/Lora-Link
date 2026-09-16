#ifdef ROLE_PING
#include "ping.h"
#define main_fcn main_ping
#endif

#ifdef ROLE_PONG
#include "pong.h"
#define main_fcn main_pong
#endif

int main(void)
{
	main_fcn();
}
