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
**   Frank McKenna(fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:01:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlateFiberMaterial.cpp,v $

//
// Ed "C++" Love
//
// Generic Plate XC::Fiber XC::Material
//


#include <material/nD/nd_adaptor/PlateFiberMaterial.h>
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include <utility/matrix/ID.h> 
#include "material/nD/TipoMaterialND.h"

//static vector and matrices
XC::Vector  XC::PlateFiberMaterial::stress(5);
XC::Matrix  XC::PlateFiberMaterial::tangent(5,5);

XC::PlateFiberMaterial::PlateFiberMaterial(int tag)
  : XC::NDAdaptorMaterial(ND_TAG_PlateFiberMaterial,tag,5)
  {}

//null constructor
XC::PlateFiberMaterial::PlateFiberMaterial() : 
  XC::NDAdaptorMaterial(ND_TAG_PlateFiberMaterial,0,5) 
  {}


//full constructor
XC::PlateFiberMaterial::PlateFiberMaterial(int tag, XC::NDMaterial &the3DMaterial) :
  XC::NDAdaptorMaterial(ND_TAG_PlateFiberMaterial,tag,the3DMaterial,5)
  {}

//make a clone of this material
XC::NDMaterial *XC::PlateFiberMaterial::getCopy(void) const 
  { return new XC::PlateFiberMaterial(*this); }


//make a clone of this material
XC::NDMaterial *XC::PlateFiberMaterial::getCopy(const std::string &type) const
  { return this->getCopy(); }


//send back order of strain in vector form
int XC::PlateFiberMaterial::getOrder() const
  { return 5; }


const std::string &XC::PlateFiberMaterial::getType() const 
  { return strTipoPlateFiber; }



//receive the strain
int XC::PlateFiberMaterial::setTrialStrain(const XC::Vector &strainFromElement)
{
  static const double tolerance = 1.0e-08;

  this->strain(0) = strainFromElement(0); //11
  this->strain(1) = strainFromElement(1); //22
  this->strain(2) = strainFromElement(2); //12
  this->strain(3) = strainFromElement(3); //23
  this->strain(4) = strainFromElement(4); //31

  double norm;
  static XC::Vector outOfPlaneStress(1);
  static XC::Vector strainIncrement(1);
  static XC::Vector threeDstress(6);
  static XC::Vector threeDstrain(6);
  static XC::Matrix threeDtangent(6,6);
  static XC::Vector threeDstressCopy(6); 

  static XC::Matrix threeDtangentCopy(6,6);
  static XC::Matrix dd22(1,1);

  int i, j;
  int ii, jj;

  int count = 0;
  //newton loop to solve for out-of-plane strains
  do {

    //set three dimensional strain
    threeDstrain(0) = this->strain(0);
    threeDstrain(1) = this->strain(1);

    threeDstrain(2) = this->Tstrain22;
  
    threeDstrain(3) = this->strain(2); 
    threeDstrain(4) = this->strain(3);
    threeDstrain(5) = this->strain(4);

    if(theMaterial->setTrialStrain(threeDstrain) < 0) {
      std::cerr << "PlateFiberMaterial::setTrialStrain - material failed in setTrialStrain() with strain " << threeDstrain;
      return -1;
    }

    //three dimensional stress
    threeDstress = theMaterial->getStress();

    //three dimensional tangent 
    threeDtangent = theMaterial->getTangent();

    //NDmaterial strain order          = 11, 22, 33, 12, 23, 31 
    //PlateFiberMaterial strain order =  11, 22, 12, 23, 31, 33 

    //swap matrix indices to sort out-of-plane components 
    for(i=0; i<6; i++) {

      ii = this->indexMap(i);

      threeDstressCopy(ii) = threeDstress(i);

      for(j=0; j<6; j++) {

    jj = this->indexMap(j);

    threeDtangentCopy(ii,jj) = threeDtangent(i,j);

      }//end for j

    }//end for i


    //partitioned stresses and tangent
    
    outOfPlaneStress(0) = threeDstress(2);

    dd22(0,0) = threeDtangentCopy(5,5);

    //set norm
    norm = outOfPlaneStress.Norm();

    //int Solve(const XC::Vector &V, XC::Vector &res) const;
    //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
    //condensation 
    dd22.Solve(outOfPlaneStress, strainIncrement);

    //update out of plane strains
    this->Tstrain22 -= strainIncrement(0);

    count++;
  } while(norm > tolerance && count < 10);

  return 0;
}


//send back the stress 
const XC::Vector &XC::PlateFiberMaterial::getStress(void) const
{
  const XC::Vector &threeDstress = theMaterial->getStress();
  static XC::Vector threeDstressCopy(6);

  //swap matrix indices to sort out-of-plane components 
  int i, ii;
  for(i=0; i<6; i++) {
    
    ii = this->indexMap(i);
    
    threeDstressCopy(ii) = threeDstress(i);
    
  }//end for i

  for(i=0; i<5; i++) 
    this->stress(i)     = threeDstressCopy(i);

  return this->stress;
}

//send back the tangent 
const XC::Matrix  &XC::PlateFiberMaterial::getTangent(void) const
{
  static XC::Matrix dd11(5,5);
  static XC::Matrix dd12(5,1);
  static XC::Matrix dd21(1,5);
  static XC::Matrix dd22(1,1);
  static XC::Matrix dd22invdd21(1,5);

  static XC::Matrix threeDtangentCopy(6,6);
  const XC::Matrix &threeDtangent = theMaterial->getTangent();

  //swap matrix indices to sort out-of-plane components 
  int i,j, ii, jj;
  for(i=0; i<6; i++) {

    ii = this->indexMap(i);

    for(j=0; j<6; j++) {

      jj = this->indexMap(j);

      threeDtangentCopy(ii,jj) = threeDtangent(i,j);

    }//end for j

  }//end for i
  
  dd22(0,0) = threeDtangentCopy(5,5);

  for(i=0; i<5; i++) {

    dd12(i,0) = threeDtangentCopy(i,5);
    dd21(0,i) = threeDtangentCopy(5,i);
    
    for(int j=0; j<5; j++) 
      dd11(i,j) = threeDtangentCopy(i,j);
    
  }//end for i
    
  //int Solve(const XC::Vector &V, XC::Vector &res) const;
  //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
  //condensation 
  dd22.Solve(dd21, dd22invdd21);
  this->tangent   = dd11; 
  this->tangent  -= (dd12*dd22invdd21);

  return this->tangent;
}


const XC::Matrix  &XC::PlateFiberMaterial::getInitialTangent(void) const
  {
  std::cerr << "PlateFiberMaterial::getInitialTangent() - not yet implemented\n";
  return this->getTangent();
}


int XC::PlateFiberMaterial::indexMap(int i) const
{
  int ii;

  switch(i+1) { //add 1 for standard vector indices

    case 1 :
      ii = 1; 
      break;
 
    case 2 :
      ii = 2;
      break;

    case 3 :
      ii = 6;
      break;

    case 4 :
      ii = 3;
      break;

    case 5 :
      ii = 4;
      break;

    case 6 :
      ii = 5;
      break;

    default :
      ii = 1;
      break;

  } //end switch

  ii--;

  return ii;
}



//print out data
void XC::PlateFiberMaterial::Print(std::ostream &s, int flag)
{
  s << "General Plate XC::Fiber XC::Material \n";
  s << " Tag: " << this->getTag() << "\n"; 
  s << "using the 3D material : \n";

  theMaterial->Print(s, flag);

  return;
}


//! @brief Sends object through the channel being passed as parameter.
int XC::PlateFiberMaterial::sendSelf(CommParameters &cp) 
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PlateFiberMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
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
 


