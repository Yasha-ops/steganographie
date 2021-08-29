#ifndef FUNC_H_
#define FUNC_H_

#include <stdio.h>
#include <stdlib.h>


#include <err.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

SDL_Surface* get_image(char* path);
SDL_Surface* display_image(SDL_Surface *img);
void init_sdl();
void wait_for_keypressed();

void steganographie(SDL_Surface* image, char* message);
int binary_list(char* message, int* list);

void convert_file_char(char* path, char* message);
void decrypt(SDL_Surface* image, char* buffer);
#endif
