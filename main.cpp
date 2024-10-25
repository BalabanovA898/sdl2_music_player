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
#define INITIAL_PATH "/home/andrey/Music"
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

void error_end(int error_code, const char *error_title, const char *error_description);

int  parse_arguments (int argc, const char *argv[], bool &is_running, bool &is_playlist, std::vector<std::string> &playlist);

void initSDL(SDL_Window **win, SDL_Renderer **ren);
void quitSDL(SDL_Window **win, SDL_Renderer **ren);
void clear_screen (SDL_Renderer **ren, int screen_width, int screen_height);

void toggle_pause (bool &isPlaying, Button &button);
void change_track (uint newTrackNumber, uint *currentTrack, PlayingState &currentState, List &tl);
void change_volume (int newVolume, int &currentVolume);
void change_dir (std::string path, std::string &currentPath, List &list, std::vector<ListElement> vle);

std::vector<std::string> get_dir_entries_MP3 (std::string path);
std::vector<std::string> get_dir_entries (std::string path);
const char* get_current_playing_state_as_string (PlayingState state);

int get_random_int (int min, int max);

int main (int argc, const char** argv) {
  int current_window_width = INITIAL_WINDOW_WIDTH,
    current_window_height = INITIAL_WINDOW_HEIGHT;
  
  bool is_running  = true,
    is_playing  = false,
    is_playlist = false;
  
  PlayingState current_state = PlayingState::CONTINUE;
  
  int current_volume = STANDART_VOLUME;
  uint current_track = 0;
  
  std::vector<std::string> playlist  {};
  std::string reading_path = INITIAL_PATH;
  std::string buffer_path  = INITIAL_PATH;
  
  if (parse_arguments(argc, argv, is_running, is_playlist, playlist)) {
    error_end(4, "Error while parsing arguments. ", SDL_GetError());
  }
  
  AppMode current_mode = AppMode::IDLE;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  
  initSDL(&window, &renderer);
  
  TTF_Font *font = TTF_OpenFont(PATH_TO_FONT, 18);     
  Text track_title = Text (0 + LEFT_PADDING, LINE_HEIGHT * 0, 0, 0, "");
  Text volume = Text (0 + LEFT_PADDING, LINE_HEIGHT * 1, 0, 0, std::to_string(current_volume).c_str());
  
  Button continue_state_button = Button (0 + LEFT_PADDING, LINE_HEIGHT * 2 + LINE_GAP, BUTTON_WIDTH / 3, LINE_HEIGHT * 2 - LINE_GAP, "c");
  Button repeat_state_button = Button (0 + LEFT_PADDING + BUTTON_WIDTH / 3, LINE_HEIGHT * 2 + LINE_GAP, BUTTON_WIDTH / 3, LINE_HEIGHT * 2 - LINE_GAP, "r");
  Button quit_on_end_state_button = Button (0 + LEFT_PADDING + BUTTON_WIDTH / 3 * 2, LINE_HEIGHT * 2 + LINE_GAP, BUTTON_WIDTH / 3, LINE_HEIGHT * 2 - LINE_GAP, "q");
  
  Button play_button = Button (0 + LEFT_PADDING, LINE_HEIGHT * 4 + LINE_GAP, BUTTON_WIDTH, LINE_HEIGHT * 2 - LINE_GAP, "Pause");
  Button volume_up_button = Button (BUTTON_WIDTH + LEFT_PADDING * 2, LINE_HEIGHT * 4 + LINE_GAP, BUTTON_WIDTH / 2, LINE_HEIGHT * 2 - LINE_GAP, "+");
  
  Button prev_button = Button (0 + LEFT_PADDING, LINE_HEIGHT * 6 + LINE_GAP, BUTTON_WIDTH / 2 - LEFT_PADDING / 2, LINE_HEIGHT * 2 - LINE_GAP, "<");
  Button next_button = Button (LEFT_PADDING + LEFT_PADDING / 2 + BUTTON_WIDTH / 2, LINE_HEIGHT * 6 + LINE_GAP, BUTTON_WIDTH / 2 - LEFT_PADDING / 2, LINE_HEIGHT * 2 - LINE_GAP, ">");
  Button volume_down_button = Button (BUTTON_WIDTH + LEFT_PADDING * 2, LINE_HEIGHT * 6 + LINE_GAP, BUTTON_WIDTH / 2, LINE_HEIGHT * 2 - LINE_GAP, "-");
  
  Button exit_button = Button (current_window_width - 30, 0, 30, 30, "x");
  
  Text path_changer = Text (0 + LEFT_PADDING, LINE_HEIGHT * 8, 0, 0, buffer_path.c_str());
  
  List track_list = List (0 + LEFT_PADDING, LINE_HEIGHT * 10, current_window_width, current_window_height, current_track);
  
  for (uint index = 0; index < playlist.size(); index++) {
    ListElement new_element = ListElement (0, 0, 300, LINE_HEIGHT, playlist[index].c_str(), playlist[index].c_str());
    track_list.elements_push(new_element);
  }
  
  Mix_Music *music = nullptr;
  
  while (is_playlist) {
    music = Mix_LoadMUS(playlist[current_track].c_str());
    if (music == NULL) {
      current_track += 1;
      break;
    }
    const char *title = Mix_GetMusicTitle(music);
    
    track_title.set_text(title);
    volume.set_text(std::to_string(current_volume).c_str());
    
    Mix_VolumeMusic(current_volume);
    Mix_PlayMusic(music, 0);
    
    is_playing = true;
    SDL_Event event;
    
    while (is_running)  {
      while (SDL_PollEvent(&event)) {
	continue_state_button.event_handler(event);
	repeat_state_button.event_handler(event);
	quit_on_end_state_button.event_handler(event);
	play_button.event_handler(event);
	next_button.event_handler(event);
	prev_button.event_handler(event);
	volume_up_button.event_handler(event);
	volume_down_button.event_handler(event);
	exit_button.event_handler(event);
	switch (event.type) {
	case SDL_QUIT:
	  is_playlist = false;
	  is_running  = false;
	  break;
	case SDL_TEXTINPUT:
	  if (current_mode == AppMode::TEXTINPUT) {
	    buffer_path += event.text.text;
	    path_changer.set_text(buffer_path.c_str());
	  }
	  break;
	case SDL_KEYDOWN:
	  switch (event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    current_mode = AppMode::IDLE;
	    if (current_mode == AppMode::TEXTINPUT)
	      SDL_StopTextInput();
	    continue;
	  case SDLK_SPACE:
	    if (current_mode == AppMode::IDLE)
	      toggle_pause(is_playing, play_button);
	    continue;
	  case SDLK_DOWN:
	    if (current_volume > 5)
	      change_volume(current_volume - 5, current_volume);
	    continue;
	  case SDLK_UP:
	    if (current_volume < MIX_MAX_VOLUME - 5)
	      change_volume(current_volume + 5, current_volume);
	    continue;
	  case SDLK_LEFT:
	    if (Mix_GetMusicPosition(music) > 5)
	      Mix_SetMusicPosition(Mix_GetMusicPosition(music) - 5);
	    continue;
	  case SDLK_RIGHT:
	    if (current_mode == AppMode::IDLE)
	      Mix_SetMusicPosition(Mix_GetMusicPosition(music) + 5);
	    continue;
	  case SDLK_r:
	    if (current_mode == AppMode::IDLE)
	      current_state = PlayingState::REPEAT;
	    continue;
	  case SDLK_c:
	    if (current_mode == AppMode::IDLE)
	      current_state = PlayingState::CONTINUE;
	    continue;
	  case SDLK_q:
	    if (current_mode == AppMode::IDLE)
	      current_state = PlayingState::QUITONEND;
	    continue;
	  case SDLK_RIGHTBRACKET:
	    if (current_mode == AppMode::IDLE) {
	      if (current_track + 1 < playlist.size())
		change_track(current_track + 1, &current_track, current_state, track_list);
	      else
		change_track(0, &current_track, current_state, track_list);
	      goto changeTrackPoint;
	    }
	    continue;
	  case SDLK_LEFTBRACKET:
	    if (current_mode == AppMode::IDLE) {
	      if (current_track == 0)
		change_track(playlist.size() - 1, &current_track, current_state, track_list);
	      else
		change_track(current_track - 1, &current_track, current_state, track_list);
	      goto changeTrackPoint;
	    }
	    continue;
	  case SDLK_p:
	    if (current_mode == AppMode::IDLE) {
	      change_track(get_random_int(0, playlist.size() - 1), &current_track, current_state, track_list);
	      goto changeTrackPoint;
	    }
	    continue;
	  case SDLK_i:
	    if (current_mode == AppMode::IDLE) {
	      current_mode = AppMode::TEXTINPUT;
	      SDL_StartTextInput();
	    }
	    continue;
	  case SDLK_BACKSPACE:
	    if (current_mode == AppMode::TEXTINPUT)
	      buffer_path.pop_back();
	    continue;
	  case SDLK_RETURN:
	    if (std::filesystem::is_directory(buffer_path)) {
	      reading_path = buffer_path;
	      playlist = get_dir_entries_MP3(reading_path);
	      current_track = 0;
	      std::vector<ListElement> newTrackList {};
	      for (uint index = 0; index < playlist.size(); index++) {
		ListElement newElement = ListElement (0, 0, 300, LINE_HEIGHT, playlist[index].c_str(), playlist[index].c_str());
		newTrackList.push_back(newElement);
	      }
	      track_list.elements = newTrackList;
	      change_track(current_track, &current_track, current_state, track_list);
	      goto changeTrackPoint;
	    } else {
	      buffer_path = reading_path;
	      path_changer.set_text(buffer_path.c_str());
	    }
	    break;
	  }
	  break;
	case SDL_MOUSEBUTTONDOWN:
	  if (continue_state_button.check_click()) {
	    current_state = PlayingState::CONTINUE;
	  }
	  else if (repeat_state_button.check_click()) {
	    current_state = PlayingState::REPEAT;
	  }
	  else if (quit_on_end_state_button.check_click()) {
	    current_state = PlayingState::QUITONEND;
	  }
	  else if (play_button.check_click())
	    toggle_pause(is_playing, play_button);
	  else if (next_button.check_click()) {
	    if (current_track  + 1 < playlist.size())
	      change_track(current_track + 1, &current_track, current_state, track_list);
	    else
	      change_track(0, &current_track, current_state, track_list);
	    goto changeTrackPoint;
	  }
	  else if (prev_button.check_click()) {
	    if (current_track == 0)
	      change_track(playlist.size() - 1, &current_track, current_state, track_list);
	    else
	      change_track(current_track - 1, &current_track, current_state, track_list);
	    goto changeTrackPoint;
	  }
	  else if (volume_up_button.check_click()) {
	    if (current_volume < MIX_MAX_VOLUME - 5)
	      change_volume(current_volume + 5, current_volume);
	  }
	  else if (volume_down_button.check_click()) {
	    if (current_volume > 5)
	      change_volume(current_volume - 5, current_volume);
	  } else if (exit_button.check_click()) {
	    is_running = false;
	    is_playlist = false;
	  }
	  break;
	default:
	  break;
	}
      } //Event
      
      SDL_GetWindowSize(window, &current_window_width, &current_window_height);
      track_list.resize_list(current_window_width, current_window_height - PADDING - LINE_HEIGHT*10);
      exit_button.move(current_window_width - 30, 0);
      
      volume.set_text(std::to_string(current_volume).c_str());
      
      clear_screen(&renderer, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);        
      
      if (track_title.render_text(renderer, font) ||
	  volume.render_text(renderer, font) ||
	  path_changer.render_text(renderer, font) != 0) {
	error_end(4, "Error with rendering.", SDL_GetError());
      }
      
      if (continue_state_button.render(renderer, font) ||
	  repeat_state_button.render(renderer, font) ||
	  quit_on_end_state_button.render(renderer, font) ||
	  play_button.render(renderer, font) ||
	  next_button.render(renderer, font) ||
	  prev_button.render(renderer, font) ||
	  volume_up_button.render(renderer, font) ||
	  volume_down_button.render(renderer, font) ||
	  exit_button.render(renderer, font) != 0)
	error_end(4, "Error with rendering.", SDL_GetError());
      
      if (track_list.render(renderer, font) != 0)
	error_end(4, "Error with rendering.", SDL_GetError());
      
      SDL_RenderPresent(renderer);
      
      if (!Mix_PlayingMusic())
	break;
      
      SDL_Delay(1000/FPS);       
    } // while (is_running)
    
  changeTrackPoint:; //goto point for Next, Previous or Random Track option
    
    switch (current_state) {
    case PlayingState::CONTINUE:
      if (current_track + 1 < playlist.size())
	change_track(current_track + 1, &current_track, current_state, track_list);
      else
	change_track(0, &current_track, current_state, track_list);
      break;
    case PlayingState::QUITONEND:
      is_playlist = false;
      break;
    case PlayingState::REPEAT:
      break;
    case PlayingState::CHANGINGTRACK:
      current_state = PlayingState::CONTINUE;
      break;
    }
    Mix_FreeMusic(music);
  } // while (is_playlist)
  
  TTF_CloseFont(font);
  quitSDL(&window, &renderer);
  return 0;
} //main

