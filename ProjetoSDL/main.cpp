//
//  main.cpp
//  ProjetoSDL
//
//  Copyright © 2016 Lemonade. All rights reserved.
//

#include <iostream>

#include "render.h"

int main(int argc, const char *argv[]) {
    Render render(800, 600, "Projeto SDL!");

    if (!render.init())
        exit(1);

    bool mainLoop = true;

    while (mainLoop) {
        if (render.poll() == false)
            mainLoop = false;

        render.draw();
    }

    return 0;
}
