#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Color couleur_pixel(const SDL_Surface *image , int X, int Y)
{
    // retourne la quantité de rouge , vert , bleu et opacité pour un pixel de coordonnées (X,Y)

    Uint8 bpp = image->format->BitsPerPixel;

    Uint8 *ppixel = (Uint8 *) image->pixels + Y*(image->pitch) + X*bpp;   // on récupère l'information du pixel choisi
    Uint32 data_pixel = *(Uint32*) ppixel;

    SDL_Color color = {0 , 0 , 0 , 0};

    SDL_GetRGBA(data_pixel , image->format , &color.r , &color.g , &color.b , &color.a );   // on remplit le contenu de color

    //printf("Le pixel est composé de %d rouge, %d vert et %d bleu et est %d opaque.\n",color.r , color.g , color.b , color.a);
    return color;
}

int *couleur_image(SDL_Surface *image, int *color_image)
{
    // retourne la quantité de rouge , vert , bleu et opacité pour une image

    int i; // parcourt en largeur
    int j; // parcourt en longueur

    // on récupère la taille de l'image
    int largeur = image -> w;
    int longueur = image -> h;

    Uint8 bpp = image->format->BitsPerPixel;
    SDL_Color color_pixel = {0x00 , 0x00 , 0x00 , SDL_ALPHA_OPAQUE};    // couleur d'un pixel
    

    for (i = 0 ; i <= largeur ; i++)
    {
        int Y = i;
        for (j = 0 ; j <= longueur ; j++)
        {
            int X = j;
            
            Uint8 *ppixel = (Uint8 *) image->pixels + Y*image->pitch + X*bpp;   // on récupère l'information du pixel choisi
            Uint32 data_pixel = *(Uint32*) ppixel;

            SDL_GetRGBA(data_pixel , image->format , &color_pixel.r , &color_pixel.g , &color_pixel.b , &color_pixel.a );

            // on ajoute les valeurs obtenues du pixel dans celle de l'image entière
            color_image[0] += color_pixel.r;
            color_image[1] += color_pixel.g;
            color_image[2] += color_pixel.b;
            color_image[3] += color_pixel.a;
        }
        
    }

    for (i = 0 ; i <= 4 ; i++)
    {
        color_image[i] /= (largeur*longueur);
    }

    return color_image;
}

// Fonction pour la valeur absolue :
double val_abs(double a)
{
    if(a < 0) a=-a;
    return a;
}

SDL_Surface *moy_pixel(SDL_Surface *image, float eps)
{
    int n = image -> w;
    int m = image -> h;

    float y = (float) 1/(m*n);
    
    int i,j;
    //float moyenne = 0.0;
    float m_R = 0.0;
    float m_V = 0.0;
    float m_B = 0.0;
    
    //int *color_image;

    for (i = 0 ; i < m ; i++)
    {   
        for (j = 0 ; j < n ; j++)
        {
            SDL_Color res_amplitude1 = couleur_pixel(image , i,j);
            float r1 = res_amplitude1.r;
            float v1 = res_amplitude1.g;
            float b1 = res_amplitude1.b;
            
            m_R += r1 ;
            m_V += v1 ;
            m_B += b1 ;
            
            //moyenne += (r1 + v1 + b1);
            
        }
    }
   
    m_R = m_R*y ;
    m_V = m_V*y ;
    m_B = m_B*y ;
    
    //moyenne = (float) moyenne*y;

 
    for (i = 0 ; i < m ; i++)
    {
        for (j = 0 ; j < n ; j++)
        {
            SDL_Color res_amplitude1 = couleur_pixel(image , i,j);
            float r1 = res_amplitude1.r;
            float v1 = res_amplitude1.g;
            float b1 = res_amplitude1.b;

            
            if (abs(m_R - r1) > eps)
            {
                r1 = m_R ;
            }
            if (abs(m_V - v1) > eps)
            {
                v1 = m_V ;
            }
            if (abs(m_B - b1) > eps)
            {
                b1 = m_R ;
            }
            
        }
    }
    
    return image;
}