void error_end(int error_code, const char *error_title, const char *error_description) {
  std::cerr << error_title << ' ' << error_description << std::endl;
  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
  exit(error_code);
}


int parse_arguments (int argc, const char* argv[], bool& is_running, bool& is_playlist, std::vector<std::string>& playlist) {
  std::string initial_path = INITIAL_PATH;  
  if (argc == 1)
    playlist = get_dir_entries_MP3(initial_path);
  else if (argc == 2) {
    if (std::filesystem::is_directory(argv[1])) 
      playlist = get_dir_entries_MP3(argv[1]);
    else 
      playlist.push_back(argv[1]);
  } else {
    int current_element_index = 1;
    while (current_element_index < argc) {
      std::string current_argument(argv[current_element_index]);
      if (current_argument == "--") {
	if (current_element_index + 1 >= argc) {
	  is_running = false;
	  return -1;
	}
	current_element_index += 1;
	std::string current_argument(argv[current_element_index]);
	
	if (current_argument == "help") {
	  is_running = false;
	  std::printf("TODO Write this help message");
	  return 0;
	} else if (current_argument == "dev") { 
	  initial_path = "/home/andrey/Music";
	} else if (current_argument == "linux" || current_argument == "gnu-linux") {
	  initial_path = "~/Music";
	} else if (current_argument == "windows") {
	  initial_path = "c:\\";
	}
	current_element_index += 1;
      }
    }
  }
  
  if (playlist[0] != "")
    is_playlist = true;
  return 0;
}

