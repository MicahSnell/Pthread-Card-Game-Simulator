# Pthread Card Game Simulator
## Description
This program simulates a card game called "Pair War." This simulation was done using the POSIX thread library `pthread.h`. The program contains four external functions, dealerThread, playerThread, shuffle, and printDeck. One thread was assigned to the dealerThread function, and three threads were assigned to the playerThread function. The other two functions provided functionality with the deck of cards. The shuffle function created the deck of cards by placing fifty-two random integers with values ranging from one to thirteen into a queue. The deck of cards was implemented using a queue because of the need for first-in first-out interaction as well as dynamic size. The values 1, 11, 12, and 13 were used to represent the Ace, Jack, Queen, and King cards, respectively. The shuffle function only allowed four of each value from one to thirteen to be placed in the queue, which properly simulates a genuine deck of cards. The main function was responsible for seeding the random number generator with the users argument, as well as creating and closing of the output file and threads. The dealer and each player output information for the user in two places. Detailed output about each action taken is placed in a log
file titled "log.txt," and an overview of who each round and what card(s) each player had in their hands is printed to screen. 

## How to Build
Navigate inside the `src` folder and type:
```bash
make
```
To build the program using the provided `makefile`

## How to Run
Run the program with:
```bash
./play <positive integer>
```
Where the positive integer is a seed that will be used by the pseudo-random number generator. Notice the variance in the lengths of each round based on the given seed.
