#include "Component.h"
#include "Text.h"

#include <SDL2/SDL.h>

#ifndef BUTTON_H
#define BUTTON_H

class Button : Component {
private:
  bool is_released = false; 
  bool is_pressed = false;
  Text text = Text (x + w / 4, y + h / 4, w, h, "");
public:
  Button (int positionX, int positionY, int width, int height, const char *initial_text) : Component (positionX, positionY, width, height) {
    text.set_text(initial_text);
  }
  void event_handler (SDL_Event e) {
    switch (e.type)
      {   
      case SDL_MOUSEBUTTONDOWN:
	if ((e.motion.x > x && e.motion.x < x + w) && (e.motion.y > y && e.motion.y < y + h))
	  is_pressed = true;
	break;
      case SDL_MOUSEBUTTONUP:
	is_pressed = false;
	is_released = false;
	break;
      default:
	break;
      }
  }
  
  bool check_click() {
    if (is_pressed && !is_released) {
      is_released = true;
      return true;
    }
    return false;
  }
  
  int render (SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect dest = SDL_Rect {x, y, w, h};
    SDL_RenderDrawRect(renderer, &dest);
    if (text.render_text(renderer, font)) {
      return 1;
    }
            return 0;
  }
  
  void set_text (const char *new_text) {
    this->text.set_text(new_text);
  }
  
  const char *get_text () {
    return this->text.get_text();
  }
  
  void move (int newX, int newY) {
    this->x = newX;
    this->y = newY;
    this->text.move(newX + w / 4, newY + h / 4);
  };
 
};

#endif
