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
//poli_puntero.h
#ifndef POLI_PUNTERO_H
#define POLI_PUNTERO_H

#include "utility/utils/stl/puntero.h"

template <class N>
class poli_puntero: public puntero<N>
  {
  private:
    typedef puntero<N> pN;

    operator N*(void)
      { return pN::get_ptr(); }
    N *get_ptr(void) const
      { return pN::get_ptr(); }
    inline void libera(void)
      {
        if(!pN::nulo()) delete get_ptr();
        pN::put(NULL);
      }
    void copia(N *const p)
      {
        libera();
        if(p!=NULL) pN::put(p->Copia());
      }
    void copia(const poli_puntero<N> &otro)
      { copia(otro.get_ptr()); }
    void copia(const N &n)
      {
        libera();
        copia(n.Copia());
      }
  public:

    poli_puntero(void): pN() {}
/*     poli_puntero(const N &n): pN() { copia(n); } */
/*     poli_puntero(N *const p): pN() { copia(p); } */
    poli_puntero(const poli_puntero<N> &otro)
      : pN() { copia(otro); }
    poli_puntero<N> &operator =(N *const p) 
      {
        copia(p);
        return *this;
      }
/*     poli_puntero<N> &operator =(const N &n)  */
/*       { */
/*         copia(n); */
/*         return *this; */
/*       } */
/*     poli_puntero<N> &operator =(const pN &otro) */
/*       { */
/*         copia(otro); */
/*         return *this; */
/*       } */
    poli_puntero<N> &operator =(const poli_puntero<N> &otro)
      { 
        copia(otro);
        return *this;
      }
    inline void put(N *const p)
      { copia(p); }
    inline void put(const N &n)
      { copia(n); }
    ~poli_puntero() { libera(); }
  };

template <class N>
class poli_var: public poli_puntero<N>
  {
  };

#endif





