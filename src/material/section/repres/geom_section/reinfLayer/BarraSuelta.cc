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
//BarraSuelta.cpp

#include <cmath>
#include <utility/matrix/Matrix.h>

#include <material/section/repres/geom_section/reinfLayer/BarraSuelta.h>
#include <material/section/repres/geom_section/reinfBar/ReinfBar.h>

#include "xc_basic/src/texto/cadena_carac.h"


XC::BarraSuelta::BarraSuelta(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat,0,0.0,0.0),
    posit(2) {}

XC::BarraSuelta::BarraSuelta(ListReinfLayer *owr,Material *mat, double reinfBarArea,const Vector &position)
  : ReinfLayer(owr,mat,1,0.0,reinfBarArea),posit(position) {}

XC::BarraSuelta::BarraSuelta(const ReinfBar &bar)
  : ReinfLayer(nullptr,bar.getMaterialPtr(),1,bar.getDiameter(),bar.getArea()),posit(getPosition()) 
  { set_owner(const_cast<EntCmd *>(bar.Owner())); }

void XC::BarraSuelta::setPosition(const Vector &Position)
  { posit= Position; }

double XC::BarraSuelta::getMaxY(void) const
  { return posit(0); }
double XC::BarraSuelta::getMaxZ(void) const
  { return posit(1); }
double XC::BarraSuelta::getMinY(void) const
  { return posit(0); }
double XC::BarraSuelta::getMinZ(void) const
  { return posit(1); }

//! @brief Returns a array de barras de refuerzo.
const XC::VectorReinfBar &XC::BarraSuelta::getReinfBars(void) const
  {
    reinfBars.resize(1);
    reinfBars.put(1,ReinfBar(getReinfBarArea(),getMaterialPtr(),posit));
    return reinfBars;         
  }

const XC::Vector &XC::BarraSuelta::getPosition(void) const
  { return posit; }

XC::ReinfLayer *XC::BarraSuelta::getCopy (void) const
  { return new BarraSuelta(*this); }

void XC::BarraSuelta::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Suelta";
     ReinfLayer::Print(s,flag);
     s << "\n Position: " << posit;
  }

