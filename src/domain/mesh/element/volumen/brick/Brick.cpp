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

// $Revision: 1.24 $
// $Date: 2006/01/10 18:41:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/brick/Brick.cpp,v $

// Ed "C++" Love
//
// Eight node Brick element
//

#include "Brick.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/volumen/brick/shp3d.h>
#include <utility/recorder/response/ElementResponse.h>

#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"

//static data
double  XC::Brick::xl[3][8] ;

XC::Matrix  XC::Brick::stiff(24,24) ;
XC::Vector  XC::Brick::resid(24) ;
XC::Matrix  XC::Brick::mass(24,24) ;


//quadrature data

const double  XC::Brick::sg[] = { -one_over_root3,
                               one_over_root3  } ;

const double  XC::Brick::wg[] = { 1.0, 1.0, 1.0, 1.0,
                              1.0, 1.0, 1.0, 1.0  } ;


static XC::Matrix B(6,3) ;

const int brick_nstress= 6;

//! @brief null constructor
XC::Brick::Brick(void)
  :BrickBase(ELE_TAG_Brick), Ki(nullptr)
  {
    B.Zero();
  }


//! @brief full constructor
XC::Brick::Brick(int tag,const NDMaterial *ptr_mat)
  :BrickBase(tag, ELE_TAG_Brick,NDMaterialPhysicalProperties(8,ptr_mat)), Ki(nullptr)
  {
    B.Zero();
  }

//*********************************************************************
//! @brief full constructor
XC::Brick::Brick( int tag, int node1,int node2,int node3,int node4,int node5,int node6,int node7,int node8, NDMaterial &theMaterial,const BodyForces3D &bForces)
  :BrickBase(tag,ELE_TAG_Brick,node1,node2,node3,node4,node5,node6,node7,node8,NDMaterialPhysicalProperties(8,&theMaterial)), bf(bForces), Ki(nullptr)
  {
    B.Zero();
  }
//******************************************************************


//! @brief Virtual constructor.
XC::Element* XC::Brick::getCopy(void) const
  { return new Brick(*this); }


//! @brief destructor
XC::Brick::~Brick(void)
  {
    if(Ki)
      {
        delete Ki;
        Ki= nullptr;
      }
  }


//! @brief set domain
void  XC::Brick::setDomain(Domain *theDomain)
  {
    BrickBase::setDomain(theDomain);
  }


//! @brief return number of dofs
int XC::Brick::getNumDOF(void) const
  { return 24 ; }

//! @brief Return the tensión media in the element.
XC::Vector XC::Brick::getAvgStress(void) const
  { return physicalProperties.getCommittedAvgStress(); }

//! Returns the index of the (i,j) component of the strain tensor
//! (resp. stress tensor) in the vector that stores these components.
size_t XC::Brick::getVectorIndex(const size_t &i,const size_t &j)
  {
    size_t retval= 0;
    if(i<=j)
      {
        if(i==j)
          retval= (i-1);
        else if(i==1)
          retval= (j==2) ? 3 : 5;
        else
          retval= 4;
      }
    else
      retval= getVectorIndex(j,i);
    return retval;
  }

//! @brief Return the componente (i,j) de la tensión media in the element.
double XC::Brick::getAvgStress(const size_t &i,const size_t &j) const
  {
    const size_t iComp= getVectorIndex(i,j);
    return physicalProperties.getCommittedAvgStress(iComp);
  }

//! @brief Return the average strain in the element.
XC::Vector XC::Brick::getAvgStrain(void) const
  { return physicalProperties.getCommittedAvgStrain(); }

//! @brief Return the componente (i,j) de la average strain in the element.
double XC::Brick::getAvgStrain(const size_t &i,const size_t &j) const
  {
    const size_t iComp= getVectorIndex(i,j);
    return physicalProperties.getCommittedAvgStress(iComp);
  }

