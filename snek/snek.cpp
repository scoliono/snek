#include "stdafx.h"

#define LENGTH 750
#define FPS 45
const int SCREEN_TICKS_PER_FRAME = 1000 / FPS;

const int tile_size = 10;

typedef enum block_states {
	BLANK,
	SNAKE,
	FOOD
} block_state_t;

typedef enum directions {
	LEFT,
	RIGHT,
	UP,
	DOWN
} direction_t;

int snake_length = 1;
int snake_head_pos[(LENGTH / tile_size)*(LENGTH / tile_size)][2] = {0, 0};
int food_eaten = 0;
direction_t snake_direction = RIGHT;
block_state_t board[LENGTH / tile_size][LENGTH / tile_size];

SDL_Rect snake;
SDL_Rect food;
SDL_Rect blank;

TTF_Font *fnt = NULL;

void spawn_food()
{
	bool valid = false;
	int x, y;
	while (!valid)
	{
		x = rand() % (LENGTH / tile_size);
		y = rand() % (LENGTH / tile_size);
		if (board[x][y] == BLANK)
		{
			valid = true;
		}
	}
	board[x][y] = FOOD;
}

void push_to_front(int x, int y)
{
	// Shift everything down by one
	int temp[(LENGTH / tile_size)*(LENGTH / tile_size)][2];
	for (int i = 0; i < (LENGTH / tile_size)*(LENGTH / tile_size)-1; i++)
	{
		temp[i][0] = snake_head_pos[i][0];
		temp[i][1] = snake_head_pos[i][1];
	}
	for (int i = 0; i < (LENGTH / tile_size)*(LENGTH / tile_size)-1; i++)
	{
		snake_head_pos[i + 1][0] = temp[i][0];
		snake_head_pos[i + 1][1] = temp[i][1];
	}
	// Put the new value at the front
	snake_head_pos[0][0] = x;
	snake_head_pos[0][1] = y;
}

