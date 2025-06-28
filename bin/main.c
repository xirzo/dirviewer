#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "dv.h"

/* Result: */

/*
   test
   ├── a
   │   └── b
   └── lol
   */

int main(int argc, char *argv[]) {
    int c;
    int digit_optind = 0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;

        static struct option long_options[] = {
            {"exclude-dir",  required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "", long_options, &option_index);

        if (c == -1){
            break;
        }

        switch (c) {
            case 0: {
                struct stat sb;

                if (stat(optarg, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
                    fprintf(stderr, "error: exclude-dir directory does not exist\n");
                    break;
                }

                printf("\n");
                break;
            }

            default: {
                fprintf(stderr, "error: wrong option character code\n");
                break;
            }
        }
    }

    return 0;
}
