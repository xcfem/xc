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

#ifndef UniaxialStateVars_h
#define UniaxialStateVars_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief UniaxialStateVars almacena los valores de la
//! deformación, la tensión y la rigidez del material.
class UniaxialStateVars: public MovableObject
  {
  private:
    double strain;
    double stress;
    double tangent;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    UniaxialStateVars(void);

    inline const double &getStrain(void) const
      { return strain; }
    inline const double &getStress(void) const
      { return stress; }
    inline const double &getTangent(void) const
      { return tangent; }
    inline double &Strain(void)
      { return strain; }
    inline double &Stress(void)
      { return stress; }
    inline double &Tangent(void)
      { return tangent; }

    int revertToStart(const double &);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    
  };
} // end of XC namespace


#endif