//print out element data
void  XC::Brick::Print( std::ostream &s, int flag )
{

  if(flag == 2) {

    s << "#Brick\n";

    int i;
    const int numNodes = 8;

    for(i=0; i<numNodes; i++)
      {
        const Vector &nodeCrd = theNodes[i]->getCrds();
        const Vector &nodeDisp = theNodes[i]->getDisp();
        s << "#NODE " << nodeCrd(0) << " " << nodeCrd(1) << " " << nodeCrd(2)
          << " " << nodeDisp(0) << " " << nodeDisp(1) << " " << nodeDisp(2) << std::endl;
     }

    // spit out the section location & invoke print on the scetion
    static Vector avgStress(brick_nstress);
    static Vector avgStrain(brick_nstress);
    avgStress= physicalProperties.getCommittedAvgStress();
    avgStrain= physicalProperties.getCommittedAvgStrain();

    s << "#AVERAGE_STRESS ";
    for(i=0; i<brick_nstress; i++)
      s << avgStress(i) << " ";
    s << std::endl;

    s << "#AVERAGE_STRAIN ";
    for(i=0; i<brick_nstress; i++)
      s << avgStrain(i) << " ";
    s << std::endl;

    /*
    physicalProperties.Print(s,flag);
    */

  } else {

    s << "Standard Eight Node XC::Brick \n";
    s << "Element Number: " << this->getTag() << std::endl;
    s << "Nodes: " << theNodes;

    s << "Material XC::Information : \n ";
    physicalProperties.Print( s, flag );

    s << std::endl;
    s << this->getTag() << " " << theNodes.getTagNode(0)
      << " " << theNodes.getTagNode(1)
          << " " <<theNodes.getTagNode(2)
          << " " <<theNodes.getTagNode(3)
          << " " <<theNodes.getTagNode(4)
          << " " <<theNodes.getTagNode(5)
          << " " <<theNodes.getTagNode(6)
          << " " <<theNodes.getTagNode(7)
      << std::endl;

    s << "Resisting Force (no inertia): " << this->getResistingForce();
  }
}


//return stiffness matrix
const XC::Matrix&  XC::Brick::getTangentStiff(void) const
  {
    int tang_flag = 1; //get the tangent
    //do tangent and residual here
    formResidAndTangent( tang_flag );
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }


//return secant matrix
//const XC::Matrix&  XC::Brick::getSecantStiff( )

const XC::Matrix&  XC::Brick::getInitialStiff(void) const
  {
    if(!Ki)
      {

  //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31
  static const int ndm = 3;
  static const int ndf = 3;
  static const int numberNodes = 8;
  static const int numberGauss = 8;
  static const int nShape = 4;

  int i, j, k, p, q;
  int jj, kk;


  static double volume;
  static double xsj;  // determinant jacaobian matrix
  static double dvol[numberGauss]; //volume element
  static double gaussPoint[ndm];
  static Vector strain(brick_nstress);  //strain
  static double shp[nShape][numberNodes];  //shape functions at a gauss point
  static double Shape[nShape][numberNodes][numberGauss]; //all the shape functions
  static Matrix stiffJK(ndf,ndf); //nodeJK stiffness
  static Matrix dd(brick_nstress,brick_nstress);  //material tangent


  //---------B-matrices------------------------------------

    static XC::Matrix BJ(brick_nstress,ndf);      // B matrix node J

    static XC::Matrix BJtran(ndf,brick_nstress);

    static XC::Matrix BK(brick_nstress,ndf);      // B matrix node k

    static XC::Matrix BJtranD(ndf,brick_nstress);

  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( );

  //compute basis vectors and local nodal coordinates
  computeBasis( );

  //gauss loop to compute and save shape functions

  int count = 0;
  volume = 0.0;

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i];
        gaussPoint[1] = sg[j];
        gaussPoint[2] = sg[k];

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl );

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q];
        } // end for p


        //volume element to also be saved
        dvol[count] = wg[count] * xsj;

        //volume += dvol[count];

        count++;

      } //end for k
    } //end for j
  } // end for i


  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i];
    } // end for p


    dd = physicalProperties[i]->getInitialTangent( );
    dd *= dvol[i];

    jj = 0;
    for( j = 0; j < numberNodes; j++ ) {

      BJ = computeB( j, shp );

      //transpose
      //BJtran = transpose( brick_nstress, ndf, BJ );
      for(p=0; p<ndf; p++) {
        for(q=0; q<brick_nstress; q++)
          BJtran(p,q) = BJ(q,p);
      }//end for p

      //BJtranD = BJtran * dd;
      BJtranD.addMatrixProduct(0.0,  BJtran, dd, 1.0);

      kk = 0;
      for( k = 0; k < numberNodes; k++ ) {

        BK = computeB( k, shp );


        //stiffJK =  BJtranD * BK ;
        stiffJK.addMatrixProduct(0.0,  BJtranD, BK, 1.0);

        for( p = 0; p < ndf; p++ )  {
          for( q = 0; q < ndf; q++ )
            stiff( jj+p, kk+q ) += stiffJK( p, q );
        } //end for p

        kk += ndf;

      } // end for k loop

      jj += ndf;

    } // end for j loop
  } //end for i gauss loop

  Ki = new Matrix(stiff);
      }
    if(isDead())
      stiff*=dead_srf;
  return stiff;
}


