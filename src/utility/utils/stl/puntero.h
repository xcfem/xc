// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//puntero.h
#ifndef PUNTERO_H
#define PUNTERO_H

#include <cstdlib>
#include <cassert>
#include <functional>

template <class N>
class puntero
  {
  private:
    N *ptr;
  public:
    puntero(void): ptr(NULL) {}
    puntero(N *const p): ptr(p) {}
    puntero(const puntero<N> &otro): ptr(otro.ptr) {}
    puntero<N> &operator =(N *const p)
       { 
        ptr= p;
        return *this;
      }
    puntero<N> &operator =(const puntero<N> &otro)
      { 
        ptr= otro.ptr;
        return *this;
      }
    ~puntero(void) { ptr= NULL; }
    inline bool nulo(void) const
      { return (ptr==NULL); }
    N &operator *(void) const
      {
        assert(!nulo());
        return *ptr;
      }
    inline operator const N *(void) const
      { return ptr; }
    inline operator N *const(void) const
      { return ptr; }
    inline operator N*(void)
      { return ptr; }
    inline operator const bool(void) const
      { return ptr; }
    inline void put(N *const p)
      { ptr= p; }
    inline N* operator->(void)
      { return ptr; }
    inline const N* operator->(void) const
      { return ptr; }
    inline N *get_ptr(void) const
      { return ptr; }
    inline N *const get_const_ptr(void) const
      { return ptr; }  
    inline bool operator==(const puntero<N> &p2) const
      { return (ptr == p2.ptr); }
    inline bool operator==(const N *const p2) const
      { return (ptr == p2); }
    bool operator!=(const N *const p2) const
      { return (ptr != p2); }      
    friend bool operator ==(const N *const p1,const puntero<N> &p2)
      { return (p1 == p2.ptr); }
    friend bool operator !=(const N *const p1,const puntero<N> &p2)
      { return (p1 == p2.ptr); }
  };

#endif
