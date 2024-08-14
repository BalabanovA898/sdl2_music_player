#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <random>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Text.h"
#include "Button.h"
#include "ListElement.h"
#include "List.h"

#include "ProgrammConst.h"
#include "GraphicConst.h"

#ifdef DEV
#define INITIAL_PATH "/run/media/andrey/485D-B2A3/"
#endif

#ifdef PLATFOTM_LINUX
#define INITIAL_PATH "/home/andrey/Music/"
#endif

#ifdef PLATFORM_WINDOWS
#define INITIAL_PATH "c:/Music"
#endif

typedef unsigned int uint;
typedef enum {REPEAT, CONTINUE, QUITONEND, CHANGINGTRACK} PlayingState;
typedef enum {IDLE, TEXTINPUT} AppMode;

void errorEnd(int errorCode, const char *errorTitle, const char *errorDescription);
void initSDL(SDL_Window **win, SDL_Renderer **ren);
void quitSDL(SDL_Window **win, SDL_Renderer **ren);
void clearScreen (SDL_Renderer **ren, int screenWidth, int screenHeight);

void togglePause (bool &isPlaying, Button &button);
void changeTrack (uint newTrackNumber, uint *currentTrack, PlayingState &currentState, List &tl);
void changeVolume (int newVolume, int &currentVolume);
void changeDir (std::string path, std::string &currentPath, List &list, std::vector<ListElement> vle);

std::vector<std::string> getDirEntriesMP3 (std::string path);
std::vector<std::string> getDirEntries (std::string path);
const char* getCurrentPlayingStateAsString (PlayingState state);

int getRandomInt (int min, int max);

