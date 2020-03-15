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

// $Revision: 1.11 $
// $Date: 2003/02/14 23:01:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/EnhancedQuad.cpp,v $

#include "EnhancedQuad.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <domain/domain/Domain.h>
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"


//static data
double XC::EnhancedQuad::xl[2][4] ;
XC::Matrix XC::EnhancedQuad::stiff(8,8) ;
XC::Vector XC::EnhancedQuad::resid(8) ;
XC::Matrix XC::EnhancedQuad::mass(8,8) ;

double XC::EnhancedQuad::stressData[3][4] ;
double XC::EnhancedQuad::tangentData[3][3][4] ;


//quadrature data

const double  XC::EnhancedQuad::sg[] = { -one_over_root3,
                                     one_over_root3,
                                     one_over_root3,
                                     -one_over_root3 } ;

const double  XC::EnhancedQuad::tg[] = { -one_over_root3,
                                     -one_over_root3,
                                     one_over_root3,
                                     one_over_root3 } ;

const double  XC::EnhancedQuad::wg[] = { 1.0, 1.0, 1.0, 1.0 } ;



//null constructor
XC::EnhancedQuad::EnhancedQuad(void)
  : QuadBase4N<NDMaterialPhysicalProperties>(ELE_TAG_EnhancedQuad,NDMaterialPhysicalProperties(4,nullptr)),
    alpha(4) , Ki(nullptr)
  {
    physicalProperties= NDMaterialPhysicalProperties(4,nullptr);
    //zero enhanced parameters
    alpha.Zero( ) ;
  }


//full constructor
XC::EnhancedQuad::EnhancedQuad(int tag, int node1,
                         int node2,
                            int node3,
                         int node4,
                         NDMaterial &theMat,
                         const std::string &type )
  : QuadBase4N<NDMaterialPhysicalProperties>(tag,ELE_TAG_EnhancedQuad,node1,node2,node3,node4,NDMaterialPhysicalProperties(4,theMat,type)),
    alpha(4), Ki(0)
  {
    //zero enhanced parameters
    alpha.Zero( ) ;
  }

//! @brief Virtual constructor.
XC::Element* XC::EnhancedQuad::getCopy(void) const
  { return new EnhancedQuad(*this); }


//destructor
XC::EnhancedQuad::~EnhancedQuad(void)
  { if(Ki) delete Ki; }

//! @brief Check the material type.
bool XC::EnhancedQuad::check_material_type(const std::string &type) const
  { return physicalProperties.check_material_elast_plana(type); }

//set domain
void  XC::EnhancedQuad::setDomain( Domain *theDomain )
  {
    QuadBase4N<NDMaterialPhysicalProperties>::setDomain(theDomain);
  }


//! @brief return number of dofs
int XC::EnhancedQuad::getNumDOF(void) const
  { return 8; }


//print out element data
void  XC::EnhancedQuad::Print( std::ostream &s, int flag ) const
{
  s << std::endl ;
  s << "Enhanced Strain Four XC::Node Quad \n" ;
  s << "Element Number: " << this->getTag() << std::endl ;
  s << "Node 1 : " << theNodes.getTagNode(0) << std::endl ;
  s << "Node 2 : " << theNodes.getTagNode(1) << std::endl ;
  s << "Node 3 : " << theNodes.getTagNode(2) << std::endl ;
  s << "Node 4 : " << theNodes.getTagNode(3) << std::endl ;

  s << "Material Information : \n " ;
  physicalProperties.Print( s, flag ) ;

  s << std::endl ;
}

//return stiffness matrix
const XC::Matrix&  XC::EnhancedQuad::getTangentStiff(void) const
  {
    int tang_flag = 1 ; //get the tangent
    //do tangent and residual here
    formResidAndTangent( tang_flag ) ;
    if(isDead())
      stiff*=dead_srf;
    return stiff ;
  }


