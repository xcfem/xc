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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/BeamFiberMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition of XC::BeamFiberMaterial.
// The XC::BeamFiberMaterial class is a wrapper class that performs static
// condensation on a three-dimensional material model to give the 11, 12, and 13
// stress components which can then be integrated over an area to model a
// shear flexible 3D beam.

#include <material/nD/nd_adaptor/BeamFiberMaterial.h>
#include "utility/matrix/ID.h"
#include "material/nD/TipoMaterialND.h"


XC::Vector XC::BeamFiberMaterial::stress(3);
XC::Matrix XC::BeamFiberMaterial::tangent(3,3);

XC::BeamFiberMaterial::BeamFiberMaterial(void)
  : XC::NDAdaptorMaterial(ND_TAG_BeamFiberMaterial,3),
Tstrain33(0.0), Tgamma23(0.0),
Cstrain33(0.0), Cgamma23(0.0)
  {}

XC::BeamFiberMaterial::BeamFiberMaterial(int tag, XC::NDMaterial &theMat)
  : XC::NDAdaptorMaterial(ND_TAG_BeamFiberMaterial,tag,theMat,3),
    Tstrain33(0.0), Tgamma23(0.0),
    Cstrain33(0.0), Cgamma23(0.0)
  {}

XC::NDMaterial *XC::BeamFiberMaterial::getCopy(void) const
  { return new BeamFiberMaterial(*this); }

XC::NDMaterial* XC::BeamFiberMaterial::getCopy(const std::string &type) const
  {
    if((type==strTipoBeamFiber))
      return this->getCopy();
    else
      return nullptr;
  }

int XC::BeamFiberMaterial::getOrder(void) const
  { return 3; }

const std::string &XC::BeamFiberMaterial::getType(void) const 
  { return strTipoBeamFiber; }

int XC::BeamFiberMaterial::commitState(void)
  {
    Cstrain33 = Tstrain33;
    Cgamma23 = Tgamma23;
    return NDAdaptorMaterial::commitState();
  }

int XC::BeamFiberMaterial::revertToLastCommit(void)
  {
    Tstrain33 = Cstrain33;
    Tgamma23 = Cgamma23;
    return NDAdaptorMaterial::revertToLastCommit();
  }

int XC::BeamFiberMaterial::revertToStart()
  {
    Tstrain33 = 0.0;
    Tgamma23  = 0.0;
    Cstrain33 = 0.0;
    Cgamma23  = 0.0;
    return NDAdaptorMaterial::revertToStart();
  }

//receive the strain
//NDmaterial strain order        = 11, 22, 33, 12, 23, 31  
//BeamFiberMaterial strain order = 11, 12, 31, 22, 33, 23
int XC::BeamFiberMaterial::setTrialStrain(const XC::Vector &strainFromElement)
{
  static const double tolerance = 1.0e-08;

  this->strain(0) = strainFromElement(0);
  this->strain(1) = strainFromElement(1);
  this->strain(2) = strainFromElement(2);

  //newton loop to solve for out-of-plane strains

  double norm;
  static XC::Vector condensedStress(3);
  static XC::Vector strainIncrement(3);
  static XC::Vector threeDstress(6);
  static XC::Vector threeDstrain(6);
  static XC::Matrix threeDtangent(6,6);
  static XC::Vector threeDstressCopy(6); 
  static XC::Matrix threeDtangentCopy(6,6);
  static XC::Matrix dd22(3,3);

  int i, j;
  int ii, jj;

  do {
    //set three dimensional strain
    threeDstrain(0) = this->strain(0);
    threeDstrain(1) = this->Tstrain22;
    threeDstrain(2) = this->Tstrain33;
    threeDstrain(3) = this->strain(1); 
    threeDstrain(4) = this->Tgamma23;
    threeDstrain(5) = this->strain(2);

    if(theMaterial->setTrialStrain(threeDstrain) < 0) {
      std::cerr << "BeamFiberMaterial::setTrialStrain - setStrain failed in material with strain " << threeDstrain;
      return -1;   
    }

    //three dimensional stress
    threeDstress = theMaterial->getStress();

    //three dimensional tangent 
    threeDtangent = theMaterial->getTangent();

    //NDmaterial strain order        = 11, 22, 33, 12, 23, 31  
    //BeamFiberMaterial strain order = 11, 12, 31, 22, 33, 23

    //swap matrix indices to sort out-of-plane components 
    for(i=0; i<6; i++) {

      ii = this->indexMap(i);

      threeDstressCopy(ii) = threeDstress(i);

      for(j=0; j<6; j++) {

    jj = this->indexMap(j);
    
    threeDtangentCopy(ii,jj) = threeDtangent(i,j);

      }//end for j
       
    }//end for i


    //out of plane stress and tangents
    for(i=0; i<3; i++) {

      condensedStress(i) = threeDstressCopy(i+3);

      for(j=0; j<3; j++) 
    dd22(i,j) = threeDtangentCopy(i+3,j+3);

    }//end for i

    //set norm
    norm = condensedStress.Norm();

    //condensation 
    dd22.Solve(condensedStress, strainIncrement);

    //update out of plane strains
    this->Tstrain22 -= strainIncrement(0);
    this->Tstrain33 -= strainIncrement(1);
    this->Tgamma23  -= strainIncrement(2);

  } while(norm > tolerance);

  return 0;
}

