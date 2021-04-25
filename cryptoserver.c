// TCP-based crypto server
// This server interacts with the clients
// Make sure the crypto server is running before a client is connected
// Modified from wordlengthserver.c

// by Kevin Vo
// Oct. 16, 2018
// UCID: 30003154

// @@@@@@@@@@@@ Bonus Decode
// "0xe124 0x968c 0xb1b0 0x4848 0x2831 0x5d60 0x4848!" decodes to
// "There is NO business like SHOW business!"

// Include libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>

// Constants
#define MAX_TWEET_LENGTH 140
#define PORTNUM 8675
#define PRIME 200003 // Chosen to be greater than the number of English words

// Menu items
#define SEQ_INDEX 1
#define WORD_SUM 2
#define MY_HASH 3
#define OK 0

int connected_sock;

// Signal handler
void catcher( int sig )
  {
    close(connected_sock);
    exit(0);
  }

// Displays menu for server
void menu() {
    printf("\n");
    printf("Please choose from the following selections:\n");
    printf("  1 - Change hash to Sequential Index\n");
    printf("  2 - Change hash to Word Sum\n");
    printf("  3 - Change hash to My Cool Hash\n");
    printf("  0 - Continue\n");
    printf("What do you want? ");
}

// Dictionary Structure --------------------------------------------
typedef struct dictEntry {
    char *key;
    char *word;
} dictEntry;

typedef struct dict_s {
    int len;
    int max;
    dictEntry *entry;
} dict_s, *dict_t;

// Finds index of the key (the encoding) if it is in the dictionary
int findIndexKey(dict_t dict, char *key) {
    for (int i = 0; i < dict->len; i++) {
        if (!strcmp(dict->entry[i].key, key)) {
            return i;
        }
    }
    return -1; // return -1 if not in dictionary
}

// Finds index of the word (the decoding) if it is in the dictionary
int findIndexWord(dict_t dict, char *word) {
    for (int i = 0; i < dict->len; i++) {
        if (!strcmp(dict->entry[i].word, word)) {
            return i;
        }
    }
    return -1; // return -1 if not in dictionary
}

// Adds a word into the dictionary if it does not exist in the dictionary
void add(dict_t dict, char *key, char *value) {
   int idx = findIndexWord(dict, value);
   if (idx == -1) {
	   dict->entry[dict->len].key = strdup(key);
	   //fprintf(stderr, "%s\n", dict->entry[dict->len].key);
	   dict->entry[dict->len].word = strdup(value);
	   //fprintf(stderr, "%s\n", dict->entry[dict->len].word);
	   dict->len++;
   }
   if (dict->len == dict->max) {
       dict->max *= 2;
       dict->entry = realloc(dict->entry, dict->max * sizeof(dictEntry));
   }
   return;
}

// Creates a new dictionary
dict_t newDict(void) {
    dict_s defaultDict = {0, 10, malloc(10 * sizeof(dictEntry))};
    dict_t d = malloc(sizeof(dict_s));
    *d = defaultDict;
    return d;
}
// ------------------------------------------------------------

