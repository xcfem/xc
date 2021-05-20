#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <ctime>
#include <malloc.h>

//! @brief Devuelve la memoria reservada
unsigned long getMemoryUsed(void)
  {
    // Per delorie.com:
    // Example:
    // struct mallinfo info = mallinfo();
    // printf("Memory in use: %d bytes\n", info.usmblks + info.uordblks);
    // printf("Total heap size: %d bytes\n", info.arena);
    struct mallinfo meminfo;
    meminfo= mallinfo();
    return meminfo.usmblks + meminfo.uordblks;
  }