//return mass matrix
const XC::Matrix &XC::Brick::getMass(void) const
  {
    int tangFlag = 1;
    formInertiaTerms(tangFlag);
    if(isDead())
      mass*=dead_srf;
    return mass;
  }



int XC::Brick::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::Brick::addLoad - load type unknown for truss with tag: " << this->getTag() << std::endl;
    return -1;
  }

int XC::Brick::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    static const int numberNodes = 8;
    static const int ndf = 3;

  int i;

  // check to see if have mass
  bool haveRho= physicalProperties.haveRho();

  if(!haveRho)
    return 0;

  // Compute mass matrix
  int tangFlag = 1;
  formInertiaTerms( tangFlag );

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
const XC::Vector &XC::Brick::getResistingForce(void) const
  {
    int tang_flag = 0; //don't get the tangent
    formResidAndTangent( tang_flag );
    if(!load.Nulo())
      resid-= load;
    if(isDead())
      resid*=dead_srf;
    return resid;
  }


//! @brief Get residual with inertia terms.
const XC::Vector &XC::Brick::getResistingForceIncInertia(void) const
  {
    static Vector res(24);

    int tang_flag = 0; //don't get the tangent

    //do tangent and residual here
    formResidAndTangent( tang_flag );
    formInertiaTerms( tang_flag );

    res = resid;

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();

    if(!load.Nulo())
      res-= load;
    if(isDead())
      res*=dead_srf;
    return res;
  }


//*********************************************************************
//form inertia terms

void XC::Brick::formInertiaTerms( int tangFlag ) const
  {

  static const int ndm = 3;

  static const int ndf = 3;

  static const int numberNodes = 8;

  static const int numberGauss = 8;

  static const int nShape = 4;

  static const int massIndex = nShape - 1;

  double xsj;  // determinant jacaobian matrix

  double dvol[numberGauss]; //volume element

  static double shp[nShape][numberNodes];  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss]; //all the shape functions

  static double gaussPoint[ndm];

  static XC::Vector momentum(ndf);

  int i, j, k, p, q;
  int jj, kk;

  double temp, rho, massJK;


  //zero mass
  mass.Zero( );

  //compute basis vectors and local nodal coordinates
  computeBasis( );

  //gauss loop to compute and save shape functions

  int count = 0;

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i];
        gaussPoint[1] = sg[j];
        gaussPoint[2] = sg[k];

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl );

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q];
        } // end for p


        //volume element to also be saved
        dvol[count] = wg[count] * xsj;

        count++;

      } //end for k
    } //end for j
  } // end for i



  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i];
    } // end for p


    //node loop to compute acceleration
    momentum.Zero( );
    for( j = 0; j < numberNodes; j++ )
      //momentum += shp[massIndex][j] * ( theNodes[j]->getTrialAccel()  );
      momentum.addVector( 1.0,
                          theNodes[j]->getTrialAccel(),
                          shp[massIndex][j] );


    //density
    rho = physicalProperties[i]->getRho();


    //multiply acceleration by density to form momentum
    momentum *= rho;


    //residual and tangent calculations node loops
    jj = 0;
    for( j = 0; j < numberNodes; j++ ) {

      temp = shp[massIndex][j] * dvol[i];

      for( p = 0; p < ndf; p++ )
        resid( jj+p ) += ( temp * momentum(p) ) ;


      if( tangFlag == 1 ) {

         //multiply by density
         temp *= rho;

         //node-node mass
         kk = 0;
         for( k = 0; k < numberNodes; k++ ) {

            massJK = temp * shp[massIndex][k];

            for( p = 0; p < ndf; p++ )
              mass( jj+p, kk+p ) += massJK;

            kk += ndf;
          } // end for k loop

      } // end if tang_flag

      jj += ndf;
    } // end for j loop


  } //end for i gauss loop

}

