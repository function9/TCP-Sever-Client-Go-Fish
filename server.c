/* 
 * echoservert.c - A concurrent echo server using threads
 */
/* $begin echoservertmain */
#include "csapp.h"
#include "deck.h"
#include "card.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "gofish.h"
#include <string.h>
char translate[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
size_t deckSize = 52;
void *thread(void *vargp);

void echo(int connfd)
{  
	char buff[80];  
	char check;
	player_again:
        read(connfd, buff, sizeof(buff)); 
	if (buff[0] != NULL) {
		user_guess(buff[0], connfd);
		check = check_add_book(&user);
		if(check != 0){
			transfer_cards(&user, &garbage, check);
			transfer_cards(&user, &garbage, check);
			transfer_cards(&user, &garbage, check);
			if(user.hand_size == 1){
				add_card(&user,next_card());
			}
			transfer_cards(&user, &garbage, check);
			printf("Player 1 books ");
			printf("%c\n", check);
			printf("Player 1 gets another turn\n");
			goto player_again;
		}
		computer_again:
		computer_guess(computer_play(&computer), connfd);
		check = check_add_book(&computer);
		if(check != 0){
			transfer_cards(&computer, &garbage, check);
			transfer_cards(&computer, &garbage, check);
			transfer_cards(&computer, &garbage, check);
			if(computer.hand_size == 1){
				add_card(&computer,next_card());
			}
			transfer_cards(&computer, &garbage, check);	
			printf("Player 2 books ");
			printf("%c\n", check);
			printf("Player 2 gets another turn\n");
			goto computer_again;
		}
		print_hand(&user, connfd);
    		print_book(&user, 1, connfd); 
		print_book2(&computer, connfd);
		if(game_over(&user) == 1){
			char buff[] = "Player 1 Wins";
			write(connfd, buff, sizeof(buff)); 
			char end[] = "~~";
			write(connfd, end, sizeof(end)); 
			exit(0);
		}
		if(game_over(&computer) ==1){
			char buff[] = "Computer Wins";
			write(connfd, buff, sizeof(buff));
			char end[] = "~~";
			write(connfd, end, sizeof(end));  
			exit(0);

		}
	}
	end(connfd);
}

int main(int argc, char **argv) 
{
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }


    shuffle();
    user.card_list = (struct hand*) malloc(sizeof(struct hand));
    computer.card_list = (struct hand*) malloc(sizeof(struct hand));
    garbage.card_list = (struct hand*) malloc(sizeof(struct hand));	
    deal_player_cards(&user);
    deal_player_cards(&computer);
    listenfd = Open_listenfd(argv[1]);
    while (1) {
	
        clientlen=sizeof(struct sockaddr_storage);
	connfdp = Malloc(sizeof(int)); //line:conc:echoservert:beginmalloc
	*connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen); //line:conc:echoservert:endmalloc
	Pthread_create(&tid, NULL, thread, connfdp);
	if(listenfd == 3){	
    	printf("Go Fish Is Ready To Start\n");
    	}
	
    }
}

/* Thread routine */
void *thread(void *vargp) 
{  
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self()); //line:conc:echoservert:detach
    Free(vargp); 
	print_hand(&user, connfd);
    	print_book(&user, 1, connfd);
	print_book2(&computer, connfd);
 	end(connfd);
    while(1){	
    	echo(connfd);
    }
    printf("Closing Connection");
    Close(connfd);
    return NULL;
}
void end(int connfd){
	char buff[2] = "*\n";
	write(connfd, buff, sizeof(buff)); 

}
int shuffle(){
	printf("Shuffling deck...\n");
	printf("Done");
	printf("\n");
	deck_instance.top_card = 0;
	char s = 'C';
	int count = 0;
	for(int x = 0; x < 52; x++){
		if(x == 13){
			s = 'D';
		}
		if(x == 26){
			s = 'H';
		}
		if(x == 39){
			s = 'S';
		}
		deck_instance.list[x].suit = s;
		deck_instance.list[x].rank = count;
		count++;
		if(count > 12){
			count = 0;
		}
	}
	srand(time(NULL));
	for(int i = 51; i > 0; i--){
		int j = rand() % (i+1);
		swap(&deck_instance.list[i], &deck_instance.list[j]);
	}
	return 0;
}

void swap(struct card *a, struct card *b){
	struct card temp = *a;
	*a = *b;
	*b = temp;
}
size_t deck_size(){
	return deckSize;
}

struct card* next_card(){
	struct card* temp = &(deck_instance.list[deck_instance.top_card]);
	struct card *save = malloc(sizeof(struct card));
	*save = *temp;
	deck_instance.list[deck_instance.top_card].rank = -1;
	deck_instance.list[deck_instance.top_card].suit = ' ';
	deckSize--;
	deck_instance.top_card++;
	return save;
}

