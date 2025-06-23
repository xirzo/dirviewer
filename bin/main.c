#include <unistd.h>
#include "dv.h"

/* Result: */

/*
test
├── a
│   └── b
└── lol
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
