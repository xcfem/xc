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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              19AUg2003
//# UPDATE HISTORY:    Sept 2003
//#                    May28, 2004
//#
//===============================================================================

#include <material/nD/FiniteDeformation/FiniteDeformationMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/straint.h>
#include <cmath>
#include <utility/matrix/ID.h>

using namespace XC;
XC::stresstensor errstresstensor;
XC::straintensor errstraintensor;

//-----------------------------------------------------------------------------------------------------------------------------------------------
XC::FiniteDeformationMaterial::FiniteDeformationMaterial(int tag,
                                                       int classTag)
:XC::NDMaterial(tag, classTag)
  {}



//Zhao (zcheng@ucdavis.edu) 
// added Sept 22 2003 for Large Deformation, F is the Deformation Grandient
int XC::FiniteDeformationMaterial::setTrialF(const straintensor &f)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    return -1;
  }

int XC::FiniteDeformationMaterial::setTrialFIncr(const XC::straintensor &df)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    return -1;
  }

int XC::FiniteDeformationMaterial::setTrialC(const XC::straintensor &c)
  {
     std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
     return -1;
  }

int XC::FiniteDeformationMaterial::setTrialCIncr(const XC::straintensor &c)
  {
     std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
     return -1;
  }

const XC::stresstensor XC::FiniteDeformationMaterial::getPK1StressTensor(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    return errstresstensor;    
  }

const XC::stresstensor XC::FiniteDeformationMaterial::getCauchyStressTensor(void) const
  {
     std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
     return errstresstensor;    
  }

const XC::straintensor &XC::FiniteDeformationMaterial::getF(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    return errstraintensor;    
  }

const XC::straintensor &XC::FiniteDeformationMaterial::getC(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    return errstraintensor;    
  }

const XC::straintensor XC::FiniteDeformationMaterial::getFp(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    return errstraintensor;    
  }
// Only For Large Deformation, END////////////////////////////
