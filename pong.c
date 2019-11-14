/********************************************************************************************************
* FILE NAME: pong.c																						*
* 																										*
* PURPOSE: The purpose of this file is to not be idle at work so I'm making a game for the Brio Sport	*
*		   easter egg																					*
*		   																								*
* FILE REFERENCES: 																						*
*	NAME				I/O 			DESCRIPTION														*
*	----				---				-----------														*
* 	actuator_control.h	None			Needed for actuator direction macros and instruction type		*
*	encode.h			None			Needed for position macros										*
*	syncing.h							Needed for syncing types and defintions							*
*																										*
* EXTERNAL VARIABLES: 																					*
*	NAME				I/O 			DESCRIPTION														*
*	----				---				-----------														*
*	actuatorData		None			One indice of the tables that hold the actuator massag data is	*
*										passed into GetActuatorInstructions to be decompressed			*
* 																										*
* EXTERNAL REFERENCES: 																					*
* 	NAME 								DESCRIPTION														*
* 	----				 				-----------														*
*	None								None															*
* 																										*
* ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES: 											*
* 	Not Applicable...yet																				*
* 																										*
* NOTES:																								*
*																										*
* REQUIREMENTS/FUNCTIONAL SPECIFICATIONS REFERENCES: 													*
*																										*
* DEVELOPMENT HISTORY: 																					*
* 	Date 			Author 		Release 	Description of Change 										*
* 	---- 			------ 		------- 	-------------------- 										*
* 	11/14/2019		JSV			Init. Dev.	Initial build												*
* 																										*
* ALGORITHM (PDL) 																						*
* 	todo 																								*
*																										*
********************************************************************************************************/
#include "pong.h"

/* External Variable Definitions */
ball_t ball = {0};
paddle_t paddle[2] = {0};
int score[2] = {0};
int pong_state = 0; // 0 for main menu, 1 for playing, 2 for game over
int winning_player = 0; // 0 for no one yet, 1 for user, 2 for CPU
bool exit_pong = false; // is true to exit 

//surfaces
static SDL_Surface *screen; // TODO: remove and replace these below
static SDL_Surface *title;
static SDL_Surface *numbermap;
static SDL_Surface *end;

//textures
SDL_Texture *screen_texture;

//initialize starting position and sizes of game elemements
void init_game(void) {
	
	ball.x = SCREEN_WIDTH / 2;
	ball.y = SCREEN_HEIGHT / 2;
	ball.w = 10;
	ball.h = 10;
	ball.dy = 1;
	ball.dx = 1;
	
	paddle[0].x = 20;
	paddle[0].y = SCREEN_HEIGHT / 2 - 50 ;
	paddle[0].w = 10;
	paddle[0].h = 50;

	paddle[1].x = SCREEN_WIDTH - 20 - 10;
	paddle[1].y = SCREEN_HEIGHT / 2 - 50;
	paddle[1].w = 10;
	paddle[1].h = 50;
}

int check_score(void) {
	
	int i;

	//loop through player scores
	for(i = 0; i < 2; i++) {
	
		//check if score is @ the score win limit
		if (score[i] == 10 ) {
		
			//reset scores
			score[0] = 0;
			score[1] = 0;
			
			//return 1 if user's score @ limit
			if (i == 0) {

				return USER;	

			//return 2 if CPU score is @ limit
			} else {
				
				return CPU;
			}
		}
	}
	
	//return 0 if no one has reached a score of 10 yet
	return 0;
}

//if return value is 1 collision occured. if return is 0, no collision.
int check_collision(ball_t a, paddle_t b) {

	int left_a, left_b;
	int right_a, right_b;
	int top_a, top_b;
	int bottom_a, bottom_b;

	left_a = a.x;
	right_a = a.x + a.w;
	top_a = a.y;
	bottom_a = a.y + a.h;

	left_b = b.x;
	right_b = b.x + b.w;
	top_b = b.y;
	bottom_b = b.y + b.h;
	

	if (left_a > right_b) {
		return 0;
	}

	if (right_a < left_b) {
		return 0;
	}

	if (top_a > bottom_b) {
		return 0;
	}

	if (bottom_a < top_b) {
		return 0;
	}

	return 1;
}

