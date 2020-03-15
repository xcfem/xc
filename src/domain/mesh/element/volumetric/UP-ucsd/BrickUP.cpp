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

///////////////////////////////////////////////////////////////////////////////
// Description: This file contains the class declaration for                 //
// BrickUP, an 8-node cubic element for solid-fluid fully coupled analysis.  //
// This implementation is a simplified u-p formulation of Biot theory        //
// (u - solid displacement, p - fluid pressure). Each element node has three //
// DOFs for u and 1 DOF for p.                                               //
//                                                                           //
// Written by Zhaohui Yang        (March 2004)                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/BrickUP.cpp,v $

// by Zhaohui Yang (Modified based on Ed "C++" Love's XC::Brick element)
//
// Eight node XC::BrickUP element
//

#include "BrickUP.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/volumetric/brick/shp3d.h>
#include <utility/recorder/response/ElementResponse.h>
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"

namespace XC{
//static data

XC::Matrix  BrickUP::stiff(32,32) ;
XC::Vector  BrickUP::resid(32) ;
XC::Matrix  BrickUP::mass(32,32) ;
XC::Matrix  BrickUP::damp(32,32) ;

//quadrature data
const double  BrickUP::sg[] = { -one_over_root3,
                               one_over_root3  } ;

const double  BrickUP::wg[] = { 1.0, 1.0, 1.0, 1.0,
                              1.0, 1.0, 1.0, 1.0  } ;
} //namespace XC



//null constructor
XC::BrickUP::BrickUP(void)
  : BrickBase(ELE_TAG_BrickUP), rho_f(0), kc(0), Ki(0)
  {
    perm[0] = perm[1] = perm[2] = 0.;
  }


//*********************************************************************
//full constructor
XC::BrickUP::BrickUP(int tag,
                         int node1,int node2,
                            int node3, int node4,
                         int node5, int node6,
                         int node7, int node8,
                         NDMaterial &theMaterial, double bulk, double rhof,
                        double p1, double p2, double p3,
                   const BodyForces3D &bForces)
  : BrickBase(tag,ELE_TAG_BrickUP,node1,node2,node3,node4,node5,node6,node7,node8,NDMaterialPhysicalProperties(8,&theMaterial)), bf(bForces), rho_f(rhof),kc(bulk), Ki(nullptr)
  {
    // Permeabilities
    perm[0] = p1;
    perm[1] = p2;
    perm[2] = p3;
  }
//******************************************************************

//! @brief Virtual constructor.
XC::Element* XC::BrickUP::getCopy(void) const
  { return new BrickUP(*this); }


//destructor
XC::BrickUP::~BrickUP(void)
  {
    if(Ki) delete Ki;
  }


//set domain
void  XC::BrickUP::setDomain( Domain *theDomain )
  {
    BrickBase::setDomain(theDomain);
    theNodes.checkNumDOF(4,getTag());
  }


//return number of dofs
int XC::BrickUP::getNumDOF(void) const
  { return 32; }


