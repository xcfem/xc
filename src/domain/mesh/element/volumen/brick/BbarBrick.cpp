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
/* *********************************************************************
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

// $Revision: 1.16 $
// $Date: 2003/08/28 22:42:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/brick/BbarBrick.cpp,v $

// Ed "C++" Love
//
// Eight node XC::BbarBrick element
//

#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <utility/handler/ErrorHandler.h>
#include <domain/mesh/element/volumen/brick/BbarBrick.h>
#include <domain/mesh/element/volumen/brick/shp3d.h>
#include <utility/recorder/response/ElementResponse.h>
#include "utility/actor/actor/MatrixCommMetaData.h"


#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "domain/mesh/element/gauss_models/GaussModel.h"

//static data
double  XC::BbarBrick::xl[3][8] ;

 XC::Matrix  XC::BbarBrick::stiff(24,24) ;
 XC::Vector  XC::BbarBrick::resid(24) ;
 XC::Matrix  XC::BbarBrick::mass(24,24) ;


//quadrature data
const double  XC::BbarBrick::sg[] = { -one_over_root3,
                               one_over_root3  } ;

const double  XC::BbarBrick::wg[] = { 1.0, 1.0, 1.0, 1.0,
                              1.0, 1.0, 1.0, 1.0  } ;



//null constructor
XC::BbarBrick::BbarBrick(void)
  : BrickBase(ELE_TAG_BbarBrick), Ki(0)
  { }


//*********************************************************************
//full constructor
XC::BbarBrick::BbarBrick(  int tag,
                         int node1,
                         int node2,
                            int node3,
                         int node4,
                         int node5,
                         int node6,
                         int node7,
                         int node8,
                         NDMaterial &theMaterial,
                         const BodyForces3D &bForces)
  :BrickBase(tag,ELE_TAG_BbarBrick,node1,node2,node3,node4,node5,node6,node7,node8,NDMaterialPhysicalProperties(8,&theMaterial)), bf(bForces), Ki(nullptr)
  { }
//******************************************************************

//! @brief Constructor virtual.
XC::Element* XC::BbarBrick::getCopy(void) const
  { return new BbarBrick(*this); }


//destructor
XC::BbarBrick::~BbarBrick(void)
  {
    if(Ki)
      {
        delete Ki;
        Ki= nullptr;
      }
  } 


//set domain
void XC::BbarBrick::setDomain( Domain *theDomain )
  {
    BrickBase::setDomain(theDomain);
  }


//return number of dofs
int XC::BbarBrick::getNumDOF(void) const
  { return 24 ; }


//print out element data
void  XC::BbarBrick::Print( std::ostream &s, int flag )
{
  s << std::endl ;
  s << "Volume/Pressure Eight XC::Node XC::BbarBrick \n" ;
  s << "Element Number: " << this->getTag() << std::endl ;
  s << "Node 1 : " << theNodes.getTagNode(0) << std::endl ;
  s << "Node 2 : " << theNodes.getTagNode(1) << std::endl ;
  s << "Node 3 : " << theNodes.getTagNode(2) << std::endl ;
  s << "Node 4 : " << theNodes.getTagNode(3) << std::endl ;
  s << "Node 5 : " << theNodes.getTagNode(4) << std::endl ;
  s << "Node 6 : " << theNodes.getTagNode(5) << std::endl ;
  s << "Node 7 : " << theNodes.getTagNode(6) << std::endl ;
  s << "Node 8 : " << theNodes.getTagNode(7) << std::endl ;

  s << "Material Information : \n " ;
  physicalProperties.Print( s, flag ) ;

  s << std::endl ;
}

//return stiffness matrix
const XC::Matrix &XC::BbarBrick::getTangentStiff(void) const
  {
    int tang_flag = 1 ; //get the tangent

    //do tangent and residual here
    formResidAndTangent( tang_flag ) ;

    if(isDead())
      stiff*=dead_srf;
    return stiff ;
  }


//return stiffness matrix
const XC::Matrix &XC::BbarBrick::getInitialStiff(void) const
  {
    if(!Ki)
      {

    //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

    static const int ndm = 3 ;
    static const int ndf = 3 ;
    static const int nstress = 6 ;
    static const int numberNodes = 8 ;
    static const int numberGauss = 8 ;
    static const int nShape = 4 ;

    int i, j, k, p, q ;
    int jj, kk ;

    static double volume ;
    static double xsj ;  // determinant jacaobian matrix
    static double dvol[numberGauss] ; //volume element
    static double gaussPoint[ndm] ;
    static XC::Vector strain(nstress) ;  //strain
    static double shp[nShape][numberNodes] ;  //shape functions at a gauss point
    static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions
    static double shpBar[nShape][numberNodes] ;  //mean value of shape functions
    static XC::Matrix stiffJK(ndf,ndf) ; //nodeJK stiffness
    static XC::Matrix dd(nstress,nstress) ;  //material tangent


    //---------B-matrices------------------------------------
    static XC::Matrix BJ(nstress,ndf) ;      // B matrix node J
    static XC::Matrix BJtran(ndf,nstress) ;
    static XC::Matrix BK(nstress,ndf) ;      // B matrix node k
    static XC::Matrix BJtranD(ndf,nstress) ;
    //-------------------------------------------------------

    //zero stiffness and residual
    stiff.Zero();

    //compute basis vectors and local nodal coordinates
    computeBasis( ) ;

    //zero mean shape functions
    for(p = 0; p < nShape; p++ )
      {
        for( q = 0; q < numberNodes; q++ )
          shpBar[p][q] = 0.0 ;
      } // end for p

    //zero volume
    volume = 0.0 ;

    //gauss loop to compute and save shape functions
    int count = 0 ;
    for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i] ;
        gaussPoint[1] = sg[j] ;
        gaussPoint[2] = sg[k] ;

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl ) ;

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q] ;
        } // end for p

        //volume element to also be saved
        dvol[count] = wg[count] * xsj ;

        //add to volume
        volume += dvol[count] ;

        //add to mean shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            shpBar[p][q] += ( dvol[count] * shp[p][q] ) ;
        } // end for p

        count++ ;

      } //end for k
    } //end for j
  } // end for i


  //mean value of shape functions
  for( p = 0; p < nShape; p++ ) {
    for( q = 0; q < numberNodes; q++ )
      shpBar[p][q] /= volume ;
  } // end for p


  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i] ;
    } // end for p

    dd = physicalProperties[i]->getInitialTangent( ) ;
    dd *= dvol[i] ;

    //residual and tangent calculations node loops

    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeBbar( j, shp, shpBar ) ;

      //transpose
      //BJtran = transpose( nstress, ndf, BJ ) ;
      for(p=0; p<ndf; p++) {
        for(q=0; q<nstress; q++)
          BJtran(p,q) = BJ(q,p) ;
      }//end for p

      //BJtranD = BJtran * dd ;
      BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0);

      kk = 0 ;
      for( k = 0; k < numberNodes; k++ ) {

        BK = computeBbar( k, shp, shpBar ) ;

        //stiffJK =  BJtranD * BK  ;
        stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0) ;

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            stiff( jj+p, kk+q ) += stiffJK( p, q ) ;
        } //end for p

        kk += ndf ;
      } // end for k loop

      jj += ndf ;

    } // end for j loop
  } //end for i gauss loop

  Ki= new Matrix(stiff);
      }
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }


//return mass matrix
const XC::Matrix&  XC::BbarBrick::getMass(void) const
  {
    int tangFlag = 1 ;
    formInertiaTerms( tangFlag ) ;
    if(isDead())
      mass*=dead_srf;
    return mass ;
  }


int XC::BbarBrick::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::BbarBrick::addLoad - load type unknown for ele with tag: " << this->getTag() << std::endl;
    return -1;
  }

int XC::BbarBrick::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
  static const int numberNodes = 8;
  static const int ndf = 3 ;

  int i;

  // check to see if have mass
  bool haveRho= physicalProperties.haveRho();
  if(!haveRho)
    return 0;

  // Compute mass matrix
  int tangFlag = 1 ;
  formInertiaTerms( tangFlag ) ;

  // store computed RV fro nodes in resid vector
  int count = 0;
  for(i=0; i<numberNodes; i++) {
    const XC::Vector &Raccel = theNodes[i]->getRV(accel);
    for(int j=0; j<ndf; j++)
      resid(count++) = Raccel(j);
  }

  // create the load vector if one does not exist
  if(load.Nulo())
    load.reset(numberNodes*ndf);

  // add -M * RV(accel) to the load vector
  load.addMatrixVector(1.0, mass, resid, -1.0);

  return 0;
}


//get residual
const XC::Vector &XC::BbarBrick::getResistingForce(void) const
  {
    int tang_flag = 0 ;
    formResidAndTangent(tang_flag); //don't get the tangent
    if(!load.Nulo())
      resid-= load;
    if(isDead())
      resid*=dead_srf;
    return resid;
  }


//get residual with inertia terms
const XC::Vector &XC::BbarBrick::getResistingForceIncInertia(void) const
  {
    static Vector res(24);
    int tang_flag = 0 ; //don't get the tangent

    //do tangent and residual here
    formResidAndTangent(tang_flag);

    formInertiaTerms(tang_flag);
    res= resid;

    // add the damping forces if rayleigh damping
    if(!rayFactors.Nulos())
      res += this->getRayleighDampingForces();
    if(!load.Nulo())
      res -= load;
    if(isDead())
      resid*=dead_srf;
    return res ;
  }


//*********************************************************************
//form inertia terms

void XC::BbarBrick::formInertiaTerms( int tangFlag ) const
  {

  static const int ndm = 3 ;

  static const int ndf = 3 ;

  static const int numberNodes = 8 ;

  static const int numberGauss = 8 ;

  static const int nShape = 4 ;

  static const int massIndex = nShape - 1 ;

  double xsj ;  // determinant jacaobian matrix

  double dvol[numberGauss] ; //volume element

  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions

  static double gaussPoint[ndm] ;

  static XC::Vector momentum(ndf) ;

  int i, j, k, p, q ;
  int jj, kk ;

  double temp, rho, massJK ;


  //zero mass
  mass.Zero( ) ;

  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

  //gauss loop to compute and save shape functions

  int count = 0 ;

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i] ;
        gaussPoint[1] = sg[j] ;
        gaussPoint[2] = sg[k] ;

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl ) ;

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q] ;
        } // end for p

        //volume element to also be saved
        dvol[count] = wg[count] * xsj ;

        count++ ;

      } //end for k
    } //end for j
  } // end for i



  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i] ;
    } // end for p


    //node loop to compute acceleration
    momentum.Zero( ) ;
    for( j = 0; j < numberNodes; j++ )
      //momentum += shp[massIndex][j] * ( theNodes[j]->getTrialAccel()  ) ;
      momentum.addVector( 1.0,
                          theNodes[j]->getTrialAccel(),
                          shp[massIndex][j] ) ;


    //density
    rho = physicalProperties[i]->getRho() ;

    //multiply acceleration by density to form momentum
    momentum *= rho ;


    //residual and tangent calculations node loops
    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      temp = shp[massIndex][j] * dvol[i] ;

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
void  XC::BbarBrick::formResidAndTangent( int tang_flag ) const
{

  //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

  static const int ndm = 3 ;

  static const int ndf = 3 ;

  static const int nstress = 6 ;

  static const int numberNodes = 8 ;

  static const int numberGauss = 8 ;

  static const int nShape = 4 ;

  int i, j, k, p, q ;
  int jj, kk ;

  int success ;

  static double volume ;

  static double xsj ;  // determinant jacaobian matrix

  static double dvol[numberGauss] ; //volume element

  static double gaussPoint[ndm] ;

  static XC::Vector strain(nstress) ;  //strain

  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions

  static double shpBar[nShape][numberNodes] ;  //mean value of shape functions

  static XC::Vector residJ(ndf) ; //nodeJ residual

  static XC::Matrix stiffJK(ndf,ndf) ; //nodeJK stiffness

  static XC::Vector stress(nstress) ;  //stress

  static XC::Matrix dd(nstress,nstress) ;  //material tangent


  //---------B-matrices------------------------------------

    static XC::Matrix BJ(nstress,ndf) ;      // B matrix node J

    static XC::Matrix BJtran(ndf,nstress) ;

    static XC::Matrix BK(nstress,ndf) ;      // B matrix node k

    static XC::Matrix BJtranD(ndf,nstress) ;

  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( ) ;
  resid.Zero( ) ;

  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;


  //zero mean shape functions
  for( p = 0; p < nShape; p++ ) {
    for( q = 0; q < numberNodes; q++ )
      shpBar[p][q] = 0.0 ;
  } // end for p

  //zero volume
  volume = 0.0 ;


  //gauss loop to compute and save shape functions
  int count = 0 ;

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i] ;
        gaussPoint[1] = sg[j] ;
        gaussPoint[2] = sg[k] ;

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl ) ;

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q] ;
        } // end for p

        //volume element to also be saved
        dvol[count] = wg[count] * xsj ;

        //add to volume
        volume += dvol[count] ;

        //add to mean shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            shpBar[p][q] += ( dvol[count] * shp[p][q] ) ;
        } // end for p

        count++ ;

      } //end for k
    } //end for j
  } // end for i


  //mean value of shape functions
  for( p = 0; p < nShape; p++ ) {
    for( q = 0; q < numberNodes; q++ )
      shpBar[p][q] /= volume ;
  } // end for p


  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i] ;
    } // end for p


    //zero the strains
    strain.Zero( ) ;


    // j-node loop to compute strain
    for( j = 0; j < numberNodes; j++ )  {

      //compute B matrix

      BJ = computeBbar( j, shp, shpBar ) ;

      //nodal displacements
      const XC::Vector &ul = theNodes[j]->getTrialDisp( ) ;

      //compute the strain
      //strain += (BJ*ul) ;
      strain.addMatrixVector(1.0,  BJ,ul,1.0 ) ;

    } // end for j



    //send the strain to the material
    success= const_cast<NDMaterial *>(physicalProperties[i])->setTrialStrain( strain ) ;

    //compute the stress
    stress = physicalProperties[i]->getStress( ) ;


    //multiply by volume element
    stress  *= dvol[i] ;

    if( tang_flag == 1 ) {
      dd = physicalProperties[i]->getTangent( ) ;
      dd *= dvol[i] ;
    } //end if tang_flag


    //residual and tangent calculations node loops

    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeBbar( j, shp, shpBar ) ;

      //transpose
      //BJtran = transpose( nstress, ndf, BJ ) ;
      for(p=0; p<ndf; p++) {
        for(q=0; q<nstress; q++)
          BJtran(p,q) = BJ(q,p) ;
      }//end for p


      //residual
      //residJ = BJtran * stress ;
      residJ.addMatrixVector(0.0,  BJtran,stress,1.0);

      //residual
      for( p = 0; p < ndf; p++ ) {
        resid( jj + p ) += residJ(p)  ;
            resid( jj + p ) -= dvol[i]*bf[p]*shp[3][j];
      }

      if( tang_flag == 1 ) {

        //BJtranD = BJtran * dd ;
        BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0);

         kk = 0 ;
         for( k = 0; k < numberNodes; k++ ) {

            BK = computeBbar( k, shp, shpBar ) ;


            //stiffJK =  BJtranD * BK  ;
            stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0) ;

            for( p = 0; p < ndf; p++ )  {
               for( q = 0; q < ndf; q++ )
                  stiff( jj+p, kk+q ) += stiffJK( p, q ) ;
            } //end for p

            kk += ndf ;
          } // end for k loop

      } // end if tang_flag

      jj += ndf ;
    } // end for j loop


  } //end for i gauss loop


  return ;
}


//************************************************************************
//compute local coordinates and basis

void XC::BbarBrick::computeBasis(void) const
  {
    //nodal coordinates
    int i ;
    for( i = 0; i < 8; i++ )
      {
        const XC::Vector &coorI = theNodes[i]->getCrds( ) ;
        xl[0][i] = coorI(0) ;
        xl[1][i] = coorI(1) ;
        xl[2][i] = coorI(2) ;
      }  //end for i
  }

//*************************************************************************
//compute B

const XC::Matrix &XC::BbarBrick::computeBbar( int node,
                                 const double shp[4][8],
                                 const double shpBar[4][8]) const
{

  static XC::Matrix Bbar(6,3) ;

  //static XC::Matrix Bdev(3,3) ;
  static double Bdev[3][3] ;

  //static XC::Matrix BbarVol(3,3) ;
  static double BbarVol[3][3] ;

  static const double one3 = 1.0/3.0 ;


//---B Matrices in standard {1,2,3} mechanics notation---------
//
//                -                        -
//               |  2N,1    -N,2     -N,3   |
// Bdev =  (1/3) |  -N,1    2N,2     -N,3   |  (3x3)
//               |  -N,1    -N,2     2N,3   |
//                -                        -
//
//                -                       -
//               |  N,1      N,2     N,3   |
// Bvol =  (1/3) |  N,1      N,2     N.3   |  (3x3)
//               |  N,1      N,2     N,3   |
//                -                       -
//
//                -                   -
//               |                     |
//               |    Bdev + Bvol      |
//   B       =   |                     |
//               |---------------------|   (6x3)
//               | N,2     N,1     0   |
//               |   0     N,3    N,2  |
//               | N,3      0     N,1  |
//                -                   -
//
//---------------------------------------------------------------


  Bbar.Zero( ) ;

  //deviatoric
  Bdev[0][0] = 2.0*shp[0][node] ;
  Bdev[0][1] =    -shp[1][node] ;
  Bdev[0][2] =    -shp[2][node] ;

  Bdev[1][0] =    -shp[0][node] ;
  Bdev[1][1] = 2.0*shp[1][node] ;
  Bdev[1][2] =    -shp[2][node] ;

  Bdev[2][0] =    -shp[0][node] ;
  Bdev[2][1] =    -shp[1][node] ;
  Bdev[2][2] = 2.0*shp[2][node] ;

  //volumetric
  BbarVol[0][0] = shpBar[0][node] ;
  BbarVol[0][1] = shpBar[1][node] ;
  BbarVol[0][2] = shpBar[2][node] ;

  BbarVol[1][0] = shpBar[0][node] ;
  BbarVol[1][1] = shpBar[1][node] ;
  BbarVol[1][2] = shpBar[2][node] ;

  BbarVol[2][0] = shpBar[0][node] ;
  BbarVol[2][1] = shpBar[1][node] ;
  BbarVol[2][2] = shpBar[2][node] ;



  //extensional terms
  for( int i=0; i<3; i++ ){
    for( int j=0; j<3; j++ )
      Bbar(i,j) = one3*( Bdev[i][j] + BbarVol[i][j] ) ;
  }//end for i


  //shear terms
  Bbar(3,0) = shp[1][node] ;
  Bbar(3,1) = shp[0][node] ;

  Bbar(4,1) = shp[2][node] ;
  Bbar(4,2) = shp[1][node] ;

  Bbar(5,0) = shp[2][node] ;
  Bbar(5,2) = shp[0][node] ;

  return Bbar ;

}

//***********************************************************************

 XC::Matrix  XC::BbarBrick::transpose( int dim1,
                                       int dim2,
                                       const XC::Matrix &M )
{
  int i ;
  int j ;

  Matrix Mtran( dim2, dim1 ) ;

  for( i = 0; i < dim1; i++ ) {
     for( j = 0; j < dim2; j++ )
         Mtran(j,i) = M(i,j) ;
  } // end for i

  return Mtran ;
}

//**********************************************************************



//! @brief Send members del objeto through the channel being passed as parameter.
int XC::BbarBrick::sendData(CommParameters &cp)
  {
    int res= BrickBase::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::BbarBrick::recvData(const CommParameters &cp)
  {
    int res= BrickBase::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int  XC::BbarBrick::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(15);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int  XC::BbarBrick::recvSelf(const CommParameters &cp)
  {
    inicComm(15);
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


XC::Response *XC::BbarBrick::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
{
  if(argv[0] == "force" || argv[0] == "forces")
    return new XC::ElementResponse(this, 1, resid);

  else if(argv[0] == "stiff" || argv[0] == "stiffness")
    return new XC::ElementResponse(this, 2, stiff);

  else if(argv[0] == "material" || argv[0] == "integrPoint") {
    int pointNum = atoi(argv[1]);
    if(pointNum > 0 && pointNum <= 8)
      return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
    else
      return 0;
  } else if(argv[0] == "stresses") {
    return new XC::ElementResponse(this, 3, Vector(48));
  }

  // otherwise response quantity is unknown for the brick class
  else
    return 0;
}

int XC::BbarBrick::getResponse(int responseID, Information &eleInfo)
{
  static XC::Vector stresses(48);

  if(responseID == 1)
    return eleInfo.setVector(this->getResistingForce());

  else if(responseID == 2)
    return eleInfo.setMatrix(this->getTangentStiff());

  else if(responseID == 3) {

    // Loop over the integration points
    int cnt = 0;
    for(int i = 0; i < 8; i++) {

      // Get material stress response
      const XC::Vector &sigma = physicalProperties[i]->getStress();
      stresses(cnt++) = sigma(0);
      stresses(cnt++) = sigma(1);
      stresses(cnt++) = sigma(2);
      stresses(cnt++) = sigma(3);
      stresses(cnt++) = sigma(4);
      stresses(cnt++) = sigma(5);
    }
    return eleInfo.setVector(stresses);

  }
  else

    return -1;
}