int deal_player_cards(struct player* target){
	for(int i = 0; i < 7; i++){
		add_card(target, next_card());
	}
	return 1;
}
void print_hand(struct player* target, int connfd){
	struct hand* current = target->card_list;
	//printf("Player 1's Hand - ");
	char buff[200] = "Player 1's Hand - ";
	int n = 18;
	
	while(current->next != NULL){
		//printf("%c",translate[current->top.rank]);
		buff[n] = translate[current->top.rank];
		n++;
		//printf("%c",current->top.suit);
		buff[n] = current->top.suit;
		n++;
		//printf(" ");
		buff[n] = ' ';
		n++;
		current = current->next;
	}
	buff[n] = '\0';
	
	//printf("\n");
	write(connfd, buff, sizeof(buff)); 
}

void print_book(struct player* target, int player, int connfd){
	char buff[200] = "Player 1's Book - ";
	int x = 0;
	int n = 18;
	while( target->book[x] == 'A' || target->book[x] == '2' || target->book[x] == '3' || target->book[x] == '4' || target->book[x] == '5' || target->book[x] == '6' || target->book[x] == '7' || target->book[x] == '8' || target->book[x] == '9' || target->book[x] == 'T' || target->book[x] == 'J' || target->book[x] == 'Q' || target->book[x] == 'K'){
		if(player == 1){
			buff[n] = target->book[x];
			n++;
			buff[n] = ' ';
			n++;
		}
		//printf("%c", target->book[x]);
		//printf(" ");
		x++;
	}
	buff[n] = '\0';
	write(connfd, buff, sizeof(buff)); 
	//printf("\n");
}
void print_book2(struct player* target, int connfd){
	char buff[200] = "Player 2's Book - ";
	int x = 0;
	int n = 18;
	while( target->book[x] == 'A' || target->book[x] == '2' || target->book[x] == '3' || target->book[x] == '4' || target->book[x] == '5' || target->book[x] == '6' || target->book[x] == '7' || target->book[x] == '8' || target->book[x] == '9' || target->book[x] == 'T' || target->book[x] == 'J' || target->book[x] == 'Q' || target->book[x] == 'K'){
			buff[n] = target->book[x];
			n++;
			buff[n] = ' ';
			n++;
		//printf("%c", target->book[x]);
		//printf(" ");
		x++;
	}
	buff[n] = '\0';
	write(connfd, buff, sizeof(buff)); 
	//printf("\n");
}
int add_card(struct player* target, struct card* new_card){
	struct hand* current = target->card_list;
	if(current != target->card_list) return 0;
	while(current->next != NULL){
		current = current->next;
	}
	current->top = *new_card;
	current->next = (struct hand*) malloc(sizeof(struct hand));
	target->hand_size++;
	return 0;
}

int remove_card(struct player* target, struct card* old_card){
	struct hand* temp = target->card_list;
	struct hand* prev = target->card_list;
	int count = 0;
	while(temp->next != NULL){
		if(temp->top.rank == old_card->rank && temp->top.suit == old_card->suit){
			break;
		}
		else{
			count++;
			prev = temp;
			temp= temp->next;
		}
	}
	if(count == 0){
		target->card_list = target->card_list->next;
		target->hand_size--;
		return 0;
	}
	if(temp == NULL) return 1;	
	prev->next = temp->next;
	target->hand_size--;
	return 0;
}