//print out element data
void  XC::BrickUP::Print( std::ostream &s, int flag ) const
{

  if(flag == 2) {

    s << "#Brick\n";

    int i;
    const int numNodes = 8;
    const int nstress = 6 ;

    for(i=0; i<numNodes; i++) {
      const XC::Vector &nodeCrd = theNodes[i]->getCrds();
      const XC::Vector &nodeDisp = theNodes[i]->getDisp();
      s << "#NODE " << nodeCrd(0) << " " << nodeCrd(1) << " " << nodeCrd(2)
        << " " << nodeDisp(0) << " " << nodeDisp(1) << " " << nodeDisp(2) << std::endl;
     }

    // spit out the section location & invoke print on the scetion
    static Vector avgStress(7);
    static Vector avgStrain(nstress);
    avgStress= physicalProperties.getCommittedAvgStress();
    avgStrain= physicalProperties.getCommittedAvgStrain();

    s << "#AVERAGE_STRESS ";
    for(i=0; i<7; i++)
      s << avgStress(i) << " " ;
    s << std::endl;

    s << "#AVERAGE_STRAIN ";
    for(i=0; i<nstress; i++)
      s << avgStrain(i) << " " ;
    s << std::endl;

    /*
    physicalProperties.Print(s,flag);
     */

  } else {

    s << std::endl ;
    s << "Eight XC::Node XC::BrickUP \n" ;
    s << "Element Number: " << this->getTag() << std::endl ;
    s << "Node 1 : " << theNodes.getTagNode(0) << std::endl ;
    s << "Node 2 : " << theNodes.getTagNode(1) << std::endl ;
    s << "Node 3 : " << theNodes.getTagNode(2) << std::endl ;
    s << "Node 4 : " << theNodes.getTagNode(3) << std::endl ;
    s << "Node 5 : " << theNodes.getTagNode(4) << std::endl ;
    s << "Node 6 : " << theNodes.getTagNode(5) << std::endl ;
    s << "Node 7 : " << theNodes.getTagNode(6) << std::endl ;
    s << "Node 8 : " << theNodes.getTagNode(7) << std::endl ;

    s << "Material XC::Information : \n " ;
    physicalProperties.Print( s, flag ) ;

    s << std::endl ;
  }
}


//return stiffness matrix
const XC::Matrix &XC::BrickUP::getTangentStiff(void) const
  {
    int tang_flag = 1 ; //get the tangent
    //do tangent and residual here
    formResidAndTangent( tang_flag ) ;
    if(isDead())
      stiff*=dead_srf;
    return stiff ;
  }


//return secant matrix
//const XC::Matrix&  XC::BrickUP::getSecantStiff( )

const XC::Matrix&  XC::BrickUP::getInitialStiff(void) const
  {
    if(!Ki)
      {

  //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31
  static const int ndm = 3 ;
  static const int ndf = 3 ;
  static const int ndff = 4 ;
  static const int nstress = 6 ;
  static const int numberNodes = 8 ;
  static const int numberGauss = 8 ;
  static const int nShape = 4 ;

  int i, j, k, p, q ;
  int jj, kk ;

  static double volume ;
  static double xsj ;  // determinant jacobian matrix
  static double dvol[numberGauss] ; //volume element
  static double gaussPoint[ndm] ;
  static XC::Vector strain(nstress) ;  //strain
  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point
  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions
  static XC::Matrix stiffJK(ndf,ndf) ; //nodeJK stiffness
  static XC::Matrix dd(nstress,nstress) ;  //material tangent


  //---------B-matrices------------------------------------

    static XC::Matrix BJ(nstress,ndf) ;      // B matrix node J

    static XC::Matrix BJtran(ndf,nstress) ;

    static XC::Matrix BK(nstress,ndf) ;      // B matrix node k

    static XC::Matrix BJtranD(ndf,nstress) ;

  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( ) ;

  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

  //gauss loop to compute and save shape functions

  int count = 0 ;
  volume = 0.0 ;

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

        //volume += dvol[count] ;

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


    dd = physicalProperties[i]->getInitialTangent( ) ;
    dd *= dvol[i] ;

    jj = 0;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeB( j, shp ) ;

      //transpose
      //BJtran = transpose( nstress, ndf, BJ ) ;
      for(p=0; p<ndf; p++) {
        for(q=0; q<nstress; q++)
          BJtran(p,q) = BJ(q,p) ;
      }//end for p

      //BJtranD = BJtran * dd ;
      BJtranD.addMatrixProduct(0.0,  BJtran, dd, 1.0) ;

      kk = 0 ;
      for( k = 0; k < numberNodes; k++ ) {

        BK = computeB( k, shp ) ;


        //stiffJK =  BJtranD * BK  ;
        stiffJK.addMatrixProduct(0.0,  BJtranD, BK, 1.0) ;

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            stiff( jj+p, kk+q ) += stiffJK( p, q ) ;
        } //end for p

        kk += ndff ;

      } // end for k loop

      jj += ndff ;

    } // end for j loop
  } //end for i gauss loop

  Ki = new Matrix(stiff);
      }
    if(isDead())
      stiff*=dead_srf;
    return stiff ;
  }