// main for server
int main() {
    struct sockaddr_in server;
    static struct sigaction act;
    char messagein[MAX_TWEET_LENGTH];
    char messageout[MAX_TWEET_LENGTH*8];
    int listening_sock;
    int choice;
    int hash = 1; // default to sequence index hashing

    // Create three dictionaries
    dict_t seqDict = newDict();
    dict_t wordDict = newDict();
    dict_t myDict = newDict();

    // Initialize messages
    bzero(messagein, MAX_TWEET_LENGTH);
    bzero(messageout, MAX_TWEET_LENGTH*8);

    // Setup Signal Handler
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    // Initialize Server
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // Setup socket
    if((listening_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error in socket() call.\n");
		exit(1);
    }

    // Bind the socket to address and port
    if(bind(listening_sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) < 0) {
		printf("Error in bind() call.\n");
		exit(1);
    }

    // listen for connections from clients
    if (listen(listening_sock, 5) == -1) {
		fprintf(stderr, "Error in listen() call.\n");
		exit(1);
    }

    printf("Welcome! I am the all encoding/decoding crypto-server!!\n\n");
    printf("Listening on port %d\n\n", PORTNUM);

    // Print menu
    printf("The current hashing choice is %d.\n", hash);
    menu();
    scanf("%d", &choice);

    // Display menu loop to choose hash options
    while(choice != OK) {
    	if (choice == SEQ_INDEX) {
    		hash = 1;
    	} else if (choice == WORD_SUM) {
    		hash = 2;
    	} else if (choice == MY_HASH) {
    		hash = 3;
    	} else {
    		printf("Invalid menu selection. Please try again.\n");
    	}

        printf("\nThe current hashing choice is %d.\n", hash);
        menu();
		scanf("%d", &choice);
    }

    fprintf(stderr, "\nNow accepting clients!\n\n");

    // Main loop
    while (1) {

    	// Accept Connection
    	if((connected_sock = accept(listening_sock, NULL, NULL)) < 0) {
    	    printf("Error in accept() call.\n");
    	    exit(1);
    	}

		// Receive message from client
		while(recv(connected_sock, messagein, MAX_TWEET_LENGTH, 0) > 0) {
			// Print the received message
			fprintf(stderr, "Received tweet: %s\n\n", messagein);

			// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			char *token = strtok(messagein, "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ "); //all punctuation is taken away
			char *words[MAX_TWEET_LENGTH];
			int i = 0;
			int j = 0;

			// separate all the words
			while (token != NULL) {
				words[i] = token;
				token = strtok(NULL, "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ ");
				i++;
			}

			// Create outgoing message

			// check if tweet is encoded
			if (strstr(words[0], "0x") != NULL) {
				// Decode stuff

				// check which hash dict to use
				dict_t aDict = seqDict; // Default to sequence dictionary

				if (hash == SEQ_INDEX) {
					aDict = seqDict;
				} else if (hash == WORD_SUM) {
					aDict = wordDict;
				} else if (hash == MY_HASH) {
					aDict = myDict;
				} else {
					fprintf(stderr, "Something went wrong... No hash scheme available.");
				}

				while (j < i) {

					// check if key is in the dictionary
					if (findIndexKey(aDict, words[j]) == -1) {
						strcat(messageout, "\nKey is not in the dictionary.\n");
						break;
					} else {
						strcat(messageout, aDict->entry[findIndexKey(aDict, words[j])].word);
						strcat(messageout, " "); // Create encoding message now
						j++;
					}
				}

			} else {
				// Encode stuff

				// check which hash to use
				if (hash == SEQ_INDEX) {
					int k = 1;
					char key[16];

					// Go through all the words
					while (j < i) {
						// check if word is in the dictionary
						if (findIndexWord(seqDict, words[j]) == -1) {
							// create a key for the word using the sequence hash
							sprintf(key, "0x%x", k);
							while (findIndexKey(seqDict, key) != -1) { // Increase sequence number key if already in seqDict
								k++;
								sprintf(key, "0x%x", k);
							}
								add(seqDict, key, words[j]); // Insert into dictionary
						}
						strcat(messageout, seqDict->entry[findIndexWord(seqDict, words[j])].key);
						strcat(messageout, " "); // Create encoding message now
						j++;

						// Clear buffers
						bzero(key, 16);
					}

				} else if (hash == WORD_SUM) {
					int sum = 0;
					char key[16];

					// Go through all the words
					while (j < i) {
						// check if word is in the dictionary
						if (findIndexWord(wordDict, words[j]) == -1) {
							// create a key for the word using word sum hash
							char word[strlen(words[j])];
							strcpy(word, words[j]);

							for (int k = 0; k < strlen(word); k++) {
								sum += word[k];
							}
								sprintf(key, "0x%04x", sum);
								add(wordDict, key, words[j]); // Insert into dictionary
															  // Note that it will overwrite the stored word
															  // if there is a collision
						}
						strcat(messageout, wordDict->entry[findIndexWord(wordDict, words[j])].key);
						strcat(messageout, " "); // Create encoding message now
						j++;

						// Clear buffers
						bzero(key, 16);
						sum = 0;
					}

				} else if (hash == MY_HASH) {
					/* based off of FNV hash function
					 *
					 * This hash function considers all the characters in the word as well as it's length.
					 * The last 32 bits are stored as the key.
					 * 32-bits as the key were chosen to ensure that it could contain all the english words in the dictionary
					 * The prime number considered was also chosen to be higher than the number of all english words in the dictionary
					 * It uses some chaining from the previous char in the string to create the key.
					 * Strengths:
					 * 		 - It has much less collisions than word sums (on a few tests of large dictionaries)
					 *  		This means that decoding from the encoded message will be more accurate and more deterministic
					 *  	- The key size is the same at 32 bits unlike Seq Indexing
					 *  	- Simple and fast to implement
					 *  Weaknesses:
					 *  	- The key size may be too large for the actual number of English words in common usage
					 *  	- Words with the same size and beginnings such as "win" and "wig" have similar keys
					 *  	- Not much security in these keys if you want to send confidential tweets
					*/
					char key[32];

					// Go through all the words
					while (j < i) {
						// check if word is in the dictionary
						if (findIndexWord(myDict, words[j]) == -1) {
							// create a key for the word using my hash
							char word[strlen(words[j])];
							strcpy(word, words[j]);

							// the hash will take the length of the words to start
							int hash = strlen(word);

							// for every character in the word, multiply by a prime
							// and XOR with the ASCII char of the word
							// chain the result for the length of the word
							for (int k = 0; k < strlen(word); k++) {
								hash = hash*PRIME;
								hash = hash ^ word[k];
							}
							sprintf(key, "0x%08x", hash); // resulting key will be 32 bits
							add(myDict, key, words[j]); // Insert into dictionary
														// Note that it will overwrite the stored word
														// if there is a collision
						}
						strcat(messageout, myDict->entry[findIndexWord(myDict, words[j])].key);
						strcat(messageout, " "); // Create encoding message now
						j++;

						// Clear buffers
						bzero(key, 32);
					}

				} else {
					fprintf(stderr, "Something went wrong... No hash scheme available.");
				}
			}

			fprintf(stderr, "The encoded/decoded tweet is: \n");
			fprintf(stderr, "%s\n\n", messageout);

			// Send result back to client
			send(connected_sock, messageout, strlen(messageout), 0);

			// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

			// Clear out the message
			bzero(messagein, MAX_TWEET_LENGTH);
			bzero(messageout, MAX_TWEET_LENGTH*8);
		}

		// Close the connected socket
		close(connected_sock);
	}
    // Close the listening socket
    close(listening_sock);
    return 0;
}