int main (int argc, const char *argv[]) {
    int currentWindowWidth = INITIAL_WINDOW_WIDTH,
        currentWindowHeight = INITIAL_WINDOW_HEIGHT;

    bool isRunning  = true,
         isPlaying  = false,
         isPlaylist = false;

    PlayingState currentState = PlayingState::CONTINUE;

    int currentVolume = STANDART_VOLUME;
    uint currentTrack = 0;

    std::vector<std::string> playlist  {};
    std::string readingPath = INITIAL_PATH;
    std::string bufferPath  = INITIAL_PATH;

    AppMode currentMode = AppMode::IDLE;

    if (argc != 2)
        isRunning = false;
    else {
        if (std::filesystem::is_directory(argv[1])) 
            playlist = getDirEntriesMP3(argv[1]);
        else 
            playlist.push_back(argv[1]);
    } 

    if (playlist[0] != "")
        isPlaylist = true;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    initSDL(&window, &renderer);

    TTF_Font *font = TTF_OpenFont(PATH_TO_FONT, 18);     
    Text trackTitle = Text (0 + LEFT_PADDING, LINE_HEIGHT * 0, 0, 0, "");
    Text volume = Text (0 + LEFT_PADDING, LINE_HEIGHT * 1, 0, 0, std::to_string(currentVolume).c_str());
    Text playingStateText (0 + LEFT_PADDING, LINE_HEIGHT * 2, 0, 0, getCurrentPlayingStateAsString(currentState));

    Button playButton = Button (0 + LEFT_PADDING, LINE_HEIGHT * 3 + LINE_GAP, BUTTON_WIDTH, LINE_HEIGHT * 2 - LINE_GAP, "Pause");
    Button volumeUpButton = Button (BUTTON_WIDTH + LEFT_PADDING * 2, LINE_HEIGHT * 3 + LINE_GAP, BUTTON_WIDTH / 2, LINE_HEIGHT * 2 - LINE_GAP, "+");

    Button prevButton = Button (0 + LEFT_PADDING, LINE_HEIGHT * 5 + LINE_GAP, BUTTON_WIDTH / 2 - LEFT_PADDING / 2, LINE_HEIGHT * 2 - LINE_GAP, "<");    
    Button nextButton = Button (LEFT_PADDING + LEFT_PADDING / 2 + BUTTON_WIDTH / 2, LINE_HEIGHT * 5 + LINE_GAP, BUTTON_WIDTH / 2 - LEFT_PADDING / 2, LINE_HEIGHT * 2 - LINE_GAP, ">");
    Button volumeDownButton = Button (BUTTON_WIDTH + LEFT_PADDING * 2, LINE_HEIGHT * 5 + LINE_GAP, BUTTON_WIDTH / 2, LINE_HEIGHT * 2 - LINE_GAP, "-");

    Text pathChanger = Text (0 + LEFT_PADDING, LINE_HEIGHT * 7, 0, 0, bufferPath.c_str());

    List trackList = List (0 + LEFT_PADDING, LINE_HEIGHT * 9, currentWindowWidth, currentWindowHeight, currentTrack);

    for (uint index = 0; index < playlist.size(); index++) {
        ListElement newElement = ListElement (0, 0, 300, LINE_HEIGHT, playlist[index].c_str(), playlist[index].c_str());
        trackList.elementsPush(newElement);
    }

    std::vector<std::string> initialDirEntries = getDirEntries(readingPath);
    std::vector<ListElement> pathEntries {};

    Mix_Music *music = nullptr;

    while (isPlaylist) {
        music = Mix_LoadMUS(playlist[currentTrack].c_str());
        if (music == NULL) {
            currentTrack += 1;
            break;
        }
        const char *title = Mix_GetMusicTitle(music);

        trackTitle.setText(title);
        volume.setText(std::to_string(currentVolume).c_str());
        playingStateText.setText(getCurrentPlayingStateAsString(currentState));

        Mix_VolumeMusic(currentVolume);
        Mix_PlayMusic(music, 0);

        isPlaying = true;
        SDL_Event event;

        while (isRunning) {
            while (SDL_PollEvent(&event)) {
                playButton.eventHandler(event);
                nextButton.eventHandler(event);
                prevButton.eventHandler(event);
                volumeUpButton.eventHandler(event);
                volumeDownButton.eventHandler(event);
                switch (event.type) {
                    case SDL_QUIT:
                        isPlaylist = false;
                        isRunning  = false;
                        break;
                    case SDL_TEXTINPUT:
                        if (currentMode == AppMode::TEXTINPUT) {
                            bufferPath += event.text.text;
                            pathChanger.setText(bufferPath.c_str());
                        }
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
                        case SDLK_TAB:
                            isRunning = false;
                            isPlaylist = false;
                            continue;
                        case SDLK_ESCAPE:
                            currentMode = AppMode::IDLE;
                            if (currentMode == AppMode::TEXTINPUT)
                                SDL_StopTextInput();
                            continue;
                        case SDLK_SPACE:
                            if (currentMode == AppMode::IDLE)
                                togglePause(isPlaying, playButton);
                            continue;
                        case SDLK_DOWN:
                            if (currentVolume > 5)
                                changeVolume(currentVolume - 5, currentVolume);
                            continue;
                        case SDLK_UP:
                            if (currentVolume < MIX_MAX_VOLUME - 5)
                                changeVolume(currentVolume + 5, currentVolume);
                            continue;
                        case SDLK_LEFT:
                            if (Mix_GetMusicPosition(music) > 5)
                                Mix_SetMusicPosition(Mix_GetMusicPosition(music) - 5);
                            continue;
                        case SDLK_RIGHT:
                            if (currentMode == AppMode::IDLE)
                                Mix_SetMusicPosition(Mix_GetMusicPosition(music) + 5);
                            continue;
                        case SDLK_r:
                            if (currentMode == AppMode::IDLE)
                                currentState = PlayingState::REPEAT;
                            continue;
                        case SDLK_c:
                            if (currentMode == AppMode::IDLE)
                                currentState = PlayingState::CONTINUE;
                            continue;
                        case SDLK_q:
                            if (currentMode == AppMode::IDLE)
                                currentState = PlayingState::QUITONEND;
                            continue;
                        case SDLK_RIGHTBRACKET:
                            if (currentMode == AppMode::IDLE) {
                                if (currentTrack <= playlist.size() - 1)
                                    changeTrack(currentTrack + 1, &currentTrack, currentState, trackList);
                                else
                                     changeTrack(0, &currentTrack, currentState, trackList);
                                goto changeTrackPoint;
                            }
                            continue;
                        case SDLK_LEFTBRACKET:
                            if (currentMode == AppMode::IDLE) {
                                if (currentTrack == 0)
                                    changeTrack(playlist.size() - 1, &currentTrack, currentState, trackList);
                                else
                                    changeTrack(currentTrack - 1, &currentTrack, currentState, trackList);
                                goto changeTrackPoint;
                            }
                            continue;
                        case SDLK_p:
                            if (currentMode == AppMode::IDLE) {
                                changeTrack(getRandomInt(0, playlist.size() - 1), &currentTrack, currentState, trackList);
                                goto changeTrackPoint;
                            }
                            continue;
                        case SDLK_i:
                            if (currentMode == AppMode::IDLE) {
                                currentMode = AppMode::TEXTINPUT;
                                SDL_StartTextInput();
                            }
                            continue;
                        case SDLK_BACKSPACE:
                            if (currentMode == AppMode::TEXTINPUT)
                                bufferPath.pop_back();
                            continue;
                        case SDLK_RETURN:
                            if (std::filesystem::is_directory(bufferPath)) {
                                readingPath = bufferPath;
                                playlist = getDirEntriesMP3(readingPath);
                                currentTrack = 0;
                                std::vector<ListElement> newTrackList {};
                                for (uint index = 0; index < playlist.size(); index++) {
                                    ListElement newElement = ListElement (0, 0, 300, LINE_HEIGHT, playlist[index].c_str(), playlist[index].c_str());
                                    newTrackList.push_back(newElement);
                                }
                                trackList.elements = newTrackList;
                                changeTrack(currentTrack, &currentTrack, currentState, trackList);
                                goto changeTrackPoint;
                            } else {
                                bufferPath = readingPath;
                                pathChanger.setText(bufferPath.c_str());
                            }
                            break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if (playButton.checkClick())
                            togglePause(isPlaying, playButton);
                        else if (nextButton.checkClick()) {
                            if (currentTrack <= playlist.size() - 1)
                                changeTrack(currentTrack + 1, &currentTrack, currentState, trackList);
                            else
                                changeTrack(0, &currentTrack, currentState, trackList);
                            goto changeTrackPoint;
                        }
                        else if (prevButton.checkClick()) {
                            if (currentTrack == 0)
                                changeTrack(playlist.size() - 1, &currentTrack, currentState, trackList);
                            else
                                 changeTrack(currentTrack - 1, &currentTrack, currentState, trackList);
                            goto changeTrackPoint;
                        }
                        else if (volumeUpButton.checkClick()) {
                            if (currentVolume < MIX_MAX_VOLUME - 5)
                                changeVolume(currentVolume + 5, currentVolume);
                        }
                        else if (volumeDownButton.checkClick()) {
                            if (currentVolume > 5)
                                changeVolume(currentVolume - 5, currentVolume);
                        }
                        break;
                    default:
                        break;
                }
            } //Event
            SDL_GetWindowSize(window, &currentWindowWidth, &currentWindowHeight);
            trackList.resize(currentWindowWidth, currentWindowHeight - PADDING);

            volume.setText(std::to_string(currentVolume).c_str());
            playingStateText.setText(getCurrentPlayingStateAsString(currentState));

            clearScreen(&renderer, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);        

            if (trackTitle.renderText(renderer, font) ||
            volume.renderText(renderer, font) ||
            playingStateText.renderText(renderer, font) ||
            pathChanger.renderText(renderer, font) != 0) {
                errorEnd(4, "Error with rendering.", SDL_GetError());
            }
            
            if (playButton.render(renderer, font) ||
            nextButton.render(renderer, font) ||
            prevButton.render(renderer, font) ||
            volumeUpButton.render(renderer, font) ||
            volumeDownButton.render(renderer, font) != 0) {
                errorEnd(4, "Error with rendering.", SDL_GetError());
            }

            if (trackList.render(renderer, font) != 0)
                errorEnd(4, "Error with rendering.", SDL_GetError());

            SDL_RenderPresent(renderer);

            if (!Mix_PlayingMusic())
                break;

            SDL_Delay(1000/FPS);       
        } // while (isRunning)

        changeTrackPoint:; //goto point for Next, Previous or Random Track option

        switch (currentState) {
            case PlayingState::CONTINUE:
                if (currentTrack < playlist.size())
                    changeTrack(currentTrack + 1, &currentTrack, currentState, trackList);
                else
                    changeTrack(0, &currentTrack, currentState, trackList);
                break;
            case PlayingState::QUITONEND:
                isPlaylist = false;
                break;
            case PlayingState::REPEAT:
                break;
            case PlayingState::CHANGINGTRACK:
                currentState = PlayingState::CONTINUE;
                break;
        }
        Mix_FreeMusic(music);
    } // while (isPlaylist)
    
    TTF_CloseFont(font);
    quitSDL(&window, &renderer);
    return 0;
} //main