//*********************************************************************
//form residual and tangent
int
XC::Brick::update(void)
{

  //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

  static const int ndm = 3;

  static const int ndf = 3;

  static const int numberNodes = 8;

  static const int numberGauss = 8;

  static const int nShape = 4;

  int i, j, k, p, q;
  int success;

  static double volume;

  static double xsj;  // determinant jacaobian matrix

  static double dvol[numberGauss]; //volume element

  static double gaussPoint[ndm];

  static XC::Vector strain(brick_nstress);  //strain

  static double shp[nShape][numberNodes];  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss]; //all the shape functions

  //---------B-matrices------------------------------------

    static XC::Matrix BJ(brick_nstress,ndf);      // B matrix node J

    static XC::Matrix BJtran(ndf,brick_nstress);

    static XC::Matrix BK(brick_nstress,ndf);      // B matrix node k

    static XC::Matrix BJtranD(ndf,brick_nstress);

  //-------------------------------------------------------


  //compute basis vectors and local nodal coordinates
  computeBasis( );

  //gauss loop to compute and save shape functions

  int count = 0;
  volume = 0.0;

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i];
        gaussPoint[1] = sg[j];
        gaussPoint[2] = sg[k];

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl );

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q];
        } // end for p


        //volume element to also be saved
        dvol[count] = wg[count] * xsj;

        //volume += dvol[count];

        count++;

      } //end for k
    } //end for j
  } // end for i


  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i];
    } // end for p


    //zero the strains
    strain.Zero( );

    // j-node loop to compute strain
    for( j = 0; j < numberNodes; j++ )  {

      /**************** fmk - unwinding for performance
      //compute B matrix
      BJ = computeB( j, shp );

      //nodal displacements
      const XC::Vector &ul = theNodes[j]->getTrialDisp( );

      //compute the strain
      //strain += (BJ*ul);
      strain.addMatrixVector(1.0,  BJ,ul,1.0 );
      ***************************************************/


      //               | N,1      0     0    |
      //   B       =   |   0     N,2    0    |
      //               |   0      0     N,3  |   (6x3)
      //               | N,2     N,1     0   |
      //               |   0     N,3    N,2  |
      //               | N,3      0     N,1  |

      //      B(0,0) = shp[0][node];
      //      B(1,1) = shp[1][node];
      //      B(2,2) = shp[2][node];
      //      B(3,0) = shp[1][node];
      //      B(3,1) = shp[0][node];
      //      B(4,1) = shp[2][node];
      //      B(4,2) = shp[1][node];
      //      B(5,0) = shp[2][node];
      //      B(5,2) = shp[0][node];

      double b00 = shp[0][j];
      double b11 = shp[1][j];
      double b22 = shp[2][j];
      double b30 = shp[1][j];
      double b31 = shp[0][j];
      double b41 = shp[2][j];
      double b42 = shp[1][j];
      double b50 = shp[2][j];
      double b52 = shp[0][j];

      const XC::Vector &ul = theNodes[j]->getTrialDisp();

      double ul0 = ul(0);
      double ul1 = ul(1);
      double ul2 = ul(2);

      strain(0) += b00 * ul0;
      strain(1) += b11 * ul1;
      strain(2) += b22 * ul2;
      strain(3) += b30 * ul0 + b31 * ul1;
      strain(4) += b41 * ul1 + b42 * ul2;
      strain(5) += b50 * ul0 + b52 * ul2;

    } // end for j

    //send the strain to the material
    success = physicalProperties[i]->setTrialStrain( strain );

  } //end for i gauss loop

  return 0;
}


