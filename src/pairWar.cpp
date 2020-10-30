/**
 * File: pairWar.cpp
 *
 * Author: Micah Snell
 * Course: CS 4328 - Operating Systems
 * Program: #1
 * Due Date: 10/04/20
 *
 * This program simulates a card game called "Pair War." Pair war is a simple
 * game where players win by getting a matching pair of cards. The game starts
 * with a dealer shuffling the deck and giving each player a card in a clockwise
 * fashion. Once all players have been dealt their first card, the player who
 * received the first card starts the round by drawing a card from the top of
 * the deck. If the new card matches the card they currently hold, they declare
 * themselves the winner and the round ends. Otherwise the player discards one
 * of their two cards at random, and places it at the bottom of the deck. This
 * process repeats until someone wins. Each player will be given the chance to
 * start a round, so 1 game will have as many rounds as there are players.
 *
 * This simulation was done using the POSIX thread library "pthread.h." The deck
 * of cards was implemented using a queue because of the need for first-in
 * first-out interaction as well as dynamic size. The dealer and each player
 * output information for the user in two places. Detailed output about each
 * action taken is placed in a log file titled "log.txt," and an overview of who
 * won each round and what card(s) each player had in their hands is printed to
 * the screen.
 */
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <fstream>

using namespace std;

const int NUM_PLAYERS = 3;
const int NUM_ROUNDS = 3;

ofstream log;

int turnIndicator = 0, //indicates whos turn it is
    whosNext,          //indicates who goes next
    firstCard,         //holds the first card dealt to the players
    exitCount;         //indicates how many players have left

bool winner;           //indicates someone has won

queue<int> deck;       //the 'deck' of cards

pthread_mutex_t hold;

//PThread Functions
void *dealerThread(void *);
void *playerThread(void *);

//Deck manipulation functions
void shuffle();
void printDeck(ostream &);

/**
 * @desc main controls the setup of the simulation. First main
 *   declares all necessary variables and then opens the log file. Next main
 *   verifies that the correct number of arguments were passed by the user.
 *   Then the random number generator is initialized using the users argument.
 *   Next the threads are created and then joined, which is followed with
 *   closing the log file and finishing the program.
 * @param A number to seed the random number generator.
 * @return N/A.
 */
int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << endl << endl
         << "***Program Terminated***" << endl
	       << "Incorrect number of arguments, please provide a seed for the "
         << "random number generator." << endl
	       << "Ex: ./program1 42" << endl << endl;
    return -1;
  }

  pthread_t dealer,
            players[NUM_PLAYERS];

  long t, i = 0;

  log.open("log.txt");
  if (!log) {
    cout << endl << endl
         << "***Program Terminated***" << endl
         << "Log file failed to open." << endl << endl;

    return -1;
  }

  //seed the random number generator using the command line argument
  srand(atoi(argv[1]));

  pthread_mutex_init(&hold, NULL);

  pthread_create(&dealer, NULL, dealerThread, (void *)i);
  for (t = 0; t < NUM_PLAYERS; ++t)
    pthread_create(&players[t], NULL, playerThread, (void *)(t + 1));

  for (t = 0; t < NUM_PLAYERS; ++t)
    pthread_join(players[t], NULL);
  pthread_join(dealer, NULL);

  cout << "Execution complete." << endl
       << "Detailed results have been written to 'log.txt'" << endl;

  log.close();
  pthread_mutex_destroy(&hold);
  pthread_exit(NULL);
}

/**
 * @desc dealThread is the thread that simulates the dealer. It loops
 *   until all rounds have been played. It starts by setting all flag variables
 *   to inital values, and then shuffles the deck. Then it deals each player
 *   their first card. It then tells which player to start the round, and then
 *   waits for a winner, and its turn to reshuffle the deck and start the next
 *   round.
 * @param An unused ID variable.
 * @return N/A.
 */
void *dealerThread(void *t) {
  whosNext = 0;

  while (whosNext < NUM_ROUNDS) {
    int count = 0;
        winner = false;
        exitCount = 0;

    ++whosNext;

    shuffle();
    cout << endl
         << "Round " << whosNext << ", player " << whosNext << " starts."
         << endl;

    while (turnIndicator == 0 && count < NUM_PLAYERS) {
      pthread_mutex_lock(&hold);
      turnIndicator = whosNext;

      firstCard = deck.front();
      deck.pop();

      ++whosNext;
      if (whosNext > NUM_PLAYERS)
        whosNext = 1;
      ++count;
      pthread_mutex_unlock(&hold);

      while (turnIndicator != 0) {
        //wait
      }
    }

    turnIndicator = whosNext;
    while (!winner) {
      //wait for a winner
    }
    while (turnIndicator != 0) {
      //wait to exit
    }

    log << "DEALER: shuffle" << endl << endl;

    cout << "DECK: ";
    printDeck(cout);
  }

  pthread_exit((void *) 0);
}

/**
 * @desc playerThread is the thread that simulates the players
 *   in the card game. The function loops until all rounds have been played.
 *   The loop starts by receiving the first card from the dealer, then a loop
 *   begins that ends when a player wins.
 * @param A long value that represents the thread's ID number.
 * @return N/A.
 */
