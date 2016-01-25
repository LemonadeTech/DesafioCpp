//
//  main.cpp
//  ProjetoSDL
//
//  Copyright © 2016 Lemonade. All rights reserved.
//

#include "MainWindow.h"

#undef main

int main(int argc, const char * argv[])
{
    MainWindow mainwindow;
    
    SDL_Event e;
    bool quit = false;

    while (!quit)
    {
        mainwindow.render();
        
        if (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
                quit = true;
    }
    
    return 0;
}