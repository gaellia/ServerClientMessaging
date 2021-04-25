# ServerClientMessaging
The second project in CPSC 441 where we send encrypted messages.

## Compiling, Configuring and Using the Server
Server should be set up before using the client
  1. Open a terminal in the folder where the file “cryptoserver.c” is located.
  2. Type “gcc cryptoserver.c -o server” and press enter.
  3. To run the server, type “./server”.
  4. The resulting menu will ask for the hash scheme to use for this session. Select from options 1-3. Defaults to Sequential Index hashing.
  5. To finish setting up the server, press 0 to continue. The server is now ready to accept a client.

## Compiling, Configuring and Using the Client
  1. Open a terminal in the folder where the file “cryptoclient.c” is located.
  2. Type “gcc cryptoclient.c -o client” and press enter.
  3. To run the client, type “./client”.
  4. It will ask the user to choose 1 – Enter a tweet or 0 – Exit the program.
  5. When the user chooses to enter a tweet, they can type any message to a max of 140 characters to send to the server. Press enter when finished.
  6. The client will receive either the encoded or decoded message from the server’s dictionaries and the previous prompt to continue “tweeting” is displayed.

## Design
The protocol used was TCP. This was chosen to ensure that the message is reliably delivered both
ways as the client will need to connect to the server many times to decode/encode their tweets. The
message should be in tact, in order and complete when delivered back to the client, regardless of the
time it takes. For something like streaming UDP would be preferred because delay should be minimized,
but in this case, although the size of code is larger, TCP can deliver an entire message. A TCP
implementation may also be easier to port as it is more widely used.

## Features and Non-Features
  • Splits the words in the tweet ignoring punctuation. However, this means that all punctuation is lost and contractions such as “don’t” will be considered two words “don” and “t”
  • Not multi-threaded which means only one client can connect
  • Dictionaries are lost after the server goes down
  • Only one hash scheme can be selected per session
  • All three hash schemes are implemented; you can choose one at the beginning of the server’s life
  • Client can send encoded or plain text messages to the server and receive an appropriate response

## Testing
Testing was done on the university campus computers in the Math Sciences computer lab on a single
machine using the loopback interface.
