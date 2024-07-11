#include <ctype.h>
#include <stdlib.h>

#include "utils.h"

int str_to_pid(const char *str)
{
    return (int)strtol(str, NULL, 10);
}
