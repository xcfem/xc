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
//pset.h
#ifndef PSET_H
#define PSET_H

#include "puntero_srt.h"
#include <set>

template <class N>
struct ptr_menor
 {
   bool operator()(const puntero_srt<N> &s1,const puntero_srt<N> &s2) const
   {
     return s1<s2;
   }
 };

template <class N>
class pset : public std::set<puntero_srt<N>,ptr_menor<N> >
  {
  public:
    typedef puntero_srt<N> ps_N;
    typedef std::set<puntero_srt<N>,ptr_menor<N> > set_psN;
    typedef typename pset<N>::const_iterator c_iterator;
    pset(void): set_psN() {}
    pset(const pset<N> &otro): set_psN(otro) {}
    pset<N> &operator =(const pset<N> &otro)
      {
        set_psN::operator=(otro);
        return *this;
      }
    friend bool operator==(const pset<N> &pv1,const pset<N> &pv2)
      { return (( const set_psN &) pv1 == (const set_psN &)pv2); }
    //friend std::ostream &operator<<(std::ostream &os,const pset<N> &pl);
  };

template <class N>
std::ostream &operator<<(std::ostream &os,const pset<N> &pl)
  {
     typename pset<N>::c_iterator i;
     size_t j= 0,sz = pl.size();
     for (i= pl.begin();i != pl.end();i++)
       {
         os << (*(*i));
         j++;
         if (j < sz) os << ',';
       }
     return os;
  }

#endif