//return mass matrix
const XC::Matrix&  XC::BrickUP::getMass(void) const
  {
    int tangFlag = 1 ;
    formInertiaTerms( tangFlag ) ;
    if(isDead())
      mass*=dead_srf;
    return mass ;
  }


//return mass matrix
const XC::Matrix&  XC::BrickUP::getDamp(void) const
  {
    int tangFlag = 1 ;
    formDampingTerms( tangFlag ) ;
    if(isDead())
      damp*=dead_srf;
    return damp ;
  }

void XC::BrickUP::formDampingTerms(int tangFlag) const
{
  static const int ndm = 3 ;
  //static const int ndf = 3 ;
  static const int ndff = 4 ;
  static const int numberNodes = 8 ;
  static const int numberGauss = 8 ;
  static const int numberDOFs = 32 ;
  static const int nShape = 4 ;
  static double volume ;
  static double xsj ;  // determinant jacobian matrix
  static double dvol[numberGauss] ; //volume element
  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point
  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions
  static double gaussPoint[ndm] ;
  static XC::Vector a(ndff*numberNodes) ;

  int i, j, k, p, q, m, i1, j1;
  //int jj, kk ;
  //double temp, rhot, massJK ;


  //zero damp
  damp.Zero( ) ;

  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

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

        count++ ;

      } //end for k
    } //end for j
  } // end for i

  if(rayFactors.getBetaK() != 0.0)
    damp.addMatrix(1.0, this->getTangentStiff(), rayFactors.getBetaK());
  if(rayFactors.getBetaK0() != 0.0)
    damp.addMatrix(1.0, this->getInitialStiff(), rayFactors.getBetaK0());
  if(rayFactors.getBetaKc() != 0.0)
    damp.addMatrix(1.0, Kc, rayFactors.getBetaKc());


  if(rayFactors.getAlphaM() != 0.0) {
        this->getMass();
    for(i = 0; i < numberDOFs; i += ndff) {
      for(j = 0; j < numberDOFs; j += ndff) {
        damp(i,j) += mass(i,j)*rayFactors.getAlphaM();
        damp(i+1,j+1) += mass(i+1,j+1)*rayFactors.getAlphaM();
        damp(i+2,j+2) += mass(i+2,j+2)*rayFactors.getAlphaM();
          }
    }
  }

  // Compute coupling matrix
  for(i = 0; i < numberDOFs; i += ndff) {
    i1 = i / ndff;
    for(j = 3; j < numberDOFs; j += ndff) {
      j1 = (j-3) / ndff;
      for(m = 0; m < numberGauss; m++) {
            damp(i,j) += -dvol[m]*Shape[0][i1][m]*Shape[3][j1][m];
            damp(i+1,j) += -dvol[m]*Shape[1][i1][m]*Shape[3][j1][m];
            damp(i+2,j) += -dvol[m]*Shape[2][i1][m]*Shape[3][j1][m];
          }
      damp(j,i) = damp(i,j);
      damp(j,i+1) = damp(i+1,j);
      damp(j,i+2) = damp(i+2,j);
    }
  }

  // Compute permeability matrix
  for(i = 3; i < numberDOFs; i += ndff) {
    int i1 = (i-3) / ndff;
    for(j = 3; j < numberDOFs; j += ndff) {
      int j1 = (j-3) / ndff;
      for(m = 0; m < numberGauss; m++) {
            damp(i,j) -= dvol[m]*(perm[0]*Shape[0][i1][m]*Shape[0][j1][m] +
                                  perm[1]*Shape[1][i1][m]*Shape[1][j1][m]+
                                                  perm[2]*Shape[2][i1][m]*Shape[2][j1][m]);
          }
    }
  }

  if(tangFlag == 0) {
    for( k = 0; k < numberNodes; k++ ) {
      const XC::Vector &vel = theNodes[k]->getTrialVel();
          for( p = 0; p < ndff; p++ )
                  a( k*ndff+p ) = vel(p);
        } // end for k loop

    resid.addMatrixVector(1.0,damp,a,1.0);
  } // end if tang_flag

  return ;
}


