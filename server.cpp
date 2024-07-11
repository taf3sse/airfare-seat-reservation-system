#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define PORT_NUMBER 5437
#define MAX_CLIENTS 10

int client_sockets[MAX_CLIENTS];
int seat_map[10][10]; // 10x10 seat map for simplicity
int available_seats = 100;

void initialize_seat_map()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			seat_map[i][j] = 0;
		}
	}
}

void print_seat_map()
{
	printf("[ --- Airfare selling system---]\n");
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			printf("%d ", seat_map[i][j]);
		}
		printf("\n");
	}
	printf("[ -----------------------------]\n");
}

void handle_client(int client_socket)
{
	int ppid;
	if (read(client_socket, &ppid, sizeof(ppid)) <= 0)
	{
		perror("Failed to read client pid");
		return;
	}
	printf("[Server]: Client %d connected\n", ppid);

	char msg[BUFFER_SIZE];
	snprintf(msg, sizeof(msg), "[Server] The system has %d seats available out of 10 rows and 10 cols!\n", available_seats);
	if (write(client_socket, msg, strlen(msg)) < 0)
	{
		perror("Failed to send initial message");
		return;
	}

	int ack;
	if (read(client_socket, &ack, sizeof(ack)) <= 0)
	{
		perror("Failed to receive an ACK handshake");
		return;
	}

	int availability[3] = {10, 10, available_seats};
	if (write(client_socket, availability, sizeof(availability)) < 0)
	{
		perror("Failed to send seat availability");
		return;
	}

	while (available_seats > 0)
	{
		int seat_request[2];
		if (read(client_socket, seat_request, sizeof(seat_request)) <= 0)
		{
			perror("Failed to read seat request");
			return;
		}

		int row = seat_request[0];
		int col = seat_request[1];
		if (row < 1 || row > 10 || col < 1 || col > 10 || seat_map[row - 1][col - 1] != 0)
		{
			snprintf(msg, sizeof(msg), "[Server] Seat on row %d col %d unavailable for Client %d\n", row, col, ppid);
		}
		else
		{
			seat_map[row - 1][col - 1] = 1;
			available_seats--;
			snprintf(msg, sizeof(msg), "[Server] Seat on row %d col %d reserved for Client %d\n", row, col, ppid);
			print_seat_map();
		}

		if (write(client_socket, msg, strlen(msg)) < 0)
		{
			perror("Failed to send reservation status");
			return;
		}

		if (read(client_socket, &ack, sizeof(ack)) <= 0)
		{
			perror("Failed to receive an ACK handshake");
			return;
		}

		if (write(client_socket, &available_seats, sizeof(available_seats)) < 0)
		{
			perror("Failed to send remaining seats");
			return;
		}
	}

	close(client_socket);
	printf("[Server]: Client %d disconnected\n", ppid);
}

void sigint_handler(int sig)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (client_sockets[i] != 0)
		{
			close(client_sockets[i]);
		}
	}
	exit(0);
}

int main()
{
	signal(SIGINT, sigint_handler);

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		perror("Failed to create socket");
		return 1;
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT_NUMBER);

	if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Failed to bind socket");
		close(server_socket);
		return 1;
	}

	if (listen(server_socket, MAX_CLIENTS) < 0)
	{
		perror("Failed to listen on socket");
		close(server_socket);
		return 1;
	}

	printf("Server is running on port %d\n", PORT_NUMBER);

	initialize_seat_map();
	print_seat_map();

	while (1)
	{
		int client_socket = accept(server_socket, NULL, NULL);
		if (client_socket < 0)
		{
			perror("Failed to accept client connection");
			continue;
		}

		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (client_sockets[i] == 0)
			{
				client_sockets[i] = client_socket;
				break;
			}
		}

		handle_client(client_socket);
		// print_seat_map();
	}

	close(server_socket);
	return 0;
}
