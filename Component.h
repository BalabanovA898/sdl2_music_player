#ifndef COMPONENT_H
#define COMPONENT_H

class Component {
public:
  int x, y;
  int w, h;

  Component (int positionX, int positionY, int width, int height) {
    x = positionX;
    y = positionY;
    w = width;
    h = height;
  };
  
  void resize (int new_width, int new_height) {
    this->w = new_width;
    this->h = new_height;
  };
  
};
#endif
