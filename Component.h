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

        void resize (int newWidth, int newHeight) {
            this->w = newWidth;
            this->h = newHeight;
        };

};
#endif