//return secant matrix
const XC::Matrix&  XC::EnhancedQuad::getInitialStiff(void) const
{

  if(!Ki)
    {

  static const int ndm = 2 ;

  static const int ndf = 2 ;

  static const int nstress = 3 ;

  static const int numberNodes = 4 ;

  static const int numberGauss = 4 ;

  static const int nShape = 3 ;

  static const int nEnhanced = 4 ;

  static const int nModes = 2 ;

  static const int numberDOF = 8 ;


  int i, j, k, p, q ;
  int jj, kk ;

  static double xsj[numberGauss] ;  // determinant jacobian matrix

  static double dvol[numberGauss] ; //volume element

  static XC::Vector strain(nstress) ;  //strain

  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions

  static XC::Vector residJ(ndf) ; //nodeJ residual

  static XC::Matrix stiffJK(ndf,ndf) ; //nodeJK stiffness

  static XC::Matrix stiffKJ(ndf,ndf) ; //nodeKJ stiffness

  static XC::Vector stress(nstress) ;  //stress

  static XC::Matrix dd(nstress,nstress) ;  //material tangent

  static XC::Matrix J0(ndm,ndm) ; //Jacobian matrix at center of element

  static XC::Matrix J0inv(ndm,ndm) ; //inverse of above


  static XC::Matrix Kee(nEnhanced,nEnhanced) ;

  static XC::Vector residE(nEnhanced) ;

  static XC::Vector Umode(ndf) ;

  static XC::Vector dalpha(nEnhanced) ;

  static XC::Matrix Kue(numberDOF,nEnhanced) ;

  static XC::Matrix Keu(nEnhanced,numberDOF) ;

  static XC::Matrix KeeInvKeu(nEnhanced,numberDOF) ;


  //---------B-matrices------------------------------------

    static XC::Matrix BJ(nstress,ndf) ;      // B matrix node J

    static XC::Matrix BJtran(ndf,nstress) ;

    static XC::Matrix BK(nstress,ndf) ;      // B matrix node k

    static XC::Matrix BKtran(ndf,nstress) ;

    static XC::Matrix BJtranD(ndf,nstress) ;

    static XC::Matrix BKtranD(ndf,nstress) ;
  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( ) ;

  Kee.Zero( ) ;
  residE.Zero( ) ;

  Kue.Zero( ) ;
  Keu.Zero( ) ;


  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

  //compute Jacobian and inverse at center
  double L1 = 0.0 ;
  double L2 = 0.0 ;
  computeJacobian( L1, L2, xl, J0, J0inv ) ;

  //gauss loop to compute and save shape functions
  double det ;
  for( i = 0; i < numberGauss; i++ ) {

    //get shape functions
    shape2d( sg[i], tg[i], xl, shp, det ) ;

    //save shape functions
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          Shape[p][q][i] = shp[p][q] ;
    } // end for p

    //save jacobian determinant
    xsj[i] = det ;

    //volume element to also be saved
    dvol[i] = wg[i] * det ;

  } // end for i gauss loop

  Kee.Zero( ) ;
  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //tangent
    dd = physicalProperties[i]->getInitialTangent( ) ;

    //multiply by volume element
    dd *= dvol[i] ;
    saveData( i, stress, dd ) ;
  }
  Kee.Zero( ) ;
  for( i = 0; i < numberGauss; i++ ) {
    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
      for( q = 0; q < numberNodes; q++ )
        shp[p][q]  = Shape[p][q][i] ;
    } // end for p

    //enhanced residual and tangent calculations loops
    jj = 0 ;
    for( j = 0; j < nModes; j++ ) {

      //compute B matrix
      BJ = computeBenhanced( j, sg[i], tg[i], xsj[i], J0inv ) ;

      //transpose
      BJtran = transpose( BJ ) ;

      //BJtranD = BJtran * dd ;
      BJtranD.addMatrixProduct(0.0, BJtran, dd, 1.0) ;

      kk = 0 ;
      for( k = 0; k < nModes; k++ ) {

        BK = computeBenhanced( k, sg[i], tg[i], xsj[i], J0inv ) ;

        //stiffJK =  BJtranD * BK  ;
        stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0) ;

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            Kee( jj+p, kk+q ) += stiffJK( p, q ) ;
        } //end for p

        kk += ndf ;
      } // end for k loop

      jj += ndf ;
    } // end for j loop
  } // end for i gauss loop

  /// HERE
  for( i = 0; i < numberGauss; i++ ) {
    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i] ;
    } // end for p


    //recover stress and tangent from saved data
    getData( i, stress, dd ) ;
    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeB( j, shp ) ;

      //transpose
      BJtran = transpose( BJ ) ;

      //BJtranD = BJtran * dd ;
      BJtranD.addMatrixProduct(0.0, BJtran, dd, 1.0) ;

      //node-node stiffness
      kk = 0 ;
      for( k = 0; k < numberNodes; k++ ) {

        BK = computeB( k, shp ) ;

        //stiffJK =  BJtranD * BK  ;
        stiffJK.addMatrixProduct(0.0, BJtranD, BK,1.0) ;

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            stiff( jj+p, kk+q ) += stiffJK( p, q ) ;
        } //end for p

        kk += ndf ;
      } // end for k loop


      //node-enhanced stiffness Kue
      kk = 0 ;
      for( k = 0; k < nModes; k++ ) {

        BK = computeBenhanced( k, sg[i], tg[i], xsj[i], J0inv ) ;

        //stiffJK =  BJtranD * BK  ;
        stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0) ;

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            Kue( jj+p, kk+q ) += stiffJK( p, q ) ;
        } //end for p

        kk += ndf ;
      } // end for k loop

      //enhanced-node stiffness Keu
      kk = 0 ;
      for( k = 0; k < nModes; k++ ) {

        BK = computeBenhanced( k, sg[i], tg[i], xsj[i], J0inv ) ;

        //transpose
        BKtran = transpose( BK ) ;

        //BKtranD = BKtran * dd ;
        BKtranD.addMatrixProduct(0.0, BKtran,dd,1.0 ) ;

        //stiffKJ = (BKtran*dd)*BJ ;
        stiffKJ.addMatrixProduct(0.0, BKtranD,BJ,1.0) ;

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            Keu( kk+p, jj+q ) += stiffKJ( p, q ) ;
        } //end for p

        kk += ndf ;
      } // end for k loop

      jj += ndf ;
    } // end for j loop


  } //end for i gauss loop


  Kee.Solve( Keu, KeeInvKeu ) ;

  //stiff -= ( Kue * KeeInvKeu ) ;
  stiff.addMatrixProduct(1.0,  Kue, KeeInvKeu, -1.0);

  Ki = new Matrix(stiff);
    }

    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }



