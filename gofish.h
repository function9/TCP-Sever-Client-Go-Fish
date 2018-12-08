#ifndef GOFISH_H
#define GOFISH_H
#include "player.h"
/*
 * Function: print_hand
 * ---------------------
 *  prints the player's hand. 
 *
 *  target: the target player
 *  
 */
void print_hand(struct player* target, int connfd);
/*
 * Function: print_book
 * ---------------------
 *  prints the player's hand. 
 *
 *  target: the target player
 *  player: 1 for player 1, 2 for player 2
 */
void print_book(struct player* target, int player, int connfd);
/*
 * Function: user_guess
 * ---------------------
 *  helper function that calls transfer_cards if user guesses
 *  correctly
 *
 *  guess: character that user is guessing
 */
void user_guess(char guess, int connfd);
/*
 * Function: user_guess
 * ---------------------
 *  helper function that calls transfer_cards if computer guesses
 *  correctly
 *
 *  guess: character that computer is guessing
 */
void computer_guess(char guess, int connfd);


#endif
