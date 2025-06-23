#include <unistd.h>
#include "dv.h"

/* Result I wanna get: */

/*
.
├── bin
│   └── main
├── include
│   └── GLFW
│       ├── glfw3.h
│       └── glfw3native.h
├── lib
│   └── libglfw3.a
└── src
    └── main.c
*/

int main(int argc, char *argv[]) {
    if (argc == 1) {
        dirview(".");
    } else {
        while (--argc > 0) {
            dirview(*++argv);
        }
    }

    return 0;
}
