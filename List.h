#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "ListElement.h"
#include "Component.h"

#include <vector>

#ifndef LIST_H
#define LIST_H

#include "GraphicConst.h"

typedef unsigned int uint;

class List : Component {
    private:
        uint chosenElementIndex;
        uint maxElementOnScreen = h / (LINE_HEIGHT + LINE_GAP) - LIST_MAX_ELEMENT_ON_SCREEN_OFFSET;
    public:
        std::vector<ListElement> elements{};
        List (int positionX, int positionY, int width, int height, int chosenElement) : Component (positionX, positionY, width, height) {
            this->chosenElementIndex = chosenElement;
        }

        int render (SDL_Renderer *renderer, TTF_Font *font) {
            int offset =  chosenElementIndex > maxElementOnScreen ? chosenElementIndex : 0;
            for (uint index = offset; index < this->elements.size() && index < maxElementOnScreen + offset; index++) {
                if (index == this->chosenElementIndex) {
                    this->elements[index].toggleChosen(true);
                } else {
                    this->elements[index].toggleChosen(false);
                }
                if (this->elements[index].render(renderer, font, LEFT_PADDING, LINE_HEIGHT * (index - offset) + LINE_GAP * (index - offset) + y, w)) {
                    return 1;
                }
            }
            return 0;
        }

        void setChosenElement (int newChosenElementIndex) {
            this->chosenElementIndex = newChosenElementIndex;
        }

        int getChosenElement () {
            return this->chosenElementIndex;
        }

        const char *getChosenElementContent () {
            return this->elements[this->chosenElementIndex].getContent();
        }

        void elementsPush (ListElement newElement) {
            this->elements.push_back(newElement);
        };

        std::vector<ListElement> getElements () {
            return this->elements;
        }

        void resize (int newWidth, int newHeight) {
            this->w = newWidth;
            this->h = newHeight;
            maxElementOnScreen = h / (LINE_HEIGHT + LINE_GAP) + LIST_MAX_ELEMENT_ON_SCREEN_OFFSET;
        } 
};

#endif
