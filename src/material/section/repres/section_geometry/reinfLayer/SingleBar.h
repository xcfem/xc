// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//SingleBar.h

#ifndef SingleBar_h 
#define SingleBar_h 

#include <material/section/repres/section_geometry/reinfLayer/ReinfLayer.h>
#include <utility/matrix/Vector.h>

namespace XC {
class ReinfBar;

//! @ingroup MATSCCReinforcement
//
//! @brief Single rebar (not included in a reinforcement layer).
class SingleBar: public ReinfLayer
  {
  private:
    Vector posit; //!< Position of the bar center.
  protected:


    friend class ListReinfLayer;
    friend class ReinfLayer;
    SingleBar(ListReinfLayer *,Material *mat);
    SingleBar(ListReinfLayer *,Material *mat, double  reinfBarArea, const Vector &position);
    SingleBar(const ReinfBar &bar);
    ReinfLayer *getCopy(void) const;
    
  public:
    // edition functions
    void setPosition(const Vector &);

    // inquiring functions
    const VectorReinfBar &getReinfBars(void) const;
  
    const Vector &getPosition(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;


    void Print(std::ostream &s, int flag =0) const;   
  };
} // end of XC namespace


#endif