//return mass matrix
const XC::Matrix&  XC::EnhancedQuad::getMass(void) const
  {
    int tangFlag = 1 ;
    formInertiaTerms( tangFlag ) ;
    if(isDead())
      mass*=dead_srf;
    return mass ;
  }



int XC::EnhancedQuad::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::EnhancedQuad::addLoad - load type unknown for ele with tag: " << this->getTag() << std::endl;
    return -1;
  }

int XC::EnhancedQuad::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    static const int numberNodes = 4 ;
    static const int ndf = 2 ;

    int i;

    // check to see if have mass
    const bool haveRho= physicalProperties.haveRho();
    if(!haveRho)
      return 0;

  // Compute mass matrix
  int tangFlag = 1 ;
  formInertiaTerms( tangFlag ) ;

  // store computed RV from nodes in resid vector
  int count = 0;
  for(i=0; i<numberNodes; i++) {
    const XC::Vector &Raccel = theNodes[i]->getRV(accel);
    for(int j=0; j<ndf; j++)
      resid(count++) = Raccel(i);
  }

  // create the load vector if one does not exist
  if(load.isEmpty())
    load.reset(numberNodes*ndf);

  // add -M * RV(accel) to the load vector
  load.addMatrixVector(1.0, mass, resid, -1.0);

  return 0;
}


//get residual
const XC::Vector &XC::EnhancedQuad::getResistingForce(void) const
  {
    int tang_flag = 0 ; //don't get the tangent
    formResidAndTangent( tang_flag ) ;
    // subtract external loads
    if(!load.isEmpty())
      resid-= load;
    if(isDead())
      resid*=dead_srf;
    return resid ;
  }


