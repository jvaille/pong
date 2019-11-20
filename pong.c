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
paddle_t prevPaddle[2] = {0};
int score[2] = {0};
int pong_state = 0; // 0 for main menu, 1 for playing, 2 for game over
int winning_player = 0; // 0 for no one yet, 1 for user, 2 for CPU
bool menuDrawn = false; // is false before the menu is drawn, true after. Prevents unneccessary redraws
bool gameOverDrawn = false; // is false before the game over screen is drawn, true after. Prevents unneccessary redraws.
unsigned int victoryColorIterator = 0;


//initialize starting position and sizes of game elemements
void init_game(void) {
	
	ball.x = MAX_X / 2 - 10; // horizontal middle of screen - ball width so that it starts on the user's side of the net
	ball.y = MAX_Y / 2; // vertical middle of screen // don't offset half of height of ball so ball isn't exactly centered. Prevent AFK rapid back and forth glitch
	ball.w = 10;
	ball.h = 10;
	ball.dy = (rand() % 5) - 2; // move ball in random direction vertically in range [-2,2] // num in range [lower,upper] = (rand() % (upper - lower + 1)) + lower
	ball.dx = -1; // move ball directly towards user
	
	paddle[0].x = 20; // far left edge of screen + 20px offset
	paddle[0].y = MAX_Y / 2 - 25; // vertical middle of screen - half length of paddle
	paddle[0].w = 10;
	paddle[0].h = 50;
	
	prevPaddle[0] = paddle[0];

	paddle[1].x = MAX_X - 20 - 10; // far right edge of screen - ( 20px offset + 10px width of paddle )
	paddle[1].y = MAX_Y / 2 - 25; // vertical middle of screen - half length of paddle
	paddle[1].w = 10;
	paddle[1].h = 50;
	
	prevPaddle[1] = paddle[1];
	
	TFT_Clear(TFT_BLACK);
	
	menuDrawn = false; // ensure this is false so menu draws the first time for sure
	gameOverDrawn = false; // ensure this is false so game over draws the first time for sure
}
// draws paddles for the first time
void init_paddles(void){
	TFT_FillRect(paddle[0].x,paddle[0].y,paddle[0].w,paddle[0].h,TFT_WHITE);
	TFT_FillRect(paddle[1].x,paddle[1].y,paddle[1].w,paddle[1].h,TFT_WHITE);
}
//see if anybody has won
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
int check_collision(ball_t b, paddle_t p) {

	int left_b, left_p;
	int right_b, right_p;
	int top_b, top_p;
	int bottom_b, bottom_p;

	left_b = b.x;
	right_b = b.x + b.w;
	top_b = b.y;
	bottom_b = b.y + b.h;

	left_p = p.x;
	right_p = p.x + p.w;
	top_p = p.y;
	bottom_p = p.y + p.h;
	

	if (left_b > right_p) {
		return 0;
	}

	if (right_b < left_p) {
		return 0;
	}

	if (top_b > bottom_p) {
		return 0;
	}

	if (bottom_b < top_p) {
		return 0;
	}

	return 1;
}

