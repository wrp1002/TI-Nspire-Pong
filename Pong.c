#include <os.h>
#include <SDL/SDL.h>
#include <math.h>
#include <time.h>

float PI = 3.14159;
SDL_bool done = SDL_FALSE;
float maxSpeed = 3.5;
float accSpeed = .2;
int winningScore = 10;

struct Player {
	float x, y, velY;
	int score;
};

struct Ball {
	float x, y, speed, dir;
};



float GetAngle(float x1, float y1, float x2, float y2) {
	return atan2(y1 - y2, x1 - x2);
}

int GetDigit(int x, int n) {
    while (n--) {
        x /= 10;
    }
    return (x % 10);
}

void DrawScore(SDL_Surface *numbers, SDL_Surface *screen, int x, int y, int score) {
	int first = GetDigit(score, 1);
	int second = GetDigit(score, 0);
	
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_Rect size;
	size.y = 0;
	size.x = 14 * first;
	size.w = 14;
	size.h = 20;
	if (first > 0)
	SDL_BlitSurface(numbers, &size, screen, &dest);
	
	
	dest.x = x + 14;
	dest.y = y;
	size.y = 0;
	size.x = 14 * second;
	size.w = 14;
	size.h = 20;
	SDL_BlitSurface(numbers, &size, screen, &dest);
}

