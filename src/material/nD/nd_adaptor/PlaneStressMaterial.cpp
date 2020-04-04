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
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlaneStressMaterial.cpp,v $

//
// Ed "C++" Love
//
// Generic Plane Stress XC::Material
//


#include <material/nD/nd_adaptor/PlaneStressMaterial.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h> 
#include "material/nD/NDMaterialType.h"


//static vector and matrices
XC::Vector  XC::PlaneStressMaterial::stress(3) ;
XC::Matrix  XC::PlaneStressMaterial::tangent(3,3) ;


//! @brief Constructor.
XC::PlaneStressMaterial::PlaneStressMaterial(int tag)
  :XC::NDAdaptorMaterial(ND_TAG_PlaneStressMaterial,tag,3) {}


//! @brief Constructor.
XC::PlaneStressMaterial::PlaneStressMaterial(int tag, XC::NDMaterial &the3DMaterial )
  : XC::NDAdaptorMaterial(ND_TAG_PlaneStressMaterial,tag,the3DMaterial,3)
  {
    Tgamma02 = 0.0;
    Tgamma12 = 0.0;
    Cgamma02 = 0.0;
    Cgamma12 = 0.0;
  }

//make a clone of this material
XC::NDMaterial *XC::PlaneStressMaterial::getCopy(void) const 
  { return new PlaneStressMaterial(*this); }

//make a clone of this material
XC::NDMaterial *XC::PlaneStressMaterial::getCopy(const std::string &type) const
  { return this->getCopy( ) ; }


//send back order of strain in vector form
int XC::PlaneStressMaterial::getOrder( ) const
  { return 3; }


const std::string &XC::PlaneStressMaterial::getType( ) const 
  { return strTypePlaneStress ; }



//swap history variables
int XC::PlaneStressMaterial::commitState(void) 
  {
    Cgamma02 = Tgamma02;
    Cgamma12 = Tgamma12;
    return NDAdaptorMaterial::commitState();
  }



//revert to last saved state
int XC::PlaneStressMaterial::revertToLastCommit( )
  {
    Tgamma02 = Cgamma02;
    Tgamma12 = Cgamma12;
    return NDAdaptorMaterial::revertToLastCommit( )  ;
  }


//revert to start
int XC::PlaneStressMaterial::revertToStart( )
  {
    Tgamma12  = 0.0;
    Tgamma02  = 0.0;
    Cgamma12  = 0.0;
    Cgamma02  = 0.0;
    return NDAdaptorMaterial::revertToStart();
  }


//receive the strain
int XC::PlaneStressMaterial::setTrialStrain( const XC::Vector &strainFromElement )
{
  static const double tolerance = 1.0e-08 ;

  this->strain(0) = strainFromElement(0) ;
  this->strain(1) = strainFromElement(1) ;
  this->strain(2) = strainFromElement(2) ;

  // return theMaterial->setTrialStrain( threeDstrain ) ;
  double norm ;

  static XC::Vector outOfPlaneStress(3) ;
  static XC::Vector strainIncrement(3) ;
  static XC::Vector threeDstress(6) ;
  static XC::Vector threeDstrain(6) ;
  static XC::Matrix threeDtangent(6,6) ;
  static XC::Vector threeDstressCopy(6) ; 
  static XC::Matrix threeDtangentCopy(6,6) ;

  static XC::Matrix dd22(3,3) ;

  int i, j ;
  int ii, jj ;

  //newton loop to solve for out-of-plane strains
  do {

    //set three dimensional strain
    threeDstrain(0) = this->strain(0) ;
    threeDstrain(1) = this->strain(1) ;
    threeDstrain(2) = this->Tstrain22 ;
    threeDstrain(3) = this->strain(2) ; 
    threeDstrain(4) = this->Tgamma12 ;
    threeDstrain(5) = this->Tgamma02 ;

    if(theMaterial->setTrialStrain( threeDstrain ) < 0) {
      std::cerr << "PlaneStressMaterial::setTrialStrain() - setTrialStrain in material failed with strain " << threeDstrain;
      return -1;
    }

    //three dimensional stress
    threeDstress = theMaterial->getStress( ) ;

    //three dimensional tangent 
    threeDtangent = theMaterial->getTangent( ) ;

    //NDmaterial strain order          = 11, 22, 33, 12, 23, 31 
    //PlaneStressMaterial strain order = 11, 22, 12, 33, 23, 31 

    //swap matrix indices to sort out-of-plane components 
    for( i=0; i<6; i++ ) {

      ii = this->indexMap(i) ;

      threeDstressCopy(ii) = threeDstress(i) ;

      for( j=0; j<6; j++ ) {

    jj = this->indexMap(j) ;
    
    threeDtangentCopy(ii,jj) = threeDtangent(i,j) ;

      }//end for j
       
    }//end for i


    //partitioned stresses and tangent
    for( i=0; i<3; i++ ) {

      outOfPlaneStress(i) = threeDstressCopy(i+3) ;

      for( j=0; j<3; j++ ) 
    dd22(i,j) = threeDtangentCopy(i+3,j+3) ;

    }//end for i


    //set norm
    norm = outOfPlaneStress.Norm( ) ;

    //int Solve(const XC::Vector &V, XC::Vector &res) const;
    //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
    //condensation 
    dd22.Solve( outOfPlaneStress, strainIncrement ) ;

    //update out of plane strains
    this->Tstrain22 -= strainIncrement(0) ;
    this->Tgamma12  -= strainIncrement(1) ;
    this->Tgamma02  -= strainIncrement(2) ;

  } while( norm > tolerance ) ;

  return 0;
}


