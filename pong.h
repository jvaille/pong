#ifndef PONG_H
#define PONG_H
/********************************************************************************************************
* FILE NAME: pong.h																						*
* 																										*
* PURPOSE: The purpose of this file is to not be idle at work so I'm making a game for the Brio Sport	*
*		   easter egg																					*
*		   																								*
* GLOBAL VARIABLES: 																					*
* 	Variable 		Type 					Description 												*
* 	-------- 		---- 					----------- 												*
*	actSyncFlags	sync_flags				Structure for holding flags to accommodate syncing 			*
*																										*
* DEVELOPMENT HISTORY: 																					*
* 	Date 			Author 		Release 	Description of Change 										*
* 	---- 			------ 		------- 	-------------------- 										*
* 	11/14/2019		JSV			Init. Dev.	Initial build												*
* 																										*
********************************************************************************************************/
/* includes */
#include <stdbool.h>
#include "includes.h"
#include "tft_drv.h"

/* User Created Type Declarations */
typedef struct ball_s {

	int x,y; 	// position on the screen 
	int w,h; 	// ball width and height
	int dx,dy; // movement vector 
} ball_t;

typedef struct paddle_s {

	int x,y; // position on the screen
	int w,h; // paddle width and height
} paddle_t;

typedef struct net_seg_s {

	int x,y; // position on the screen
	int w,h; // net segment width and height
} net_seg_t;

/* Constant Macros Definitions */
#define USER		1
#define CPU			2

#define GAME_IDLE	0
#define GAME_ON		1
#define GAME_OVER	2

#define DOWN		0
#define UP			1

/* Extern Variable Declarations */
extern ball_t ball; // the ball
extern paddle_t paddle[2], prevPaddle[2]; // the paddles, one for user, one for CPU
extern int score[2]; // the score, one for user, one for CPU
extern int pong_state; // 0 for main menu, 1 for playing, 2 for game over
extern int winning_player; // 1 for user, 2 for CPU
extern bool exit_pong; // is true to exit 
extern bool menuDrawn; // is false before the menu is drawn, true after. Prevents unneccessary redraws
extern bool gameOverDrawn; // is false before the game over screen is drawn, true after. Prevents unneccessary redraws.

/* Function Declarations */
void init_game(void); //initialize starting position and sizes of game elemements
void init_paddles(void); // draws paddles for the first time
int check_score(void); // returns 1 or 2 if player 1 or 2 has won, 0 if not
int check_collision(ball_t b, paddle_t p); //if returns 1 if a collision occured, 0 if not
void move_ball(void); // This routine moves each ball by its motion vector. 
void move_paddle_ai(void); // Moves the CPUs paddle
void move_paddle(int d); // Moves the user's paddle
void draw_game_over(int p); 
void draw_menu(void); 
void draw_net(void);
void draw_ball(void);
void draw_paddles(void);
void draw_user_score(void);
void draw_cpu_score(void);

#endif // PONG_H