/* This routine moves each ball by its motion vector. */
void move_ball(void) {
	
	/* Move the ball by its motion vector. */
	ball.x += ball.dx;
	ball.y += ball.dy;
	
	/* Turn the ball around if it hits the edge of the screen. */
	if (ball.x < 0) { // if ball hits left edge of screen
		
		score[1] += 1; // give player 1 a point
		init_game(); // redraw the screen from start
	}

	if (ball.x > (SCREEN_WIDTH - 10)) { //if ball hits right edge of screen
		
		score[0] += 1; // give player 0 a point
		init_game(); // redraw the screen drom start
	}

	if (ball.y < 0 || ball.y > (SCREEN_HEIGHT - 10)) { // if ball hits top or bottom edge of screen
		
		ball.dy = -ball.dy; // change ball vertical direction
	}

	//check for collision with the paddle
	int i;

	for (i = 0; i < 2; i++) {
		
		int c = check_collision(ball, paddle[i]); 

		//collision detected	
		if (c == 1) {
			
			//ball moving left
			if (ball.dx < 0) {
					
				ball.dx -= 1;

			//ball moving right
			} else {
					
				ball.dx += 1;
			}
			
			//change ball direction
			ball.dx = -ball.dx;
			
			//change ball angle based on where on the paddle it hit
			int hit_pos = (paddle[i].y + paddle[i].h) - ball.y;

			if (hit_pos >= 0 && hit_pos < 7) { // TODO: resolve all these magic numbers...has something to do with paddle size
				ball.dy = 4;
			}

			if (hit_pos >= 7 && hit_pos < 14) {
				ball.dy = 3;
			}
			
			if (hit_pos >= 14 && hit_pos < 21) {
				ball.dy = 2;
			}

			if (hit_pos >= 21 && hit_pos < 28) {
				ball.dy = 1;
			}

			if (hit_pos >= 28 && hit_pos < 32) {
				ball.dy = 0;
			}

			if (hit_pos >= 32 && hit_pos < 39) {
				ball.dy = -1;
			}

			if (hit_pos >= 39 && hit_pos < 46) {
				ball.dy = -2;
			}

			if (hit_pos >= 46 && hit_pos < 53) {
				ball.dy = -3;
			}

			if (hit_pos >= 53 && hit_pos <= 60) {
				ball.dy = -4;
			}

			//ball moving right
			if (ball.dx > 0) {

				//teleport ball to avoid mutli collision glitch
				if (ball.x < 30) { // TODO: resolve this magic number, has something to do with screen size
				
					ball.x = 30;
				}
				
			//ball moving left
			} else {
				
				//teleport ball to avoid mutli collision glitch
				if (ball.x > 600) { // TODO: resolve this magic number, has something to do with screen size
				
					ball.x = 600;
				}
			}
		}
	}
}

void move_paddle_ai(void) {

	int center = paddle[0].y + 25; // TODO: why the +25?
	int screen_center = SCREEN_HEIGHT / 2 - 25; // TODO: why the -25?
	int ball_speed = ball.dy;

	if (ball_speed < 0) {
	
		ball_speed = -ball_speed;
	}

	//ball moving right
	if (ball.dx > 0) {
		
		//return to center position
		if (center < screen_center) {
			
			paddle[0].y += ball_speed;
		
		} else {
		
			paddle[0].y -= ball_speed;	
		}

	//ball moving left
	} else {
	
		//ball moving down
		if (ball.dy > 0) { 
			
			if (ball.y > center) { 
				
				paddle[0].y += ball_speed;

			} else { 
			
				paddle[0].y -= ball_speed;
			}
		}
		
		//ball moving up
		if (ball.dy < 0) {
		
			if (ball.y < center) { 
				
				paddle[0].y -= ball_speed;
			
			} else {
			
				paddle[0].y += ball_speed;
			}
		}

		//ball moving stright across
		if (ball.dy == 0) {
			
			if (ball.y < center) { 
			
				paddle[0].y -= 5;

			} else {
			
				paddle[0].y += 5;
			}
		}	 		
	}
}