int XC::BrickUP::addLoad(ElementalLoad *theLoad, double loadFactor)
{
  std::cerr << "XC::BrickUP::addLoad - load type unknown for truss with tag: " << this->getTag() << std::endl;
  return -1;
}

int XC::BrickUP::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
  static const int numberNodes = 8 ;
  //static const int numberGauss = 8 ;
  static const int ndf = 3 ;
  static const int ndff = 4 ;

  int i;

  // Compute mass matrix
  int tangFlag = 1 ;
  formInertiaTerms( tangFlag ) ;

  // store computed RV from nodes in resid vector
  int count = 0;
  for(i=0; i<numberNodes; i++) {
    const XC::Vector &Raccel = theNodes[i]->getRV(accel);
    for(int j=0; j<ndf; j++)
      resid(count++) = Raccel(j);

        resid(count++) = 0.0;
  }

  // create the load vector if one does not exist
  if(load.isEmpty())
    load.reset(numberNodes*ndff);

  // add -M * RV(accel) to the load vector
  load.addMatrixVector(1.0, mass, resid, -1.0);

  return 0;
}


//get residual
const XC::Vector&  XC::BrickUP::getResistingForce(void) const
  {
    int tang_flag = 0 ; //don't get the tangent
    formResidAndTangent( tang_flag ) ;

    if(!load.isEmpty())
      resid-= load;
    if(isDead())
      resid*=dead_srf;
    return resid ;
  }


//get residual with inertia terms
const XC::Vector&  XC::BrickUP::getResistingForceIncInertia(void) const
  {
    static Vector res(32);

    int tang_flag = 0 ; //don't get the tangent

    //do tangent and residual here
    formResidAndTangent( tang_flag ) ;
    formInertiaTerms( tang_flag ) ;
    formDampingTerms( tang_flag ) ;

    res = resid;

    if(!load.isEmpty())
      res -= load;
    if(isDead())
      res*=dead_srf;
    return res;
  }


//*********************************************************************
//form inertia terms

void   XC::BrickUP::formInertiaTerms(int tangFlag) const
{
  static const int ndm = 3 ;
  static const int ndf = 3 ;
  static const int ndff = 4 ;
  static const int numberNodes = 8 ;
  static const int numberGauss = 8 ;
  static const int nShape = 4 ;
  static const int massIndex = nShape - 1 ;
  static double volume ;
  static double xsj ;  // determinant jacobian matrix
  static double dvol[numberGauss] ; //volume element
  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point
  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions
  static double gaussPoint[ndm] ;
  static XC::Vector a(ndff*numberNodes) ;

  int i, j, k, p, q ;
  int jj, kk ;

  double temp, rhot, massJK ;


  //zero mass
  mass.Zero( ) ;

  //compute basis vectors and local nodal coordinates
  computeBasis( ) ;

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

    // average material density
      rhot = mixtureRho(i);

    //mass and compressibility calculations node loops
    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      temp = shp[massIndex][j] * dvol[i] ;

         //multiply by density
         temp *= rhot ;

         //node-node mass
         kk = 0 ;
         for( k = 0; k < numberNodes; k++ ) {

            massJK = temp * shp[massIndex][k] ;

            for( p = 0; p < ndf; p++ )
                  mass( jj+p, kk+p ) += massJK ;

            // Compute compressibility terms
            mass( jj+3, kk+3 ) += -dvol[i]*Shape[3][j][i]*Shape[3][k][i]/kc;

            kk += ndff ;
          } // end for k loop

      jj += ndff ;
    } // end for j loop

  } //end for i gauss loop

  if( tangFlag == 0 ) {
    for( k = 0; k < numberNodes; k++ ) {
      const XC::Vector &acc = theNodes[k]->getTrialAccel();
          for( p = 0; p < ndff; p++ )
                  a( k*ndff+p ) = acc(p);
        } // end for k loop

    resid.addMatrixVector(1.0,mass,a,1.0);
  } // end if tang_flag
}

