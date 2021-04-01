#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define BYTES_PER_PIXEL (3)

void get_pixel_at(Uint8 *pixels, long x, long y, Uint32 pitch, Uint8* r, Uint8 *g, Uint8 *b) {
    
    const Uint32 offset = BYTES_PER_PIXEL * x + y * pitch;
    *r = (Uint32)pixels[offset];
    *g = (Uint32)pixels[offset + 1];
    *b = (Uint32)pixels[offset + 2];
}


void set_pixel_at(Uint8 *pixels, long x, long y, Uint32 pitch, Uint8 r, Uint8 g, Uint8 b) {
    const Uint32 offset = BYTES_PER_PIXEL * x + y * pitch;
    pixels[offset    ] = r;
    pixels[offset + 1] = g;
    pixels[offset + 2] = b;
}

void keep_only_red(SDL_Surface* image) {
    const Uint32 width = image->w;
    const Uint32 height = image->h;
    // pitch = length of each line in bytes
    // this may not always be equal to the width as
    // some image formats put some paddings between the lines
    const Uint32 pitch = image->pitch; 
    Uint8 *pixels = (Uint8*)image->pixels;

    Uint8 r, g, b;
    for (long j = 0; j < height; j++) {
	for (long i = 0; i < width; i++) {
	    get_pixel_at(pixels, i, j, pitch, &r, &g, &b);
	    set_pixel_at(pixels, i, j, pitch, r, 0, 0); 
	}
    }    
}

int main(int argc, char **argv) {

    if (argc < 2) {
	fprintf(stderr, "Arg count < 2: usage ./prog <image path>\n");
	return -1;
    }
    char* filename = argv[1];
    
    SDL_Surface *image, *converted;
    
    image = IMG_Load(filename);
    if (image == NULL) {
	fprintf(stderr, "Could not load image: %s\n", IMG_GetError());
	return -1;
    }
    
    converted = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB24, 0);
    SDL_FreeSurface(image);
    if (!converted) {
	fprintf(stderr, "Could not convert image: %s\n",  SDL_GetError());
	return -1;
    }

    keep_only_red(converted);

    IMG_SavePNG(converted, "converted.png");
    
    SDL_FreeSurface(converted);
    
    return 0;
}