//*********************************************************************
//form residual and tangent
void  XC::Brick::formResidAndTangent( int tang_flag ) const
{

  //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

  static const int ndm = 3;

  static const int ndf = 3;

  static const int numberNodes = 8;

  static const int numberGauss = 8;

  static const int nShape = 4;

  int i, j, k, p, q;

  //int success;

  static double volume;

  static double xsj;  // determinant jacaobian matrix

  static double dvol[numberGauss]; //volume element

  static double gaussPoint[ndm];

  static double shp[nShape][numberNodes];  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss]; //all the shape functions

  static XC::Vector residJ(ndf); //nodeJ residual

  static XC::Matrix stiffJK(ndf,ndf); //nodeJK stiffness

  static XC::Vector stress(brick_nstress);  //stress

  static XC::Matrix dd(brick_nstress,brick_nstress);  //material tangent


  //---------B-matrices------------------------------------

    static XC::Matrix BJ(brick_nstress,ndf);      // B matrix node J

    static XC::Matrix BJtran(ndf,brick_nstress);

    static XC::Matrix BK(brick_nstress,ndf);      // B matrix node k

    static XC::Matrix BJtranD(ndf,brick_nstress);

  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( );
  resid.Zero( );

  //compute basis vectors and local nodal coordinates
  computeBasis( );

  //gauss loop to compute and save shape functions

  int count = 0;
  volume = 0.0;

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {
      for( k = 0; k < 2; k++ ) {

        gaussPoint[0] = sg[i];
        gaussPoint[1] = sg[j];
        gaussPoint[2] = sg[k];

        //get shape functions
        shp3d( gaussPoint, xsj, shp, xl );

        //save shape functions
        for( p = 0; p < nShape; p++ ) {
          for( q = 0; q < numberNodes; q++ )
            Shape[p][q][count] = shp[p][q];
        } // end for p


        //volume element to also be saved
        dvol[count] = wg[count] * xsj;

        //volume += dvol[count];

        count++;

      } //end for k
    } //end for j
  } // end for i


  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //extract shape functions from saved array
    for( p = 0; p < nShape; p++ ) {
       for( q = 0; q < numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i];
    } // end for p


    //compute the stress
    stress = physicalProperties[i]->getStress( );


    //multiply by volume element
    stress  *= dvol[i];

    if( tang_flag == 1 ) {
      dd = physicalProperties[i]->getTangent( );
      dd *= dvol[i];
    } //end if tang_flag


    double stress0 = stress(0);
    double stress1 = stress(1);
    double stress2 = stress(2);
    double stress3 = stress(3);
    double stress4 = stress(4);
    double stress5 = stress(5);

    //residual and tangent calculations node loops

    int jj = 0;
    for( j = 0; j < numberNodes; j++ ) {

      /* ************** fmk - unwinding for performance
      ************************************************* */

      //               | N,1      0     0    |
      //   B       =   |   0     N,2    0    |
      //               |   0      0     N,3  |   (6x3)
      //               | N,2     N,1     0   |
      //               |   0     N,3    N,2  |
      //               | N,3      0     N,1  |

      //      B(0,0) = shp[0][node];
      //      B(1,1) = shp[1][node];
      //      B(2,2) = shp[2][node];
      //      B(3,0) = shp[1][node];
      //      B(3,1) = shp[0][node];
      //      B(4,1) = shp[2][node];
      //      B(4,2) = shp[1][node];
      //      B(5,0) = shp[2][node];
      //      B(5,2) = shp[0][node];

      double b00 = shp[0][j];
      double b11 = shp[1][j];
      double b22 = shp[2][j];
      double b30 = shp[1][j];
      double b31 = shp[0][j];
      double b41 = shp[2][j];
      double b42 = shp[1][j];
      double b50 = shp[2][j];
      double b52 = shp[0][j];

      residJ(0) = b00 * stress0 + b30 * stress3 + b50 * stress5;
      residJ(1) = b11 * stress1 + b31 * stress3 + b41 * stress4;
      residJ(2) = b22 * stress2 + b42 * stress4 + b52 * stress5;

      BJ = computeB( j, shp );

      //transpose
      //BJtran = transpose( brick_nstress, ndf, BJ );
      for(p=0; p<ndf; p++) {
        for(q=0; q<brick_nstress; q++)
          BJtran(p,q) = BJ(q,p);
      }//end for p


      //residual
      for( p = 0; p < ndf; p++ )
        {
          resid( jj + p ) += residJ(p) ;
            resid( jj + p ) -= dvol[i]*bf[p]*shp[3][j];
        }

      if( tang_flag == 1 ) {

        //BJtranD = BJtran * dd;
        BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0);

        int kk = 0;
         for( k = 0; k < numberNodes; k++ ) {

            BK = computeB( k, shp );


            //stiffJK =  BJtranD * BK ;
            stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0);

            for( p = 0; p < ndf; p++ )  {
               for( q = 0; q < ndf; q++ )
                  stiff( jj+p, kk+q ) += stiffJK( p, q );
            } //end for p

            kk += ndf;
          } // end for k loop

      } // end if tang_flag

      jj += ndf;
    } // end for j loop


  } //end for i gauss loop


  return;
}