void *playerThread(void *t) {
  long threadID = (long)t;

  int hand[2] = {0, 0},
      random;

  while (whosNext < NUM_ROUNDS) {
    while (threadID != turnIndicator) {
      //wait
    }

    pthread_mutex_lock(&hold);
    hand[0] = firstCard;
    turnIndicator = 0;
    pthread_mutex_unlock(&hold);

    while (!winner) {
      while (threadID != turnIndicator) {
        //wait
      }
      if (winner) {
        log << "PLAYER " << threadID << ": exits round" << endl;

        cout << "PLAYER " << threadID << ":" << endl
	           << "HAND " << hand[0] << endl
	           << "WIN no" << endl;

          ++turnIndicator;
          ++exitCount;
          if (turnIndicator > NUM_PLAYERS)
	         turnIndicator = 1;
          if (exitCount == NUM_PLAYERS)
	         turnIndicator = 0;
          break;
      }

      pthread_mutex_lock(&hold);
      log << "PLAYER " << threadID << ": hand " << hand[0] << endl;

      hand[1] = deck.front();
      deck.pop();
      log << "PLAYER " << threadID << ": draws " << hand[1] << endl;

      if (hand[0] == hand[1]) {
        log << "PLAYER " << threadID << ": hand " << hand[0] << " "
            << hand[1] << endl
            << "PLAYER " << threadID << ": wins" << endl
            << "PLAYER " << threadID << ": exits round" << endl;

        cout << "PLAYER " << threadID << ":" << endl
	           << "HAND " << hand[0] << " " << hand[1] << endl
	           << "WIN yes" << endl;

        ++exitCount;
        winner = true;
      }
      else {
        random = rand() % 2;     //will be index 0 or 1

        log << "PLAYER " << threadID << ": discards " << hand[random] << endl;
        deck.push(hand[random]); //put card at bottom of deck
        hand[random] = 0;        //remove the card from hand

        if (hand[0] == 0) {      //keep valid card in index 0
          hand[0] = hand[1];
          hand[1] = 0;
        }

        log << "PLAYER " << threadID << ": hand " << hand[0] << endl;
        log << "DECK: ";
        printDeck(log);
      }

      ++turnIndicator;
      if (turnIndicator > NUM_PLAYERS)
        turnIndicator = 1;

      pthread_mutex_unlock(&hold);
    }
  }

  pthread_exit((void *) 0);
}

/**
 * @desc shuffle puts 52 random integers with values between 1 and 13
 *   in a queue called 'deck.' The values 1, 11, 12, and 13 represent the Ace,
 *   Jack, Queen, and King cards, respectively. The number 52 represents the
 *   number of cards in a standard deck.
 * @param None.
 * @return N/A.
 */
void shuffle() {
  bool addCard;
  int  cardCount[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
       card;

  while (deck.size() != 52) {
    addCard = true;

    card = rand() % 13 + 1;  // a random number between 1 and 13
    switch (card) {          // check how many there are of that card
      case 1:
        ++cardCount[0];
        if (cardCount[0] > 4) {
          --cardCount[0];
          addCard = false;
        }
        break;
      case 2:
        ++cardCount[1];
        if (cardCount[1] > 4) {
          --cardCount[1];
          addCard = false;
        }
        break;
      case 3:
        ++cardCount[2];
        if (cardCount[2] > 4) {
          --cardCount[2];
          addCard = false;
        }
        break;
      case 4:
        ++cardCount[3];
        if (cardCount[3] > 4) {
          --cardCount[3];
          addCard = false;
        }
        break;
      case 5:
        ++cardCount[4];
        if (cardCount[4] > 4) {
	        --cardCount[4];
	        addCard = false;
        }
        break;
      case 6:
        ++cardCount[5];
        if (cardCount[5] > 4) {
	        --cardCount[5];
	        addCard = false;
        }
        break;
      case 7:
        ++cardCount[6];
        if (cardCount[6] > 4) {
	        --cardCount[6];
	        addCard = false;
        }
        break;
      case 8:
        ++cardCount[7];
        if (cardCount[7] > 4) {
	        --cardCount[7];
	        addCard = false;
        }
        break;
      case 9:
        ++cardCount[8];
        if (cardCount[8] > 4) {
	        --cardCount[8];
	        addCard = false;
        }
        break;
      case 10:
        ++cardCount[9];
        if (cardCount[9] > 4) {
	        --cardCount[9];
	        addCard = false;
        }
        break;
      case 11:
        ++cardCount[10];
        if (cardCount[10] > 4) {
	        --cardCount[10];
	        addCard = false;
        }
        break;
      case 12:
        ++cardCount[11];
        if (cardCount[11] > 4) {
	        --cardCount[11];
	        addCard = false;
        }
        break;
      case 13:
        ++cardCount[12];
        if (cardCount[12] > 4) {
	        --cardCount[12];
	        addCard = false;
        }
        break;
      default:
        addCard = false;
        break;
    }

    if (addCard)        // if there are less than 4 of that card
      deck.push(card);  // add it to the deck
  }
}

/**
 * @desc printDeck prints the current contents of the deck of cards.
 * @param An output stream operator.
 * @return N/A.
 */
void printDeck(ostream &out) {
  int temp;

  for (int i = 0; i < deck.size(); ++i) {
    out << deck.front() << " ";
    temp = deck.front();
    deck.pop();
    deck.push(temp);
  }
  out << endl << endl;
}
