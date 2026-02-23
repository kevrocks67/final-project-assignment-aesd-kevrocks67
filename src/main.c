#include <stdio.h>
#include "lock.h"

int main(int argc, char *argv[]) {
    printf("Door Security Daemon Starting...\n");
    printf("Is Locked?: %d\n", is_locked());
    return 0;
}
