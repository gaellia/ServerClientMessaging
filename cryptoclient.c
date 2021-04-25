// TCP-based crypto client
// This client interacts with the crypto server.
// Make sure the crypto server is running before using this client
// Modified from wordlengthclient.c

// by Kevin Vo
// Oct. 16, 2018
// UCID: 30003154

// Include libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

// Constants
#define MAX_HOSTNAME_LENGTH 64
#define MAX_TWEET_LENGTH 140
#define PORTNUM 8675

// Menu items
#define DONE 0
#define ENTER 1

// Displays menu for client
void menu() {
    printf("\n");
    printf("Please choose from the following selections:\n");
    printf("  1 - Enter a tweet\n");
    printf("  0 - Exit program\n");
    printf("What do you want? ");
}

// Main function for client
int main() {
    int sock;
    char c;
    struct sockaddr_in server;
    struct hostent *address;
    char hostname[MAX_HOSTNAME_LENGTH];
    char message[MAX_TWEET_LENGTH];
    char messageback[MAX_TWEET_LENGTH*8];
    int choice, len, bytes;

    // Setup Server
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORTNUM);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Creating the client socket
    if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error in socket()\n");
		exit(-1);
    }

    // Connect to the server
    if(connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
		printf("Error in connect()\n");
		exit(1);
    }

    // Print menu
    menu();
    scanf("%d", &choice);

    // main loop: read a tweet (max of 140 characters),
    // send to server the message
    // print answer received
    while(choice != DONE) {
    	if (choice == ENTER) {

			// get the character from the client
			c = getchar();

			printf("Enter your tweet (max 140 characters): ");
			len = 0;
			while( (c = getchar()) != '\n' ) {
				message[len] = c;
				len++;
			}
			// null terminate the message
			message[len] = '\0';

			// Check the length of the tweet
			if (strlen(message) > 140) {
				printf("Your tweet was too long\n");
				exit(1);
			}

			// send the message to the server
			send(sock, message, len, 0);

			// receive message from server
			if ((bytes = recv(sock, messageback, MAX_TWEET_LENGTH*8, 0)) > 0) {
				// make the message null terminated
				messageback[bytes] = '\0';
				printf("Message received from server: \n");
				printf("%s\n", messageback);

				// Clear out the message
				bzero(messageback, MAX_TWEET_LENGTH*8);
			}
			else {
				// Error in case server dies
				printf("Server failed!\n");
				close(sock);
				exit(1);
			}

    	}
    	else {
    		printf("Invalid menu selection. Please try again.\n");
    	}

		menu();
		scanf("%d", &choice);
    }

    // End program
    close(sock);
    exit(0);
}