void initSDL(SDL_Window **win, SDL_Renderer **ren) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    error_end(1, "Error with initializing a SDL.", SDL_GetError());
  
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    error_end(1, "Error with initializing an audio module.", Mix_GetError());
  
  if (TTF_Init() != 0)
    error_end(1, "Error with initializing a TTF.", TTF_GetError());
  
  *win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (*win == nullptr)
    error_end(2, "Error with creating a window.", SDL_GetError());
  *ren = SDL_CreateRenderer(*win, 0, SDL_RENDERER_ACCELERATED);
  if (*ren == nullptr)
    error_end(2, "Error with initializing a renderer.", SDL_GetError());
}

void quitSDL(SDL_Window **win, SDL_Renderer **ren) {
  SDL_DestroyRenderer(*ren);
  SDL_DestroyWindow(*win);
  Mix_Quit();
  TTF_Quit();
  SDL_Quit();
}

void clear_screen (SDL_Renderer **ren, int screen_width, int screen_height) {
  SDL_Rect dest {0, 0, screen_width, screen_height};
  
  SDL_SetRenderDrawColor(*ren, 255, 255, 255, 255);
  
  SDL_RenderClear(*ren);
  SDL_RenderDrawRect(*ren, &dest);
}

void toggle_pause (bool &is_playing, Button &button) {
  if (is_playing){ 
    Mix_PauseMusic();
    button.set_text("Play");
    is_playing = false;
  } 
  else {
    Mix_ResumeMusic();
    button.set_text("Pause");
    is_playing = true;
  }
}

