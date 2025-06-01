#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define MAX_CANVAS_SIZE 99
#define MAX_TILES		64 // for now (8 x 8)
#define TILE_SIZE		16

typedef struct ISLE_Tile {
	SDL_FRect src;
} ISLE_Tile;

typedef struct ISLE_Client {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int windowWidth;
	int windowHeight;
	SDL_FRect panel;
	SDL_FRect canvas;
	SDL_FRect tileIcon;
} ISLE_Client;

extern ISLE_Client client;
extern FILE *openFile;
extern int canvasData[15][13];
extern ISLE_Tile tileSet[MAX_TILES];

#endif