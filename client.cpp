#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT_NUMBER 5437

typedef enum _opmode
{
    manual,
    automatic
} opmode_t;

void setconn_addr(char *fname, char *ip, int *port, int *timeout)
{
    FILE *fp;
    if ((fp = fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "Could not open %s\n", fname);
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    read = getline(&line, &len, fp); // [Connection]
    line = NULL;

    for (int i = 0; i < 3; i++)
    {
        if ((read = getline(&line, &len, fp)) != '\n')
        {
            strtok(line, " = ");
            if (i == 0)
            {
                strcpy(ip, strtok(NULL, line) + 2);
                line = NULL;
                continue;
            }
            else if (i == 1)
            {
                *port = atoi(strtok(NULL, line) + 2);
                line = NULL;
                continue;
            }
            else
            {
                *timeout = atoi(strtok(NULL, line) + 2);
            }
        }
    }

    ip[strlen(ip) - 1] = '\0';

    if (line)
        free(line);
    fclose(fp);
}

void run_client(int sockfd, int rows, int cols, int seats, int ppid, opmode_t mode)
{
    srand(time(NULL)); // Seed the random number generator once

    while (seats > 0)
    {
        int user_row, user_col, nbytes = 0;
        char msg_buff[BUFFER_SIZE];
        bzero(msg_buff, sizeof(msg_buff));

        if (mode == manual)
        {
            printf("Enter the row your seat should be reserved (1 - %d): ", rows);
            scanf("%d", &user_row);
            printf("Enter the column your seat should be reserved (1 - %d): ", cols);
            scanf("%d", &user_col);
        }
        else if (mode == automatic)
        {
            user_row = (rand() % rows) + 1;
            user_col = (rand() % cols) + 1;
        }

        printf("\n[Client %d] sending reservation for seat on row %d col %d to the server\n", ppid, user_row, user_col);
        int payload[] = {user_row, user_col};

        if (write(sockfd, payload, sizeof(payload)) < 0)
        {
            fprintf(stderr, "\n[Client #%d Error(Payload)]: %s\n", ppid, strerror(errno));
        }

        nbytes = read(sockfd, msg_buff, sizeof(msg_buff) - 1);
        if (nbytes <= 0)
        {
            printf("\n[Client #%d Error ]: Read server reservation results\n", ppid);
            break;
        }
        msg_buff[nbytes] = '\0';

        printf("%s\n", msg_buff);

        int ack = 1;
        if (write(sockfd, &ack, sizeof(ack)) < 0)
            fprintf(stderr, "[Client %d Error (Write ACK)]: %s\n", ppid, strerror(errno));

        int rem_seats = -1;

        nbytes = read(sockfd, &rem_seats, sizeof(rem_seats));
        if (nbytes <= 0)
        {
            fprintf(stderr, "\n[Client %d Error (Read reserved)]: %s\n", ppid, strerror(errno));
            break;
        }

        if (rem_seats > -1)
        {
            seats = rem_seats;
            printf("\n[Client #%d] There are currently %d seats available\n", ppid, seats);
        }
        else
            printf("\n[Client #%d] Error retrieving seats left\n", ppid);

        if (mode == automatic)
        {
            int sleep_duration = (rand() % 5) + 1;
            printf("\n[Client %d (Automatic Mode)] : sleeping for %d seconds...\n", ppid, sleep_duration);
            printf("-------------------------------------------------------------------\n");
            sleep(sleep_duration);
        }
    }

    printf("\n[Client] All seats reserved. Closing connection\n");
}

opmode_t client_mode;

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[BUFFER_SIZE];
    struct sockaddr_in serv_addr;
    char ip[16] = "127.0.0.1"; // default IP
    int port = PORT_NUMBER;    // default port
    int timeout = 4;           // default timeout value

    if (argc >= 2)
    {
        if (!strcmp(argv[1], "manual"))
            client_mode = manual;
        else if (!strcmp(argv[1], "automatic"))
            client_mode = automatic;
        else // default to manual mode if cannot understand
            client_mode = manual;

        if (argc == 3)
        {
            printf("Will read creds from file\n");
            setconn_addr(argv[2], ip, &port, &timeout);
        }
    }
    else
    {
        fprintf(stderr, "Usage %s <mode> [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    for (int i = 0; i < timeout; i++)
    {
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            fprintf(stderr, "\nError : %s \n", strerror(errno));

            if (i == timeout - 1)
            {
                printf("\nConnection failed after %d attempts. Server may be offline\n", timeout);
                return 1;
            }
            printf("Retrying in 3 seconds\n");
            sleep(3);
        }
        else
        {
            printf("Successfully connected to the server\n");
            break;
        }
    }

    pid_t ppid = getpid();
    if (write(sockfd, &ppid, sizeof(ppid)) < 0)
        fprintf(stderr, "[Client #%d Error (pid)] %s\n", ppid, strerror(errno));

    n = read(sockfd, recvBuff, sizeof(recvBuff) - 1);
    if (n <= 0)
    {
        printf("\n[Client #%d Error ]: Read initial server message\n", ppid);
        return EXIT_FAILURE;
    }
    recvBuff[n] = '\0';
    printf("%s\n", recvBuff);

    int ack = 1;
    if (write(sockfd, &ack, sizeof(ack)) < 0)
        fprintf(stderr, "[Client %d Error (Write ACK)]: %s\n", ppid, strerror(errno));

    int availability[3] = {0, 0, 0};
    n = read(sockfd, availability, sizeof(availability));
    if (n <= 0)
    {
        printf("\n[Client %d]: Error Read Seat Availability\n", ppid);
        return EXIT_FAILURE;
    }

    if (availability[0] == 0 || availability[1] == 0 || availability[2] == 0)
    {
        printf("\n[Client #%d] Error reading in availabilities from the system\n", ppid);
        return EXIT_FAILURE;
    }

    run_client(sockfd, availability[0], availability[1], availability[2], ppid, client_mode);

    close(sockfd);

    return EXIT_SUCCESS;
}
