//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//print_trace.cc

#include "print_trace.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>

void print_trace(void)
  {
    void *array[10];
    char **strings;
     
    const size_t size= backtrace(array, 10);
    strings = backtrace_symbols(array, size);
     
    std::cerr << "Obtained " << size << " stack frames.\n";
     
    for(size_t i = 0; i < size; i++)
      std::cerr << strings[i] << std::endl;
     
    free (strings);
  }
#else
void print_trace(void)
  { std::cerr << "print_trace not implemented on this system." << std::endl; }
#endif
