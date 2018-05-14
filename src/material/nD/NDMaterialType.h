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


#ifndef NDMaterialType_H
#define NDMaterialType_H

#include <string>

namespace XC {
  const std::string strType3D= "3D";
  const std::string strTypeAxiSymmetric= "AxiSymmetric";
  const std::string strTypeAxiSymmetric2D= "AxiSymmetric2D";
  const std::string strTypeBeamFiber= "BeamFiber";
  const std::string strTypeElasticIsotropic3D= "ElasticIsotropic3D";
  const std::string strTypeFDEP3D= "FDEP3D";
  const std::string strTypeFiniteDeformationEP3D= "FiniteDeformationEP3D";
  const std::string strTypeFluidSolidPorous= "FluidSolidPorous";
  const std::string strTypePlaneStress2D= "PlaneStress2D";
  const std::string strTypePlaneStress= "PlaneStress";
  const std::string strTypePlaneStrain2D= "PlaneStrain2D";
  const std::string strTypePlaneStrain= "PlaneStrain";
  const std::string strTypePlateFiber= "PlateFiber";
  const std::string strTypePressureIndependMultiYield= "PressureIndependMultiYield";
  const std::string strTypePressureIndependMultiYield02= "PressureIndependMultiiYield02";
  const std::string strTypeTemplate3Dep= "Template3Dep";
  const std::string strTypeThreeDimensional= "ThreeDimensional";
  const std::string strTypeThreeDimensionalFD= "ThreeDimensionalFD";

  const std::string strTypeError= "error";
} // end of XC namespace

#endif