void move_paddle(int d) { // TODO: integrate key scan to take key value isntead of integer

	// if the down arrow is pressed move paddle down
	if (d == 0) {
		
		if(paddle[1].y >= SCREEN_HEIGHT - paddle[1].h) {
		
			paddle[1].y = SCREEN_HEIGHT - paddle[1].h;
		
		} else { 
		
			paddle[1].y += 5;
		}
	}
	
	// if the up arrow is pressed move paddle up
	if (d == 1) {

		if(paddle[1].y <= 0) {
		
			paddle[1].y = 0;

		} else {
		
			paddle[1].y -= 5;
		}
	}
}

void draw_game_over(int p) { // TODO: port this over to TFT LCD stuff

	SDL_Rect p1;
	SDL_Rect p2;
	SDL_Rect cpu;
	SDL_Rect dest;

	p1.x = 0;
	p1.y = 0;
	p1.w = end->w;
	p1.h = 75;

	p2.x = 0;
	p2.y = 75;
	p2.w = end->w;
	p2.h = 75;
	
	cpu.x = 0;
	cpu.y = 150;
	cpu.w = end->w;
	cpu.h = 75;

	dest.x = (SCREEN_WIDTH / 2) - (end->w / 2);
	dest.y = (SCREEN_HEIGHT / 2) - (75 / 2);
	dest.w = end->w;
	dest.h = 75;

	
	switch (p) { // 1 means users won, 2 means CPU won
	
		case USER:			
			SDL_BlitSurface(end, &p1, screen, &dest);
		break;
		case CPU:
			SDL_BlitSurface(end, &p2, screen, &dest);
		break;
		default:
			SDL_BlitSurface(end, &cpu, screen, &dest);
		break;
	}
	
}

void draw_menu(void) { // TODO: replace with clear screen? + draw text?

	SDL_Rect src;
	SDL_Rect dest;

	src.x = 0;
	src.y = 0;
	src.w = title->w;
	src.h = title->h;

	dest.x = (SCREEN_WIDTH / 2) - (src.w / 2);
	dest.y = (SCREEN_HEIGHT / 2) - (src.h / 2);
	dest.w = title->w;
	dest.h = title->h;

	SDL_BlitSurface(title, &src, screen, &dest);
}

void draw_net(void) { 

	net_seg_t net_seg;
	
	net_seg.x = SCREEN_WIDTH / 2;
	net_seg.y = 20;
	net_seg.w = 5;
	net_seg.h = 15;

	//draw the net
	int i;

	for(i = 0; i < 15; i++) {
		
		TFT_FillRect(net_seg.x, net_seg.y, net_seg.w, net_set.h, TFT_WHITE);

		net_seg.y = net_seg.y + 30;
	}
}

void draw_ball(void) { 
	
	TFT_FillRect(ball.x, ball.y, ball.w, ball.h, TFT_WHITE);
	
}

void draw_paddles(void) {

	int i;

	for (i = 0; i < 2; i++) {
	
		TFT_FillRect(paddle[i].x, paddle[i].y, paddle[i].w, paddle[i].h, TFT_WHITE)
	}
}

void draw_user_score(void) {
	
	// SDL_Rect src;
	// SDL_Rect dest;
	// 
	// src.x = 0;
	// src.y = 0;
	// src.w = 64;
	// src.h = 64;
	// 
	// dest.x = (SCREEN_WIDTH / 2) - src.w - 12; //12 is just padding spacing
	// dest.y = 0;
	// dest.w = 64;
	// dest.h = 64;
	// 
	// if (score[0] > 0 && score[0] < 10) {
	// 	
	// 	src.x += src.w * score[0];
	// }
	// 
	// SDL_BlitSurface(numbermap, &src, screen, &dest);
	
	int ones_i, tens_i;
	XCHAR ones_c, tens_c;
	static XCHAR userScore[3] = {0};
	int x,y;
	
	ones_i = score[0]%10;
	tens_i = (score[0]/10)&10;
	
	ones_c = ones_i + '0';
	tens_c = tens_i + '0';
	
	userScore[0] = tens_c;
	userScore[1] = ones_c;
	userScore[2] = 0;
	
	x = 0 + 15;
	y = 0 + 15;
	
	TFT_ShowString(x,y,userScore,TFT_WHITE,&Arial_Narrow_28);
}

