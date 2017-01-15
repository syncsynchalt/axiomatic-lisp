#include "axiom.h"
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    setlinebuf(stdout);
    init_cells();
    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        if (!f)
            die("Unable to open %s: %s", argv[i], strerror(errno));
        parse(f, 0);
    }
    parse(stdin, 0);
    return 0;
}
