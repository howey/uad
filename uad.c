#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

//The number of players
size_t nPlayers = 3;

typedef struct {
    char suit;
    char rank;
} Card;

typedef struct {
    //The cards played by each player, length of the number of players
    Card * cards;
    //The number of the player who started
    char start;
    //The trump suit of the round
    char trump;
} Round;

typedef struct {
    char bid;
    char id;
} Player;

//The state of the game
typedef struct {
    //Length of rounds will be equal to the number of cards in a players hand at the start of the round
    Round * rounds;
    //The cards in each player's hand.
    Card ** hands;
    //The players and their bids
    Player * player;
    //The number of cards
    size_t nCards;
} Game;

Game init(Game *); 
void copyGame(Game *, Game *); 

char cardRank(Card card, char lead, char trump) {
    if(card.suit == trump) {
        return card.rank + 26;
    }
    if(card.suit == lead) {
        return card.rank + 13;
    }
    
    return card.rank;
}

//Returns the id of a player
char getWinner(Round round) {
    char max = 0;
    char winner = 0;
    char lead = round.cards[round.start].suit;
    char trump = round.trump;

    for(size_t i = 0; i < nPlayers; i++) {
        char rank = cardRank(round.cards[i], lead, trump);
        if(rank > max) {
            max = rank;
            winner = i;
        }
    }

    return winner;
}

//helper function for utilityVector
char utility(Game game, Player player) {
    char tricks = 0;
    //9 rounds
    for(size_t i = 0; i < game.nCards; i++) {
        if(player.id == getWinner(game.rounds[i])) {
            tricks++;
        }
    }
    if(tricks == player.bid) {
        return (10 + 2 * player.bid);
    }

    return tricks;
}

char * utilityVector(Game game) {
    char * util = (char *)malloc(sizeof(char) * nPlayers);
    for(int i = 0; i < nPlayers; i++) {
        util[i] = utility(game, game.player[i]);
    }
    return util;
}

//if the game is in a terminal state, return 1; otherwise, return 0
char isTerminal(Game game) {
    //If someone has a card left in their hand, the game isn't over
    for(int i = 0; i < nPlayers; i++) {
        for(int j = 0; j < game.nCards; j++) {
            //a rank of zero means no card
            if(game.hands[i][j].rank) {
                return 0;
            }
        }
    }

    return 1;
}

//Given a game and the current player, generate all possible successors to that game
//Remember to allocate the successors array
//Returns the number of successors
size_t getSuccessors(Game game, int roundNumber, char playerId, Game * successors, size_t nSuccessors) {
    size_t j = 0;
    //is this a new round?
    char first = 1;
    for(int i = 0; i < nPlayers; i++) {
       if(game.rounds[roundNumber].cards[i].rank) {
           first = 0;
           break;
       }
    }

    //play the ith card
    void playCard(Card card, char i) {
       successors[j].rounds[roundNumber].cards[playerId] = card;
       successors[j].hands[playerId][i].rank = 0;
       j++;
    }

    //if not, must follow suit. do we have any cards of suit?
    char haveSuit = 0;
    if(!first) {
        char s = game.rounds[roundNumber].start;
        for(int i = 0; i < game.nCards; i ++) {
            if(game.hands[playerId][i].rank != 0 &&
               game.hands[playerId][i].suit == game.rounds[roundNumber].cards[s].suit) {
                haveSuit = 1;
                break;
            }
        }
    }

    for(int i = 0; i < game.nCards; i++) {
       //A rank of zero means no card in that position
       if(game.hands[playerId][i].rank != 0) {
           if(!first) {
               char s = game.rounds[roundNumber].start;
               if(haveSuit) {
                   if(game.hands[playerId][i].suit == game.rounds[roundNumber].cards[s].suit) {
                       successors[j] = init(&game);
                       playCard(game.hands[playerId][i], i);
                   }
               }
               else {
                   successors[j] = init(&game);
                   playCard(game.hands[playerId][i], i);
               }
           }
           else {
               successors[j] = init(&game);
               //Set the start player and suit
               successors[j].rounds[roundNumber].start = playerId;
               //Play the card
               playCard(game.hands[playerId][i], i);
           }
       }
    }

    return j;
}

char char2Suit(char c) {
    switch(c) {
        case 'h':
        case 'H':
            return 0;
        case 's':
        case 'S':
            return 1;
        case 'c':
        case 'C':
            return 2;
        case 'd':
        case 'D':
            return 3;
        default:
            assert(NULL);
            return 0;
    }
}