void draw_cpu_score(void) {
	
	// SDL_Rect src;
	// SDL_Rect dest;
	// 
	// src.x = 0;
	// src.y = 0;
	// src.w = 64;
	// src.h = 64;
	// 
	// dest.x = (SCREEN_WIDTH / 2) + 12;
	// dest.y = 0;
	// dest.w = 64;
	// dest.h = 64;
	// 
	// if (score[1] > 0 && score[1] < 10) {
	// 	
	// 	src.x += src.w * score[1];
	// }
	// 
	// SDL_BlitSurface(numbermap, &src, screen, &dest);
	
	int ones_i, tens_i;
	XCHAR ones_c, tens_c;
	static XCHAR cpuScore[3] = {0};
	int x,y;
	
	ones_i = score[1]%10;
	tens_i = (score[1]/10)&10;
	
	ones_c = ones_i + '0';
	tens_c = tens_i + '0';
	
	cpuScore[0] = tens_c;
	cpuScore[1] = ones_c;
	cpuScore[2] = 0;
	
	x = (SCREEN_WIDTH/2) + 15;
	y = 0 + 15;
	
	TFT_ShowString(x,y,cpuScore,TFT_WHITE,&Arial_Narrow_28);
}

int main (void) {		
	
	// Initialize the ball position data. 
	init_game();
	
	//render loop
	while(exit_pong == false) {
	
		//TODO: scan for keys
		
		if (keystate!=KEY_DN && keystate!=KEY_UP && keystate!=KEY_OK) { // if user presses any key that isn't OK, up or down then exit// TODO: syntax here
		
			exit_pong = true;
		}
		
		if (keystate == KEY_DN && pong_state == GAME_ON) { // if user presses down while the game is going // TODO: syntax
			
			move_paddle(0); // move paddle down
		}

		if (keystate == KEY_UP && pong_state == GAME_ON) { // if user presses up while the game is going // TODO: syntax
			
			move_paddle(1); // move paddle up
		}
		
		//draw background
		//SDL_RenderClear(renderer);
		//SDL_FillRect(screen, NULL, 0x000000ff);
		TFT_Clear(TFT_BLACK); // TODO: check this out...not sure about OG syntax (above) but I think they just clear the whole screen every time
		
		//display main menu
		if (pong_state == GAME_IDLE ) {
		
			if (keystate == KEY_OK) { // "press OK to start"
				
				pong_state = GAME_ON; 
			}
		
			//draw menu 
			draw_menu();
		
		//display gameover
		} else if (pong_state == GAME_OVER) {
		
			if (keystate == KEY_OK) { // "press OK to exit to menu"
				pong_state = GAME_IDLE;
				
				//delay for a little bit so the space bar press dosnt get triggered twice
				//while the main menu is showing
            			DelayMS(500);
			}

			draw_game_over(winning_player);
				
		//display the game
		} else if (pong_state == GAME_ON) {
			
			//check score
			winning_player = check_score();
		
			//if either player wins, change to game over pong_state
			if (winning_player) {
				
				pong_state = GAME_OVER;	

			} 

			// paddle ai movement
			move_paddle_ai();

			// Move the balls for the next frame. 
			move_ball();
			
			// draw net
			draw_net();

			// draw paddles
			draw_paddles();
			
			// Put the ball on the screen.
			draw_ball();
	
			// draw the score
			draw_user_score();
	
			// draw the score
			draw_cpu_score();
		}
	
	}

	 
	return 0;
	
}

