//
//  main.cpp
//  ProjetoSDL
//
//  Copyright © 2016 Lemonade. All rights reserved.
//

// NOTE: Didn't have worked when tested on a MacBook Pro :/
// (OpenGL v4.1 INTEL-10.6.33 using Intel HD Graphics 4000 OpenGL Engine)
//
// But worked on my personal laptop with linux
// (OpenGL v4.5.13399 Compatibility Profile Context 15.201.1151 using AMD Radeon HD 7700M Series)

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
