#include "common.h"
#include "ui.h"

SDL_Texture *drkcanvas = NULL;
SDL_Texture *litcanvas = NULL;
SDL_Texture *bg = NULL;
SDL_Texture *tileSetImg = NULL;
SDL_Texture *openButton = NULL;
SDL_Texture *saveButton = NULL;
SDL_Texture *tilePanel = NULL;
SDL_Texture *select = NULL;

// Initialize SDL, window, and renderer.
void initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL failed to initialize: %s\n", SDL_GetError());
	}
	else
		client.window = SDL_CreateWindow("ISLE", client.windowWidth, client.windowHeight, SDL_WINDOW_RESIZABLE);

	client.renderer = SDL_CreateRenderer(client.window, NULL);
	if (client.renderer == NULL) {
		printf("SDL renderer failed to create: %s\n", SDL_GetError());
	}
	else
		SDL_SetRenderDrawColor(client.renderer, 75, 75, 75, 255);
}

// Callback function for open file dialog
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
				if (fscanf(openFile, "%d", &canvasData[x][y]) == EOF)
					return 1;
			}
		}
		fclose(openFile); // should I leave it open?
	}
}

// Loads image files as textures
SDL_Texture *loadTexture(char *filename) {
	SDL_Texture *returnTex = IMG_LoadTexture(client.renderer, filename);
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
	SDL_RenderTexture(client.renderer, tex, NULL, &dest);
}

// Blits tiles to renderer
void blitTile(SDL_Texture *tex, SDL_FRect *src, float x, float y) {
	SDL_FRect dest;
	dest.x = x;
	dest.y = y;
	dest.w = TILE_SIZE * 2;
	dest.h = TILE_SIZE * 2;

	SDL_RenderTexture(client.renderer, tex, src, &dest);
}

void loadUITextures() {
	drkcanvas = loadTexture("gfx/darkgrey.png");
	litcanvas = loadTexture("gfx/lightgrey.png");
	bg = loadTexture("gfx/bg.png");
	tileSetImg = loadTexture("tileset.png");
	openButton = loadTexture("gfx/open.png");
	saveButton = loadTexture("gfx/save.png");
	tilePanel = loadTexture("gfx/tilepanel.png");
	select = loadTexture("gfx/select.png");
}

// Draws canvas full of tiles (or empty)
void drawCanvas(int xOffset, int yOffset) {
	SDL_FRect emptySq = { 0, 0, 32, 32 };
	int colourChange = 1;
	for (int y = 0; y < 13; y++) {
		for (int x = 0; x < 15; x++) {
			if (canvasData[x][y] == 99) {
				if ((colourChange % 2) >= 1)
					blitTile(drkcanvas, &emptySq, x * 32 + xOffset, y * 32 + yOffset);
				else
					blitTile(litcanvas, &emptySq, x * 32 + xOffset, y * 32 + yOffset);
				colourChange++;
			}
			else if (canvasData[x][y] == 0) {
				blitTile(bg, &emptySq, x * 32 + xOffset, y * 32 + yOffset);
			}
			else
				blitTile(tileSetImg, &tileSet[canvasData[x][y]].src, x * 32 + xOffset, y * 32 + yOffset);
		}
	}
}

// Draws tileset to the sidebar
void drawTileset(int x, int y) {
	for (int i = 0; i < MAX_TILES; i++) {
		blitTile(tileSetImg, &tileSet[i].src, x + (i % 5) * 32, y + (i / 5) * 32); // scaled for 16 * 2 = 32
	}
}

// Draws upper toolbar buttons
void drawButtons() {
	blitSprite(openButton, 0, 0, 64, 32);
	blitSprite(saveButton, 66, 0, 64, 32);
}

// Draws side tilepanel
void drawTilePanel(int x, int y) {
	const int tileSize = 32;
	SDL_FRect sprite = { 0, 0, tileSize, tileSize };
	// top row
	blitTile(tilePanel, &sprite, x, y);
	sprite.x += tileSize;
	for (int i = 1; i < 6; i++) {
		blitTile(tilePanel, &sprite, x + (i * tileSize), y);
	}
	sprite.x += tileSize;
	blitTile(tilePanel, &sprite, x + (6 * tileSize), y);
	// middle rows
	sprite.x += tileSize;
	for (int i = 1; i < 10; i++) {
		blitTile(tilePanel, &sprite, x, y + i * tileSize);
		sprite.x += tileSize;
		blitTile(tilePanel, &sprite, x + 6 * tileSize, y + i * tileSize);
		sprite.x -= tileSize;
	}
	// bottom row
	sprite.x += tileSize * 2;
	blitTile(tilePanel, &sprite, x, y + 10 * tileSize);
	sprite.x += tileSize;
	for (int i = 1; i < 6; i++) {
		blitTile(tilePanel, &sprite, x + (i * tileSize), y + 10 * tileSize);
	}
	sprite.x += tileSize;
	blitTile(tilePanel, &sprite, x + (6 * tileSize), y + 10 * tileSize);
}