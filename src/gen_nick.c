/*	gen_nick.c
	Author: William Woodruff
	------------------------
	Generates a pseudorandom nickname for the IRC server.
	Format: 10 characters, "et#######\0".
*/


#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*	gen_nick
	generates a pseudorandom nickname.
	arg: char *nick, a 10-character string to fill.
*/
void gen_nick(char *nick)
{
	srand(time(NULL));
	#ifdef _WIN32
		sprintf(nick, "et%d", rand() << 8 );
	#else
		snprintf(nick, 10, "et%d", rand() % 10000000);
	#endif
}
