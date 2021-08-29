#include "func.h"


SDL_Surface* get_image(char* path)
{
    SDL_Surface* image = IMG_Load(path);

    if (!image)
        errx(3, "Can't load %s: %s", path, IMG_GetError());

    return image;
}


void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;

    // Set the window to the same size as the image
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL)
    {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                img->w, img->h, SDL_GetError());
    }

    // Blit onto the screen surface
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    // return the screen for further uses
    return screen;
}


void update_surface(SDL_Surface* screen, SDL_Surface* image)
{
    if (SDL_BlitSurface(image, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, image->w, image->h);
}
    static inline
Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}


Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch (surface->format->BytesPerPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;
    }

    return 0;
}


void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch(surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}


void wait_for_keypressed()
{
    SDL_Event event;

    // Wait for a key to be down.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYDOWN);

    // Wait for a key to be up.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYUP);
}



int binary_list(char* message, int* list)
{
    int len = 1;
    for(; *message != '\0'; message++, len++)
        for(int k = 7; k >= 0; k--, list++)
            *list = ( (*(message) & (1 << k)) >> k );

    for(int i = 0; i != 8; i++, list++)
        *list = 0;
    return len*8;
}


void steganographie(SDL_Surface* image, char* message)
{
    int len_list = (strlen(message)+1)*8;

    int* list_i = calloc(len_list, sizeof(int));

    int* list = list_i;

    binary_list(message, list);

    int i = 0, j = 0;

    while(len_list != 0)
    {
        if(j >= image->h)
        {
            i++;
            j=0;
        }
        Uint8 r,g,b;

        Uint32 pixel = get_pixel(image, i,j);

        SDL_GetRGB(pixel, image->format, &r, &g ,&b);


        if (*list)
            r |= 1UL;
        else
            r &= ~(1UL);

        len_list--;
        if (len_list <= 0)
            goto quit_loop;
        list++;


        if (*list)
            g |= 1UL;
        else
            g &= ~(1UL);

        len_list--;
        if (len_list <= 0)
            goto quit_loop;
        list++;


        if (*list)
            b |= 1UL;
        else
            b &= ~(1UL);

        len_list--;
        list++;

quit_loop:
        pixel = SDL_MapRGB(image->format, r, g, b);
        put_pixel(image, i, j, pixel);
        j++;
    }

    free(list_i);
}



void decrypt(SDL_Surface* image, char* buffer)
{
    //char* buffer = calloc(image->w*image->h*3, sizeof(char));

    char letter = 0;

    int count = 0;

    for(int i = 0; i != image->w; i++)
    {
        for(int j = 0; j != image->h; j++)
        {
            Uint8 r,g,b;

            Uint32 pixel = get_pixel(image, i,j);

            SDL_GetRGB(pixel, image->format, &r, &g ,&b);

            if (count == 8){
                *buffer = letter;
                if (letter == 0)
                    return;
                buffer++, letter = 0, count = 0;
            }

            letter = (r & 1) | (letter << 1);
            count ++;

            if (count == 8){
                *buffer = letter;
                if (letter == 0)
                    return;
                buffer++, letter = 0, count = 0;
            }

            letter = (g & 1) | (letter << 1);
            count ++;

            if (count == 8){
                *buffer = letter;
                if (letter == 0)
                    return;
                buffer++, letter = 0, count = 0;
            }

            letter = (b & 1) | (letter << 1);
            count++;
        }
    }
    return;
}



void convert_file_char(char* path, char* message)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        errx(1, "Error : Unable to open the file");
    char buffer[16];
    int r = 0;
    while((r = read(fd, buffer, 16)) > 0)
    {
        if(r == 0)
            return;
        for(int i = 0; i != r; i++, message++)
        {
            *message = buffer[i];
        }
    }
    *message = '\0';
    close(fd);
}








