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
//BodyForces.h

#ifndef BodyForces_h
#define BodyForces_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! \ingroup FEMisc
//
//! @brief Body forces over an element.
class BodyForces: public MovableObject
  {
  protected:
    Vector forces; //!< Body force components.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    BodyForces(const size_t &sz);

    inline size_t size(void) const
      { return forces.Size(); } 

    inline double &operator[](const size_t &i)
      { return forces[i]; }
    inline const double &operator[](const size_t &i) const
      { return forces[i]; }
    inline double &operator()(const size_t &i)
      { return forces(i); }
    inline const double &operator()(const size_t &i) const
      { return forces(i); }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s) const;
  };

inline std::ostream &operator<<(std::ostream &os,const BodyForces &bf)
  {
    bf.Print(os);
    return os;
  }
} //end of XC namespace

#endif