SDL_Surface *correction_image(SDL_Surface *image, Uint32 nb_sub){

   
    Uint8 bpp = image->format->BitsPerPixel;
    const Uint32 x = image->w;
    const Uint32 y = image->h;
    Uint32 a=floor(x/nb_sub)+1; // floor : partie entière     a c'est le nombre de sous divisions verticales
    Uint32 b=floor(y/nb_sub)+1; // b : nb de sous divisions horizontales
    SDL_Surface *temp = NULL; // sous division
    temp = (SDL_Surface *) malloc(sizeof(SDL_Surface));

    if (!temp)
    {
        printf("Erreur allocation mémoire\n");
        exit(1);
    }

    SDL_Rect rect1; 
    SDL_Rect rect2;

    int i,j;
    
    for(i=1;i++;i<a){
        for(j=1;j++;j<b){  // on ne fait ça qu'une fois
            
            if ((a*(i+1)>x) && (b*(j+1)>y)){ // si on dépasse l'image de base en bas et à droite
                
                rect1.x=a*i;
                rect1.y=b*j;
                rect1.w=x - a*i;
                rect1.h=y - b*j;
                temp->w=x - a*i; // SEG FAULT A PARTIR D'ICI DONC A VOIR SI C'EST REPERCUTE DANS LES AUTRES IF
                
                temp->h=y - b*j;     
                  
                rect2.x=0;
                rect2.y=0;
                
                rect2.w=x - a*i;
                
                rect2.h=y - b*j;
                
                SDL_BlitSurface(image,&rect1,temp,&rect2);

                
                temp = moy_pixel(temp,0.01);
                printf("ici\n");
                SDL_BlitSurface(temp,&rect2,image,&rect1);

            }
            else if ((a*(i+1)<=x) && (b*(j+1)>y)){   // si on dépasse l'image par le bas
                
                rect1.x=a*i;
                rect1.y=b*j;
                rect1.w=a;
                rect1.h=y - b*j;

                temp->w=a;
                temp->h=y - b*j;        
                rect2.x=0;
                rect2.y=0;
                rect2.w=a;
                rect2.h=y - b*j;

                SDL_BlitSurface(image,&rect1,temp,&rect2);

                temp = moy_pixel(temp,0.01);

                SDL_BlitSurface(temp,&rect2,image,&rect1);

                
            }
            else if ((a*(i+1)>x) && (b*(j+1)<=y)){ // si on dépasse par la droite
                
                rect1.x=a*i;
                rect1.y=b*j;
                rect1.w=x - a*i;
                rect1.h=b;
                   
                temp->w=x - a*i;
                temp->h=b;        
                rect2.x=0;
                rect2.y=0;
                rect2.w=x - a*i;
                rect2.h=b;

                SDL_BlitSurface(image,&rect1,temp,&rect2);

                temp = moy_pixel(temp,0.01);

                SDL_BlitSurface(temp,&rect2,image,&rect1);

            }
            else if ((a*(i+1)<=x) && (b*(j+1)<=y)){ // si tout va bien
                
//          On définit le rectangle à extraire      
                rect1.x=a*i;
                rect1.y=b*j;
                rect1.w=a;
                rect1.h=b;
//          On redéfinit la taille de l'image temporaire
                temp->w=a;
                temp->h=b;        
                rect2.x=0;
                rect2.y=0;
                rect2.w=a;
                rect2.h=b;

                SDL_BlitSurface(image,&rect1,temp,&rect2);
//          moyenne à remplacer par le nom de la fonction
                temp = moy_pixel(temp,0.01);

                SDL_BlitSurface(temp,&rect2,image,&rect1);
                
            }
        }
    }
    return image;
}


int main()
{

    SDL_version nb;
    SDL_VERSION(&nb);

    int i,j;

    char *filename = "images/pepers.jpg";
    
    SDL_Surface *image, *converted;
    
    image = IMG_Load(filename);
    
    if (image == NULL) {
        fprintf(stderr, "Could not load image: %s\n", IMG_GetError());
        return -1;
    }
    
    moy_pixel(image,0.01);

    correction_image(image,3);

    return 0;
}