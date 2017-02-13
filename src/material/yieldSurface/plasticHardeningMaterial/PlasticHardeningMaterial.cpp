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
//
//
//////////////////////////////////////////////////////////////////////

#include "PlasticHardeningMaterial.h"
#include <cmath>
#include "utility/matrix/Vector.h"


#define evolDebug 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::PlasticHardeningMaterial::PlasticHardeningMaterial(int tag, int classTag)
:Material(tag, classTag), val_hist(0), val_trial(0),
 residual(1.0), sFactor(1.0) {}

int XC::PlasticHardeningMaterial::setTrialValue(double xVal, double factor)
  {
    sFactor = factor;
    val_trial = xVal;
    if(val_trial < 0) val_trial = 0;
    return 0;	
  }

double XC::PlasticHardeningMaterial::getTrialValue(void)
  { return val_trial; }

int XC::PlasticHardeningMaterial::setTrialIncrValue(double dxVal)
  {
    sFactor = 1.0;
    val_trial = val_hist + dxVal;
    if(val_trial < 0) val_trial = 0;
    return 0;	
  }


void XC::PlasticHardeningMaterial::setResidual(double res)
  { residual = res; }

int XC::PlasticHardeningMaterial::commitState(void)
  {
    val_hist= val_trial;
    sFactor = 1.0;
    //cerr << "------ Ep value = " <<  val_hist << std::endl;
    return 0;	
  }

int XC::PlasticHardeningMaterial::revertToLastCommit(void)
  {
    val_trial = val_hist;
    return 0;	
  }

int XC::PlasticHardeningMaterial::revertToStart (void)
  {
    val_trial = 0;
    val_hist = 0;
    return 0;	
  }

//! @brief Returns the valor de la generalized stress.
const XC::Vector &XC::PlasticHardeningMaterial::getGeneralizedStress(void) const
  {
    static Vector retval;
    std::cerr << "PlasticHardeningMaterial::getGeneralizedStress no implementada." << std::endl;
    return retval;
  }

//! @brief Returns the valor de la deformación generalizada.
const XC::Vector &XC::PlasticHardeningMaterial::getGeneralizedStrain(void) const
  {
    static Vector retval;
    std::cerr << "PlasticHardeningMaterial::getGeneralizedStrain no implementada." << std::endl;
    return retval;
  }

//! @brief Asigna a la initial deformation el valor being passed as parameter.
void XC::PlasticHardeningMaterial::setInitialGeneralizedStrain(const Vector &)
  {
    std::cerr << "PlasticHardeningMaterial::setInitialGeneralizedStrain no implementada." << std::endl;
  }

//! @brief Añade a la initial deformation el valor being passed as parameter.
const XC::Vector &XC::PlasticHardeningMaterial::getInitialGeneralizedStrain(void) const
  {
    static Vector retval;
    std::cerr << "PlasticHardeningMaterial::getInitialGeneralizedStrain no implementada." << std::endl;
    return retval;
  }


XC::Response *XC::PlasticHardeningMaterial::setResponse(const std::vector<std::string> &argv, Information &matInformation)
  { return 0; }

int XC::PlasticHardeningMaterial::getResponse(int responseID, Information &matInformation)
  { return -1; }

void XC::PlasticHardeningMaterial::Print(std::ostream &s, int flag)
  {
     s << "+Plastic XC::Material: " << getTag() << std::endl;
     s << "|  sFactor =  " << sFactor << std::endl;
     s << "|  val_hist = " << val_hist << ", val_trial = " << val_trial << std::endl;
     return;
  }