//get residual with inertia terms
const XC::Vector &XC::EnhancedQuad::getResistingForceIncInertia(void) const
  {
    int tang_flag = 0 ; //don't get the tangent
    static Vector res(8);

    //do tangent and residual here
    formResidAndTangent( tang_flag ) ;

    //inertia terms
    formInertiaTerms( tang_flag ) ;

    res = resid;

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();

    // subtract external loads
    if(!load.isEmpty())
      res-= load;
    if(isDead())
      res*=dead_srf;
    return res;
  }


//*********************************************************************
//form inertia terms

void XC::EnhancedQuad::formInertiaTerms( int tangFlag ) const
{

  static const int ndf = 2 ;

  static const int numberNodes = 4 ;

  static const int numberGauss = 4 ;

  static const int nShape = 3 ;

  static const int massIndex = nShape - 1 ;

  double xsj ;  // determinant jacobian matrix

  double dvol ; //volume element

  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point

  static XC::Vector momentum(ndf) ;

  int i, j, k, p ;
  int jj, kk ;

  double temp, massJK ;


  //zero mass
  mass.Zero( ) ;

  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //get shape functions
    shape2d( sg[i], tg[i], xl, shp, xsj ) ;

    //volume element
    dvol = wg[i] * xsj ;

    //node loop to compute acceleration
    momentum.Zero( ) ;
    for( j = 0; j < numberNodes; j++ )
      //momentum += shp[massIndex][j] * ( theNodes[j]->getTrialAccel()  ) ;
      momentum.addVector( 1.0,
                          theNodes[j]->getTrialAccel(),
                          shp[massIndex][j] ) ;

    //density
    const double rho= physicalProperties[i]->getRho() ;

    //multiply acceleration by density to form momentum
    momentum *= rho ;


    //residual and tangent calculations node loops
    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      temp = shp[massIndex][j] * dvol ;

      for( p = 0; p < ndf; p++ )
        resid( jj+p ) += ( temp * momentum(p) )  ;


      if( tangFlag == 1 ) {

         //multiply by density
         temp *= rho ;

         //node-node mass
         kk = 0 ;
         for( k = 0; k < numberNodes; k++ ) {

            massJK = temp * shp[massIndex][k] ;

            for( p = 0; p < ndf; p++ )
              mass( jj+p, kk+p ) += massJK ;

            kk += ndf ;
          } // end for k loop

      } // end if tang_flag

      jj += ndf ;
    } // end for j loop
  } //end for i gauss loop
}

