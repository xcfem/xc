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
//RayleighDampingFactors.h

#ifndef RayleighDampingFactors_h
#define RayleighDampingFactors_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"

class TritrizPos3d;

namespace XC {

class Vector;
class Information;

//! \ingroup Elem
//
//! @brief Rayleigh damping factors
class RayleighDampingFactors: public EntCmd, public MovableObject
  {
    double alphaM;
    double betaK;
    double betaK0;
    double betaKc; 
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    RayleighDampingFactors(void);
    RayleighDampingFactors(const double &alphaM,const double &betaK,const double &betaK0,const double &betaKc);
    explicit RayleighDampingFactors(const Vector &);

    inline const double &getAlphaM(void) const
      { return alphaM; }
    inline const double &getBetaK(void) const
      { return betaK; }
    inline const double &getBetaK0(void) const
      { return betaK0; }
    inline const double &getBetaKc(void) const
      { return betaKc; }
    inline bool Nulos(void) const
      { return (alphaM == 0.0 && betaK == 0.0 && betaK0 == 0.0 && betaKc == 0.0); }
    inline bool KNulos(void) const
      { return (betaK == 0.0 && betaK0 == 0.0 && betaKc == 0.0); }
    int updateParameter(int parameterID, Information &info);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag= 0) const;

  };

std::ostream &operator<<(std::ostream &os,const RayleighDampingFactors &rF);

} // end of XC namespace

#endif