int main(int argc, char *argv[])
{
	SDL_Window* win = NULL;
	SDL_Surface* scr = NULL;

	srand(time(NULL));

	snake.w = tile_size * 0.9;
	snake.h = tile_size * 0.9;

	food.w = tile_size * 0.9;
	food.h = tile_size * 0.9;

	blank.w = tile_size * 0.9;
	blank.h = tile_size * 0.9;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		wchar_t buffer[256];
		swprintf(buffer, 256, L"%s", SDL_GetError());
		printf("%s", SDL_GetError());
		MessageBox(NULL, buffer, (LPCWSTR)L"SDL Initialization Failed", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
	}

	win = SDL_CreateWindow("snek", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, LENGTH, LENGTH+20, SDL_WINDOW_SHOWN);

	if (win == NULL)
	{
		wchar_t buffer[256];
		swprintf(buffer, 256, L"%s", SDL_GetError());
		printf("%s", SDL_GetError());
		MessageBox(NULL, buffer, (LPCWSTR)L"Failed to Create Window", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
	}

	SDL_Renderer* render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (render == NULL)
	{
		wchar_t buffer[256];
		swprintf(buffer, 256, L"%s", SDL_GetError());
		printf("%s", SDL_GetError());
		MessageBox(NULL, buffer, (LPCWSTR)L"Failed to Create Renderer", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
	}
	else
	{
		//Initialize renderer color
		SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0xFF);

		//Initialize PNG loading
		/*if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		{
			wchar_t buffer[256];
			swprintf(buffer, 256, L"%s", IMG_GetError());
			MessageBox(NULL, buffer, (LPCWSTR)L"SDL_image Initialization Failed", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
		}*/

		//Initialize SDL_ttf
		if (TTF_Init() == -1)
		{
			wchar_t buffer[256];
			swprintf(buffer, 256, L"%s", TTF_GetError());
			printf("%s", TTF_GetError());
			MessageBox(NULL, buffer, (LPCWSTR)L"SDL_TTF Initialization Failed", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
		}
		fnt = TTF_OpenFont("VGA.TTF", 24);
		if (fnt == NULL)
		{
			wchar_t buffer[256];
			swprintf(buffer, 256, L"%s", TTF_GetError());
			printf("%s", TTF_GetError());
			MessageBox(NULL, buffer, (LPCWSTR)L"Failed to load VGA.TTF", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
		}
	}

	bool quit = false;
	SDL_Event e;

	// Set up board.
	for (int i = 0; i < LENGTH / tile_size; i++)
	{
		for (int j = 0; j < LENGTH / tile_size; j++)
		{
			board[i][j] = BLANK;
		}
	}
	spawn_food();

	// Game loop
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			// Change snake_direction if we get appropriate input.
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_UP:
						// Prevent accidental suicide
						if (snake_direction != DOWN || snake_length == 1) snake_direction = UP;
						break;

					case SDLK_DOWN:
						if (snake_direction != UP || snake_length == 1) snake_direction = DOWN;
						break;

					case SDLK_LEFT:
						if (snake_direction != RIGHT || snake_length == 1) snake_direction = LEFT;
						break;

					case SDLK_RIGHT:
						if (snake_direction != LEFT || snake_length == 1) snake_direction = RIGHT;
						break;

					default:
						break;
				}
			}
		}


		// Continue in snake_direction.
		switch (snake_direction)
		{
			case UP:
				push_to_front(snake_head_pos[0][0], snake_head_pos[0][1]-1);
				break;

			case DOWN:
				push_to_front(snake_head_pos[0][0], snake_head_pos[0][1]+1);
				break;

			case LEFT:
				push_to_front(snake_head_pos[0][0]-1, snake_head_pos[0][1]);
				break;

			case RIGHT:
				push_to_front(snake_head_pos[0][0]+1, snake_head_pos[0][1]);
				break;

			default:
				break;
		}

		wchar_t death_str[256];
		// Out of bounds?
		if (snake_head_pos[0][0] < 0 || snake_head_pos[0][1] < 0 || snake_head_pos[0][0] > LENGTH/tile_size || snake_head_pos[0][1] > LENGTH/tile_size)
		{
			swprintf(death_str, 256, L"You are dead. Not big surprise.\nFood eaten: %d\nSnake length: %d", food_eaten, snake_length);
			MessageBox(NULL, death_str, L"Snek hit a wall", MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);
			return 0;
		}

		// Look for collisions.
		switch (board[snake_head_pos[0][0]][snake_head_pos[0][1]])
		{
			case FOOD:
				food_eaten++;
				snake_length = 1 + 5*food_eaten;
				board[snake_head_pos[0][0]][snake_head_pos[0][1]] = SNAKE;
				spawn_food();
				break;
			case SNAKE:
				swprintf(death_str, 256, L"You are dead. Not big surprise.\nFood eaten: %d\nSnake length: %d", food_eaten, snake_length);
				MessageBox(NULL, death_str, L"Snek tried to eat himself", MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);
				return 0;
				break;
			case BLANK:
			default:
				board[snake_head_pos[0][0]][snake_head_pos[0][1]] = SNAKE;
				break;
		}

		// Remove the old position(s) that are longer than the snake's length.
		for (int i = snake_length; i < (LENGTH / tile_size)*(LENGTH / tile_size); i++)
		{
			board[snake_head_pos[i][0]][snake_head_pos[i][1]] = BLANK;
		}

		// Clear the frame.
		SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(render);

		// Start drawing.
		// Each part of the snake's body
		for (int i = 0; i < LENGTH / tile_size; i++)
		{
			for (int j = 0; j < LENGTH / tile_size; j++)
			{
				switch (board[i][j])
				{
					case SNAKE:
						snake.x = i*tile_size;
						snake.y = j*tile_size;
						SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0x00, 0xFF);
						SDL_RenderFillRect(render, &snake);
						break;
					case FOOD:
						food.x = i*tile_size;
						food.y = j*tile_size;
						SDL_SetRenderDrawColor(render, 0xFF, 0x00, 0x00, 0xFF);
						SDL_RenderFillRect(render, &food);
						break;
					case BLANK:
					default:
						blank.x = i*tile_size;
						blank.y = j*tile_size;
						SDL_SetRenderDrawColor(render, 0x00, 0x00, 0xFF, 0xFF);
						SDL_RenderFillRect(render, &blank);
						break;
				}
			}
		}

		// Render the scoreboard.
		SDL_SetRenderDrawColor(render, 0x00, 0x00, 0x00, 0xFF);
		SDL_Rect scoreboard_outline;
		scoreboard_outline.x = 0;
		scoreboard_outline.y = LENGTH;
		scoreboard_outline.h = 20;
		scoreboard_outline.w = LENGTH;
		SDL_RenderFillRect(render, &scoreboard_outline);

		SDL_Color text_color = { 0, 255, 0 };
		char score_str[128];
		sprintf_s(score_str, 128, "SNEK    LENGTH: %d    FOOD: %d", snake_length, food_eaten);
		SDL_Texture* text_texture;
		SDL_Surface* text_surface = TTF_RenderText_Solid(fnt, score_str, text_color);
		int text_w, text_h;
		if (text_surface == NULL)
		{
			wchar_t buffer[256];
			swprintf(buffer, 256, L"%s", TTF_GetError());
			printf("%s", TTF_GetError());
			MessageBox(NULL, buffer, L"SDL_TTF unable to render text surface", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
			return 0;
		}
		else
		{
			//Create texture from surface pixels
			text_texture = SDL_CreateTextureFromSurface(render, text_surface);
			if (text_texture == NULL)
			{
				wchar_t buffer[256];
				swprintf(buffer, 256, L"%s", TTF_GetError());
				printf("%s", TTF_GetError());
				MessageBox(NULL, buffer, L"SDL_TTF unable to create texture from rendered text", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
				return 0;
			}
			text_w = text_surface->w;
			text_h = text_surface->w;
			SDL_FreeSurface(text_surface);
		}

		if (text_texture == NULL)
		{
			wchar_t buffer[256];
			swprintf(buffer, 256, L"%s", TTF_GetError());
			printf("%s", TTF_GetError());
			MessageBox(NULL, buffer, L"Failed to render text", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
			return 0;
		}
		SDL_Rect render_quad = { 0, LENGTH, 500, 20 };
		SDL_RenderCopyEx(render, text_texture, NULL, &render_quad, 0.0, NULL, SDL_FLIP_NONE);

		SDL_RenderPresent(render);
		SDL_Delay(SCREEN_TICKS_PER_FRAME);
	}

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(win);
	win = NULL;
	render = NULL;

	TTF_CloseFont(fnt);
	fnt = NULL;
	TTF_Quit();
	//IMG_Quit();
	SDL_Quit();
	return 0;
}
