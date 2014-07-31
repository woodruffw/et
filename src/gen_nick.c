/*	gen_nick.c
	Author: William Woodruff
	------------------------
	Generates a pseudorandom nickname for the IRC server.
	Format: 10 characters, "et#######\0".
*/


#include <time.h>
#include <stdlib.h>
#include <string.h>

const char* gen_nick(void)
{
	srand(time(NULL));
	char nick[10];
	snprintf(nick, 10, "%s%d", "et", rand() % 10000000);
	return nick;
}