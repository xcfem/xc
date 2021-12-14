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
//TablaSegnales.h

#ifndef TABLASEGNALES_H
#define TABLASEGNALES_H

#include <map>
#include <string>


template<class T>
class TablaSegnales : public std::map<std::string,T,std::less<std::string> >
  {
  public:
    typedef std::map<std::string,T,std::less<std::string> > tabla_segnales;
    typedef typename tabla_segnales::iterator iterator;
    typedef typename tabla_segnales::const_iterator const_iterator;

    TablaSegnales(void) : tabla_segnales() {}
    inline T *CalcDir(const std::string &nmb)
      {
        iterator i= tabla_segnales::find(nmb);
        T *retval= (i != this->end()) ? &((*i).second) : NULL;
        return retval;
      }
    inline T *Nueva(const T &t)
      { return &((*this)[t.getName()]= t); }
  };
#endif