//*********************************************************************
//form residual and tangent
void  XC::BrickUP::formResidAndTangent(int tang_flag) const
{

  //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

  static const int ndm = 3 ;
  static const int ndf = 3 ;
  static const int ndff = 4 ;
  static const int nstress = 6 ;
  static const int numberNodes = 8 ;
  static const int numberGauss = 8 ;
  static const int nShape = 4 ;

  int i, j, k, p, q ;
  int jj, kk ;

  int success ;

  static double volume ;
  static double xsj ;  // determinant jacobian matrix
  static double dvol[numberGauss] ; //volume element
  static double gaussPoint[ndm] ;
  static XC::Vector strain(nstress) ;  //strain
  static double shp[nShape][numberNodes] ;  //shape functions at a gauss point
  static double Shape[nShape][numberNodes][numberGauss] ; //all the shape functions
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

  //gauss loop to compute and save shape functions

  int count = 0 ;
  volume = 0.0 ;

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

        //volume += dvol[count] ;

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


    double rhot= 0.0;

    //zero the strains
    strain.Zero( ) ;

    // j-node loop to compute strain
    for( j = 0; j < numberNodes; j++ )  {

      //compute B matrix
      BJ = computeB( j, shp ) ;

      //nodal displacements
      const Vector &ul = theNodes[j]->getTrialDisp( ) ;
      Vector ul3(3);
          ul3(0) = ul(0);
      ul3(1) = ul(1);
          ul3(2) = ul(2);
      //compute the strain
      //strain += (BJ*ul) ;
      strain.addMatrixVector(1.0,BJ,ul3,1.0 ) ;

    } // end for j

    //send the strain to the material
    success= const_cast<NDMaterial *>(physicalProperties[i])->setTrialStrain(strain);


    //residual and tangent calculations node loops

    if( tang_flag == 1 ) {
      dd = physicalProperties[i]->getTangent( ) ;
      dd *= dvol[i] ;
    } //end if tang_flag


    if( tang_flag == 0 ) {
      //compute the stress
      stress = physicalProperties[i]->getStress( ) ;

      //multiply by volume element
      stress  *= dvol[i] ;

      rhot = mixtureRho(i);
    }

    jj = 0 ;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeB( j, shp ) ;

      //transpose
      //BJtran = transpose( nstress, ndf, BJ ) ;
      for(p=0; p<ndf; p++) {
            for(q=0; q<nstress; q++)
              BJtran(p,q) = BJ(q,p) ;
      }//end for p

      if( tang_flag == 0 ) {
        //residual
        //residJ = BJtran * stress ;
        residJ.addMatrixVector(0.0,  BJtran,stress,1.0);

        for( p = 0; p < ndf; p++ ) {
          resid( jj + p ) += residJ(p)  ;

          // Subtract equiv. body forces from the nodes
              resid( jj + p ) -= dvol[i]*rhot*bf[p]*Shape[3][j][i];
                }

        // Subtract fluid body force
                resid( jj + 3 ) += dvol[i]*rho_f*(perm[0]*bf[0]*Shape[0][j][i] +
                                        perm[1]*bf[1]*Shape[1][j][i] +
                                                                                perm[2]*bf[2]*Shape[2][j][i]);
      } // end if tang_flag

      if( tang_flag == 1 ) {
             //BJtranD = BJtran * dd ;
             BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0) ;

         kk = 0 ;
         for( k = 0; k < numberNodes; k++ ) {

            BK = computeB( k, shp ) ;

            //stiffJK =  BJtranD * BK  ;
                stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0) ;

            for( p = 0; p < ndf; p++ )  {
               for( q = 0; q < ndf; q++ )
                  stiff( jj+p, kk+q ) += stiffJK( p, q ) ;
            } //end for p

            kk += ndff ;
          } // end for k loop

      } // end if tang_flag

      jj += ndff ;
    } // end for j loop

  } //end for i gauss loop


  return ;
}

