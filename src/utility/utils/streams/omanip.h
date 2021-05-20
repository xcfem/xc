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
//omanip.h

#ifndef OMANIP_H
#define OMANIP_H

#include<ostream>
#include<iomanip>


template <class TP>
class omanip
 {
 public:
   std::ostream& (*_f)(std::ostream&, TP);
   TP _a;
   omanip(std::ostream& (*f)(std::ostream&, TP), TP a) : _f(f), _a(a) {}
   //friend std::ostream& operator<< <>(std::ostream& o, const omanip<TP>& m);
 };

template <class TP>
inline std::ostream& operator<<(std::ostream& o, const omanip<TP>& m)
  { return (*m._f)(o, m._a); }

inline std::ostream &endl_msdos(std::ostream &os)
  { return os << char(13) << char(10); }

#endif
