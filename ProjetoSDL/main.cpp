//
//  main.cpp
//  ProjetoSDL
//
//  Copyright © 2016 Lemonade. All rights reserved.
//

#include <iostream>

#include "render.h"

int main(int argc, const char * argv[]) {
    Render render(800, 600, "Projeto SDL!");
    bool mainLoop = render.init();

    // XXX poll event
    while (mainLoop)
        render.draw();

    return 0;
}
