#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    // Variables for sockets
    int sockfd, portno, t, done;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char str[100];
    // Variables for vector Y and float a management
    float a;
    int n;
    int *Y = NULL;
    char choice;

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

        server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Trying to connect...\n");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    printf("Connected.\n");


    // Receive data from user and send them to socket-Server/RPC-Client
    while (1) {
        printf("\n\t\tMENU\n1. Average value of vector Y\n");
        printf("2. Calculation of max and min value of vector Y\n");
        printf("3. Product of a*Y\n");
        printf("q. To quit\n");
        
        do {
            printf("Type your choice (1, 2, 3 or q): ");
            scanf(" %c", &choice);
        } while ((choice != '1') && (choice != '2') && (choice != '3') && (choice != 'q'));

        // Send user choice to server
        send(sockfd, &choice, sizeof(char), 0);
        // ----------------------------------------------------------

        if (choice == 'q') {
            printf("Quit program...\n");
            if (Y != NULL) {
                free(Y);
            }
            close(sockfd);
            return 0;
        }

        // ----------------------------------------------------------

        if (choice == '3') {
            printf("Type float number a: ");
            scanf("%f", &a);
            send(sockfd, &a, sizeof(float), 0);
        }

        do {
            printf("Type number of vector Y elements (>0): ");
            scanf("%d", &n);
        } while (n <= 0);

        send(sockfd, &n, sizeof(int), 0);

        Y = (int *)malloc(n * sizeof(int));
        if (!Y) {
            printf("Memory allocation error...");
            close(sockfd);
            exit(1);
        }

        for (int i = 0; i < n; i++) {
            printf("Type (%d) element: ", i+1);
            scanf("%d", &Y[i]);
        }
        // Send vector Y to server
        send(sockfd, Y, n*sizeof(int), 0);
        printf("Data sent to socket server...");

        if (choice == '1') {
            printf("1");
        }

        // ----------------------------------------------------------

        else if (choice == '2') {
            printf("2");

        }

        else if (choice == '3') {
            printf("3");
        }

        free(Y);
        Y = NULL;

        
    }
    
    return 0;
}