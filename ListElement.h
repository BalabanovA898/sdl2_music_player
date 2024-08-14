#include "Text.h"
#include "Component.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#ifndef LISTELEMENT_H
#define LISTELEMENT_H

class ListElement : Component {
    private:
        bool chosen = false;
        const char *header;
        const char *content;
    public:
        ListElement (int positionX, int positionY, int width, int height, const char *header, const char *content) : Component (positionX, positionY, width, height) {
            this->header = header;
            this->content = content;
        }

        void toggleChosen (bool newState) {
            this->chosen = newState;
        }

        bool getChosen () {
            return this->chosen;
        }

        void setContent (const char *newContent) {
            this->content = newContent;
        }

        const char *getContent () {
            return this->content;
        }

        void setHeader (const char *newHeader) {
            this->header = newHeader;
        }

        const char *getHeader () {
            return this->header;
        }

        int render (SDL_Renderer *renderer, TTF_Font *font, int posX, int posY, int width) {
            if (this->chosen) SDL_SetRenderDrawColor(renderer,255,0,0,255);
            else SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_Rect dest = SDL_Rect {posX, posY, width, h};
            SDL_RenderDrawRect(renderer, &dest);
            Text text = Text (posX, posY, width, h, header);
            if (text.renderText(renderer, font)) {
                return 1;
            }
            return 0;
        }
};

#endif
