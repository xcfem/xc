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
//BeamColumnJointPhysicalProperties.cpp

#include "BeamColumnJointPhysicalProperties.h"
#include <material/uniaxial/UniaxialMaterial.h>

//! @brief Constructor
XC::BeamColumnJointPhysicalProperties::BeamColumnJointPhysicalProperties(const size_t &nMat,const UniaxialMaterial *ptr_mat)
  : UniaxialMatPhysicalProperties(nMat,ptr_mat)
  { }

XC::BeamColumnJointPhysicalProperties::BeamColumnJointPhysicalProperties(const UniaxialMaterial &theMat1, const UniaxialMaterial &theMat2, const UniaxialMaterial &theMat3, const UniaxialMaterial &theMat4, const UniaxialMaterial &theMat5, const UniaxialMaterial &theMat6, const UniaxialMaterial &theMat7, const UniaxialMaterial &theMat8, const UniaxialMaterial &theMat9, const UniaxialMaterial &theMat10, const UniaxialMaterial &theMat11, const UniaxialMaterial &theMat12, const UniaxialMaterial &theMat13)
  : UniaxialMatPhysicalProperties(13,nullptr)
  {
    // get a copy of the material and check we obtained a valid copy
    theMaterial[0] = theMat1.getCopy();
    if(!theMaterial[0]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 1" << std::endl;}
    theMaterial[1] = theMat2.getCopy();
    if(!theMaterial[1]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 2"<< std::endl;}
    theMaterial[2] = theMat3.getCopy();
    if(!theMaterial[2]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 3"<< std::endl;}
    theMaterial[3] = theMat4.getCopy();
    if(!theMaterial[3]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 4"<< std::endl;}
    theMaterial[4] = theMat5.getCopy();
    if(!theMaterial[4]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 5"<< std::endl;}
    theMaterial[5] = theMat6.getCopy();
    if(!theMaterial[5]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 6"<< std::endl;}
    theMaterial[6] = theMat7.getCopy();
    if(!theMaterial[6]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 7"<< std::endl;}
    theMaterial[7] = theMat8.getCopy();
    if(!theMaterial[7]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 8"<< std::endl;}
    theMaterial[8] = theMat9.getCopy();
    if(!theMaterial[8]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 9"<< std::endl;}
    theMaterial[9] = theMat10.getCopy();
    if(!theMaterial[9]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 10"<< std::endl;}
    theMaterial[10] = theMat11.getCopy();
    if(!theMaterial[10]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 11"<< std::endl;}
    theMaterial[11] = theMat12.getCopy();
    if(!theMaterial[11]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 12"<< std::endl;}
    theMaterial[12] = theMat13.getCopy();
    if(!theMaterial[12]){
      std::cerr << "ERROR : BeamColumnJointPhysicalProperties::Constructor failed to get a copy of material 13"<< std::endl;}

  }

