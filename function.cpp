#include "function.h"
#include <stdio.h>
#include<string.h>

void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};

void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

SDL_Surface *load_image(char *image_name, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *charset)
{
	SDL_Surface *image = SDL_LoadBMP(image_name);
	if (image == nullptr)
	{
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		exit(1);
	}
	return image;
}

board_info **board_upload(char *string)
{
	FILE *boardFile;
	boardFile = fopen(string, "r");
	char sign[BOARD_WIDTH][BOARD_HEIGHT];
	if (boardFile == NULL)
	{
		printf("File error");
		return 0;
	}
	board_info **board = new board_info*[BOARD_HEIGHT];
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		board[i] = new board_info[BOARD_WIDTH];
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			sign[i][j] = fgetc(boardFile);
			if (sign[i][j] == '\n')
			{
				j--;
			}
			else
			{	
				board[i][j].target = sign[i][j];
				board[i][j].value = sign[i][j];
			}
		}
	}
	fclose(boardFile);
	return board;
}

void display_board(board_info **board, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *charset)
{
	SDL_Surface *image1 = load_image("images/block.bmp", screen, scrtex, window, renderer, charset);
	SDL_Surface *image2 = load_image("images/case_box.bmp", screen, scrtex, window, renderer, charset);
	SDL_Surface *image3 = load_image("images/ground.bmp", screen, scrtex, window, renderer, charset);
	SDL_Surface *image4 = load_image("images/environment.bmp", screen, scrtex, window, renderer, charset);
	SDL_Surface *image5 = load_image("images/target.bmp", screen, scrtex, window, renderer, charset);
	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			switch (board[i][j].value)
			{
			case '1':
				DrawSurface(screen, image1, IMAGE_SIZE / 2 + (IMAGE_SIZE * j), IMAGE_SIZE + IMAGE_SIZE / 2 + (IMAGE_SIZE * i));
				break;
			case '2':
				DrawSurface(screen, image2, IMAGE_SIZE / 2 + (IMAGE_SIZE * j), IMAGE_SIZE + IMAGE_SIZE / 2 + (IMAGE_SIZE * i));
				break;
			case '3':
				DrawSurface(screen, image3, IMAGE_SIZE / 2 + (IMAGE_SIZE * j), IMAGE_SIZE + IMAGE_SIZE / 2 + (IMAGE_SIZE * i));
				break;
			case '4':
				DrawSurface(screen, image4, IMAGE_SIZE / 2 + (IMAGE_SIZE * j), IMAGE_SIZE + IMAGE_SIZE / 2 + (IMAGE_SIZE * i));
				break;
			case '5':
				DrawSurface(screen, image5, IMAGE_SIZE / 2 + (IMAGE_SIZE * j), IMAGE_SIZE + IMAGE_SIZE / 2 + (IMAGE_SIZE * i));
				break;
			}

		}
	}
}

void check_position(board_info **board,person *player,options choice)
{
	//check for a free field upper
	if ((board[player->position_y - 1][player->position_x].value == FREE_FIELD ||
		 board[player->position_y - 1][player->position_x].value == TARGET_FIELD) && choice == top)
	{
		player->move(NOTMOVE,DECREASE);
	}
	//check for a free field under
	else if ((board[player->position_y + 1][player->position_x].value == FREE_FIELD ||
			  board[player->position_y + 1][player->position_x].value == TARGET_FIELD) && choice == bottom)
	{
		player->move(NOTMOVE,INCREASE);
	}
	//check for a free field left
	else if ((board[player->position_y][player->position_x - 1].value == FREE_FIELD ||
			  board[player->position_y][player->position_x - 1].value == TARGET_FIELD) && choice == left)
	{
		player->move(DECREASE,NOTMOVE);
	}
	//check for a free field right
	else if ((board[player->position_y][player->position_x + 1].value == FREE_FIELD ||
		   	  board[player->position_y][player->position_x + 1].value == TARGET_FIELD) && choice == right)
	{
		player->move(INCREASE, NOTMOVE);
	}
}

