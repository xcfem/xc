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

#ifndef OgdenWEnergyBase_H
#define OgdenWEnergyBase_H

#include "WEnergy.h"

namespace XC {
//! @ingroup FDNDMat
//
//! @brief Base class for large deformation hyperelasticit Ogden material ??.
class OgdenWEnergyBase: public WEnergy
  {
  private:
    int N_Ogden;
    double *cr_Ogden;
    double *mur_Ogden;
  public:
    OgdenWEnergyBase(int , double * , double * );
    OgdenWEnergyBase(void);

    const double  wE(const double &, const Vector &);
    const Vector  disowOdlambda(const Vector & );
    const Vector  d2isowOdlambda2(const Vector & );
  };
} // end of XC namespace

#endif

