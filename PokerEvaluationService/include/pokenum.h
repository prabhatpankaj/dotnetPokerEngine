//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by poker-eval.rc

// Next default values for new objects
#include <string.h>
#include "enumdefs.h"
#include <stdlib.h>
#include <stdio.h>
#include "include/deck_std.h"
#include "include/handval.h"
#include "include/rules_std.h"
#include "include/enumdefs.h"

int pokenum(int argc, char** argv, enum_result_t* result, StdDeck_CardMask* board, StdDeck_CardMask** pocketCards, int &nboard);

static int parseArgs(int argc, char** argv,
	enum_game_t* game, enum_sample_t* enumType, int* niter,
	StdDeck_CardMask pockets[], StdDeck_CardMask* board,
	StdDeck_CardMask* dead, int* npockets, int* nboard,
	int* orderflag, int* terse) {
	/* we have a type problem: we define the masks here as
	   StdDeck_CardMask, which makes it impossible to hold jokers.
	   we need to redesign some of the deck typing to make this work... */
	enum_gameparams_t* gameParams = enumGameParams(game_holdem);
	enum { ST_OPTIONS, ST_POCKET, ST_BOARD, ST_DEAD } state;
	int ncards;
	int card;
	int i;

	state = ST_OPTIONS;
	*npockets = *nboard = ncards = 0;
	*terse = 0;
	*orderflag = 0;
	*game = game_holdem;
	*enumType = ENUM_EXHAUSTIVE;
	StdDeck_CardMask_RESET(*dead);
	StdDeck_CardMask_RESET(*board);
	for (i = 0; i < ENUM_MAXPLAYERS; i++)
		StdDeck_CardMask_RESET(pockets[i]);
	while (++argv, --argc) {
		if (state == ST_OPTIONS) {
			if (argv[0][0] != '-') {
				state = ST_POCKET;
				argv--; argc++;
			}
			else {
				if (strcmp(*argv, "-mc") == 0) {
					*enumType = ENUM_SAMPLE;
					if (argc < 1)
						return 1;
					*niter = strtol(argv[1], NULL, 0);
					if (*niter <= 0 || errno != 0)
						return 1;
					argv++; argc--;                       /* put card back in list */
				}
				else if (strcmp(*argv, "-t") == 0) {
					*terse = 1;
				}
				else if (strcmp(*argv, "-O") == 0) {
					*orderflag = 1;
				}
				else if (strcmp(*argv, "-h") == 0) {
					*game = game_holdem;
				}
				else if (strcmp(*argv, "-h8") == 0) {
					*game = game_holdem8;
				}
				else if (strcmp(*argv, "-o") == 0) {
					*game = game_omaha;
				}
				else if (strcmp(*argv, "-o8") == 0) {
					*game = game_omaha8;
				}
				else if (strcmp(*argv, "-7s") == 0) {
					*game = game_7stud;
				}
				else if (strcmp(*argv, "-7s8") == 0) {
					*game = game_7stud8;
				}
				else if (strcmp(*argv, "-7snsq") == 0) {
					*game = game_7studnsq;
				}
				else if (strcmp(*argv, "-r") == 0) {
					*game = game_razz;
				}
				else if (strcmp(*argv, "-5d") == 0) {
					*game = game_5draw;
				}
				else if (strcmp(*argv, "-5d8") == 0) {
					*game = game_5draw8;
				}
				else if (strcmp(*argv, "-5dnsq") == 0) {
					*game = game_5drawnsq;
				}
				else if (strcmp(*argv, "-l") == 0) {
					*game = game_lowball;
				}
				else if (strcmp(*argv, "-l27") == 0) {
					*game = game_lowball27;
				}
				else {                                /* unknown option switch */
					return 1;
				}
				if ((gameParams = enumGameParams(*game)) == NULL)
					return 1;
			}

		}
		else if (state == ST_POCKET) {
			if (strcmp(*argv, "-") == 0) {            /* player delimiter */
				if (ncards > 0) {                       /* is a player pending? */
					if (ncards < gameParams->minpocket)   /* too few pocket cards */
						return 1;
					(*npockets)++;
					ncards = 0;
				}
				state = ST_POCKET;
			}
			else if (strcmp(*argv, "--") == 0) {    /* board prefix */
				state = ST_BOARD;
			}
			else if (strcmp(*argv, "/") == 0) {     /* dead card prefix */
				state = ST_DEAD;
			}
			else {
				if (*npockets >= ENUM_MAXPLAYERS)           /* too many players */
					return 1;
				if (DstringToCard(StdDeck, *argv, &card) == 0) /* parse error */
					return 1;
				if (StdDeck_CardMask_CARD_IS_SET(*dead, card)) /* card already seen */
					return 1;
				StdDeck_CardMask_SET(pockets[*npockets], card);
				StdDeck_CardMask_SET(*dead, card);
				ncards++;
				if (ncards == gameParams->maxpocket) {  /* implicit player delimiter */
					(*npockets)++;
					ncards = 0;
				}
			}

		}
		else if (state == ST_BOARD) {
			if (strcmp(*argv, "/") == 0) {            /* dead card prefix */
				state = ST_DEAD;
			}
			else {
				if (DstringToCard(StdDeck, *argv, &card) == 0) /* parse error */
					return 1;
				if (StdDeck_CardMask_CARD_IS_SET(*dead, card)) /* card already seen */
					return 1;
				if (*nboard >= gameParams->maxboard) /* too many board cards */
					return 1;
				StdDeck_CardMask_SET(*board, card);
				StdDeck_CardMask_SET(*dead, card);
				(*nboard)++;
			}

		}
		else if (state == ST_DEAD) {
			if (strcmp(*argv, "-") == 0) {            /* player delimiter */
				if (ncards > 0) {                       /* is a player pending? */
					if (ncards < gameParams->minpocket)   /* too few pocket cards */
						return 1;
					(*npockets)++;
					ncards = 0;
				}
				state = ST_POCKET;
			}
			else {
				if (DstringToCard(StdDeck, *argv, &card) == 0) /* parse error */
					return 1;
				if (StdDeck_CardMask_CARD_IS_SET(*dead, card)) /* card already seen */
					return 1;
				StdDeck_CardMask_SET(*dead, card);
			}
		}
	}
	if (ncards > 0) {                             /* is a player pending? */
		if (ncards < gameParams->minpocket)         /* too few pocket cards */
			return 1;
		(*npockets)++;
		ncards = 0;
	}
	if (*npockets == 0)                           /* no players seen */
		return 1;
	return 0;
}