const XC::Vector &XC::BeamFiberMaterial::getStress(void) const
{
  const XC::Vector &threeDstress = theMaterial->getStress();
  static XC::Vector threeDstressCopy(6);

  int i, ii;
  //swap matrix indices to sort out-of-plane components 
  for(i=0; i<6; i++) {

    ii = this->indexMap(i);

    threeDstressCopy(ii) = threeDstress(i);
  }

  for(i=0; i<3; i++) 
    this->stress(i)     = threeDstressCopy(i);

  return this->stress;
}

const XC::Matrix &XC::BeamFiberMaterial::getTangent(void) const
{
  static XC::Matrix dd11(3,3);
  static XC::Matrix dd12(3,3);
  static XC::Matrix dd21(3,3);
  static XC::Matrix dd22(3,3);
  static XC::Matrix dd22invdd21(3,3);
  static XC::Matrix threeDtangentCopy(6,6);

  const XC::Matrix &threeDtangent = theMaterial->getTangent();

  //swap matrix indices to sort out-of-plane components 
  int i, j , ii, jj;
  for(i=0; i<6; i++) {

    ii = this->indexMap(i);

    for(j=0; j<6; j++) {
      
      jj = this->indexMap(j);
      
      threeDtangentCopy(ii,jj) = threeDtangent(i,j);
      
    }//end for j
       
  }//end for i


  for(i=0; i<3; i++) {
    for(j=0; j<3; j++) {
      dd11(i,j) = threeDtangentCopy(i,  j );
      dd12(i,j) = threeDtangentCopy(i,  j+3);
      dd21(i,j) = threeDtangentCopy(i+3,j );
      dd22(i,j) = threeDtangentCopy(i+3,j+3);
      
    }
  }

  //int Solve(const XC::Vector &V, XC::Vector &res) const;
  //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
  //condensation 
  dd22.Solve(dd21, dd22invdd21);
  this->tangent   = dd11; 
  this->tangent  -= (dd12*dd22invdd21);

  return this->tangent;
}

const XC::Matrix &XC::BeamFiberMaterial::getInitialTangent(void) const
{
  static XC::Matrix dd11(3,3);
  static XC::Matrix dd12(3,3);
  static XC::Matrix dd21(3,3);
  static XC::Matrix dd22(3,3);
  static XC::Matrix dd22invdd21(3,3);
  static XC::Matrix threeDtangentCopy(6,6);

  const XC::Matrix &threeDtangent = theMaterial->getInitialTangent();

  //swap matrix indices to sort out-of-plane components 
  int i, j , ii, jj;
  for(i=0; i<6; i++) {

    ii = this->indexMap(i);

    for(j=0; j<6; j++) {
      
      jj = this->indexMap(j);
      
      threeDtangentCopy(ii,jj) = threeDtangent(i,j);
      
    }//end for j
       
  }//end for i


  for(i=0; i<3; i++) {
    for(j=0; j<3; j++) {
      dd11(i,j) = threeDtangentCopy(i,  j );
      dd12(i,j) = threeDtangentCopy(i,  j+3);
      dd21(i,j) = threeDtangentCopy(i+3,j );
      dd22(i,j) = threeDtangentCopy(i+3,j+3);
      
    }
  }

  //int Solve(const XC::Vector &V, XC::Vector &res) const;
  //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
  //condensation 
  dd22.Solve(dd21, dd22invdd21);
  this->tangent   = dd11; 
  this->tangent  -= (dd12*dd22invdd21);

  return this->tangent;
}

//NDmaterial strain order        = 11, 22, 33, 12, 23, 31 
//BeamFiberMaterial strain order = 11, 12, 31, 22, 33, 23
int XC::BeamFiberMaterial::indexMap(int i) const
{
  int ii;

  if(i == 3) 
      ii = 1;
  else if(i == 5)
      ii = 2;
  else if(i == 1)
      ii = 3;
  else if(i == 2)
      ii = 4;
  else if(i == 4)
      ii = 5;
  else 
      ii = i;

  return ii;
}

void XC::BeamFiberMaterial::Print(std::ostream &s, int flag)
  {
    s << "BeamFiberMaterial, tag: " << this->getTag() << std::endl;
    s << "\tWrapped material: "<< theMaterial->getTag() << std::endl;

    theMaterial->Print(s, flag);
}

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::BeamFiberMaterial::sendData(CommParameters &cp)
  {
    int res= NDAdaptorMaterial::sendData(cp);
    res+= cp.sendDoubles(Tstrain33,Tgamma23,Cstrain33,Cgamma23,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::BeamFiberMaterial::recvData(const CommParameters &cp)
  {
    int res= NDAdaptorMaterial::recvData(cp);
    res+= cp.receiveDoubles(Tstrain33,Tgamma23,Cstrain33,Cgamma23,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::BeamFiberMaterial::sendSelf(CommParameters &cp) 
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::BeamFiberMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
