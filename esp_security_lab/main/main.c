#include <stdio.h>

extern void demo_main(void);

void app_main(void)
{
    printf("\n🔒 Starting selected security demo...\n");
    demo_main();
}