//*********************************************************************
//form residual and tangent
void  XC::EnhancedQuad::formResidAndTangent(int tang_flag) const
{

  static const double tolerance = 1.0e-08 ;

  static const int nIterations = 10 ;

  static const int ndm = 2 ;

  static const int ndf = 2 ;

  static const int nstress = 3 ;

  static const int numberNodes = 4 ;

  static const int numberGauss = 4 ;

  static const int nShape = 3 ;

  static const int nEnhanced = 4 ;

  static const int nModes = 2 ;

  static const int numberDOF = 8 ;


  int i, j, k, p, q ;
  int jj, kk ;

  int success ;

  static double xsj[numberGauss] ;  // determinant jacobian matrix

  static double dvol[numberGauss] ; //volume element

  static XC::Vector strain(nstress) ;  //strain

  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions

  static XC::Vector residJ(ndf) ; //nodeJ residual

  static XC::Matrix stiffJK(ndf,ndf) ; //nodeJK stiffness

  static XC::Matrix stiffKJ(ndf,ndf) ; //nodeKJ stiffness

  static XC::Vector stress(nstress) ;  //stress

  static XC::Matrix dd(nstress,nstress) ;  //material tangent

  static XC::Matrix J0(ndm,ndm) ; //Jacobian matrix at center of element

  static XC::Matrix J0inv(ndm,ndm) ; //inverse of above


  static XC::Matrix Kee(nEnhanced,nEnhanced) ;

  static XC::Vector residE(nEnhanced) ;

  static XC::Vector Umode(ndf) ;

  static XC::Vector dalpha(nEnhanced) ;

  static XC::Matrix Kue(numberDOF,nEnhanced) ;

  static XC::Matrix Keu(nEnhanced,numberDOF) ;

  static XC::Matrix KeeInvKeu(nEnhanced,numberDOF) ;


  //---------B-matrices------------------------------------

    static XC::Matrix BJ(nstress,ndf) ;      // B matrix node J

    static XC::Matrix BJtran(ndf,nstress) ;

    static XC::Matrix BK(nstress,ndf) ;      // B matrix node k

    static XC::Matrix BKtran(ndf,nstress) ;

    static XC::Matrix BJtranD(ndf,nstress) ;

    static XC::Matrix BKtranD(ndf,nstress) ;
  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( ) ;
  resid.Zero( ) ;

  Kee.Zero( ) ;
  residE.Zero( ) ;

  Kue.Zero( ) ;
  Keu.Zero( ) ;


  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

  //compute Jacobian and inverse at center
  double L1 = 0.0 ;
  double L2 = 0.0 ;
  computeJacobian( L1, L2, xl, J0, J0inv ) ;


  //gauss loop to compute and save shape functions
  double det ;
  for( i = 0; i < numberGauss; i++ ) {

    //get shape functions
    shape2d( sg[i], tg[i], xl, shp, det ) ;

    //save shape functions
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          Shape[p][q][i] = shp[p][q] ;
    } // end for p

    //save jacobian determinant
    xsj[i] = det ;

    //volume element to also be saved
    dvol[i] = wg[i] * det ;

  } // end for i gauss loop


  //-------------------------------------------------------------------
  //newton loop to solve for enhanced strain parameters

  int count = 0 ;
  do {

    residE.Zero( ) ;
    Kee.Zero( ) ;

    //gauss loop
    for( i = 0; i < numberGauss; i++ ) {

      //extract shape functions from saved array
      for( p = 0; p < nShape; p++ ) {
        for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i] ;
      } // end for p


      //zero the strain
      strain.Zero( ) ;

      // j-node loop to compute nodal strain contributions
      for( j = 0; j < numberNodes; j++ )  {

        //compute B matrix
        BJ = computeB( j, shp ) ;

        //nodal displacements
        const XC::Vector &ul = theNodes[j]->getTrialDisp( ) ;

        //compute the strain
        //strain += (BJ*ul) ;
        strain.addMatrixVector(1.0, BJ, ul, 1.0) ;

      } // end for j

      // j-node loop to compute enhanced strain contributions
      for( j = 0; j < nModes; j++ )  {

        //compute B matrix
        BJ = computeBenhanced( j, sg[i], tg[i], xsj[i], J0inv ) ;

        //enhanced "displacements"
        Umode(0) = this->alpha( 2*j     ) ;
        Umode(1) = this->alpha( 2*j + 1 ) ;

        //compute the strain
        //strain += (BJ*Umode) ;
        strain.addMatrixVector(1.0, BJ, Umode, 1.0) ;

      } // end for j

      success = const_cast<NDMaterial *>(physicalProperties[i])->setTrialStrain(strain);

      //compute the stress
      stress = physicalProperties[i]->getStress( ) ;

      //multiply by volume element
      stress  *= dvol[i] ;

      //tangent
      dd = physicalProperties[i]->getTangent( ) ;

      //multiply by volume element
      dd *= dvol[i] ;

      //save stress and tangent (already multiplied by volume element)
      saveData( i, stress, dd ) ;

      //enhanced residual and tangent calculations loops

      jj = 0 ;
      for( j = 0; j < nModes; j++ ) {

        //compute B matrix
        BJ = computeBenhanced( j, sg[i], tg[i], xsj[i], J0inv ) ;

        //transpose
        BJtran = transpose( BJ ) ;


        //residual
        //residJ = (BJtran * stress) ;
        //residJ *= (-1.0) ;
        residJ.addMatrixVector(0.0, BJtran,stress,-1.0) ;

        //residual
        for( p = 0; p < ndf; p++ )
          residE( jj+p ) += residJ(p)  ;


        //BJtranD = BJtran * dd ;
        BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0) ;

        kk = 0 ;
        for( k = 0; k < nModes; k++ ) {

          BK = computeBenhanced( k, sg[i], tg[i], xsj[i], J0inv ) ;

          //stiffJK =  BJtranD * BK  ;
          stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0) ;

          for( p = 0; p < ndf; p++ )  {
             for( q = 0; q < ndf; q++ )
                Kee( jj+p, kk+q ) += stiffJK( p, q ) ;
          } //end for p

            kk += ndf ;
          } // end for k loop

        jj += ndf ;
      } // end for j loop


    } // end for i gauss loop


    //solve for enhanced strain parameters

    dalpha.Zero( ) ;

    Kee.Solve( residE, dalpha ) ;

    if(dalpha(0) > 1.0e10)  std::cerr << "dalpha: " << residE << dalpha;

    this->alpha+= dalpha ;

    count++ ;
    if( count > nIterations ) {
      std::cerr << "Exceeded " << nIterations
           << " iterations solving for enhanced strain parameters " << std::endl ;
      break ;
    } //end if


    //do at least 2 iterations so saved data is good
  } while ( residE.Norm() > tolerance  ||  count < 2 ) ;


  //end enhanced strain parameters newton loop
  //-------------------------------------------------------------------


  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i] ;
    } // end for p


    //recover stress and tangent from saved data
    getData( i, stress, dd ) ;


    //residual and tangent calculations node loops

    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeB( j, shp ) ;

      //transpose
      BJtran = transpose( BJ ) ;

      //residual
      //residJ = BJtran * stress ;
      residJ.addMatrixVector(0.0, BJtran,stress,1.0) ;

      for( p = 0; p < ndf; p++ )
        resid( jj+p ) += residJ(p)  ;


      if( tang_flag == 1 ) {

        //BJtranD = BJtran * dd ;
        BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0) ;

         //node-node stiffness
         kk = 0 ;
         for( k = 0; k < numberNodes; k++ ) {

            BK = computeB( k, shp ) ;

            //stiffJK =  BJtranD * BK  ;
            stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0) ;

            for( p = 0; p < ndf; p++ )  {
               for( q = 0; q < ndf; q++ )
                  stiff( jj+p, kk+q ) += stiffJK( p, q ) ;
            } //end for p

            kk += ndf ;
          } // end for k loop


         //node-enhanced stiffness Kue
         kk = 0 ;
         for( k = 0; k < nModes; k++ ) {

            BK = computeBenhanced( k, sg[i], tg[i], xsj[i], J0inv ) ;

            //stiffJK =  BJtranD * BK  ;
            stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0) ;

            for( p = 0; p < ndf; p++ )  {
               for( q = 0; q < ndf; q++ )
                  Kue( jj+p, kk+q ) += stiffJK( p, q ) ;
            } //end for p

            kk += ndf ;
          } // end for k loop


         //enhanced-node stiffness Keu
         kk = 0 ;
         for( k = 0; k < nModes; k++ ) {

            BK = computeBenhanced( k, sg[i], tg[i], xsj[i], J0inv ) ;

            //transpose
            BKtran = transpose( BK ) ;

            //BKtranD = BKtran * dd ;
            BKtranD.addMatrixProduct(0.0, BKtran,dd,1.0 ) ;

            //stiffKJ = (BKtran*dd)*BJ ;
            stiffKJ.addMatrixProduct(0.0, BKtranD,BJ,1.0) ;

            for( p = 0; p < ndf; p++ )  {
               for( q = 0; q < ndf; q++ )
                  Keu( kk+p, jj+q ) += stiffKJ( p, q ) ;
            } //end for p

            kk += ndf ;
          } // end for k loop



      } // end if tang_flag

      jj += ndf ;
    } // end for j loop


  } //end for i gauss loop


  //static condensation of enhanced parameters

  if( tang_flag == 1 ) {

     Kee.Solve( Keu, KeeInvKeu ) ;

     //stiff -= ( Kue * KeeInvKeu ) ;
     stiff.addMatrixProduct(1.0,  Kue,KeeInvKeu,-1.0 ) ;

  } //end if

  return ;
}





