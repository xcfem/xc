//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//DqPtrsNmb.h
//deque de pointers (se emplear en la clase Set).


#ifndef DQPTRSNMB_H
#define DQPTRSNMB_H

#include "DqPtrs.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {

template <class T>
class DqPtrsNmb: public DqPtrs<T>
  {
  public:
    typedef DqPtrs<T> dq_ptr;
    typedef typename dq_ptr::const_iterator const_iterator;
    typedef typename dq_ptr::iterator iterator;

    DqPtrsNmb(EntCmd *owr= nullptr)
      : DqPtrs<T>(owr) {}
    DqPtrsNmb(const DqPtrs<T> &otro)
      : DqPtrs<T>(otro) {}
    explicit DqPtrsNmb(const std::deque<T *> &ts)
      : DqPtrs<T>(ts) {}
    explicit DqPtrsNmb(const std::set<const T *> &ts)
      : DqPtrs<T>(ts) {}

    T *BuscaNmb(const std::string &nmb);
    T *getNearest(const Pos3d &p);
    const T *getNearest(const Pos3d &p) const;

  };

//! @brief Returns a pointer to the objeto identified by the name.
template <class T>
T *DqPtrsNmb<T>::BuscaNmb(const std::string &nmb)
  {
    for(const_iterator i= this->begin();i!=this->end();i++)
      if((*i)->getName()==nmb) return *i;
    return nullptr;
  }

//! @brief Returns the object closest to the position being passed as parameter.
template <class T>
T *DqPtrsNmb<T>::getNearest(const Pos3d &p)
  {
    T *retval= nullptr;
    if(!this->empty())
      {
        const_iterator i= this->begin();
        double d2= (*i)->DistanciaA2(p);
        retval= *i; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i)->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= *i;
              }
          }
      }
    return retval;
  }

//! @brief Returns the object closest to the position being passed as parameter.
template <class T>
const T *DqPtrsNmb<T>::getNearest(const Pos3d &p) const
  {
    const T *retval= nullptr;
    if(!this->empty())
      {
        const_iterator i= this->begin();
        double d2= (*i)->DistanciaA2(p);
        retval= *i; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i)->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= *i;
              }
          }
      }
    return retval;
  }

} //end of XC namespace

#endif

