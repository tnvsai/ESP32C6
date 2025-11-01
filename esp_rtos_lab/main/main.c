#include <stdio.h>

extern void demo_main(void);  // Defined by whichever demo CMake links in

void app_main(void)
{
    printf("Starting selected demo...\n");
    demo_main();
}