void errorEnd(int errorCode, const char *errorTitle, const char *errorDescription) {
    std::cerr << errorTitle << ' ' << errorDescription << std::endl;
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    exit(errorCode);
}

void initSDL(SDL_Window **win, SDL_Renderer **ren) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        errorEnd(1, "Error with initializing a SDL.", SDL_GetError());

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
        errorEnd(1, "Error with initializing an audio module.", Mix_GetError());

    if (TTF_Init() != 0)
        errorEnd(1, "Error with initializing a TTF.", TTF_GetError());

    *win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (*win == nullptr)
        errorEnd(2, "Error with creating a window.", SDL_GetError());
    *ren = SDL_CreateRenderer(*win, 0, SDL_RENDERER_ACCELERATED);
    if (*ren == nullptr)
        errorEnd(2, "Error with initializing a renderer.", SDL_GetError());
}

void quitSDL(SDL_Window **win, SDL_Renderer **ren) {
    SDL_DestroyRenderer(*ren);
    SDL_DestroyWindow(*win);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

void clearScreen (SDL_Renderer **ren, int screenWidth, int screenHeight) {
    SDL_Rect dest {0, 0, screenWidth, screenHeight};

    SDL_SetRenderDrawColor(*ren, 255, 255, 255, 255);

    SDL_RenderClear(*ren);
    SDL_RenderDrawRect(*ren, &dest);
}

void togglePause (bool &isPlaying, Button &button) {
    if (isPlaying){ 
        Mix_PauseMusic();
        button.setText("Play");
        isPlaying = false;
    } 
    else {
        Mix_ResumeMusic();
        button.setText("Pause");
        isPlaying = true;
    }
}

void changeTrack (uint newTrackNumber, uint *currentTrack, PlayingState &currentState, List &tl) {
    *currentTrack = newTrackNumber;
    tl.setChosenElement(newTrackNumber);
    currentState = PlayingState::CHANGINGTRACK;
}

void changeVolume (int newVolume, int &currentVolume) {
    currentVolume = newVolume;
    Mix_VolumeMusic(currentVolume);    
};

std::vector<std::string> getDirEntriesMP3 (std::string path) {
    std::vector<std::string> res {};

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (!std::filesystem::is_directory(entry)) {
            if (((std::string)entry.path()).find(".mp3") != std::string::npos)
                res.push_back(entry.path());
            else
                std::cerr << "Skiped " << entry.path() << std::endl; 
        }
        else {
            std::vector<std::string> innerEntries = getDirEntriesMP3(entry.path());
            for (const std::string &j : innerEntries)
                res.push_back(j);
        }
    }
    return res;
}

std::vector<std::string> getDirEntries (std::string path) {
    std::vector<std::string> res {};
    for (const auto &entry : std::filesystem::directory_iterator(path))
        res.push_back(entry.path());
    return res;
}

const char* getCurrentPlayingStateAsString (PlayingState state) {
    switch (state)
    {
        case PlayingState::CONTINUE:
            return "continue";
            break;
        case PlayingState::QUITONEND:
            return "quit on end";
            break;
        case PlayingState::REPEAT:
            return "repeat";
            break;
        case PlayingState::CHANGINGTRACK:
            return "changing track";
            break;
        default:
            return "Unexpected";
            break;
    }
}

int getRandomInt (int min, int max) {
    return min + (std::rand() % (max - min + 1));
}
