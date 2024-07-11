#ifndef MEMORY_H
#define MEMORY_H

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uintptr_t start;
    uintptr_t end;
} MemoryRegion;

MemoryRegion* get_memory_regions(pid_t pid, int* count);
bool read_process_memory(pid_t pid, uintptr_t address, void* buffer, size_t size);
bool match_pattern(const uint8_t* data, const int* pattern, size_t pattern_size);
int* convert_signature(const char* signature, size_t* pattern_size);

ssize_t process_vm_readv(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt,
                         const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags);

#endif
