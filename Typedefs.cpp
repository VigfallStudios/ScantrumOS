#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, unsigned int n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strchr(const char* str, int ch) {
    while (*str) {
        if (*str == (char)ch)
            return (char*)str;
        str++;
    }
    return 0;
}

void* memcpy(void* dest, const void* src, unsigned long n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--)
        *d++ = *s++;
    return dest;
}

char* strcpy(char* dest, const char* src)
{
    char* ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

char* strncpy(char* dest, const char* src, uint64_t n)
{
    uint64_t i = 0;
    for (; i < n && src[i] != '\0'; ++i)
        dest[i] = src[i];
    for (; i < n; ++i)
        dest[i] = '\0';
    return dest;
}