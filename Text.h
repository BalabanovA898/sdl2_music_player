#include "Component.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#ifndef TEXT_H
#define TEXT_H

SDL_Texture *get_texture_with_text(const char *text, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
  if (surface == nullptr) 
    return nullptr;
  
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) 
    std::cerr << "Error with creating font texture. " << SDL_GetError();
  
  SDL_FreeSurface(surface);
  return texture;
}

class Text : Component {
private:
  const char *text;
public:
  Text (int positionX, int positionY, int width, int height, const char* initialText) : Component (positionX, positionY, width, height) {
    text = initialText;
  };
  const char *get_text() {
    return this->text;
  };
  void set_text(const char *new_text) {
    this->text = new_text;
  };
  int render_text (SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Texture *rendered_text = get_texture_with_text(this->text, font, SDL_Color {0, 0, 0, 255}, renderer); 
    
    if (rendered_text == nullptr)
      return 0;
    SDL_Rect dest;
    
    SDL_QueryTexture(rendered_text, NULL, NULL, &w, &h); 
    dest.w = w;
    dest.h = h;
    
    dest.x = x;
    dest.y = y;
    
    SDL_RenderCopy(renderer, rendered_text, nullptr, &dest);
    SDL_DestroyTexture(rendered_text);
    return 0;
  }
  void move (int newX, int newY) {
    this->x = newX;
    this->y = newY;
  }
};

#endif