//send back the stress 
const XC::Vector &XC::PlaneStressMaterial::getStress(void) const
{
  //three dimensional stress
  const XC::Vector &threeDstress = theMaterial->getStress();
  static XC::Vector threeDstressCopy(6);

  //partitioned stresses and tangent
  //swap matrix indices to sort out-of-plane components 
  int i, ii;
  for( i=0; i<6; i++ ) {

    ii = this->indexMap(i) ;

    threeDstressCopy(ii) = threeDstress(i) ;
  }

  for( i=0; i<3; i++ ) 
    this->stress(i)     = threeDstressCopy(i) ;
  
  return this->stress ;
}


//send back the tangent 
const XC::Matrix &XC::PlaneStressMaterial::getTangent(void) const
{
  static XC::Matrix dd11(3,3) ;
  static XC::Matrix dd12(3,3) ;
  static XC::Matrix dd21(3,3) ;
  static XC::Matrix dd22(3,3) ;

  static XC::Matrix dd22invdd21(3,3) ;
  static XC::Matrix threeDtangentCopy(6,6);

  //three dimensional tangent 
  const XC::Matrix &threeDtangent = theMaterial->getTangent( ) ;

  //NDmaterial strain order          = 11, 22, 33, 12, 23, 31 
  //PlaneStressMaterial strain order = 11, 22, 12, 33, 23, 31 

  //swap matrix indices to sort out-of-plane components 
  int i,j, ii, jj;

  for( i=0; i<6; i++ ) {

    ii = this->indexMap(i) ;

    for( j=0; j<6; j++ ) {
      jj = this->indexMap(j) ;
      threeDtangentCopy(ii,jj) = threeDtangent(i,j) ;
    }//end for j

  }//end for i


  //out of plane stress and tangents
  for( i=0; i<3; i++ ) {
    for( j=0; j<3; j++ ) {
    
      dd11(i,j) = threeDtangentCopy(i,  j  ) ;
      dd12(i,j) = threeDtangentCopy(i,  j+3) ;
      dd21(i,j) = threeDtangentCopy(i+3,j  ) ;
      dd22(i,j) = threeDtangentCopy(i+3,j+3) ;

    }//end for j
  }//end for i

  //int Solve(const XC::Vector &V, XC::Vector &res) const;
  //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
  //condensation 
  dd22.Solve( dd21, dd22invdd21 ) ;
  this->tangent   = dd11 ; 
  this->tangent  -= ( dd12*dd22invdd21 ) ;

  return this->tangent ;
}



int XC::PlaneStressMaterial::indexMap(int i) const
  {
    int ii ;
    if( i == 2 ) 
      ii = 3 ;
    else if( i == 3 )
      ii = 2 ;
    else 
      ii = i ;
    return ii ;
  }



//print out data
void XC::PlaneStressMaterial::Print( std::ostream &s, int flag ) const
 {
  s << "General Plane Stress XC::Material \n" ;
  s << " Tag: " << this->getTag() << "\n" ; 
  s << "using the 3D material : \n" ;

  theMaterial->Print( s, flag ) ;

  return ;
}

//! @brief Send object members through the communicator argument.
int XC::PlaneStressMaterial::sendData(Communicator &comm)
  {
    int res= NDAdaptorMaterial::sendData(comm);
    res+= comm.sendDoubles(Tgamma02,Tgamma12,Cgamma02,Cgamma12,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::PlaneStressMaterial::recvData(const Communicator &comm)
  {
    int res= NDAdaptorMaterial::recvData(comm);
    res+= comm.receiveDoubles(Tgamma02,Tgamma12,Cgamma02,Cgamma12,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::PlaneStressMaterial::sendSelf(Communicator &comm) 
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::PlaneStressMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
 


