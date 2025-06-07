#include "common.h"
#include "ui.h"

ISLE_Client client;
FILE *openFile = NULL;
int canvasData[15][13]; // hardcoded for now, don't forget to change extern in common header
ISLE_Tile tileSet[MAX_TILES];

// Checks what they clicked...add as we go along
void checkMouseClick(int x, int y) {
	printf("mouse: (%d, %d)\n", x, y);
	if (x >= 0 && x <= 64 && y >= 0 && y <= 32) { // Open button
		SDL_ShowOpenFileDialog(openFileCallBack, NULL, NULL, NULL, NULL, NULL, 0);
	}
	else if (x >= client.panel.x && x <= (client.panel.w + client.panel.x) && y >= client.panel.y && y <= (client.panel.h + client.panel.y)) { // Tile panel
		int tileX = (x - 8 - 3) / (TILE_SIZE * 2);
		int tileY = (y - 48 - 3) / (TILE_SIZE * 2);
		client.selectedTile = tileX + (tileY * 5);
		//printf("we clicked panel: %d, %d\n", tileX, tileY);
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

// Setup tileset
void setupTileset() {
	for (int i = 0; i < MAX_TILES; i++) {
		tileSet[i].src.x = (i % 8) * TILE_SIZE;
		tileSet[i].src.y = (i / 8) * TILE_SIZE;
		tileSet[i].src.w = TILE_SIZE;
		tileSet[i].src.h = TILE_SIZE;
	}
}

void clearCanvas() {
	for (int y = 0; y < 13; y++) {
		for (int x = 0; x < 15; x++) {
			canvasData[x][y] = 99;
		}
	}
}

int main(int argc, char *args[]) {
	// need a better way to do this, move to a function in ui.c
	setupUI();
	initSDL();
	int quit = 1;
	SDL_Event events;
	clearCanvas();
	loadUITextures();

	while (quit) {
		// Input
		SDL_PollEvent(&events);
		quit = eventHandler(events);

		// Logic
		SDL_GetWindowSize(client.window, &client.windowWidth, &client.windowHeight);
		setupTileset();

		// Render
		SDL_RenderClear(client.renderer);
		drawButtons();
		drawTilePanel(client.panel.x, client.panel.y);
		drawTileset(8 + 3, 48 + 3);
		drawTileSelect(client.selectedTile);
		drawCanvas(client.canvas.x, client.canvas.y);
		SDL_RenderPresent(client.renderer);
	}
	return 0;
}