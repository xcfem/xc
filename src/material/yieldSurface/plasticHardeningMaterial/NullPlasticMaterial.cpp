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

#include "material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h"
#define CLASSTAG_nullptr_PLASTIC_MAT -1

XC::NullPlasticMaterial::NullPlasticMaterial()
: PlasticHardeningMaterial(-1, CLASSTAG_nullptr_PLASTIC_MAT)
{
}

XC::NullPlasticMaterial::NullPlasticMaterial(int tag)
: PlasticHardeningMaterial(tag, CLASSTAG_nullptr_PLASTIC_MAT)
{
}


double XC::NullPlasticMaterial::getTrialPlasticStiffness()
  { return 0; }

 XC::PlasticHardeningMaterial * XC::NullPlasticMaterial::getCopy()
{
     NullPlasticMaterial *nullPM = new  NullPlasticMaterial(this->getTag());
     return nullPM;
}
