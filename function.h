#ifndef FUNCTION_H
#define FUNCTION_H
#define TEXT_SIZE 128
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TIMER_POSITION 5
#define START_POSITION 5
#define NUMBER_OF_CASES 6
#define BOARD_WIDTH 16
#define BOARD_HEIGHT 11
#define IMAGE_SIZE 40
#define INCREASE 1
#define DECREASE -1
#define NOTMOVE 0
#define FREE_FIELD '4'
#define CASE_FIELD '2'
#define TARGET_FIELD '5'
#define FILE_NAME "board.txt"
#define NEW_GAME 1

enum options
{
	left = -1,
	top = 0,
	right = 1,
	bottom = 2,
	new_game = 3,
	end_game = 4
};

struct board_info
{
	char value;
	char target;
};

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
SDL_Surface *load_image(char *image_name, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *charset = NULL);
board_info **board_upload(char *string);
void display_board(board_info **board, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *charset);
struct person
{
	int position_x = START_POSITION;
	int position_y = START_POSITION;
	SDL_Surface *player_surface;
	void move(int x, int y)
	{
		position_x += x;
		position_y += y;
	}
	void show(SDL_Surface *screen)
	{
		DrawSurface(screen, player_surface, (IMAGE_SIZE/2)+(IMAGE_SIZE*position_x), IMAGE_SIZE + (IMAGE_SIZE / 2) + (IMAGE_SIZE*position_y));
	}
};
void check_position(board_info **board, person *player, options choice);
void move_cases(board_info **board, person *player, options choice);
void remove_board(board_info **board);
int check_end_game(board_info **board, person *player, options choice,int start_game = 0);
#endif

