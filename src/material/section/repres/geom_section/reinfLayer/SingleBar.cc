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
//SingleBar.cpp

#include <cmath>
#include <utility/matrix/Matrix.h>

#include <material/section/repres/geom_section/reinfLayer/SingleBar.h>
#include <material/section/repres/geom_section/reinfBar/ReinfBar.h>

#include "xc_basic/src/text/text_string.h"


//! @brief Constructor.
XC::SingleBar::SingleBar(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat,0,0.0,0.0),
    posit(2) {}

//! @brief Constructor.
XC::SingleBar::SingleBar(ListReinfLayer *owr,Material *mat, double reinfBarArea,const Vector &position)
  : ReinfLayer(owr,mat,1,0.0,reinfBarArea),posit(position) {}

//! @brief Constructor.
XC::SingleBar::SingleBar(const ReinfBar &bar)
  : ReinfLayer(nullptr,bar.getMaterialPtr(),1,bar.getDiameter(),bar.getArea()),posit(getPosition()) 
  { set_owner(const_cast<CommandEntity *>(bar.Owner())); }

//! @brief Sets the position of the bar.
void XC::SingleBar::setPosition(const Vector &Position)
  { posit= Position; }

//! @brief Returns the max y coordinate.
double XC::SingleBar::getMaxY(void) const
  { return posit(0); }
//! @brief Returns the max z coordinate.
double XC::SingleBar::getMaxZ(void) const
  { return posit(1); }
//! @brief Returns the min y coordinate.
double XC::SingleBar::getMinY(void) const
  { return posit(0); }
//! @brief Returns the min z coordinate.
double XC::SingleBar::getMinZ(void) const
  { return posit(1); }

//! @brief Returns a rebars array.
const XC::VectorReinfBar &XC::SingleBar::getReinfBars(void) const
  {
    reinfBars.resize(1);
    reinfBars.put(1,ReinfBar(getReinfBarArea(),getMaterialPtr(),posit));
    return reinfBars;         
  }

//! @brief Returns the position of the bar.
const XC::Vector &XC::SingleBar::getPosition(void) const
  { return posit; }

//! @brief Virtual constructor.
XC::ReinfLayer *XC::SingleBar::getCopy (void) const
  { return new SingleBar(*this); }

//! @brif Displays information about the bar.
void XC::SingleBar::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Single";
     ReinfLayer::Print(s,flag);
     s << "\n Position: " << posit;
  }