int XC::EnhancedQuad::update(void)
  { return 0; }


//************************************************************************
void XC::EnhancedQuad::saveData(int gp, const XC::Vector &stress, const XC::Matrix &tangent)
  {

    //save stress
    for(int i=0; i<3; i++ )
      stressData[i][gp] = stress(i) ;

    //save tangent
    for(int i=0; i<3; i++ )
      {
        for(int j=0; j<3; j++ )
          tangentData[i][j][gp] = tangent(i,j) ;
      } //end for i
    return ;
  }


//************************************************************************
//recover stress and tangent data

void  XC::EnhancedQuad::getData(int gp, Vector &stress, Matrix &tangent ) const
{

  int i, j ;

  //get stress
  for( i=0; i<3; i++ )
    stress(i) = stressData[i][gp] ;

  //get tangent
  for( i=0; i<3; i++ ) {
    for( j=0; j<3; j++ )
      tangent(i,j) = tangentData[i][j][gp] ;
  } //end for i

  return ;

}

//************************************************************************
//compute local coordinates and basis

void XC::EnhancedQuad::computeBasis(void) const
{
  //nodal coordinates

  int i ;
  for( i = 0; i < 4; i++ ) {

       const XC::Vector &coorI = theNodes[i]->getCrds( ) ;

       xl[0][i] = coorI(0) ;
       xl[1][i] = coorI(1) ;

  }  //end for i

}