double XC::BrickUP::mixtureRho(int i) const
  {
    const double rhoi= physicalProperties[i]->getRho();
    //double e = 0.7;  //physicalProperties[i]->getVoidRatio();
    //n = e / (1.0 + e);
    //return n * rho_f + (1.0-n) * rhoi;
    return rhoi;
  }

//*************************************************************************
//compute B

const XC::Matrix &XC::BrickUP::computeB( int node, const double shp[4][8] ) const
{

  static XC::Matrix B(6,3) ;

//---B XC::Matrix in standard {1,2,3} mechanics notation---------
//
//                -                   -
//               | N,1      0     0    |
//   B       =   |   0     N,2    0    |
//               |   0      0     N,3  |   (6x3)
//               | N,2     N,1     0   |
//               |   0     N,3    N,2  |
//               | N,3      0     N,1  |
//                -                   -
//
//-------------------------------------------------------------------


  B.Zero( ) ;

  B(0,0) = shp[0][node] ;
  B(1,1) = shp[1][node] ;
  B(2,2) = shp[2][node] ;

  B(3,0) = shp[1][node] ;
  B(3,1) = shp[0][node] ;

  B(4,1) = shp[2][node] ;
  B(4,2) = shp[1][node] ;

  B(5,0) = shp[2][node] ;
  B(5,2) = shp[0][node] ;

  return B ;

}

//***********************************************************************

 XC::Matrix  XC::BrickUP::transpose( int dim1,
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

//! @brief Send object members through the channel being passed as parameter.
int XC::BrickUP::sendData(CommParameters &cp)
  {
    int res= BrickBase::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],bf[2],rho_f,kc,getDbTagData(),CommMetaData(8));
    res+= cp.sendDoubles(perm[0],perm[1],perm[2],getDbTagData(),CommMetaData(9));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(10,11,12,13));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::BrickUP::recvData(const CommParameters &cp)
  {
    int res= BrickBase::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],bf[2],rho_f,kc,getDbTagData(),CommMetaData(8));
    res+= cp.receiveDoubles(perm[0],perm[1],perm[2],getDbTagData(),CommMetaData(9));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(10,11,12,13));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int  XC::BrickUP::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(14);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int  XC::BrickUP::recvSelf(const CommParameters &cp)
  {
    inicComm(14);
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


XC::Response *XC::BrickUP::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, resid);
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, stiff);
    else if(argv[0] == "mass")
      return new ElementResponse(this, 3, mass);
    else if(argv[0] == "damp")
      return new ElementResponse(this, 4, damp);
    else if(argv[0] == "material" || argv[0] == "integrPoint")
      {
        const int pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= 8)
          return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
        else
          return 0;
      }
    else if(argv[0] == "stresses")
      { return new ElementResponse(this, 5, Vector(48)); }
    // otherwise response quantity is unknown for the XC::BrickUP class
    else
      return 0;
  }

int XC::BrickUP::getResponse(int responseID, Information &eleInfo)
  {
    static XC::Vector stresses(48);
    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());
    else if(responseID == 2)
      return eleInfo.setMatrix(this->getTangentStiff());
    else if(responseID == 3)
      return eleInfo.setMatrix(this->getMass());
    else if(responseID == 4)
      return eleInfo.setMatrix(this->getDamp());
    else if(responseID == 5)
      {
        // Loop over the integration points
        int cnt = 0;
        for(int i = 0; i < 8; i++)
          {
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
