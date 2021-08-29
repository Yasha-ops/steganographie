#include "func.h"

#include <string.h>


void help()
{
    printf("Usage: \n\tsteganography [OPTION] .. [INPUT IMAGE]\n");
    printf("\nDescription : \n\t Steganography is the practice of concealing a \
message within\n\t another message or a physical object. \
In computing/electronic contexts, \n \t a computer file, message, image, or \
video is concealed within another file, \n\t message, image, or video. \n");
    printf("\nOptions :\n");
    printf ("\t-i,--input [INPUT IMAGE]  : input image to hide the message.\n");
    printf ("\t-o,--output [OUTPUT NAME] : give an output name for the result image. ;\n");
    printf ("\t-s,--string [STRING]      : string to be hided.\n");
    printf( "\t-f,--file [FILE PATH]     : file to hide.\n");
    printf( "\t-d,--decrypt              : decrypt an image.\n");
    printf ("\t-h, --help                : show this help message.\n");
    printf("\nAuthors:\n\tYassine Damiri.\n");
    printf("\nLicense:\n\t\tMIT License \n \
\tCopyright (c) 2021 YassineDamiri\n");
}



int main(int argc, char** argv)
{
    if (argc < 2){
        help();
        errx(1, "Error : No options found !\n");
    }

    char used = 0;
    char output_name_bool = 0;
    char decrypt_bool = 0;
    char input_bool = 0;


    char* output_name = calloc(128, sizeof(char));
    char* input_name = calloc(128, sizeof(char));
    char* message = calloc(1382400, sizeof(char));// 1920*1920*3/8


    for(; argc != 0; argc--)
    {
        if(!strcmp("-h", *argv) || !strcmp("--help", *argv))
        {
            help();
            return 0;
        }

        if( !strcmp("-i", *argv) || !strcmp("--input", *argv))
        {
            input_bool = 1;
            strcpy(input_name, *(argv+1));
        }

        if( !strcmp("-o", *argv) || !strcmp("--output", *argv))
        {
            output_name_bool = 1;
            strcpy(output_name, *(argv+1));
        }

        if(!strcmp("-s", *argv)  || !strcmp("--string", *argv))
        {

            if (used)
                errx(1, "Error : -f and -s can't be used together \
                        togglreplease select one");
            used = 1;
            strcpy(message, *(argv+1));
        }


        if(!strcmp("-f", *argv) || !strcmp("--file", *argv))
        {
            if (used)
                errx(1, "Error : -f and -s can't be used together \
                        please select one");
            used = 1;
            convert_file_char(*(argv+1), message);
        }

        if(!strcmp("-d", *argv) || !strcmp("--decrypt", *argv))
        {
            if (used)
                errx(1, "Error : -f / -s can't be used with -d !");
            decrypt_bool = 1;
        }
        argv++;
    }


    if(!input_bool && !decrypt_bool)
        errx(1, "Error : No input image, use -i [IMAGE PATH]");

    if(!used && !decrypt_bool)
        errx(1, "Error : No input text, use -s [STRING] or -f [FILE PATH]");

    if(!output_name_bool)
        strcpy(output_name, "output.bmp");

    init_sdl();

    SDL_Surface* image = get_image(input_name);

    if(decrypt_bool)
    {
        printf("Decrypting image ...\n");
        printf("########################\n");
        char* buffer = calloc(image->w*image->h*3, sizeof(char));
        decrypt(image, buffer);
        printf("Decrypted message : %s\n", buffer);
        free(buffer);
        return 0;
    }

    printf("Image processing...\n");

    printf("########################\n");

    steganographie(image, message);

    printf("Message added to '%s', output picture \
'%s' saved ! \n", input_name,output_name);

    if(SDL_SaveBMP(image , output_name) != 0)
        errx(1, "Error : SDL_SaveBMP failed  %s\n", SDL_GetError());


    SDL_FreeSurface(image);


    free(output_name);
    free(input_name);
    free(message);
    return 0;
}
