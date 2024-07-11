#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>

#include "memory.h"

MemoryRegion* get_memory_regions(pid_t pid, int* count)
{
    char maps_path[256];
    sprintf(maps_path, "/proc/%d/maps", pid);
    FILE* maps_file = fopen(maps_path, "r");
    if (!maps_file) {
        perror("Failed to open maps file");
        return NULL;
    }

    MemoryRegion* regions = NULL;
    int capacity = 0;
    *count = 0;

    char line[256];
    while (fgets(line, sizeof(line), maps_file))
    {
        if (*count >= capacity)
        {
            capacity = capacity == 0 ? 10 : capacity * 2;
            regions = (MemoryRegion*)realloc(regions, capacity * sizeof(MemoryRegion));
            if (!regions) {
                perror("Failed to allocate memory for regions");
                fclose(maps_file);
                return NULL;
            }
        }

        uintptr_t start, end;
        if (sscanf(line, "%lx-%lx", &start, &end) != 2) {
            continue; // Skip lines that don't match the expected format
        }
        regions[*count].start = start;
        regions[*count].end = end;
        (*count)++;
    }

    fclose(maps_file);
    return regions;
}



bool read_process_memory(pid_t pid, uintptr_t address, void* buffer, size_t size)
{
    struct iovec local_iov = { buffer, size };
    struct iovec remote_iov = { (void*)address, size };
    ssize_t nread = process_vm_readv(pid, &local_iov, 1, &remote_iov, 1, 0);

    return nread == size;
}

bool match_pattern(const uint8_t* data, const int* pattern, size_t pattern_size)
{
    for (size_t i = 0; i < pattern_size; ++i)
    {
        if (pattern[i] != -1 && data[i] != pattern[i])
            return false;
    }
    return true;
}

int* convert_signature(const char* signature, size_t* pattern_size)
{
    char* signature_copy = strdup(signature);
    if (!signature_copy) {
        return NULL;
    }

    char* token = strtok(signature_copy, " ");
    size_t size = 0;
    size_t capacity = 10;
    int* pattern = (int*)malloc(capacity * sizeof(int));
    if (!pattern) {
        free(signature_copy);
        return NULL;
    }

    while (token != NULL)
    {
        if (size >= capacity)
        {
            capacity *= 2;
            int* temp = (int*)realloc(pattern, capacity * sizeof(int));
            if (!temp) {
                free(pattern);
                free(signature_copy);
                return NULL;
            }
            pattern = temp;
        }

        if (strcmp(token, "??") == 0)
            pattern[size] = -1;
        else
            pattern[size] = strtol(token, NULL, 16);
        size++;
        token = strtok(NULL, " ");
    }

    free(signature_copy);
    *pattern_size = size;
    return pattern;
}