int search(struct player* target, char rank){
	char translate[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
	struct hand* current = target->card_list;
	if(current == NULL){
		return 0;
	}
	while(current->next != NULL){
		if(translate[(current->top.rank)] == rank){
			return 1;
		}
		current = current->next;
	}
	return 0;
}

int game_over(struct player* target){
	int count = 0;
		while(target->book[count] != NULL){
			count++;
		}
	if(count > 4){
		return 1;
	}
	return 0;
}

char check_add_book(struct player* target){
	int ranks[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	char translate[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
	int count = 0;
	struct hand* current; 
	for(int i = 0; i < 13; i++){
		count = 0;
		current = target->card_list;
		while(current != NULL){
			if(current->top.rank == ranks[i]){
				count++;
			}		
			current = current->next;
		}
		if(count == 4){
			int x = 0;
			while(target->book[x] != NULL){
				x++;
			}
			target->book[x] = translate[ranks[i]];
			return target->book[x];	
		}
	}
	return 0;
}

int transfer_cards(struct player* src, struct player* dest, char rank){
	int z = search(src, rank);
	if(z == 0){
		return 0;
	}
	char translate[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
	struct hand* current = src->card_list;
	while(current->next != NULL){
		if(translate[current->top.rank] == rank) break;
		current = current->next;
	}
	if(current == NULL) return 0;
	if(translate[current->top.rank] != rank){
		return 0;
	}	
	add_card(dest, &(current->top));
	remove_card(src, &(current->top));
	return 1;
}
int reset_player(struct player* target){
	//iterate through card_list and free everything
	target->card_list = NULL;
	target->card_list = (struct hand*) malloc(sizeof(struct hand));
	for(int i = 0; i < 7; i++){
		target->book[i] = 0;
	}
	target->hand_size = 0;
	return 0;
}
void user_guess(char guess, int connfd){
	struct card* temp;
	struct hand* last;
	int x = search(&computer, guess);
	//write(connfd, buff, sizeof(buff)); 
	
	if(x==0){
		//printf("Player 2 has no %c's\n", guess);
		char buff[200] = "Player 2 has no ";
		buff[16] = guess;
		buff[17] = 39;
		buff[18] = 's';
		buff[19] = '\n';
		write(connfd, buff, sizeof(buff));
		temp = next_card();
		add_card(&user, temp);
		//printf("Go Fish, Player 1 Draws ");
		//printf("%c", translate[temp->rank]);
		//printf("%c", temp->suit);
		//printf("\n");
		char buffer[200] = "Go Fish, Player 1 Draws ";
		buffer[24] = translate[temp->rank];
		buffer[25] = temp->suit;
		buffer[26] = '\n';
		write(connfd, buffer, sizeof(buffer));
		check_add_book(&user);
	}
	if(x==1){
		last = user.card_list;
		transfer_cards(&computer, &user, guess);
		int z = transfer_cards(&computer, &user, guess);
		if(computer.hand_size == 1){
			add_card(&computer, next_card());
		}
		z += transfer_cards(&computer, &user, guess);
		size_t count = user.hand_size-1;
		char buff[200] = "Player 2 has ";
		//printf("Player 2 has ");
		int n = 13;
		while(count > 0){
			if(last->next == NULL) return;
			if(last == NULL) return;
			last = last->next;
			count--;
			if(count == z && z != 0){
				buff[n] = translate[last->top.rank];
				n++;
				//printf("%c", translate[last->top.rank]);
				buff[n] = last->top.suit;
				n++;
				//printf("%c", last->top.suit);
				buff[n] = ' ';
				n++;
				//printf(" ");
				z--;
			}
		}
		//printf("%c", translate[last->top.rank]);
		buff[n] = translate[last->top.rank];
				n++;
		//printf("%c", last->top.suit);
		buff[n] = last->top.suit;
				n++;
		//printf("\n");
		buff[n] = '\n';
		write(connfd, buff, sizeof(buff));
	}

}
void computer_guess(char guess, int connfd){
	struct card* temp;
	struct hand* last;
	int x = search(&user, guess);
	if(x==0){
		//printf("Player 1 has no %c's\n", guess);
		char buff[200] = "Player 1 has no ";
		buff[16] = guess;
		buff[17] = 39;
		buff[18] = 's';
		buff[19] = '\n';
		write(connfd, buff, sizeof(buff));
		temp = next_card();
		add_card(&computer, temp);
		//printf("Go Fish, Player 2 Draws a Card\n");
		char buffer[200] = "Go Fish, Player 2 Draws a Card\n";
		write(connfd, buffer, sizeof(buffer));
		check_add_book(&computer);
	}
	if(x==1){
		last = computer.card_list;
		transfer_cards(&user, &computer, guess);
		int z = transfer_cards(&user, &computer, guess);
		if(computer.hand_size == 1){
			add_card(&user, next_card());
		}
		z += transfer_cards(&user, &computer, guess);
		size_t count = computer.hand_size-1;
		char buff[200] = "Player 1 has ";
		int n = 13;
		//printf("Player 1 has ");
		while(count > 0){
			last = last->next;
			count--;
			if(count == z && z != 0){
				buff[n] = translate[last->top.rank];
				n++;
				//printf("%c", translate[last->top.rank]);
				buff[n] = last->top.suit;
				n++;
				//printf("%c", last->top.suit);
				buff[n] = ' ';
				n++;
				//printf(" ");
				z--;
			}
		}
		//printf("%c", translate[last->top.rank]);
		buff[n] = translate[last->top.rank];
				n++;
		//printf("%c", last->top.suit);
		buff[n] = last->top.suit;
				n++;
		//printf("\n");
		buff[n] = '\n';
		write(connfd, buff, sizeof(buff));
	}

}
char computer_play(struct player* target){
	char translate[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
	//printf("Player 2's turn, enter a Rank: ");
	srand(time(NULL));
	int random = rand() % (int)computer.hand_size;	
	struct hand* current = computer.card_list;
	if(current != target->card_list){
		return translate[1];	
	}
	while(random-1 > 0){
		current = current->next;
		random--;
	}
	char send = translate[current->top.rank];
	//printf("%c", send);
	//printf("\n");
	return send;
}



/* $end echoservertmain */
