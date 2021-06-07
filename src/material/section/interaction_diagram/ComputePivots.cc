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
//ComputePivots.cc

#include "ComputePivots.h"
#include "material/section/fiber_section/fiber/FiberContainer.h"
#include "utility/geom/d1/Segment3d.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "PivotsUltimateStrains.h"
#include "utility/matrix/Vector.h"
#include "material/uniaxial/UniaxialMaterial.h"

//! @brief Return a 3D point with coordinates (strain,f.y,f.z) where
//! f.y and f.z are the coordinates of the centroid of the fiber.
//! @param f: material fiber.
//! @param strain: strain at the fiber position.
inline Pos3d getPos3d(const XC::Fiber *f,const double &strain= 0.0)
  { return Pos3d(strain,f->getLocY(),f->getLocZ()); }

//! @brief Center for the local reference system
inline Pos3d getCenterOfMass(const XC::FiberPtrDeque &fs)
  { return Pos3d(0.0,fs.getCenterOfMassY(),fs.getCenterOfMassZ()); }

//! @brief Local reference system.
Ref3d3d getRef3d(const XC::FiberPtrDeque &fs, const double &theta)
  {
    const Pos3d g= getCenterOfMass(fs);
    return Ref3d3d(g,Vector3d(1,0,0),Vector3d(0,cos(theta),sin(theta)));
  }

//! @brief Constructor.
XC::ComputePivots::ComputePivots(const PivotsUltimateStrains &ap,const FiberContainer &fs,const FiberPtrDeque &fsC,const FiberPtrDeque &fsS,const double &theta)
  : Ref3d3d(getRef3d(fs, theta)), agot_pivots(ap), fibers(fs),CFibers(fsC),SFibers(fsS) {}

//! @brief Return a pointer to the fiber with the minimal y coordinate
//! from those of the SFibers (steel fibers) set.
const XC::Fiber *XC::ComputePivots::getFiberSMinY(void) const
  {
    const size_t i= SFibers.getFiberWithMinCoord(*this,2);
    return SFibers[i];
  }

//! @brief Return a pointer to the fiber with the minimal y coordinate
//! from those of the CFibers (concrete fibers) set.
const XC::Fiber *XC::ComputePivots::getFiberCMinY(void) const
  {
    const size_t i= CFibers.getFiberWithMinCoord(*this,2);
    return CFibers[i];
  }

//! @brief Return a pointer to the fiber with the maximum y coordinate
//! from those of the CFibers (concrete fibers) set.
const XC::Fiber *XC::ComputePivots::getFiberCMaxY(void) const
  {
    const size_t i= CFibers.getFiberWithMaxCoord(*this,2);
    return CFibers[i];
  }


//! @brief Returns the point with zero strain in concrete (XXX enhance explanation).
Pos3d XC::ComputePivots::getDPoint(void) const
  {
    Pos3d retval;
    const Fiber *t= getFiberCMinY();
    retval= getPos3d(t,0.0);
    return retval;
  }

//! @brief Most tensioned steel fiber
Pos3d XC::ComputePivots::calcPositionPivotA(void) const
  {
    Pos3d retval;
    if(!SFibers.empty()) //There are rebars.
      {
        const Fiber *t= getFiberSMinY();
        const Pos3d pos_t= getPos3d(t,agot_pivots.getUltimateStrainAPivot()); //Yield strain in A pivot.
        const double v_min_s= getLocalPosition(pos_t)(2);
        if(v_min_s<0) //Cell is in tension zone.
          retval= pos_t;
        else //Cell is in compression zone.
          retval= getDPoint();
      }
    else //No rebars.
      retval= getDPoint();
    return retval;
  }

//! @brief Most compressed concrete fiber.
Pos3d XC::ComputePivots::calcPositionPivotB(void) const
  {
    const Fiber *t= getFiberCMaxY();
    Pos3d retval= getPos3d(t,agot_pivots.getUltimateStrainBPivot());
    return retval;
  }

//! @brief Intersection of uniform strain (typically 2%) line and line BD 
Pos3d XC::ComputePivots::calcPositionPivotC(void) const
  {
    const Pos3d D= getDPoint();
    const Pos3d B= calcPositionPivotB();
    const Line3d s(D,B);
    GeomObj::list_Pos3d lp= s.getIntersection(1,agot_pivots.getUltimateStrainCPivot());
    assert(lp.size()>0); //List must not be empty.
    const Pos3d retval= *lp.begin();
    return retval;
  }