char suit2Char(char c) {
    switch(c) {
        case 0:
            return 'H';
        case 1:
            return 'S';
        case 2: 
            return 'C';
        case 3:
            return 'D';
        default:
            assert(NULL);
            return 0;
    }
}

char char2Rank(char c) {
    switch(c) {
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'X':
        case 'x':
            return 10;
        case 'J':
        case 'j':
            return 11;
        case 'Q':
        case 'q':
            return 12;
        case 'K':
        case 'k':
            return 13;
        case 'A':
        case 'a':
            return 14;
        default:
            assert(NULL);
            return 0;
    }
}

char rank2Char(char c) {
    switch(c) {
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'x';
        case 11:
            return 'J';
        case 12:
            return 'Q';
        case 13:
            return 'K';
        case 14:
            return 'A';
        default:
            assert(NULL);
            return 0;
    }
}

//Given initialized rounds a and b, copy fields of b into  a
void copyRounds(Round * a, Round * b) {
    for(int i = 0; i < nPlayers; i++) {
        a->cards[i] = b->cards[i];
    }
    a->start = b->start;
    a->trump = b->trump;
}

//Given initialized games a and b, copy fields of b into a
void copyGame(Game * a, Game * b) {
    for(int i = 0; i < b->nCards; i++) {
        copyRounds(&a->rounds[i], &b->rounds[i]);
    }
    for(int i = 0; i < nPlayers; i++) {
        a->player[i] = b->player[i];
        for(int j = 0; j < a->nCards; j++) {
            a->hands[i][j] = b->hands[i][j];
        }
    }
}

void freeGame(Game * state) {
    for(size_t i = 0; i < nPlayers; i++) {
        free(state->hands[i]);
    }
    free(state->hands);

    for(size_t i = 0; i < state->nCards; i++) {
        free(state->rounds[i].cards);
    }
    free(state->rounds);
    free(state->player);
}

//Prompts for number of players and their hands
//returns a game state
//If game is not NULL, init will return a copy of the game passed in
Game init(Game * game) {
    Game state;
    if(game) {
        state.rounds = (Round *) malloc(sizeof(Round) * game->nCards);
        state.player = (Player *) malloc(sizeof(Player) * nPlayers);
        for(size_t i = 0; i < game->nCards; i++) {
            state.rounds[i].cards = (Card *) malloc(sizeof(Card) * nPlayers);
        }

        state.hands = (Card **) malloc(sizeof(Card *) * nPlayers);

        for(size_t i = 0; i < nPlayers; i++) {
            state.hands[i] = malloc(sizeof(Card) * game->nCards);
        }
        state.nCards = game->nCards;
        copyGame(&state, game);
        return state;
    }
    char c;
    printf("\nHow many people are playing? ");
    //newlines
    c = getchar(); getchar();
    size_t numberOfPlayers = c - 48;
    printf("\nHow many cards does each player have? ");
    c = getchar(); getchar();
    size_t numberOfCards = c - 48;
    state.rounds = (Round *) malloc(sizeof(Round) * numberOfCards);
    state.player = (Player *) malloc(sizeof(Player) * numberOfPlayers);
    nPlayers = numberOfPlayers;
    state.nCards = numberOfCards;

    printf("\nWhat is trump suit? ");
    c = getchar(); getchar();

    for(size_t i = 0; i < state.nCards; i++) {
        state.rounds[i].cards = malloc(sizeof(Card) * nPlayers);
        state.rounds[i].trump = char2Suit(c);
    }

    state.hands = (Card **) malloc(sizeof(Card *) * nPlayers);

    for(size_t i = 0; i < nPlayers; i++) {
        state.hands[i] = malloc(sizeof(Card) * state.nCards);
    }

    for(size_t i = 0; i < nPlayers; i++) {
        printf("\nWhat's Player %zu's bid? ", i + 1);
        c = getchar(); getchar();
        state.player[i].bid = (c - 48);
        state.player[i].id = i;
        printf("\nAnd what does Player %zu have in their hand?", i + 1);
        for(size_t j = 0; j < state.nCards; j++) {
            Card card;
            printf("\nRank: ");
            c = getchar(); getchar();
            card.rank = char2Rank(c);
            printf("\nSuit: ");
            c = getchar(); getchar();
            card.suit = char2Suit(c);
            state.hands[i][j] = card;
        }
    }

    return state;
}

