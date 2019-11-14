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
#define SCREEN_WIDTH 320	//window height // TODO: get thes from whereve they exist in remote source
#define SCREEN_HEIGHT 480	//window width

#define USER		1
#define CPU			2

#define GAME_IDLE	0
#define GAME_ON		1
#define GAME_OVER	2

/* Extern Variable Declarations */
extern ball_t ball; // the ball
extern paddle_t paddle[2]; // the paddles, one for user, one for CPU
extern int score[2]; // the score, one for user, one for CPU
extern int pong_state; // 0 for main menu, 1 for playing, 2 for game over
extern int winning_player; // 1 for user, 2 for CPU
extern bool exit_pong; // is true to exit 

/* Function Declarations */
void init_game(void); //initialize starting position and sizes of game elemements
int check_score(void); // returns 1 or 2 if player 1 or 2 has won, 0 if not
int check_collision(ball_t a, paddle_t b); //if returns 1 if a collision occured, 0 if not
void move_ball(void); // This routine moves each ball by its motion vector. 
void move_paddle_ai(void);
void move_paddle(int d);
void draw_game_over(int p); // TODO: port TFT LCD stuff and maybe get rid of this function?
void draw_menu(void); // TODO: replace with clear screen?
void draw_net(void);
void draw_ball(void);
void draw_paddles(void);
void draw_user_score(void);
void draw_cpu_score(void);




#endif // PONG_H
