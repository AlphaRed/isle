#ifndef UI_H
#define UI_H

void initSDL();
void SDLCALL openFileCallBack(void *userdata, const char *const *filelist, int filter);
SDL_Texture *loadTexture(char *filename);
void blitSprite(SDL_Texture *tex, float x, float y, float w, float h);
void blitTile(SDL_Texture *tex, SDL_FRect *src, float x, float y);
void loadUITextures();
void drawCanvas(int xOffset, int yOffset);
void drawTileset(int x, int y);
void drawButtons();
void drawTilePanel(int x, int y);
void drawTileSelect(int tileNum);
void setupUI();

#endif