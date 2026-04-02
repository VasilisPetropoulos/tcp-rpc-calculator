#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// Error handling
void error(char *msg) {
    perror(msg);
    exit(1);
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
    // Variables that initialize results
    float avg_res;  // Use for choice 1
    int max_min_res[2]; // Use for choice 2
    float *Y_res;   // Use for choice 3

    // Check if user has put hostname and port arguments
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    // Initialize port number and sockfd with user's values
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Check socket
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    // Get socket name
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // Set struct to connect to TCP server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Trying to connect...\n");

    // Try to connect to socket server
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

        // Termination choice
        if (choice == 'q') {
            printf("Quit program...\n");
            if (Y != NULL) {
                free(Y);
            }
            close(sockfd);
            return 0;
        }
        
        // ----------------------------------------------------------

        // Ask user for float number a
        if (choice == '3') {
            printf("Type float number a: ");
            scanf("%f", &a);
            send(sockfd, &a, sizeof(float), 0);
        }

        // Ask user for number of elements of Y
        do {
            printf("Type number of vector Y elements (>0): ");
            scanf("%d", &n);
        } while (n <= 0);

        // Send number of elements n to socket server
        send(sockfd, &n, sizeof(int), 0);

        // Allocate memory for vector Y
        Y = (int *)malloc(n * sizeof(int));
        if (!Y) {
            printf("Memory allocation error...");
            close(sockfd);
            exit(1);
        }

        // Initialize vector Y with user's values
        for (int i = 0; i < n; i++) {
            printf("Type (%d) element: ", i+1);
            scanf("%d", &Y[i]);
        }
        // Send vector Y to server
        send(sockfd, Y, n*sizeof(int), 0);

        printf("Data sent to socket server...");

        if (choice == '1') {
            // Read from tcp server (rpc client) avg_res
            read(sockfd, &avg_res, sizeof(float));
            printf("\nAverage of vector Y is: %.3f\n", avg_res);
        }

        // ----------------------------------------------------------

        else if (choice == '2') {
            // Read from tcp server (rpc client) max_min_res
            read(sockfd, max_min_res, 2*sizeof(int));
            printf("\nMax: %d\tMin: %d\n", max_min_res[0], max_min_res[1]);
        }

        else if (choice == '3') {
            // Allocate memory for Y_res
            Y_res = (float *)malloc(n*sizeof(float));
            if (!Y_res) {
                printf("Memory allocating error...");
                exit(1);
            }

            // Read from tcp server (rpc client) Y_res
            read(sockfd, Y_res, n*sizeof(float));
            printf("\nProduction of a*Y:\n[");
            for (int i = 0; i < n-1; i++) {
                printf("%.3f\t", Y_res[i]);
            }
            printf("%.3f]\n", Y_res[n-1]);

            free(Y_res);
        }

        free(Y);
        Y = NULL;
    }
    
    return 0;
}