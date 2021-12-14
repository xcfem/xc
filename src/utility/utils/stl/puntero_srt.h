// -*-c++-*-
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
//puntero_srt.h
#ifndef PUNTERO_SRT_H
#define PUNTERO_SRT_H

#include "puntero.h"
#include <iostream>

//Puntero con operador menor por defecto.
template <class N>
class puntero_srt : public puntero<N>
  {
  public:
    typedef puntero<N> pN;
    puntero_srt(void): pN() {}
    puntero_srt(N *const p): pN(p) {}
    puntero_srt(const puntero_srt<N> &otro): pN(otro) {}
    puntero_srt<N> &operator =(N *const p)
      {
        pN::operator=(p);
        return *this;
      }
    puntero_srt<N> &operator =(const puntero_srt<N> &otro)
      { 
        pN::operator=(otro);
        return *this;
      }
    ~puntero_srt(void) {}
    bool menor_que(const puntero_srt<N> &p2) const
      {
        if(!this->nulo())
          if(!p2.nulo())
            return (*(this->get_const_ptr()) < *p2.get_const_ptr());
          else
            return false;
        else
          if(!p2.nulo())
            return true;
          else
            return false;
      }
    inline bool operator<(const puntero_srt<N> &p2) const
      { return this->menor_que(p2); }
    inline friend std::ostream &operator <<(std::ostream stream, const puntero_srt<N> &p)
      {
        if (p.ptr != 0)
          stream << *p.ptr;
        else
          stream << '#';
        return stream;
      }
  };

template <class N>
inline bool operator<(const puntero_srt<N> &p1,const puntero_srt<N> &p2)
  { return p1.menor_que(p2); }

#endif

