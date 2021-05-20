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
//pdeque.h
#ifndef PDQUEUE_H
#define PDQUEUE_H

#include "puntero.h"
#include <deque>
#include <ostream>

template <class N>
class pdeque : public std::deque< puntero<N> >
  {
  public:
    typedef std::deque< puntero<N> > deque_pN;
    typedef typename deque_pN::size_type sz_type;
    typedef puntero<N> pN;

    pdeque(void): deque_pN() {}
    pdeque(sz_type n,const pN &value = pN())
      :deque_pN(n,value) {}
    pdeque(const pdeque<N> &otro): deque_pN(otro) {}
    pdeque<N> &operator =(const pdeque<N> &otro)
      {
        deque_pN::operator=(otro);
        return *this;
      }
    void push_back(N *const x)
      { deque_pN::push_back(pN(x)); }
    friend bool operator==(const pdeque<N> &pv1,const pdeque<N> &pv2)
      { return ((const deque_pN &) pv1 == (const deque_pN &)pv2);}
  };

template <class N>
std::ostream &operator<<(std::ostream &os,const pdeque<N> &pv)
  {
    if(pv.empty())
      return os;
    typename pdeque<N>::const_iterator i= pv.begin();
    os << *i; i++;
    for(;i!=pv.end();i++)
      os << ',' << *i;
    return os;
  }

#endif

