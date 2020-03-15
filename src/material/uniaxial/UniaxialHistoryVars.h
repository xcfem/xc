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

#ifndef UniaxialHistoryVars_h
#define UniaxialHistoryVars_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief UniaxialHistoryVars stores values
//! for strain and stiffness.
class UniaxialHistoryVars: public MovableObject
  {
  private:
    double minStrain; //!< Smallest previous strain (compression)
    double unloadSlope; //!< Unloading (reloading) slope from CminStrain
    double endStrain; //!< Strain at the end of unloading from CminStrain

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    UniaxialHistoryVars(void);

    inline const double &getMinStrain(void) const
      { return minStrain; }
    inline const double &getUnloadSlope(void) const
      { return unloadSlope; }
    inline const double &getEndStrain(void) const
      { return endStrain; }
    inline double &MinStrain(void)
      { return minStrain; }
    inline double &UnloadSlope(void)
      { return unloadSlope; }
    inline double &EndStrain(void)
      { return endStrain; }

    int revertToStart(const double &);
    void zero(void);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;
    
  };
} // end of XC namespace


#endif