// move the ball according to its motion vector
void move_ball(void) {
	
	int i, hit_pos;	
	
	/* Move the ball by its motion vector. */
	ball.x += ball.dx;
	ball.y += ball.dy;
	
	/* Turn the ball around if it hits the edge of the screen. */
	if (ball.x < 0) { // if ball hits left edge of screen
		
		score[1] += 1; // give cpu a point
		init_game(); 
		init_paddles();
	}

	if (ball.x > (MAX_X - 10)) { //if ball hits right edge of screen
		
		score[0] += 1; // give user a point
		init_game(); 
		init_paddles();
	}

	/* reverse ball direction if it collides with top of bottom edge of screen */
	if (ball.y < 0 || ball.y > (MAX_Y - 10)) { 
		
		ball.dy = -ball.dy; 
	}

	//check for collision with the paddle
	for (i = 0; i < 2; i++) {
		
		int c = check_collision(ball, paddle[i]); 

		//collision detected	
		if (c == 1) {
			
			//increase velocity
			if (ball.dx < 0) { // ball moving left
					
				ball.dx -= 1;
				
			} else { // ball moving right
					
				ball.dx += 1;
			}
			
			//reverse ball direction
			ball.dx = -ball.dx;
			
			//change ball angle based on where on the paddle it hit
			hit_pos = (paddle[i].y + paddle[i].h) - ball.y;

			if (hit_pos >= 0 && hit_pos < 7) { 
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

			//teleport ball to avoid mutli collision glitch if somehow inside paddle
			if (ball.dx > 0) { // ball moving right

				if (ball.x < 40) { // ball inside user paddle
				
					ball.x = 40;
				}
				
			} else { // ball moving left
				
				if (ball.x > (MAX_X-40)) { // ball inside CPU paddle
				
					ball.x = MAX_X-40;
				}
			}
		}
	}
}
// Moves the CPUs paddle
void move_paddle_ai(void) {

	int paddle_center = paddle[1].y + 25; // use the center of paddle versus y position (corner); cause the PC paddle to move closer to the ball than it needs to to intercept
	int screen_center = MAX_Y / 2;		  // makes it behave much more like another player than always intercepting the ball with very edge of paddle

	//ball moving left, away from CPU paddle
	if (ball.dx < 0) {
		
		//return to paddle_center position
		
		if (paddle_center == screen_center){ 
			
			// if paddle is centered don't do anything			
			
		}else if (paddle_center < screen_center) {
			
			paddle[1].y += ball.dy;
		
		} else {
		
			paddle[1].y -= ball.dy;	
		}

	//ball moving right, towards CPU paddle
	} else {
		// try to block the ball
		
		//ball moving down
		if (ball.dy > 0) { 
			
			if (ball.y > paddle_center) { 
				
				paddle[1].y += ball.dy;

			} else { 
			
				paddle[1].y -= ball.dy;
			}
		}
		
		//ball moving up
		if (ball.dy < 0) {
		
			if (ball.y < paddle_center) { 
				
				paddle[1].y += ball.dy;
			
			} else {
			
				paddle[1].y -= ball.dy;
			}
		}

		//ball moving stright across
		if (ball.dy == 0) {
			
			if (ball.y < paddle_center) { 
			
				paddle[1].y -= 5;

			} else {
			
				paddle[1].y += 5;
			}
		}	 		
	}
	
	if(paddle[1].y > MAX_Y - paddle[0].h) paddle[1].y = MAX_Y - paddle[1].h;
	if(paddle[1].y < 0) paddle[1].y = 0;
	
}

void move_paddle(int d) { 

	// if the down arrow is pressed move paddle down
	if (d == DOWN) {
		
		if(paddle[0].y >= MAX_Y - paddle[0].h - 5) {
		
			paddle[0].y = MAX_Y - paddle[0].h;
		
		} else { 
		
			paddle[0].y += 5;
		}
	}
	
	// if the up arrow is pressed move paddle up
	if (d == UP) {

		if(paddle[0].y <= 5) {
		
			paddle[0].y = 0;

		} else {
		
			paddle[0].y -= 5;
		}
	}
}

void draw_game_over(int p) { 
	
	static XCHAR p1Wins[11] = {'Y','o','u',' ','W','i','n','!','!','!',0};
	static XCHAR cpuWins[12] = {'Y','o','u',' ','L','o','s','e','.','.','.',0};
	static XCHAR gameOver[10] = {'G','a','m','e',' ','O','v','e','r',0};
	static XCHAR errorTxt[27] = {'O','o','p','s',',',' ','S','o','m','e','t','h','i','n','g',' ','w','e','n','t',' ','w','r','o','n','g',0};
	static XCHAR start_tip[23] = {'P','r','e','s','s',' ','O','K',' ','t','o',' ','p','l','a','y',' ','a','g','a','i','n',0};
	static XCHAR exit_tip[28] = {'P','r','e','s','s',' ','a','n','y',' ','o','t','h','e','r',' ','k','e','y',' ','t','o',' ','e','x','i','t',0};
	
	unsigned int x_victory = (MAX_X/2);
	static unsigned int y_victory = (MAX_Y/2)-33; // Arial_Narrow_28 has a height of 33 px, make Y so bottom of text is dead center on screen
	unsigned int victoryTextLength;
	
	unsigned int x_gameOver = x_victory;
	static unsigned int y_gameOver = (MAX_Y/2)+5; // Don't worry about height of text since there's nothing below this text. Offset by 5 so theres some space between this and victory text
	unsigned int gameOverTextLength;
		
	unsigned int x_start_tip = 20;
	static unsigned int y_start_tip = 400;
		
	unsigned int x_exit_tip = 20;
	static unsigned int y_exit_tip = 400 + 18 + 5; // y_start_tip + 18 (text height) + 5px offset
			
	switch (p) { // 1 means users won, 2 means CPU won
	
		case USER:		
			victoryTextLength = TFT_GetTextLength(p1Wins,&Arial_Narrow_28); // figure out how long text is for a given font
			x_victory -= (victoryTextLength/2); // move origin left half of text length to center text on screen
			
			gameOverTextLength = TFT_GetTextLength(gameOver,&Helvetica_Bd_Cn_17); // figure out how long text is for a given font
			x_gameOver -= (gameOverTextLength/2); // move origin left half of text length to center text on screen
			
			BackLight_OFF; // turn off backlight for smooth redraw
			TFT_Clear(TFT_WHITE); // clear screen
			TFT_ShowString(x_victory,y_victory,p1Wins,TFT_BLACK,&Arial_Narrow_28); // draw victory text so bottom of text is middle of screen 
			TFT_ShowString(x_gameOver,y_gameOver,gameOver,TFT_BLACK,&Helvetica_Bd_Cn_17); // draw game over text so top of text is just below middle of screen 
			TFT_ShowString(x_start_tip,y_start_tip,start_tip,TFT_BLACK,&Helvetica_Bd_Cn_17); // display a tip for restarting pong
			TFT_ShowString(x_exit_tip,y_exit_tip,exit_tip,TFT_BLACK,&Helvetica_Bd_Cn_17); // display a tip for exiting pong
			BackLight_ON; // turn backlight back on
		break;
		case CPU:
			victoryTextLength = TFT_GetTextLength(cpuWins,&Arial_Narrow_28); // figure out how long text is for a given font
			x_victory -= (victoryTextLength/2); // move origin left half of text length to center text on screen
			
			gameOverTextLength = TFT_GetTextLength(gameOver,&Helvetica_Bd_Cn_17); // figure out how long text is for a given font
			x_gameOver -= (gameOverTextLength/2); // move origin left half of text length to center text on screen
			
			BackLight_OFF; // turn off backlight for smooth redraw
			TFT_Clear(TFT_BLACK); // clear screen
			TFT_ShowString(x_victory,y_victory,cpuWins,TFT_WHITE,&Arial_Narrow_28); // draw victory text so bottom of text is middle of screen
			TFT_ShowString(x_gameOver,y_gameOver,gameOver,TFT_WHITE,&Helvetica_Bd_Cn_17); // draw game over text so top of text is just below middle of screen 
			TFT_ShowString(x_start_tip,y_start_tip,start_tip,TFT_WHITE,&Helvetica_Bd_Cn_17); // display a tip for restarting pong
			TFT_ShowString(x_exit_tip,y_exit_tip,exit_tip,TFT_WHITE,&Helvetica_Bd_Cn_17); // display a tip for exiting pong
			BackLight_ON; // turn backlight back on
		break;
		default: /* error conditions to get here, only display error text */
			gameOverTextLength = TFT_GetTextLength(errorTxt,&Helvetica_Bd_Cn_17); // figure out how long text is for a given font
			x_gameOver -= (gameOverTextLength/2); // move origin left half of text length to center text on screen
			
			if(x_gameOver>MAX_X) x_gameOver = 0; // if we have something weird happen with the unsigned math, just set x to 0 and draw left justified text
			
			BackLight_OFF; // turn off backlight for smooth redraw
			TFT_Clear(TFT_BLACK); // clear screen
			TFT_ShowString(x_gameOver,y_gameOver,errorTxt,TFT_WHITE,&Helvetica_Bd_Cn_17); // ONLY DRAW ERROR TEXT SINCE SOMETHING BROKE IF WE're HERE
			BackLight_ON; // turn backlight back on
		break;
	}	
}

void draw_menu(void) {

	static XCHAR title[5] = {'P','o','n','g',0};
	static XCHAR start_tip[18] = {'P','r','e','s','s',' ','O','K',' ','t','o',' ','b','e','g','i','n',0};
	static XCHAR exit_tip[28] = {'P','r','e','s','s',' ','a','n','y',' ','o','t','h','e','r',' ','k','e','y',' ','t','o',' ','e','x','i','t',0};
			
	unsigned int x_title = (MAX_X/2);
	unsigned int y_title = (MAX_Y/2) - 33; // Arial_Narrow_28 has a height of 33 px, make Y so bottom of text is dead center on screen
	unsigned int titleLength;
	
	unsigned int x_start_tip = 20;
	unsigned int y_start_tip = 400; // Offset by 5 so theres some space between this and title text
		
	unsigned int x_exit_tip = 20;
	unsigned int y_exit_tip = 400 + 18 + 5; // y_start_tip + start tip text height + 5 more for space between two texts 
	
	titleLength = TFT_GetTextLength(title,&Arial_Narrow_28); // figure out how long text is for a given font // height is 33px
	x_title -= (titleLength/2); // offset origin by half of text length to center text on screen
		
	BackLight_OFF; // turn off backlight for smooth redraw
	TFT_Clear(TFT_BLACK); // clear screen
	TFT_ShowString(x_title,y_title,title,TFT_WHITE,&Arial_Narrow_28);
	TFT_ShowString(x_start_tip,y_start_tip,start_tip,TFT_WHITE,&Helvetica_Bd_Cn_17);
	TFT_ShowString(x_exit_tip,y_exit_tip,exit_tip,TFT_WHITE,&Helvetica_Bd_Cn_17);
	BackLight_ON; // turn backlight back on	
}

void draw_net(void) { 

	net_seg_t net_seg;
	int i;
	
	net_seg.x = MAX_X / 2;
	net_seg.y = 20;
	net_seg.w = 5;
	net_seg.h = 15;

	//draw the net
	for(i = 0; i < 15; i++) {
		
		TFT_FillRect(net_seg.x, net_seg.y, net_seg.w, net_seg.h, TFT_WHITE);

		net_seg.y = net_seg.y + 30;
	}
}

void draw_ball(void) { 
	
	static int oldX = MAX_X/2, oldY = MAX_Y/2;
	
	TFT_FillRect(oldX,oldY,ball.w,ball.h,TFT_BLACK); // clear old ball
	TFT_FillRect(ball.x, ball.y, ball.w, ball.h, TFT_WHITE); // draw new ball
	
	oldX = ball.x;
	oldY = ball.y; // store current positions for next redraw
}

void draw_paddles(void) {

	int i;
	int movedUpOrDown; // 0 for down, 1 for up, 2 for none
	int dy = 0; // store change in y position between last paddle draw and this paddle draw here	

	for(i = 0; i<2; i++)
	{
		// calculate change in y position
		dy = prevPaddle[i].y - paddle[i].y;
		
		// determine if the paddle is moving up or down; positive is down the screen, negative is up
		if(dy>0) movedUpOrDown = UP;
		else if(dy<0) movedUpOrDown = DOWN;
		else movedUpOrDown = 2;
		
		// make dy positive so the rectangle we draw has a positive height
		if(dy<0){
			dy = -dy;
		}
		
		switch(movedUpOrDown)
		{
			case UP:
				// start at new origin, same width, draw down the change in y, white to "extend" paddle
				TFT_FillRect(paddle[i].x, paddle[i].y, paddle[i].w,dy,TFT_WHITE); // add to top of paddle
				// start at the bottom of leftmost edge of new rectangle, same width, draw down change in y but black to cover old white
				TFT_FillRect(prevPaddle[i].x, paddle[i].y + paddle[i].h, prevPaddle[i].w, dy, TFT_BLACK); // remove some of the bottom of paddle
			break;
			case DOWN:
				// start at bottom leftmost edge of old white rectangle, same width, draw down change in y, white to "extend" paddle
				TFT_FillRect(paddle[i].x, prevPaddle[i].y + prevPaddle[i].h, paddle[i].w, dy,TFT_WHITE); // add to bottom of paddle
				// start at old origin, same width, drawn down the change in y, black to "hide" paddle
				TFT_FillRect(prevPaddle[i].x, prevPaddle[i].y, paddle[i].w, dy, TFT_BLACK); // remove some of the top of paddle
			break;
			case 2:
			default:
				break;
		}
	}
}

void draw_user_score(void) {
	
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
	
	x = (MAX_X/2) - 35;
	y = 0 + 15;
	
	TFT_ShowString(x,y,userScore,TFT_WHITE,&Arial_Narrow_28);
}

void draw_cpu_score(void) {
	
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
	
	x = (MAX_X/2) + 15;
	y = 0 + 15;
	
	TFT_ShowString(x,y,cpuScore,TFT_WHITE,&Arial_Narrow_28);
}

// include this in whichever main you want it to live in
/*
int main (void) {
	
	// register key events
	GetKeyPress();
	
	// if user presses any key that isn't OK, up or down then exit // or keyValue is registered as power key....power key doesn't register press EVENT_PRESS for pause/power-off press difference	
	if ((KeyEvent==EVENT_PRESS && KeyValue!=KEY_DN && KeyValue!=KEY_UP && KeyValue!=KEY_OK) || KeyValue==KEY_PWR) { 	
		pong_state = GAME_EXITING; // set state to exit pong 
	}
	
	// if user presses down while the game is going
	if ((KeyEvent==EVENT_PRESS || KeyEvent==EVENT_HOLD) && KeyValue == KEY_DN && pong_state == GAME_ON) {  			
		move_paddle(DOWN); // move paddle down
	}

	// if user presses up while the game is going 	
	if ((KeyEvent==EVENT_PRESS || KeyEvent==EVENT_HOLD) && KeyValue == KEY_UP && pong_state == GAME_ON) { 		
		move_paddle(UP); // move paddle up
	}
	
	// keep status' updated in case an error occurs/so remote is up to date when we exit
	if(TimeToUpdateStatus) { // if the remote has recieved data
		
		StatusUpdate(); // update the data in the storage containers
		
		// if errors appear while user is using massage functions
		if(TAPERR || KNEADERR || WALKUPERR || WALKDOWNERR || JXRETRACTERR || JXEXTENDERR || BACKACTERR || SEATACTERR || LEGACTERR || FOOTROLLERR || CALFROLLERR || MBtoRCOMMERR || JXCOMMERR || TJCOMMERR){
			pong_state = GAME_EXITING; // set state to exit pong 
		}
	}

	
	// draw the screen for various states; main menu, game active, game over
	switch(pong_state)
	{
		case GAME_ON:
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
			
			// store paddle state for next time
			prevPaddle[0] = paddle[0];
			prevPaddle[1] = paddle[1];
			
			//check score
			winning_player = check_score();
		
			//look for a winner to change state
			if (winning_player) {						
				pong_state = GAME_OVER;	
				gameOverDrawn = false; // clear this so the game over screen gets drawn
			} 
		break;
		case GAME_IDLE:
			//draw menu 
			if(!menuDrawn){
				draw_menu();
				menuDrawn = true;
			}
			
			// look for user input to change states
			if(KeyEvent==EVENT_PRESS && KeyValue == KEY_OK) {				
				pong_state = GAME_ON; 
				TFT_Clear(TFT_BLACK);
				init_paddles();
			}						
		break;
		case GAME_OVER:
			// draw game over screen
			if(!gameOverDrawn)	{
				draw_game_over(winning_player);
				gameOverDrawn = true;
				victoryColorIterator = 0; // reset victory color
			}
			
			if(winning_player==USER) // if user won, flash button LEDs
			{
				// switch which color LEDs display in
				switch(victoryColorIterator)
				{
					case 18000:
						victoryColorIterator = 0;
					break;
					case 0:
						LED_DataDWStructure.Bit.Led_3D_State = 3; // red
						LED_DataDWStructure.Bit.Led_AirStr_State = 2; // yellow
						victoryColorIterator++;
						break;
					case 3000:
						LED_DataDWStructure.Bit.Led_3D_State = 2; // yellow
						LED_DataDWStructure.Bit.Led_AirStr_State = 1; // green
						victoryColorIterator++;
						break;
					case 6000:
						LED_DataDWStructure.Bit.Led_3D_State = 1; // green
						LED_DataDWStructure.Bit.Led_AirStr_State = 4; // cyan
						victoryColorIterator++;
						break;
					case 9000:
						LED_DataDWStructure.Bit.Led_3D_State = 5; // cyan
						LED_DataDWStructure.Bit.Led_AirStr_State = 5; // blue
						victoryColorIterator++;
						break;
					case 12000:
						LED_DataDWStructure.Bit.Led_3D_State = 4; // blue
						LED_DataDWStructure.Bit.Led_AirStr_State = 6; // magenta
						victoryColorIterator++;
						break;
					case 15000:
						LED_DataDWStructure.Bit.Led_3D_State = 6; // magenta
						LED_DataDWStructure.Bit.Led_AirStr_State = 3; // red
						victoryColorIterator++;
						break;
					default:
						victoryColorIterator++;
						break;
				}
				LED_DisplayDW(&LED_DataDWStructure); // updated LEDs
			}
			
			
			// look for user input to change states
			if(KeyEvent==EVENT_PRESS && KeyValue == KEY_OK) { 
				pong_state = GAME_IDLE;
				menuDrawn = false; // clear this so the menu screen gets drawn
				
				LED_DataDWStructure.Bit.Led_3D_State = 0; // turn off 3D LED
				LED_DataDWStructure.Bit.Led_AirStr_State = 0; // turn off air LED							
				LED_DisplayDW(&LED_DataDWStructure); // update LEDs
			}
		break;
		case GAME_EXITING:
		default:
			ExitPong(); // exit pong
		break;
	}
}
}
*/
