// -*-c++-*-
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
//Axis.h

#ifndef AXIS_H
#define AXIS_H

#include "EntGeomSection.h"


class BND2d;

namespace XC {
class Spot;

//! @ingroup MATSCCEntGeom
//
//! @brief Base class for 1D entities in section definition.
class Axis: public EntGeomSection
  {
    //friend class CmbAxis;
  protected:
    size_t ndiv; //!< Number of divisions.

    friend class GeomSection;
    Axis(GeomSection *sr,const size_t &nd= 4);
    Axis(const std::string &name= "",GeomSection *m= nullptr,const size_t &nd= 4);
  public:
    ~Axis(void);
    //! @brief Returns the object dimension (0, 1, 2 or 3).
    inline virtual unsigned short int GetDimension(void) const
      { return 1; }

    virtual Spot *P1(void);
    virtual const Spot *P1(void) const;
    virtual Spot *P2(void);
    virtual const Spot *P2(void) const;
    //! @brief Returns the number of vertices.
    virtual size_t getNumberOfVertices(void) const= 0;

    virtual const Spot *getVertex(const size_t &i) const= 0;
    virtual Spot *getVertex(const size_t &i)= 0;
    virtual void SetVertice(const size_t &,Spot *)= 0;

    virtual void setNDiv(const size_t &nd)
      { ndiv= nd; }
    virtual size_t NDiv(void) const
      { return ndiv; }
    void update_topology(void);
 
    virtual BND2d Bnd(void) const= 0;


  };

std::set<const Axis *> getConnectedLines(const Spot &p);

} //end of XC namespace
#endif
