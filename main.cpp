#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include "function.h"

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) 
{
	SDL_Event event;
	SDL_Surface *screen;
	SDL_Surface *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct person *player = new person();
	bool quit = false;
	bool alert = true;
	int start_time, end_time;
	double game_time = 0;
	char text[TEXT_SIZE];
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Sokoban");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	int green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);

	SDL_ShowCursor(SDL_DISABLE);

	charset = load_image("images/cs8x8.bmp",screen,scrtex,window,renderer);
	
	SDL_SetColorKey(charset, true, 0x000000);

	player->player_surface = load_image("./images/player.bmp", screen, scrtex, window, renderer, charset);
	
	double time = 0;
	double counter = 0;
	board_info **board = board_upload(FILE_NAME);

	start_time = SDL_GetTicks();

	while (!quit)
	{
		end_time = SDL_GetTicks();
		counter = (end_time - start_time)*0.001;
		start_time = end_time;
		if (alert)
		{
			time += counter;
		}
		display_board(board, screen, scrtex, window, renderer, charset);
		player->show(screen);
		DrawRectangle(screen, TIMER_POSITION, TIMER_POSITION, SCREEN_WIDTH - TIMER_POSITION*2, TIMER_POSITION*5, red, green);
		sprintf(text, "Dawid Wesolowski		Czas trwania = %.1lf s", time);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, TIMER_POSITION*2, text, charset);
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		if (check_end_game(board, player, end_game) > NUMBER_OF_CASES && alert)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "KOMUNIKAT", "KONIEC GRY", window);
			alert = false;
		}
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_UP:
							if (check_end_game(board,player,top) < NUMBER_OF_CASES)
							{
								move_cases(board, player, top);
								check_position(board, player, top);
							}
							break;
						case SDLK_DOWN:
							if (check_end_game(board, player, bottom) < NUMBER_OF_CASES)
							{
								move_cases(board, player, bottom);
								check_position(board, player, bottom);
							}
							break;
						case SDLK_LEFT:
							if (check_end_game(board, player, left) < NUMBER_OF_CASES)
							{
								move_cases(board, player, left);
								check_position(board, player, left);
							}
							break;
						case SDLK_RIGHT:
							if (check_end_game(board, player, right) < NUMBER_OF_CASES)
							{
								move_cases(board, player, right);
								check_position(board, player, right);
							}
							break;
						case 'n':
							counter = 0;
							time = 0;
							player->position_x = START_POSITION;
							player->position_y = START_POSITION;
							remove_board(board);
							board = board_upload(FILE_NAME);
							check_end_game(board, player, new_game, NEW_GAME);
							alert = true;
							break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
	}
	remove_board(board);
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(player->player_surface);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
};
