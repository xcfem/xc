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
//poli_deque.h
#ifndef POLI_DQUEUE_H
#define POLI_DQUEUE_H

#include "poli_puntero.h"
#include <deque>

template <class N>
class poli_deque : public std::deque< poli_puntero<N> >
  {
  public:
    typedef std::deque< poli_puntero<N> > deque_pN;
    typedef typename deque_pN::size_type sz_type;
    typedef poli_puntero<N> pN;

    poli_deque(void): deque_pN() {}
    poli_deque(sz_type n,const pN &value = pN())
      :deque_pN(n,value) {}
    poli_deque(const poli_deque<N> &otro): deque_pN(otro) {}
    poli_deque<N> &operator =(const poli_deque<N> &otro)
      {
        deque_pN::operator=(otro);
        return *this;
      }
    void push_back(N *const x)
      { 
        pN p;
        p.put(x);
        deque_pN::push_back(p);
       }
    void push_back(const N &n)
      {
        pN p;
        p.put(n);
        deque_pN::push_back(p);
      }
    friend bool operator==(const poli_deque<N> &pv1,const poli_deque<N> &pv2)
      { return ((const deque_pN &) pv1 == (const deque_pN &)pv2);}
  };

template <class N>
std::ostream &operator << (std::ostream &stream,const poli_deque<N> &pv)
  {
    typename poli_deque<N>::const_iterator i;
    for (i= pv.begin();i != pv.end();i++)
      {
       	stream << *i;
	      if (i < (pv.end()-1)) stream << ',';
      }
    return stream;
  }

#endif

