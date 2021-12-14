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
//proximity.h

#ifndef PROXIMITY_H
#define PROXIMITY_H

template<class InputIter, class Pt>
const InputIter nearest(InputIter first, InputIter last,const Pt &p)
  {
    InputIter i=first;
    InputIter retval= i;
    double dist_min= dist(*retval,p);
    for(;i!=last; i++)
      {
        const double d= dist(*i,p);
        if(d<dist_min)
          {
            dist_min= d;
            retval= i;
          }
      }
    return retval;
  }


#endif