//************************************************************************
//compute local coordinates and basis

void XC::Brick::computeBasis(void) const
  {
    //nodal coordinates
    int i;
    for( i = 0; i < 8; i++ )
      {
         const XC::Vector &coorI = theNodes[i]->getCrds( );
         xl[0][i] = coorI(0);
         xl[1][i] = coorI(1);
         xl[2][i] = coorI(2);
      }  //end for i
  }

//*************************************************************************
//compute B

const XC::Matrix &XC::Brick::computeB( int node, const double shp[4][8]) const
{

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

  B(0,0) = shp[0][node];
  B(1,1) = shp[1][node];
  B(2,2) = shp[2][node];

  B(3,0) = shp[1][node];
  B(3,1) = shp[0][node];

  B(4,1) = shp[2][node];
  B(4,2) = shp[1][node];

  B(5,0) = shp[2][node];
  B(5,2) = shp[0][node];

  return B;

}

//***********************************************************************

 XC::Matrix  XC::Brick::transpose( int dim1,
                                       int dim2,
                                       const Matrix &M )
{
  int i;
  int j;

  Matrix Mtran( dim2, dim1 );

  for( i = 0; i < dim1; i++ ) {
     for( j = 0; j < dim2; j++ )
         Mtran(j,i) = M(i,j);
  } // end for i

  return Mtran;
}

//! @brief Send members through the channel being passed as parameter.
int XC::Brick::sendData(CommParameters &cp)
  {
    int res= BrickBase::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Brick::recvData(const CommParameters &cp)
  {
    int res= BrickBase::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Brick::sendSelf(CommParameters &cp)
  {
    inicComm(15);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Brick::sendSelf -- could not send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Brick::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    ID data(15);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Brick::recvSelf() - failed to receive ID data.\n";
    else
      res+= recvData(cp);
    return res;
  }


XC::Response *XC::Brick::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
{
  if(argv[0] == "force" || argv[0] == "forces")
    return new ElementResponse(this, 1, resid);

  else if(argv[0] == "stiff" || argv[0] == "stiffness")
    return new ElementResponse(this, 2, stiff);

  else if(argv[0] == "material" || argv[0] == "integrPoint") {
    int pointNum = atoi(argv[1]);
    if(pointNum > 0 && pointNum <= 8)
      return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
    else
      return 0;
  } else if(argv[0] == "stresses") {
    return new ElementResponse(this, 3, Vector(48));
  }

  // otherwise response quantity is unknown for the brick class
  else
    return 0;
}

int XC::Brick::getResponse(int responseID, Information &eleInfo)
  {
    static XC::Vector stresses(48);
    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());
    else if(responseID == 2)
      return eleInfo.setMatrix(this->getTangentStiff());
    else if(responseID == 3)
      {
        // Loop over the integration points
        int cnt = 0;
        for(int i = 0; i < 8; i++)
          {
            // Get material stress response
            const Vector &sigma = physicalProperties[i]->getStress();
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

