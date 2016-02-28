#include <gb/gb.h>
#include <gb/drawing.h>
#include <minesweeper.h>
#include <stdlib.h>
#include "game.h"
#include "graphics.h"
#include "utils.h"

#define GRID_WIDTH 20
#define GRID_HEIGHT 18

#define MARKER_NB 0

TILESET(
	game_tiles,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, // 0
	0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x1C, 0x1C, 0x04, 0x04, 0x04, 0x04, // 1
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F, 0x1F,
	0x00, 0x00, 0x3C, 0x3C, 0x04, 0x04, 0x04, 0x04, // 2
	0x3C, 0x3C, 0x20, 0x20, 0x20, 0x20, 0x3C, 0x3C,
	0x00, 0x00, 0x3C, 0x3C, 0x04, 0x04, 0x04, 0x04, // 3
	0x3C, 0x3C, 0x04, 0x04, 0x04, 0x04, 0x3C, 0x3C,
	0x00, 0x00, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, // 4
	0x3C, 0x3C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x00, 0x00, 0x3C, 0x3C, 0x20, 0x20, 0x20, 0x20, // 5
	0x3C, 0x3C, 0x04, 0x04, 0x04, 0x04, 0x3C, 0x3C,
	0x00, 0x00, 0x3C, 0x3C, 0x20, 0x20, 0x20, 0x20, // 6
	0x3C, 0x3C, 0x24, 0x24, 0x24, 0x24, 0x3C, 0x3C,
	0x00, 0x00, 0x3C, 0x3C, 0x04, 0x04, 0x04, 0x04, // 7
	0x0E, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x00, 0x00, 0x3C, 0x3C, 0x24, 0x24, 0x24, 0x24, // 8
	0x3C, 0x3C, 0x24, 0x24, 0x24, 0x24, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x7E, // Unopened
	0x00, 0x7E, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x18, 0x18, 0x38, 0x38, // Flagged
	0x18, 0x18, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x18, 0x18, 0x24, 0x3C, 0x7E, 0x5A, 0xBD, 0xFF, // Mine
	0xBD, 0xFF, 0x7E, 0x5A, 0x24, 0x3C, 0x18, 0x18,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Black
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // White
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
);

unsigned char sprites[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // Empty
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81, // Marker
	0x81,0x81,0xC1,0x81,0xA1,0xC1,0xFF,0xFF
};

enum tile {
	Zero = 0,
	One = 1,
	Two = 2,
	Three = 3,
	Four = 4,
	Five = 5,
	Six = 6,
	Seven = 7,
	Eight = 8,
	Unopened = 9,
	Flagged = 10,
	Mine = 11,
	Black = 12,
	White = 13
};

char digit_to_tile(int digit)
{
	return (char)digit; // Actually the same for now.
}

void move_marker(int x, int y)
{
	move_sprite(MARKER_NB, (SCREENWIDTH / GRID_WIDTH) * (x+1), (SCREENHEIGHT / GRID_HEIGHT) * (y+2));
}

void show_marker()
{
	set_sprite_tile(MARKER_NB, 1);
}

void hide_marker()
{
	set_sprite_tile(MARKER_NB, 0);
}

int wrap(int value, int max)
{
	if (value < 0) return max;
	if (value > max) return 0;
	return value;
}


struct loaded_tileset* loaded_game_tiles;

void set_tile(int x, int y, unsigned char tile)
{
	place_tile(loaded_game_tiles, tile, x, y);
}

void render_board(struct board* board)
{
	int x, y;
	uint8_t* tile;
	char sprite;
	for (x = 0;x<GRID_WIDTH;x += 1)
	{
		for (y = 0;y < GRID_HEIGHT;y += 1)
		{
			tile = get_tile_at(board, x, y);

			if (*tile&TILE_OPENED && *tile&TILE_MINE)
				sprite = Mine;
			else if (*tile&TILE_OPENED)
				sprite = digit_to_tile(adjacent_mine_count(tile));
			else if (*tile&TILE_FLAG)
				sprite = Flagged;
			else
				sprite = Unopened;

			set_tile(x, y, sprite);
		}
	}
}