void change_track (uint new_track_number, uint *current_track, PlayingState &current_state, List &tl) {
  *current_track = new_track_number;
  tl.set_chosen_element(new_track_number);
  current_state = PlayingState::CHANGINGTRACK;
}

void change_volume (int new_volume, int &current_volume) {
  current_volume = new_volume;
  Mix_VolumeMusic(current_volume);    
};

std::vector<std::string> get_dir_entries_MP3 (std::string path) {
  std::vector<std::string> res {};
  
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (!std::filesystem::is_directory(entry)) {
      if (((std::string)entry.path()).find(".mp3") != std::string::npos)
	res.push_back(entry.path());
      else
	std::cerr << "Skiped " << entry.path() << std::endl; 
    }
    else {
      std::vector<std::string> innerEntries = get_dir_entries_MP3(entry.path());
      for (const std::string &j : innerEntries)
	res.push_back(j);
    }
  }
  return res;
}

std::vector<std::string> get_dir_entries (std::string path) {
  std::vector<std::string> res {};
  for (const auto &entry : std::filesystem::directory_iterator(path))
    res.push_back(entry.path());
  return res;
}

const char* get_current_playing_state_as_string (PlayingState state) {
  switch (state) {
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

inline int get_random_int (int min, int max) {return min + (std::rand() % (max - min + 1));}
