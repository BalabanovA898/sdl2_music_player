#include "Text.h"
#include "Component.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#ifndef LISTELEMENT_H
#define LISTELEMENT_H

class ListElement : Component {
private:
  bool chosen = false;
  const char* header;
  const char* content;
public:
  ListElement (int positionX, int positionY, int width, int height, const char* header, const char* content) : Component (positionX, positionY, width, height) {
    this->header = header;
    this->content = content;
  }
  
  void toggle_chosen (bool new_state) {
    this->chosen = new_state;
  }
  
  bool get_chosen () {
    return this->chosen;
  }
  
  void set_content (const char *new_content) {
    this->content = new_content;
  }
  
  const char *get_content () {
    return this->content;
  }
  
  void set_header (const char *new_header) {
    this->header = new_header;
  }
  
  const char *get_header () {
    return this->header;
  }
  
  int render (SDL_Renderer* renderer, TTF_Font* font, int posX, int posY, int width) {
    if (this->chosen) SDL_SetRenderDrawColor(renderer,255,0,0,255);
    else SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_Rect dest = SDL_Rect {posX, posY, width, h};
    SDL_RenderDrawRect(renderer, &dest);
    Text text = Text (posX, posY, width, h, header);
    if (text.render_text(renderer, font)) {
      return 1;
    }
    return 0;
  }
};

#endif
