#!/bin/sh
g++ $(pkg-config --cflags --libs sdl2 SDL2_mixer SDL2_ttf) main.cpp -o player -g    



