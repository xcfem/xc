//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

