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
} ISLE_Client;

ISLE_Client client;
int canvasData[15][13]; // hardcoded for now
SDL_Texture *tileSetImg = NULL;
SDL_Texture *openButton = NULL;
SDL_Texture *saveButton = NULL;
FILE *openFile = NULL;
ISLE_Tile tileSet[MAX_TILES];
SDL_Texture *tilePanel = NULL;
SDL_Texture *drkcanvas = NULL;
SDL_Texture *litcanvas = NULL;

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

// Checks what they clicked...add as we go along
void checkMouseClick(int x, int y) {
	printf("mouse: (%d, %d)\n", x, y);
	if (x >= 0 && x <= 64 && y >= 0 && y <= 32) { // Open button
		SDL_ShowOpenFileDialog(openFileCallBack, NULL, NULL, NULL, NULL, NULL, 0);
	}
	else if (x >= client.panel.x && x <= (client.panel.w + client.panel.x) && y >= client.panel.y && y <= (client.panel.h + client.panel.y)) { // Tile panel
		printf("we clicked panel\n");
	}
	else if (x >= client.canvas.x && x <= (client.canvas.w + client.canvas.x) && y >= client.canvas.y && y <= (client.canvas.h + client.canvas.y)) { // Canvas panel
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

// Setup tileset
void setupTileset() {
	for (int i = 0; i < MAX_TILES; i++) {
		tileSet[i].src.x = (i % 8) * TILE_SIZE;
		tileSet[i].src.y = (i / 8) * TILE_SIZE;
		tileSet[i].src.w = TILE_SIZE;
		tileSet[i].src.h = TILE_SIZE;
	}
}

void drawTileset(int x, int y) {
	for (int i = 0; i < MAX_TILES; i++) {
		blitTile(tileSetImg, &tileSet[i].src, x + (i % 5) * 32,  y + (i / 5) * 32); // scaled for 16 * 2 = 32
	}
}

void clearCanvas() {
	for (int y = 0; y < 13; y++) {
		for (int x = 0; x < 15; x++) {
			canvasData[x][y] = 99;
		}
	}
}

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
		}
	}
}

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

int main(int argc, char *args[]) {
	// need a better way to do this
	client.windowWidth = 800;
	client.windowHeight = 600;
	client.panel.x = 8;
	client.panel.y = 48;
	client.panel.w = 6 * 32;
	client.panel.h = 10 * 32;
	client.canvas.x = 250;
	client.canvas.y = 64;
	client.canvas.w = 15 * 32;
	client.canvas.h = 13 * 32;
	initSDL();
	int quit = 1;
	SDL_Event events;
	tileSetImg = loadTexture("tileset.png");
	openButton = loadTexture("gfx/open.png");
	saveButton = loadTexture("gfx/save.png");
	tilePanel = loadTexture("gfx/tilepanel.png");
	drkcanvas = loadTexture("gfx/darkgrey.png");
	litcanvas = loadTexture("gfx/lightgrey.png");
	clearCanvas();

	while (quit) {
		// Input
		SDL_PollEvent(&events);
		quit = eventHandler(events);

		// Logic
		SDL_GetWindowSize(client.window, &client.windowWidth, &client.windowHeight);
		setupTileset();

		// Render
		SDL_RenderClear(client.renderer);
		blitSprite(openButton, 0, 0, 64, 32);
		blitSprite(saveButton, 66, 0, 64, 32);
		drawTilePanel(client.panel.x, client.panel.y);
		drawTileset(8 + 32, 48 + 32);
		drawCanvas(client.canvas.x, client.canvas.y);
		SDL_RenderPresent(client.renderer);
	}
	return 0;
}