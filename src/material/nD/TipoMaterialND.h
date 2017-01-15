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


#ifndef TipoMaterialND_H
#define TipoMaterialND_H

#include <string>

namespace XC {
  const std::string strTipo3D= "3D";
  const std::string strTipoAxiSymmetric= "AxiSymmetric";
  const std::string strTipoAxiSymmetric2D= "AxiSymmetric2D";
  const std::string strTipoBeamFiber= "BeamFiber";
  const std::string strTipoElasticIsotropic3D= "ElasticIsotropic3D";
  const std::string strTipoFDEP3D= "FDEP3D";
  const std::string strTipoFiniteDeformationEP3D= "FiniteDeformationEP3D";
  const std::string strTipoFluidSolidPorous= "FluidSolidPorous";
  const std::string strTipoPlaneStress2D= "PlaneStress2D";
  const std::string strTipoPlaneStress= "PlaneStress";
  const std::string strTipoPlaneStrain2D= "PlaneStrain2D";
  const std::string strTipoPlaneStrain= "PlaneStrain";
  const std::string strTipoPlateFiber= "PlateFiber";
  const std::string strTipoPressureIndependMultiYield= "PressureIndependMultiYield";
  const std::string strTipoPressureIndependMultiYield02= "PressureIndependMultiiYield02";
  const std::string strTipoTemplate3Dep= "Template3Dep";
  const std::string strTipoThreeDimensional= "ThreeDimensional";
  const std::string strTipoThreeDimensionalFD= "ThreeDimensionalFD";

  const std::string strTipoError= "error";
} // end of XC namespace

#endif
