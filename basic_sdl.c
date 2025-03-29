#include<SDL2/SDL.h>
#include<stdio.h>

int main(){
    SDL_Window *window;                 //DECLARE A WINDOW
    SDL_Renderer *renderer;             //DECLARE A RENDERER

    SDL_Init(SDL_INIT_VIDEO);           // INTIALIZE SDL2

    //Create an application window with the folllowing settings:

    window = SDL_CreateWindow("Window 1",                        //window title
                               SDL_WINDOWPOS_UNDEFINED,          //initial x position
                               SDL_WINDOWPOS_UNDEFINED,          //intital y position
                               640,                              //width, in pixels
                               480,                              //height in pixels
                               0);                               //flags ()

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetRenderDrawColor(renderer,0,0,255,255); //(renderer,R,G,B,alpha)
    
    SDL_Rect rect = {220,140,200,200}; //object of SDL_rect Struct, used to draw rectangle
    SDL_RenderFillRect(renderer,&rect);

    //We are done drawing, "present" or show to the screen what we've drawn
    SDL_RenderPresent(renderer);

    //Wait a few seconds before quitting
    SDL_Delay(2000); //value is in milli-seconds

    //close and Destroy Window
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    //quit SDl
    SDL_Quit();
    return 0;
}