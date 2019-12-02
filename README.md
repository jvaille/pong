# PONG

### Update November 2019

AFter forking, this has been re-written to work with an ILI9488 TFT_LCD paired with an ARM cortex M STM32F103. Code is not standalone.
Requires board support package provided by KT (and presumably originally Micrium). This is to support the private Project X repository 
where development of the OHCO branded massage chair software is carried out. Contact John Vaille @ john.vaille@fflbrands.com or 
johnvaille94@gmail.com for more information.

### Old Stuff from here on out

This is a recreation of the classic pong video game. Implemented in the C
programming language, using SDL library to display graphics on the screen.

To compile you need to have SDL installed on your system and the header files
available to you operating systems PATH environment variable

	gcc pong.c `sdl2-config --cflags --libs` -o pong

### Update early 2019

This has been ported from SDL version 1 to work with SDL version 2. Also added is the ability to run the
executable with the '-f' command line argument to run in fullscreen 
(will stretch the 640 * 480 image your systems native display resolution)

I will take any feature requests like updating the AI or movement animation if there is any demand. Or 
just clone the code and do it yourself. Enjoy!

## Controls
* OK to start game
* arrow keys (up/down) for movement
* any other key to exit game

## Images - Out of Date; from original forked software
![title screen](http://i.imgur.com/radat.png)

![game play](http://i.imgur.com/CZhqp.png)
