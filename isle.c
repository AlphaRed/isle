#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define MAX_CANVAS_SIZE 99
#define MAX_TILES		64 // for now (8 x 8)
#define TILE_SIZE		16

typedef struct ISLE_Tile {
	SDL_FRect src;
} ISLE_Tile;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int canvas[15][13]; // hardcoded for now
SDL_Texture *tileSetImg = NULL;
SDL_Texture *openButton = NULL;
SDL_Texture *saveButton = NULL;
FILE *openFile = NULL;
ISLE_Tile tileSet[MAX_TILES];
SDL_Texture *tilePanel = NULL;
int windowWidth = 800;
int windowHeight = 600;
SDL_Texture *drkcanvas = NULL;
SDL_Texture *litcanvas = NULL;

// Initialize SDL, window, and renderer.
void initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL failed to initialize: %s\n", SDL_GetError());
	}
	else
		window = SDL_CreateWindow("ISLE", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, NULL);
	if (renderer == NULL) {
		printf("SDL renderer failed to create: %s\n", SDL_GetError());
	}
	else
		SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);
}

void SDLCALL openFileCallBack(void *userdata, const char *const *filelist, int filter) {
	if (filelist == NULL) {
		printf("Failed to open file: %s\n", SDL_GetError());
		return;
	}
	else if (*filelist == NULL) { // cancelled the dialog or didn't choose a file
		printf("Cancelled file dialog\n");
		return;
	}

	openFile = fopen(*filelist, "r");
	if (openFile == NULL)
		printf("Failed to open the file.\n");
	else {
		// to read it through first to determine size
		for (int y = 0; y < 13; y++)
		{
			for (int x = 0; x < 15; x++)
			{
				if (fscanf(openFile, "%d", &canvas[x][y]) == EOF)
					return 1;
			}
		}
		fclose(openFile); // should I leave it open?
	}
}

// Checks what they clicked...add as we go along
void checkMouseClick(int x, int y) {
	if (x >= 0 && x <= 64 && y >= 0 && y <= 32) { // Open button
		SDL_ShowOpenFileDialog(openFileCallBack, NULL, NULL, NULL, NULL, NULL, 0);
	}
	else if (x >= 0 && x <= 200 && y >= 32 && y <= windowHeight) { // Tile panel
		printf("we clicked panel\n");
	}
	else if (x >= 200 && x <= windowWidth && y >= 32 && y <= windowHeight) { // Canvas panel
		printf("we clicked canvas\n");
	}
	else
		return;
}

// Handle events
int eventHandler(SDL_Event event) {
	if (event.type == SDL_EVENT_QUIT)
		return 0;
	else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		checkMouseClick(event.button.x, event.button.y);
	}
	else
		return 1;
}

// Loads image files as textures
SDL_Texture *loadTexture(char *filename) {
	SDL_Texture *returnTex = IMG_LoadTexture(renderer, filename);
	if (!returnTex) {
		printf("Failed to load texture: %s\n", filename);
		returnTex = NULL;
	}
	return returnTex;
}

// Blits full image
void blitSprite(SDL_Texture *tex, float x, float y, float w, float h) {
	SDL_FRect dest;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	SDL_RenderTexture(renderer, tex, NULL, &dest);
}

// Blits tiles to renderer
void blitTile(SDL_Texture *tex, SDL_FRect *src, float x, float y) {
	SDL_FRect dest;
	dest.x = x;
	dest.y = y;
	dest.w = TILE_SIZE * 2;
	dest.h = TILE_SIZE * 2;

	SDL_RenderTexture(renderer, tex, src, &dest);
}

// Setup tileset
void setupTileset() {
	for (int i = 0; i < MAX_TILES; i++) {
		tileSet[i].src.x = (i % 8) * TILE_SIZE;
		tileSet[i].src.y = (i / 8) * TILE_SIZE;
		tileSet[i].src.w = TILE_SIZE;
		tileSet[i].src.h = TILE_SIZE;
	}
}

void drawTileset() {
	int y = 64;
	for (int i = 0; i < MAX_TILES; i++) {
		blitTile(tileSetImg, &tileSet[i].src, (i % 6) * 32,  y + (i / 6) * 32); // scaled for 16 * 2 = 32
	}
}

void clearCanvas() {
	for (int y = 0; y < 13; y++) {
		for (int x = 0; x < 15; x++) {
			canvas[x][y] = 99;
		}
	}
}

void drawCanvas() {
	SDL_FRect emptySq = { 0, 0, 32, 32 };
	int colourChange = 1;
	int xOffset = 250;
	int yOffset = 64;
	for (int y = 0; y < 13; y++) {
		for (int x = 0; x < 15; x++) {
			if (canvas[x][y] == 99) {
				if ((colourChange % 2) >= 1)
					blitTile(drkcanvas, &emptySq, x * 32 + xOffset, y * 32 + yOffset);
				else
					blitTile(litcanvas, &emptySq, x * 32 + xOffset, y * 32 + yOffset);
				colourChange++;
			}
		}
	}
}

int main(int argc, char *args[]) {
	initSDL();
	int quit = 1;
	SDL_Event events;
	tileSetImg = loadTexture("tileset.png");
	openButton = loadTexture("gfx/open.png");
	saveButton = loadTexture("gfx/save.png");
	tilePanel = loadTexture("gfx/black.png");
	drkcanvas = loadTexture("gfx/darkgrey.png");
	litcanvas = loadTexture("gfx/lightgrey.png");
	clearCanvas();

	while (quit) {
		// Input
		SDL_PollEvent(&events);
		quit = eventHandler(events);

		// Logic
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);
		setupTileset();

		// Render
		SDL_RenderClear(renderer);
		blitSprite(openButton, 0, 0, 64, 32);
		blitSprite(saveButton, 66, 0, 64, 32);
		blitSprite(tilePanel, 200, 0, 2, windowHeight);
		drawTileset();
		drawCanvas();
		SDL_RenderPresent(renderer);
	}
	return 0;
}