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
//TripletMap.h

#ifndef TRIPLETMAP_H
#define TRIPLETMAP_H

#include <map>
#include <iostream>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"

typedef boost::tuple<size_t,size_t,size_t> Triplete;

inline void printTriplete(std::ostream &os,const Triplete &t)
  {
    os << "[" << t.get<0>() << ","
       << t.get<1>() << ","
       << t.get<2>() << "]";
  }

//! @brief Mapa tipo «tensor disperso».
template<class T>
class TripletMap:public std::map<Triplete,T>
  {
  public:
    typedef std::map<Triplete,T> map_Ts;
    typedef typename map_Ts::const_iterator const_iterator;
    typedef typename map_Ts::iterator iterator;
    TripletMap(void): map_Ts() {}
    void PrintMember(std::ostream &os,const_iterator &) const;
    void Print(std::ostream &) const;
  };

template<class T>
void TripletMap<T>::PrintMember(std::ostream &os,const_iterator &i) const
  {
    printTriplete(os,(*i).first);
    os << "; " << (*i).second;
  }

template<class T>
void TripletMap<T>::Print(std::ostream &os) const
  {
    for(const_iterator i=this->begin();i!=this->end();i++)
      {
        PrintMember(os,i);
        os << std::endl;
      }
  }
template<class T>
std::ostream &operator<<(std::ostream &os,const TripletMap<T> &tm)
  {
    tm.Print(os);
    return os;
  }
    

#endif