//*************************************************************************
//compute B matrix

const XC::Matrix &XC::EnhancedQuad::computeB( int node, const double shp[3][4] ) const
{

  static XC::Matrix B(3,2) ;

//---B XC::Matrix in standard {1,2,3} mechanics notation---------------
//
//                -             -
//               | +N,1      0   |
// B    =        |   0     +N,2  |    (3x2)
//               | +N,2    +N,1  |
//                -             -
//
//-------------------------------------------------------------------

  B.Zero( ) ;

  B(0,0) = shp[0][node] ;
  B(1,1) = shp[1][node] ;
  B(2,0) = shp[1][node] ;
  B(2,1) = shp[0][node] ;

  return B ;

}

//***********************************************************************
//compute enhanced strain B-matrices

const XC::Matrix &XC::EnhancedQuad::computeBenhanced( int node,
                                         double L1,
                                         double L2,
                                         double j,
                                         const XC::Matrix &Jinv ) const
{
  static XC::Matrix B(3,2) ;

  static double JinvTran[2][2] ;

  static double shape[2] ;

  static double parameter ;


  //compute JinvTran
  JinvTran[0][0] = Jinv(0,0) ;
  JinvTran[1][1] = Jinv(1,1) ;
  JinvTran[0][1] = Jinv(1,0) ;
  JinvTran[1][0] = Jinv(0,1) ;      //residual



  if( node == 0 ) {

    //first column of JinvTran
    shape[0] = JinvTran[0][0] ;
    shape[1] = JinvTran[1][0] ;

    parameter = L1 / j ;

  }
  else if( node == 1 ) {

    //second column of JinvTran
    shape[0] = JinvTran[0][1] ;
    shape[1] = JinvTran[1][1] ;

    parameter = L2 / j ;

  } //end if

  shape[0] *= parameter ;
  shape[1] *= parameter ;


  B.Zero( ) ;

  B(0,0) = shape[0] ;
  B(1,1) = shape[1] ;
  B(2,0) = shape[1] ;
  B(2,1) = shape[0] ;

  return B ;

}

//***********************************************************************
//compute Jacobian matrix and inverse at point {L1,L2}

