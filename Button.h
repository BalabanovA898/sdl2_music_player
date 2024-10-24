#include "Component.h"
#include "Text.h"

#include <SDL2/SDL.h>

#ifndef BUTTON_H
#define BUTTON_H

class Button : Component {
    private:
        bool isReleased = false; 
        bool isPressed = false;
        Text text = Text (x + w / 4, y + h / 4, w, h, "");
    public:
        Button (int positionX, int positionY, int width, int height, const char *initialText) : Component (positionX, positionY, width, height) {
            text.setText(initialText);
        }
        void eventHandler (SDL_Event e) {
            switch (e.type)
            {   
                case SDL_MOUSEBUTTONDOWN:
                    if ((e.motion.x > x && e.motion.x < x + w) && (e.motion.y > y && e.motion.y < y + h))
                       isPressed = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    isPressed = false;
                    isReleased = false;
                    break;
                default:
                    break;
            }
        }

        bool checkClick() {
            if (isPressed && !isReleased) {
                isReleased = true;
                return true;
            }
            return false;
        }

        int render (SDL_Renderer *renderer, TTF_Font *font) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect dest = SDL_Rect {x, y, w, h};
            SDL_RenderDrawRect(renderer, &dest);
            if (text.renderText(renderer, font)) {
                return 1;
            }
            return 0;
        }

        void setText (const char *newText) {
            this->text.setText(newText);
        }

        const char *getText () {
            return this->text.getText();
        }

         void move (int newX, int newY) {
            this->x = newX;
            this->y = newY;
            this->text.move(newX + w / 4, newY + h / 4);
        };

};

#endif