//Print out a game state
void printGame(Game game) {
    printf("Hands:\n");
    for(int i = 0; i < nPlayers; i++) {
        printf("Player %d:", i);
        for(int j = 0; j < game.nCards; j++) {
            if(game.hands[i][j].rank) {
                printf(" %c %c |", rank2Char(game.hands[i][j].rank), suit2Char(game.hands[i][j].suit));
            }
        }
        printf("\n");
    }   
    printf("Rounds:\n");
    for(int i = 0; i < game.nCards; i++) {
        printf("Round %d: Start %d Trump %c\n\t", i, game.rounds[i].start, suit2Char(game.rounds[i].trump));
        for(int j = 0; j < nPlayers; j++) {
            if(game.rounds[i].cards[j].rank) {
                printf(" %c %c |", rank2Char(game.rounds[i].cards[j].rank), suit2Char(game.rounds[i].cards[j].suit));
            }
        }
        printf("\n");
    }
}

char currentRound(Game game) {
    for(int i = 0; i < game.nCards; i++) {
        for(int j = 0; j < nPlayers; j++) {
            //if there's a zero rank card, the round isn't over
            if(game.rounds[i].cards[j].rank == 0) {
                return i;
            }
        }
    }
    //we should never get here (just saying that means that it will happen), if we did it means we're in a terminal state
    return 0;
}

char roundOver(Round round) {
    for(int i = 0; i < nPlayers; i++) {
        //if there's a zero rank card, the round isn't over
        if(round.cards[i].rank == 0) {
            return 0;
        }
    }

    return 1;
}

//get the minimax value of a node
char * minimax(Game game, char round, char playerId) {
    if(isTerminal(game)) {
        return utilityVector(game);
    }
    else {
        Game * successors = (Game *)malloc(sizeof(Game) * game.nCards);
        int n = getSuccessors(game, round, playerId, successors, game.nCards); 
        char * u = (char *)malloc(sizeof(char) * n);
        char r;
        char p;
        char pA[n];
        char rA[n];
        char * util[n];
        for(int i = 0; i < n; i++) {
            r = roundOver(successors[i].rounds[round]) ? round + 1 : round;
            //if we're at a new round, the winner of the last goes first
            if(r != round) {
                p = getWinner(successors[i].rounds[round]);
            }
            //otherwise it just the next player
            else {
                p = (playerId + 1) % nPlayers;
            }
            pA[i] = p;
            rA[i] = r;
            util[i] = minimax(successors[i], r, p);
            freeGame(&successors[i]);
            u[i] = util[i][playerId];
            //we've got the best move already, stop looking
            if(util[i][playerId] > 9) {
                free(u);
                return util[i];
            }
        }
        //get the node with the greatest minimax value
        char max = 0;
        char action = 0;
        for(int i = 0; i < n; i++) {
            if(u[i] > max) {
                max = u[i];
                action = i;
                p = pA[i];
                r = rA[i];
            }
        }
        free(u);
        free(successors);
        return util[action];
    }
}

void play(Game state) {
    Game game = state;
    Game * successors;
    int n;
    //the starting player is 0
    char playerId = 0;
    char playerIdMachine;

    printf("\nWhich player is the AI,");
    for(int i = 0; i < nPlayers - 1; i++) {
        printf(" %d,", i + 1);
    }
    printf(" or %d?", (int)nPlayers);
    char a = getchar(); getchar();
    playerIdMachine = (a - 48) - 1;


    successors = (Game *)malloc(sizeof(Game) * game.nCards);
    //there are nCards rounds
    for(int i = 0; i < game.nCards; i++) {
        for(int k = 0; k < nPlayers; k++) {
            n = getSuccessors(game, i, playerId, successors, game.nCards); 
            for(int l = 0; l < n; l++) {
                char p;
                char r;
                p = roundOver(successors[l].rounds[i]) ? getWinner(successors[l].rounds[i]) : (playerId + 1) % nPlayers;
                r = roundOver(successors[l].rounds[i]) ? i + 1 : i;
                if(playerId == playerIdMachine) {
                    char * m = minimax(successors[l], r, p);
                    printf("Minimax value of card %d is %d\n", l, m[playerId]);
                }
                printf("Move %d, card", l);
                printf(" %c %c \n", rank2Char(successors[l].rounds[i].cards[playerId].rank), suit2Char(successors[l].rounds[i].cards[playerId].suit));
            }
            printf("\nWhat card did Player %d play? ", playerId);
            char c = getchar(); getchar();
            int g = c - 48;
            game = successors[g];
            playerId = (playerId + 1) % nPlayers;
        }
        playerId = getWinner(game.rounds[i]);
        printf("\n");
    }

}

int main() {
    
    Game state;
    state = init(NULL);
    play(state);
    freeGame(&state);
    return 0;
}
