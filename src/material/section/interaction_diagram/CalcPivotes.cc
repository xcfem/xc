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
//CalcPivotes.cc

#include "CalcPivotes.h"
#include "material/section/fiber_section/fiber/StoFibras.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "DefAgotPivotes.h"
#include "utility/matrix/Vector.h"
#include "material/uniaxial/UniaxialMaterial.h"

inline Pos3d getPos3d(const XC::Fiber *t,const double &strain= 0.0)
  { return Pos3d(strain,t->getLocY(),t->getLocZ()); }

//! @brief Center for the local reference system
inline Pos3d getCDG(const XC::DqFibras &fs)
  { return Pos3d(0.0,fs.getYCdg(),fs.getZCdg()); }

//! @brief Local reference system.
Ref3d3d getRef3d(const XC::DqFibras &fs, const double &theta)
  {
    const Pos3d g= getCDG(fs);
    return Ref3d3d(g,Vector3d(1,0,0),Vector3d(0,cos(theta),sin(theta)));
  }

//! @brief Constructor.
XC::CalcPivotes::CalcPivotes(const DefAgotPivotes &ap,const StoFibras &fs,const DqFibras &fsC,const DqFibras &fsS,const double &theta)
  : Ref3d3d(getRef3d(fs, theta)), agot_pivotes(ap), fibras(fs),fibrasC(fsC),fibrasS(fsS) {}

const XC::Fiber *XC::CalcPivotes::getFiberSMinY(void) const
  {
    const size_t i= fibrasS.getFibraCooMin(*this,2);
    return fibrasS[i];
  }

const XC::Fiber *XC::CalcPivotes::getFiberCMinY(void) const
  {
    const size_t i= fibrasC.getFibraCooMin(*this,2);
    return fibrasC[i];
  }

const XC::Fiber *XC::CalcPivotes::getFiberCMaxY(void) const
  {
    const size_t i= fibrasC.getFibraCooMax(*this,2);
    return fibrasC[i];
  }


//! @brief Returns the points with zero strain in concrete (XXX enhance explanation).
Pos3d XC::CalcPivotes::GetPuntoD(void) const
  {
    Pos3d retval;
    const Fiber *t= getFiberCMinY();
    retval= getPos3d(t,0.0);
    return retval;
  }

//! @brief Most tensioned steel fiber
Pos3d XC::CalcPivotes::calcPositionPivotA(void) const
  {
    Pos3d retval;
    if(!fibrasS.empty()) //Hay armadura.
      {
        const Fiber *t= getFiberSMinY();
        const Pos3d pos_t= getPos3d(t,agot_pivotes.getDefAgotPivoteA()); //Yield strain in A pivot.
        const double v_min_s= GetPosLocal(pos_t)(2);
        if(v_min_s<0) //Cell is in tension zone.
          retval= pos_t;
        else //Cell is in compression zone.
          retval= GetPuntoD();
      }
    else //no hay armadura.
      retval= GetPuntoD();
    return retval;
  }

//! @brief Most compressed concrete fiber.
Pos3d XC::CalcPivotes::calcPositionPivotB(void) const
  {
    const Fiber *t= getFiberCMaxY();
    Pos3d retval= getPos3d(t,agot_pivotes.getDefAgotPivoteB());
    return retval;
  }

//! @brief Intersection of uniform strain (typically 2%) line and line BD 
Pos3d XC::CalcPivotes::calcPositionPivotC(void) const
  {
    const Pos3d D= GetPuntoD();
    const Pos3d B= calcPositionPivotB();
    const Recta3d s(D,B);
    GeomObj::list_Pos3d lp= s.Interseccion(1,agot_pivotes.getDefAgotPivoteC());
    assert(lp.size()>0); //List must not be empty.
    const Pos3d retval= *lp.begin();
    return retval;
  }