void XC::EnhancedQuad::computeJacobian(double L1, double L2,const double x[2][4],Matrix &JJ,Matrix &JJinv )
{
  int i, j, k ;

  static const double s[] = { -0.5,  0.5, 0.5, -0.5 } ;
  static const double t[] = { -0.5, -0.5, 0.5,  0.5 } ;

  static double shp[2][4] ;

  double ss = L1 ;
  double tt = L2 ;

  for( i = 0; i < 4; i++ ) {
      shp[0][i] = s[i] * ( 0.5 + t[i]*tt ) ;
      shp[1][i] = t[i] * ( 0.5 + s[i]*ss ) ;
  } // end for i


  // Construct jacobian and its inverse

  JJ.Zero( ) ;
  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {

      for( k = 0; k < 4; k++ )
          JJ(i,j) +=  x[i][k] * shp[j][k] ;

    } //end for j
  }  // end for i

  double xsj = JJ(0,0)*JJ(1,1) - JJ(0,1)*JJ(1,0) ;

  //inverse jacobian
  double jinv = 1.0 / xsj ;
  JJinv(0,0) =  JJ(1,1) * jinv ;
  JJinv(1,1) =  JJ(0,0) * jinv ;
  JJinv(0,1) = -JJ(0,1) * jinv ;
  JJinv(1,0) = -JJ(1,0) * jinv ;

  return ;

}

//************************************************************************
//shape function routine for four node quads

void XC::EnhancedQuad::shape2d( double ss, double tt, const double x[2][4], double shp[3][4], double &xsj)
  {

  int i, j, k ;

  double temp ;

  static const double s[] = { -0.5,  0.5, 0.5, -0.5 } ;
  static const double t[] = { -0.5, -0.5, 0.5,  0.5 } ;

  static XC::Matrix xs(2,2) ;
  static XC::Matrix sx(2,2) ;

  for( i = 0; i < 4; i++ ) {
      shp[2][i] = ( 0.5 + s[i]*ss )*( 0.5 + t[i]*tt ) ;
      shp[0][i] = s[i] * ( 0.5 + t[i]*tt ) ;
      shp[1][i] = t[i] * ( 0.5 + s[i]*ss ) ;
  } // end for i


  // Construct jacobian and its inverse

  xs.Zero( ) ;
  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {

      for( k = 0; k < 4; k++ )
          xs(i,j) +=  x[i][k] * shp[j][k] ;

    } //end for j
  }  // end for i

  xsj = xs(0,0)*xs(1,1) - xs(0,1)*xs(1,0) ;

  //inverse jacobian
  //inverse jacobian
  double jinv = 1.0 / xsj ;
  sx(0,0) =  xs(1,1) * jinv ;
  sx(1,1) =  xs(0,0) * jinv ;
  sx(0,1) = -xs(0,1) * jinv ;
  sx(1,0) = -xs(1,0) * jinv ;


  //form global derivatives

  for( i = 0; i < 4; i++ ) {
    temp      = shp[0][i]*sx(0,0) + shp[1][i]*sx(1,0) ;
    shp[1][i] = shp[0][i]*sx(0,1) + shp[1][i]*sx(1,1) ;
    shp[0][i] = temp ;
  } // end for i

  return ;
}

//**********************************************************************

const XC::Matrix &XC::EnhancedQuad::transpose(const XC::Matrix &M )
  {
    //we're always transposing 3x2 matrices for this element,
    //so always return a 2x3 .

    static int dim1 = 2 ;
    static int dim2 = 3 ;
    static Matrix Mtran(dim1,dim2) ;

    for(register int i = 0; i < dim1; i++)
      {
         for(register int j = 0; j < dim2; j++ )
           Mtran(i,j) = M(j,i) ;
      } // end for i
    return Mtran ;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::EnhancedQuad::sendData(CommParameters &cp)
  {
    int res= QuadBase4N<NDMaterialPhysicalProperties>::sendData(cp);
    res+= cp.sendVector(alpha,getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::EnhancedQuad::recvData(const CommParameters &cp)
  {
    int res= QuadBase4N<NDMaterialPhysicalProperties>::recvData(cp);
    res+= cp.receiveVector(alpha,getDbTagData(),CommMetaData(8));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int  XC::EnhancedQuad::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(15);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::EnhancedQuad::recvSelf(const CommParameters &cp)
  {
    inicComm(15);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

