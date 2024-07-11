#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "memory.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s <pid> <pattern>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = str_to_pid(argv[1]);
    if (pid <= 0) {
        printf("Invalid PID: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    const char* signature = argv[2];
    size_t pattern_length;
    
    int *pattern = convert_signature(signature, &pattern_length);
    if (!pattern) {
        printf("Failed to convert signature\n");
        return EXIT_FAILURE;
    }

    int regions_count = 0;
    MemoryRegion* regions = get_memory_regions(pid, &regions_count);
    if (!regions) {
        printf("Failed to get memory regions\n");
        free(pattern);
        return EXIT_FAILURE;
    }

    for(int i = 0; i < regions_count; i++)
    {
        MemoryRegion region = regions[i];
        size_t region_size = region.end - region.start;
        uint8_t* buffer = (uint8_t*)malloc(region_size);
        if(!buffer)
        {
            printf("Failed to allocate memory\n");
            free(regions);
            free(pattern);
            return EXIT_FAILURE;
        }
        
        if(read_process_memory(pid, region.start, buffer, region_size))
        {
            for(size_t i = 0; i <= region_size - pattern_length; ++i)
            {
                if(match_pattern(buffer + i, pattern, pattern_length))
                {
                    printf("Found pattern at address: %lx\n", region.start + i);
                }
            }
        }
        

        free(buffer);
    }

    printf("Total regions: %i\n", regions_count);
    free(regions);
    free(pattern);
    return EXIT_SUCCESS;
}
