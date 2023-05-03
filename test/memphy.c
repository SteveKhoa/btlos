#include <stdio.h>
#include "mem.h"
#include "mm.h"
#include "common.h"
#include "os-mm.h"


int
main ()
{
    BYTE str[] = "hello_world";

    struct memphy_struct ram;
    init_memphy(&ram, 1024, 0);

    for (int i = 0 ; i < sizeof(str)/sizeof(BYTE) - 1 ; ++i)
    {
        MEMPHY_write(&ram, i, str[i]);
    }

    for (int i = 0 ; i < sizeof(str)/sizeof(BYTE) - 1 ; ++i)
    {
        BYTE retval;
        MEMPHY_read(&ram, i, &retval);
        printf("Read:\t%c\n", retval);
    }
    return 0;
}