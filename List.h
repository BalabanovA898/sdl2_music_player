#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Text.h"
#include "ListElement.h"
#include "Component.h"

#include <vector>

#ifndef LIST_H
#define LIST_H

#include "GraphicConst.h"

typedef unsigned int uint;

class List : Component {
private:
  uint chosen_element_index;
  uint max_elements_on_screen = h / (LINE_HEIGHT + LINE_GAP) - LIST_MAX_ELEMENT_ON_SCREEN_OFFSET;
  Text page_number = Text(this->x, this->y, (float) LINE_HEIGHT, 100.0, "");
public:
  std::vector<ListElement> elements{};
  List (int positionX, int positionY, int width, int height, int chosen_element) : Component (positionX, positionY, width, height) {
    this->chosen_element_index = chosen_element;
  }
  
  int render (SDL_Renderer *renderer, TTF_Font *font) {
    //TODO: Debug lines rendering
    int page = chosen_element_index / max_elements_on_screen;
    this->page_number.set_text((std::to_string(page + 1) + "/" + std::to_string(max_elements_on_screen / elements.size() + 1)).c_str());
    this->page_number.render_text(renderer, font);
    for (uint index = 0; index < this->elements.size() && index < max_elements_on_screen; index++) {
      if (index == this->chosen_element_index) {
	this->elements[page * max_elements_on_screen + index].toggle_chosen(true);
      } else {
	this->elements[page * max_elements_on_screen + index].toggle_chosen(false);
      }
      if (this->elements[page * max_elements_on_screen + index].render(renderer, font, LEFT_PADDING, LINE_HEIGHT * (index + 1) + LINE_GAP * (index + 1) + y, w)) {
	return 1;
      }
    }
    return 0;
  }
  
  void set_chosen_element (int new_chosen_element_index) {
    this->chosen_element_index = (new_chosen_element_index % elements.size());
  }
  
  int get_chosen_element () {
    return this->chosen_element_index;
  }
  
  const char* get_chosen_element_content () {
    return this->elements[this->chosen_element_index].get_content();
  }
  
  void elements_push (ListElement new_element) {
    this->elements.push_back(new_element);
  };
  
  std::vector<ListElement> get_elements () {
    return this->elements;
  }
  
  void resize_list (int new_width, int new_height) {
    this->w = new_width;
    this->h = new_height;
    max_elements_on_screen = h / (LINE_HEIGHT + LINE_GAP) - LIST_MAX_ELEMENT_ON_SCREEN_OFFSET;
  } 
};

#endif