void move_cases(board_info **board, person *player, options choice)
{
	if (board[player->position_y - 1][player->position_x].value == CASE_FIELD &&
	   (board[player->position_y - 2][player->position_x].value == FREE_FIELD ||
		board[player->position_y - 2][player->position_x].value == TARGET_FIELD) && choice == top)
	{
		if (board[player->position_y - 1][player->position_x].target == TARGET_FIELD &&
			board[player->position_y - 1][player->position_x].value == CASE_FIELD)
		{
			board[player->position_y - 1][player->position_x].value = TARGET_FIELD;
		}
		else
		{
			board[player->position_y - 1][player->position_x].value = FREE_FIELD;
		}
		board[player->position_y - 2][player->position_x].value = CASE_FIELD;
	}
	else if (board[player->position_y + 1][player->position_x].value == CASE_FIELD &&
			(board[player->position_y + 2][player->position_x].value == FREE_FIELD ||
			 board[player->position_y + 2][player->position_x].value == TARGET_FIELD) && choice == bottom)
	{
		if (board[player->position_y + 1][player->position_x].target == TARGET_FIELD &&
			board[player->position_y + 1][player->position_x].value == CASE_FIELD)
		{
			board[player->position_y + 1][player->position_x].value = TARGET_FIELD;
		}
		else
		{
			board[player->position_y + 1][player->position_x].value = FREE_FIELD;
		}
		board[player->position_y + 2][player->position_x].value = CASE_FIELD;
	}
	else if (board[player->position_y][player->position_x - 1].value == CASE_FIELD &&
			(board[player->position_y][player->position_x - 2].value == FREE_FIELD ||
			 board[player->position_y][player->position_x - 2].value == TARGET_FIELD) && choice == left)
	{
		if (board[player->position_y][player->position_x - 1].target == TARGET_FIELD &&
			board[player->position_y][player->position_x - 1].value == CASE_FIELD)
		{
			board[player->position_y][player->position_x - 1].value = TARGET_FIELD;
		}
		else
		{
			board[player->position_y][player->position_x - 1].value = FREE_FIELD;
		}
		board[player->position_y][player->position_x - 2].value = CASE_FIELD;
	}
	else if (board[player->position_y][player->position_x + 1].value == CASE_FIELD &&
			(board[player->position_y][player->position_x + 2].value == FREE_FIELD ||
			 board[player->position_y][player->position_x + 2].value == TARGET_FIELD) && choice == right)
	{
		if (board[player->position_y][player->position_x + 1].target == TARGET_FIELD &&
			board[player->position_y][player->position_x + 1].value == CASE_FIELD)
		{
			board[player->position_y][player->position_x + 1].value = TARGET_FIELD;
		}
		else
		{
			board[player->position_y][player->position_x + 1].value = FREE_FIELD;
		}
		board[player->position_y][player->position_x + 2].value = CASE_FIELD;
	}
}

void remove_board(board_info **board)
{
	for (int i = 0; i <BOARD_HEIGHT; i++)
	{
		delete[] board[i];
	}
	delete[] board;
}

int check_end_game(board_info **board, person *player, options choice, int start_game)
{
	static int check_field;
	if (choice == new_game && start_game)
	{
		check_field = 0;
	}
	if (check_field < NUMBER_OF_CASES + 1)
	{
		if (board[player->position_y - 1][player->position_x].value == CASE_FIELD && choice == top)
		{
			if (board[player->position_y - 2][player->position_x].value == TARGET_FIELD)
			{
				check_field++;
			}
			else if (board[player->position_y - 1][player->position_x].target == TARGET_FIELD)
			{
				check_field--;
			}
			if (check_field == NUMBER_OF_CASES)
			{
				move_cases(board, player, top);
				check_position(board, player, top);
				check_field++;
			}
		}
		else if (board[player->position_y + 1][player->position_x].value == CASE_FIELD && choice == bottom)
		{
			if (board[player->position_y + 2][player->position_x].value == TARGET_FIELD)
			{
				check_field++;
			}
			else if (board[player->position_y + 1][player->position_x].target == TARGET_FIELD)
			{
				check_field--;
			}
			if (check_field == NUMBER_OF_CASES)
			{
				move_cases(board, player, bottom);
				check_position(board, player, bottom);
				check_field++;
			}
		}
		else if (board[player->position_y][player->position_x - 1].value == CASE_FIELD && choice == left)
		{
			if (board[player->position_y][player->position_x - 2].value == TARGET_FIELD)
			{
				check_field++;
			}
			else if (board[player->position_y][player->position_x - 1].target == TARGET_FIELD)
			{
				check_field--;
			}
			if (check_field == NUMBER_OF_CASES)
			{
				move_cases(board, player, left);
				check_position(board, player, left);
				check_field++;
			}
		}
		else if (board[player->position_y][player->position_x + 1].value == CASE_FIELD && choice == right)
		{
			if (board[player->position_y][player->position_x + 2].value == TARGET_FIELD)
			{
				check_field++;
			}
			else if (board[player->position_y][player->position_x + 1].target == TARGET_FIELD)
			{
				check_field--;
			}
			if (check_field == NUMBER_OF_CASES)
			{
				move_cases(board, player, right);
				check_position(board, player, right);
				check_field++;
			}
		}
		if (choice == end_game)
		{
			return check_field;
		}
	}
	return check_field;
}