void show_won_game()
{
	static const unsigned long bitmap[] = {
		0x0000, 0x0000, 0x0F00, 0x30C0, 0x5020,
		0x5220, 0xBB10, 0xBA90, 0x13A48, 0x11248,
		0x11248, 0x13A48, 0xBA90, 0xBB10, 0x5220,
		0x5020, 0x30C0, 0x0F00, 0x0000, 0x000
	};

	int y, x;
	for (y = 0;y < GRID_HEIGHT; y++)
	{
		for (x = 0;x < GRID_WIDTH;x++)
		{
			int bit = (bitmap[x] >> ((GRID_HEIGHT - 1) - y)) & 1;
			place_tile(loaded_game_tiles, bit ? White : Black, x, y);
		}
		delay(75);
	}
}

void show_game_over()
{
	// Monotone bitmap where every element is row on screen.
	// LSB is bottom of screen.
	static const unsigned long bitmap[] = {
		0x0000, 0x7DF0, 0x4510, 0x5510, 0x5DF0,
		0x0000, 0x3DE0, 0x4810, 0x3DE0, 0x0000,
		0x7DF0, 0x4150, 0x3110, 0x4000, 0x7DF0,
		0x0140, 0x7DB0, 0x5400, 0x45D0, 0x0000
	};
	int sx = 0; // Spiral coordinates calculated from center
	int sy = 0;
	long N = GRID_WIDTH > GRID_HEIGHT ? GRID_WIDTH : GRID_HEIGHT;
	long i;

	// Spiral.
	for (i = 0; i < N*N; i++)
	{
		int x = GRID_WIDTH / 2 + sx - 1;
		int y = GRID_HEIGHT / 2 + sy;
		int bit = (bitmap[x] >> ((GRID_HEIGHT-1)-y)) & 1;
		set_tile(x,y, bit ? White : Black);

		if (abs(sx) <= abs(sy) && (sx != sy || sx >= 0))
			sx += ((sy >= 0) ? 1 : -1);
		else
			sy += ((sx >= 0) ? -1 : 1);

		delay(5);
	}
}

void load_tiles()
{
	// Background tiles
	loaded_game_tiles = load_tileset(&game_tiles);

	// Sprite tiles
	set_sprite_data(0, 2, sprites);
}

// Seed prng using current time (8 bit) and previous seed.
// Call multiple times at unpredictable moments (like user input) to increase entropy.
void seed_prng()
{
	srand((rand()<<8)|DIV_REG);
}

void play_game(int difficulty)
{

	bool first_open = true;
	struct board* board;
	uint8_t *game_memory = malloc(minimum_buffer_size(GRID_WIDTH, GRID_HEIGHT));

	seed_prng();

	board = board_init(GRID_WIDTH, GRID_HEIGHT, 0.1 * (difficulty+1), game_memory);
	
	load_tiles();
	show_marker();

	SHOW_BKG;
	SHOW_SPRITES;
	SPRITES_8x8;

	render_board(board);

	while (board->_state != BOARD_GAME_OVER && board->_state != BOARD_WIN)
	{
		if (button_pressed(J_UP,50))
			move_cursor(board, UP);
		if (button_pressed(J_DOWN, 50))
			move_cursor(board, DOWN);
		if (button_pressed(J_LEFT, 50))
			move_cursor(board, LEFT);
		if (button_pressed(J_RIGHT, 50))
			move_cursor(board, RIGHT);
		if (button_pressed(J_A, -1))
		{
			// Add more entropy to prng first time we open a tile
			if (first_open)
			{
				seed_prng();
				first_open = false;
			}
			open_tile_at_cursor(board);
			render_board(board);
		}
		if (button_pressed(J_B, -1))
		{
			toggle_flag_at_cursor(board);
			render_board(board);
		}
		move_marker(board->cursor_x, board->cursor_y);
		delay(75);
	}
	hide_marker();
	
	if (board->_state == BOARD_WIN)
		show_won_game();
	else if (board->_state == BOARD_GAME_OVER)
		show_game_over();

	while (!button_pressed(J_START, -1));

	free(game_memory);
}