void Init(SDL_Surface **screen) {
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    if(screen == NULL) {
        printf("Couldn't initialize display: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_ShowCursor(SDL_DISABLE);
}

void InitPlayers(struct Player players[]) {
	players[0].x = 10;
	players[0].y = SCREEN_HEIGHT / 2 - 15;
	players[0].score = 0;
	players[0].velY = 0;
	
	players[1].x = SCREEN_WIDTH - 10;
	players[1].y = SCREEN_HEIGHT / 2 - 15;
	players[1].score = 0;
	players[1].velY = 0;
};

void InitBall(struct Ball *ball) {
	ball->x = SCREEN_WIDTH / 2;
	ball->y = -10;
	ball->speed = 0;
	ball->dir = 0;
}

void UpdateBall(struct Ball *ball, struct Player players[], int *lastScored, SDL_bool *scored) {
	ball->x += ball->speed * cos(ball->dir);
	ball->y += ball->speed * sin(ball->dir);
	
	if (ball->y < 0 || ball->y > SCREEN_HEIGHT)
		ball->dir = (360 - ((ball->dir) * (180 / PI))) * (PI / 180);
	
	if (ball->y < 0)
		ball->y = 0;
	else if (ball->y > SCREEN_HEIGHT)
		ball->y = SCREEN_HEIGHT;
	
	int playerX = players[0].x;
	int playerY = players[0].y;
	int ballX = ball->x;
	int ballY = ball->y;
	if (ballX <= playerX + 3 && ballX >= playerX && ballY >= playerY && ballY <= playerY + 30) {
		if (ball->speed < 4)
			ball->speed = 4;
		else
			ball->speed += .25;
		ball->dir = GetAngle(ballX, ballY, playerX - 5, playerY + 15);
	}
	
	playerX = players[1].x;
	playerY = players[1].y;
	if (ballX >= playerX && ballX <= playerX + 3 && ballY >= playerY && ballY <= playerY + 30) {
		if (ball->speed < 4)
			ball->speed = 4;
		else
			ball->speed += .25;
		ball->dir = GetAngle(ballX, ballY, playerX + 8, playerY + 15);
	}
	
	if (ballX < -10) {
		players[1].score++;
		*lastScored = 1;
		ball->x = SCREEN_WIDTH / 2;
		ball->y = -10;
		ball->speed = 0;
		*scored = SDL_TRUE;
	}
	else if (ballX > SCREEN_WIDTH + 10) {
		players[0].score++;
		*lastScored = 0;
		ball->x = SCREEN_WIDTH / 2;
		ball->y = -10;
		ball->speed = 0;
		*scored = SDL_TRUE;
	}
}

void DrawPlayers(struct Player players[], SDL_Surface *screen) {
	SDL_Rect place;
	place.x = players[0].x;
	place.y = players[0].y;
	place.w = 3;
	place.h = 30;
	
	SDL_FillRect(screen, &place, SDL_MapRGB(screen->format, 255, 255, 255));
	
	place.x = players[1].x;
	place.y = players[1].y;
	place.w = 3;
	place.h = 30;
	
	SDL_FillRect(screen, &place, SDL_MapRGB(screen->format, 255, 255, 255));
};

void DrawBall(struct Ball ball, SDL_Surface *screen) {
	SDL_Rect place;
	place.x = ball.x - 2;
	place.y = ball.y - 2;
	place.w = 4;
	place.h = 4;
	
	SDL_FillRect(screen, &place, SDL_MapRGB(screen->format, 255, 255, 255));
};

int Controls(struct Player players[], int gameState) {
	if (isKeyPressed(KEY_NSPIRE_SHIFT))
		done = SDL_TRUE;
	
	if (isKeyPressed(KEY_NSPIRE_DEL)) {
		return 1;
	}
	
	if (isKeyPressed(KEY_NSPIRE_CTRL) && gameState == 0) {
		show_1numeric_input("Set Winning Score", "", "", &winningScore, 1, 100);
	}
	
/* 	if (isKeyPressed(KEY_NSPIRE_SCRATCHPAD)) {
 * 		players[0].velY -= accSpeed;
 * 		if (players[0].velY < -maxSpeed)
 * 			players[0].velY = -maxSpeed;
 * 	}
 * 	else if (isKeyPressed(KEY_NSPIRE_TAB)) {
 * 		players[0].velY += accSpeed;
 * 		if (players[0].velY > maxSpeed)
 * 			players[0].velY = maxSpeed;
 * 	}
 * 	else {
 * 		if (players[0].velY < 0)
 * 			players[0].velY += accSpeed;
 * 		else if (players[0].velY > 0)
 * 			players[0].velY -= accSpeed;
 * 		if (abs(players[0].velY) <= .1)
 * 			players[0].velY = 0;
 * 	}
 * 	
 * 	if (isKeyPressed(KEY_NSPIRE_DOC)) {
 * 		players[1].velY -= accSpeed;
 * 		if (players[1].velY < -maxSpeed)
 * 			players[1].velY = -maxSpeed;
 * 	}
 * 	else if (isKeyPressed(KEY_NSPIRE_MENU)) {
 * 		players[1].velY += accSpeed;
 * 		if (players[1].velY > maxSpeed)
 * 			players[1].velY = maxSpeed;
 * 	}
 * 	else {
 * 		if (players[1].velY < 0)
 * 			players[1].velY += accSpeed;
 * 		else if (players[1].velY > 0)
 * 			players[1].velY -= accSpeed;
 * 		if (abs(players[1].velY) <= .1)
 * 			players[1].velY = 0;
 * 	}
 */
 
 if (isKeyPressed(KEY_NSPIRE_Q)) {
		players[0].velY -= accSpeed;
		if (players[0].velY < -maxSpeed)
			players[0].velY = -maxSpeed;
	}
	else if (isKeyPressed(KEY_NSPIRE_A)) {
		players[0].velY += accSpeed;
		if (players[0].velY > maxSpeed)
			players[0].velY = maxSpeed;
	}
	else {
		if (players[0].velY < 0)
			players[0].velY += accSpeed;
		else if (players[0].velY > 0)
			players[0].velY -= accSpeed;
		if (abs(players[0].velY) <= .1)
			players[0].velY = 0;
	}
	
	if (isKeyPressed(KEY_NSPIRE_P)) {
		players[1].velY -= accSpeed;
		if (players[1].velY < -maxSpeed)
			players[1].velY = -maxSpeed;
	}
	else if (isKeyPressed(KEY_NSPIRE_L)) {
		players[1].velY += accSpeed;
		if (players[1].velY > maxSpeed)
			players[1].velY = maxSpeed;
	}
	else {
		if (players[1].velY < 0)
			players[1].velY += accSpeed;
		else if (players[1].velY > 0)
			players[1].velY -= accSpeed;
		if (abs(players[1].velY) <= .1)
			players[1].velY = 0;
	}
 
	return 0;
}

void UpdatePlayers(struct Player players[]) {
	int i;
	for (i = 0; i < 2; i++) {
		players[i].y += players[i].velY;
		
		if (players[i].y < 0) {
			players[i].y = 0;
			players[i].velY = 0;
		}
		else if (players[i].y > SCREEN_HEIGHT - 30) {
			players[i].y = SCREEN_HEIGHT - 30;
			players[i].velY = 0;
		}
	}
}

int main(int arc, char **argv) {
	srand(time(0));
	enable_relative_paths(argv);
	SDL_Surface *screen = SDL_SetVideoMode(320, 240, is_cx ? 16 : 8, SDL_SWSURFACE);
	SDL_Surface *background = SDL_LoadBMP("Resources\\background.bmp.tns");
	SDL_Surface *numbers = SDL_LoadBMP("Resources\\numbers.bmp.tns");
	SDL_Surface *win = SDL_LoadBMP("Resources\\win.bmp.tns");
	SDL_SetColorKey(numbers, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(numbers->format, 255, 0, 255));
	SDL_SetColorKey(win, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(win->format, 255, 0, 255));
	
	nSDL_Font *font = nSDL_LoadFont(NSDL_FONT_TINYTYPE,255, 255, 255);
	
	float frameTime = 0;
	int gameState = 0;
	int prevTime = 0;
	int curTime = 0;
	float deltaTime = 0;
	int lastScored = rand() % 2;
	int waitTimerStart = 50;
	int waitTimer = waitTimerStart;
	SDL_bool scored = SDL_TRUE;
	SDL_bool over = SDL_FALSE;
	
	struct	Ball ball;
	struct Player players[2];
	
	Init(&screen);
	InitPlayers(players);
	InitBall(&ball);
	
	while (!done) {
		prevTime = curTime;
		curTime = SDL_GetTicks();
		deltaTime = (curTime - prevTime) / 1000.0f;
		frameTime += deltaTime;
		
		if (frameTime >= 1.0/60.0) {
			frameTime = 0;
			
			int control = Controls(players, gameState);
			
			if (control == 1) {
				scored = SDL_TRUE;
				over = SDL_FALSE;
				gameState = 0;
				lastScored = rand() % 2;
				waitTimer = waitTimerStart;
				InitPlayers(players);
				InitBall(&ball);
			}
			else if (control == 2) {
			}
			
			
			if (gameState == 1) {
				if (!over) {
					if (!scored)
						UpdateBall(&ball, players, &lastScored, &scored);
					UpdatePlayers(players);
					
					if (scored) {
						if (players[0].score == winningScore || players[1].score == winningScore) {
							over = SDL_TRUE;
							waitTimer = 200;
						}
						waitTimer--;
						if (waitTimer < 0) {
							waitTimer = waitTimerStart;
							if (lastScored == 0) {
								ball.dir = 0 + (rand() % 51 - 25) * (M_PI / 180);
								ball.y = players[1].y + 15;
							}
							else {
								ball.dir = PI + (rand() % 51 - 25) * (M_PI / 180);
								ball.y = players[0].y + 15;
							}
							ball.speed = 2;
							scored = SDL_FALSE;
						}
					}
				}
				else {
					waitTimer--;
					if (waitTimer < 0) {
						waitTimer = waitTimerStart;
						scored = SDL_TRUE;
						over = SDL_FALSE;
						gameState = 0;
						lastScored = rand() % 2;
						waitTimer = waitTimerStart;
						InitPlayers(players);
						InitBall(&ball);
					}
				}
			}
			else if (any_key_pressed()) {
				if (!isKeyPressed(KEY_NSPIRE_DEL) && !isKeyPressed(KEY_NSPIRE_CTRL) && !isKeyPressed(KEY_NSPIRE_ENTER)) {
					gameState = 1;
					if (rand() % 2 == 0)
						ball.dir = 0 + (rand() % 51 - 25) * (M_PI / 180);
					else
						ball.dir = PI + (rand() % 51 - 25) * (M_PI / 180);
					ball.speed = 2;
				}
			}
			
			SDL_BlitSurface(background, NULL, screen, NULL);
			DrawScore(numbers, screen, SCREEN_WIDTH / 2 - 25 - (numbers->w / 10 * 2), 25, players[0].score);
			DrawScore(numbers, screen, SCREEN_WIDTH / 2 + 25, 25, players[1].score);
			DrawPlayers(players, screen);
			DrawBall(ball, screen);
			if (gameState == 0) {
				SDL_Rect menu;
				menu.x = SCREEN_WIDTH / 2 - 50;
				menu.y = SCREEN_HEIGHT / 2 - 10;
				menu.w = 100;
				menu.h = 28;
				
				SDL_FillRect(screen, &menu, SDL_MapRGB(screen->format, 0, 0, 0));
				nSDL_DrawString(screen, font, SCREEN_WIDTH / 2 - nSDL_GetStringWidth(font, "Press Any Key To Start") / 2, SCREEN_HEIGHT / 2 - 5, "Press Any Key To Start");
				nSDL_DrawString(screen, font, SCREEN_WIDTH / 2 - nSDL_GetStringWidth(font, "First to 10 wins") / 2, SCREEN_HEIGHT / 2 + 5, "First to %i wins", winningScore);
			}
			if (over) {
				SDL_Rect dest;
				if (players[0].score == winningScore)
					dest.x = SCREEN_WIDTH / 2 - SCREEN_WIDTH / 4 - win->w / 2;
				else
					dest.x = SCREEN_WIDTH / 2 + SCREEN_WIDTH / 4 - win->w / 2;
				dest.y = 100;
				SDL_BlitSurface(win, NULL, screen, &dest);
			}
			
			SDL_Flip(screen);
		}
	};
	
	return 0;
}









