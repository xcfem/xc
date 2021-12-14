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

// $Revision: 1.10 $
// $Date: 2003/02/14 23:01:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/NineNodeMixedQuad.cpp,v $

// Ed "C++" Love
//
// Mixed Presssure/Volume Nine Node Quadrilateral
// Plane Strain (NOT PLANE STRESS)
//

#include "NineNodeMixedQuad.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <domain/domain/Domain.h>
#include "utility/actor/actor/MatrixCommMetaData.h"



//static data
XC::Matrix  XC::NineNodeMixedQuad::stiff(18,18);
XC::Vector  XC::NineNodeMixedQuad::resid(18);
XC::Matrix  XC::NineNodeMixedQuad::mass(18,18);
double  XC::NineNodeMixedQuad::xl[2][9];

//quadrature data
double   XC::NineNodeMixedQuad::root06 = sqrt(0.6);
double   XC::NineNodeMixedQuad::sg[] = { -root06,   0.0,      root06  };
double   XC::NineNodeMixedQuad::wg[] = {  5.0/9.0,  8.0/9.0,  5.0/9.0 };


//null constructor
XC::NineNodeMixedQuad::NineNodeMixedQuad(void)
  : ElemWithMaterial<9,NDMaterialPhysicalProperties>(0,ELE_TAG_NineNodeMixedQuad,NDMaterialPhysicalProperties(9,nullptr)), Ki(0)
  {}


//full constructor
XC::NineNodeMixedQuad::NineNodeMixedQuad(int tag,int node1,int node2,int node3,int node4,
                                         int node5,int node6,int node7,int node8,int node9,
                                         NDMaterial &theMat)
  :ElemWithMaterial<9,NDMaterialPhysicalProperties>(tag, ELE_TAG_NineNodeMixedQuad,NDMaterialPhysicalProperties(9,nullptr)), Ki(0)
  {
    theNodes.set_id_nodes(node1,node2,node3,node4,node5,node6,node7,node8,node9);
    physicalProperties.setMaterial(&theMat,"AxiSymmetric2D");
  }


//! @brief Virtual constructor.
XC::Element* XC::NineNodeMixedQuad::getCopy(void) const
  { return new NineNodeMixedQuad(*this); }

//! @brief destructor
XC::NineNodeMixedQuad::~NineNodeMixedQuad(void)
  {
    if(Ki) delete Ki;
  }


//set domain
void XC::NineNodeMixedQuad::setDomain(Domain *theDomain )
  {
    ElemWithMaterial<9,NDMaterialPhysicalProperties>::setDomain(theDomain);
  }


//return number of dofs
int XC::NineNodeMixedQuad::getNumDOF(void) const
  { return 18; }


//print out element data
void XC::NineNodeMixedQuad::Print( std::ostream &s, int flag ) const
  {
    s << std::endl;
    s << "Nine XC::Node Quad -- Mixed Pressure/Volume -- Plane Strain \n";
    s << "Element Number " << this->getTag() << std::endl;
    s << "Node 1 : " << theNodes.getTagNode(0) << std::endl;
    s << "Node 2 : " << theNodes.getTagNode(1) << std::endl;
    s << "Node 3 : " << theNodes.getTagNode(2) << std::endl;
    s << "Node 4 : " << theNodes.getTagNode(3) << std::endl;
    s << "Node 5 : " << theNodes.getTagNode(4) << std::endl;
    s << "Node 6 : " << theNodes.getTagNode(5) << std::endl;
    s << "Node 7 : " << theNodes.getTagNode(6) << std::endl;
    s << "Node 8 : " << theNodes.getTagNode(7) << std::endl;
    s << "Node 9 : " << theNodes.getTagNode(8) << std::endl;

    s << "Material XC::Information : \n ";

    physicalProperties[0]->Print( s, flag );

    s << std::endl;
  }


//return stiffness matrix
const XC::Matrix &XC::NineNodeMixedQuad::getTangentStiff(void) const
  {
    int tang_flag = 1; //get the tangent

    //do tangent and residual here
    formResidAndTangent( tang_flag );


    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }


//return secant matrix
const XC::Matrix &XC::NineNodeMixedQuad::getInitialStiff(void) const
  {
    if(!Ki)
      {

    static const int ndm = 2;
    static const int ndf = 2;
    static const int nstress = 4;
    static const int numberNodes = 9;
    static const int numberGauss = 9;
    static const int nShape = 3;
    static const int nMixed = 3;

    int i, j, k, p, q, r, s;
    int jj, kk;

    static double volume;
    static double xsj;  // determinant jacobian matrix
    static double dvol[numberGauss]; //volume element
    static double gaussPoint[ndm];
    static double natCoorArray[ndm][numberGauss];
    static XC::Vector strain(nstress);  //strain
    static double shp[nShape][numberNodes];  //shape functions at a gauss point
    static double Shape[nShape][numberNodes][numberGauss]; //all the shape functions
    static double shpBar[nShape][numberNodes][nMixed]; //mean value of shape functions
    static double rightHandSide[nShape][numberNodes][nMixed];
    static XC::Vector residJ(ndf); //nodeJ residual
    static XC::Matrix stiffJK(ndf,ndf); //nodeJK stiffness
    static XC::Vector stress(nstress);  //stress
    static XC::Matrix dd(nstress,nstress);  //material tangent
    static double interp[nMixed];
    static XC::Matrix Proj(3,3);   //projection matrix
    static XC::Matrix ProjInv(3,3);

    static XC::Matrix Iden(3,3);
    Iden(0,0) = 1.0;
    Iden(1,1) = 1.0;
    Iden(2,2) = 1.0;

    //---------B-matrices------------------------------------

      static XC::Matrix BJ(nstress,ndf);      // B matrix node J
      static XC::Matrix BJtran(ndf,nstress);
      static XC::Matrix BK(nstress,ndf);      // B matrix node k
      static XC::Matrix BJtranD(ndf,nstress);

    //-------------------------------------------------------


    //zero stiffness and residual
    stiff.Zero( );

    //node coordinates
    computeBasis();

    //zero mean shape functions
    for(p=0;p<nShape;p++)
      {
        for(q=0;q<numberNodes;q++)
          {
            for(r=0;r<nMixed;r++)
              {
                shpBar[p][q][r] = 0.0;
                rightHandSide[p][q][r] = 0.0;
              }
          }//end for q
      } // end for p

    //zero volume
    volume = 0.0;

    //zero projection matrix
    Proj.Zero( );
    ProjInv.Zero( );

    //gauss loop to compute and save shape functions
    int count = 0;
    for(i= 0;i<3;i++)
      {
        for(j= 0;j<3;j++)
          {
            gaussPoint[0] = sg[i];
            gaussPoint[1] = sg[j];

            //save gauss point locations
            natCoorArray[0][count] = gaussPoint[0];
            natCoorArray[1][count] = gaussPoint[1];


            //get shape functions
            shape2dNine( gaussPoint, xl, shp, xsj );

            //save shape functions
            for(p=0;p<nShape;p++)
              {
                for(q=0;q<numberNodes;q++)
                  Shape[p][q][count] = shp[p][q];
              } // end for p

            //volume element to also be saved
            dvol[count] = ( wg[i]*wg[j] ) * xsj;

            //add to projection matrix
            interp[0] = 1.0;
            interp[1] = gaussPoint[0];
            interp[2] = gaussPoint[1];

            for(r=0;r<nMixed;r++)
              {
                for(s=0;s<nMixed;s++)
                  Proj(r,s) += ( interp[r]*interp[s] * dvol[count] );
              }//end for r

            volume += dvol[count];

            //add to mean shape functions
            for(p=0;p<nShape;p++)
              {
                for(q=0;q<numberNodes;q++)
                  {
                    for(s=0;s<nMixed;s++)
                      rightHandSide[p][q][s] += ( shp[p][q] * interp[s] * dvol[count] );
                  }//end for q
              } // end for p
            //increment gauss point counter
            count++;
        } //end for j
      } // end for i

    //invert projection matrix
    //int Solve(const XC::Matrix &M, Matrix &res) const;
    Proj.Solve( Iden, ProjInv );

    //mean value of shape functions
    for(p=0;p<nShape;p++)
      {
        for(q=0;q<numberNodes;q++)
          {
            for(r=0;r<nMixed;r++)
              {
                for(s=0;s<nMixed;s++)
                  shpBar[p][q][r] += ( ProjInv(r,s) * rightHandSide[p][q][s] );
              }//end for r
          }//end for q
      }//end for p

    //gauss loop
    for(i=0;i<numberGauss;i++)
      {
        //extract gauss point location
        gaussPoint[0] = natCoorArray[0][i];
        gaussPoint[1] = natCoorArray[1][i];

        //extract shape functions from saved array
        for(p=0;p<nShape;p++)
          {
            for(q=0;q<numberNodes;q++)
              shp[p][q]  = Shape[p][q][i];
          } // end for p

        dd= physicalProperties[i]->getInitialTangent( );
        dd*= dvol[i];

        //residual and tangent calculations node loops
        jj = 0;
        for(j=0;j<numberNodes;j++)
          {
            BJ = computeBbar( j, gaussPoint, shp, shpBar);

            //transpose
            //BJtran = transpose( nstress, ndf, BJ );
            for(p=0;p<ndf;p++)
              {
                for(q=0;q<nstress;q++)
                  BJtran(p,q) = BJ(q,p);
              }//end for p

            //BJtranD = BJtran * dd;
            BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0);
            kk = 0;
            for(k=0;k<numberNodes;k++)
              {
                BK = computeBbar( k, gaussPoint, shp, shpBar );

                //stiffJK =  BJtranD * BK;
                stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0);
                for(p=0;p<ndf;p++)
                  {
                    for(q=0;q<ndf;q++)
                      stiff( jj+p, kk+q ) += stiffJK( p, q );
                  } //end for p
                kk += ndf;
              }//end for k loop
            jj += ndf;
          }//end for j loop
        }//end for i gauss loop
        Ki= new Matrix(stiff);
      }
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }


//! @brief return mass matrix
const XC::Matrix &XC::NineNodeMixedQuad::getMass(void) const
  {
    int tangFlag = 1;
    formInertiaTerms( tangFlag );
    if(isDead())
      mass*=dead_srf;
    return mass;
  }




int XC::NineNodeMixedQuad::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::NineNodeMixedQuad::addLoad - load type unknown for ele with tag: " << this->getTag() << std::endl;
    return -1;
  }

int XC::NineNodeMixedQuad::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    static const int numberNodes = 9;
    static const int ndf = 2;

    int i;

    // check to see if have mass
    const bool haveRho= physicalProperties.haveRho();
    if(!haveRho)
      return 0;

    // Compute mass matrix
    int tangFlag = 1;
    formInertiaTerms( tangFlag );

    // store computed RV from nodes in resid vector
    int count = 0;

    for(i=0;i<numberNodes;i++)
      {
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
const XC::Vector &XC::NineNodeMixedQuad::getResistingForce(void) const
  {
    int tang_flag = 0; //don't get the tangent
    formResidAndTangent( tang_flag );

    // subtract external loads
    if(!load.isEmpty())
      resid -= load;
    if(isDead())
      resid*=dead_srf;
    return resid;
  }


//get residual with inertia terms
const XC::Vector &XC::NineNodeMixedQuad::getResistingForceIncInertia(void) const
  {
    int tang_flag = 0; //don't get the tangent

    static Vector res(18);

    //do tangent and residual here
    formResidAndTangent( tang_flag );

    //inertia terms
    formInertiaTerms( tang_flag );

    res = resid;

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();

    // subtract external loads
    if(!load.isEmpty())
      res-= load;

    if(isDead())
      resid*=dead_srf;
    return res;
  }

//*****************************************************************************
//form inertia terms

void XC::NineNodeMixedQuad::formInertiaTerms( int tangFlag ) const
  {

  static const int ndm = 2;

  static const int ndf = 2;

  static const int numberNodes = 9;

  //  static const int numberGauss = 9;

  static const int nShape = 3;

  static const int massIndex = nShape - 1;

  double xsj;  // determinant jacobian matrix

  double dvol; //volume element

  static double shp[nShape][numberNodes];  //shape functions at a gauss point

  static XC::Vector momentum(ndf);

  static XC::Matrix sx(ndm,ndm);

  static double GaussPoint[2];

  int j, k, p, q, r;
  int jj, kk;

  double temp, rho, massJK;


  //zero mass
  mass.Zero();

  //node coordinates
  computeBasis();

  //gauss loop
  int count = 0;
  for( p=0; p<3; p++ ) {
    for( q=0; q<3; q++ ) {

    GaussPoint[0] = sg[p];
    GaussPoint[1] = sg[q];

    //get shape functions
    shape2dNine( GaussPoint, xl, shp, xsj );

    //volume element
    dvol = ( wg[p] * wg[q] ) * xsj;

    //node loop to compute acceleration
    momentum.Zero();
    for( j = 0; j < numberNodes; j++ )
      //momentum += shp[massIndex][j] * ( theNodes[j]->getTrialAccel()  );
      momentum.addVector(1.0,theNodes[j]->getTrialAccel(),shp[massIndex][j] );


    //density
    rho = physicalProperties[count]->getRho();

    //multiply acceleration by density to form momentum
    momentum *= rho;


    //residual and tangent calculations node loops
    for( jj=0, j=0; j<numberNodes; j++, jj+=ndf ) {

      temp = shp[massIndex][j] * dvol;

      for( r=0; r<ndf; r++ )
        resid( jj+r ) += ( temp * momentum(r) );


      if( tangFlag == 1 ) {

         //multiply by density
         temp *= rho;

         //node-node mass
         for( kk=0, k=0; k<numberNodes; k++, kk+=ndf ) {

            massJK = temp * shp[massIndex][k];

            for( r=0; r<ndf; r++ )
              mass( jj+r, kk+r ) += massJK;

          } // end for k loop

      } // end if tang_flag

    } // end for j loop


    count++;
    }//end for q gauss loop
  } //end for p gauss loop

}

//*********************************************************************
//form residual and tangent

void XC::NineNodeMixedQuad::formResidAndTangent( int tang_flag ) const
  {

  //strains ordered : eps11, eps22, eps33, 2*eps12
  //volumtric strains projected onto {1, \xi, \eta} natural coordinates

  static const int ndm = 2;

  static const int ndf = 2;

  static const int nstress = 4;

  static const int numberNodes = 9;

  static const int numberGauss = 9;

  static const int nShape = 3;

  static const int nMixed = 3;

  int i, j, k, p, q, r, s;
  int jj, kk;

  int success;

  static double volume;

  static double xsj;  // determinant jacobian matrix

  static double dvol[numberGauss]; //volume element

  static double gaussPoint[ndm];

  static double natCoorArray[ndm][numberGauss];

  static XC::Vector strain(nstress);  //strain

  static double shp[nShape][numberNodes];  //shape functions at a gauss point

  static double Shape[nShape][numberNodes][numberGauss]; //all the shape functions

  static double shpBar[nShape][numberNodes][nMixed]; //mean value of shape functions

  static double rightHandSide[nShape][numberNodes][nMixed];

  static XC::Vector residJ(ndf); //nodeJ residual

  static XC::Matrix stiffJK(ndf,ndf); //nodeJK stiffness

  static XC::Vector stress(nstress);  //stress

  static XC::Matrix dd(nstress,nstress);  //material tangent

  static double interp[nMixed];

  static XC::Matrix Proj(3,3);   //projection matrix
  static XC::Matrix ProjInv(3,3);

  static XC::Matrix Iden(3,3);
  Iden(0,0) = 1.0;
  Iden(1,1) = 1.0;
  Iden(2,2) = 1.0;

  //---------B-matrices------------------------------------

    static XC::Matrix BJ(nstress,ndf);      // B matrix node J

    static XC::Matrix BJtran(ndf,nstress);

    static XC::Matrix BK(nstress,ndf);      // B matrix node k

    static XC::Matrix BJtranD(ndf,nstress);

  //-------------------------------------------------------


  //zero stiffness and residual
  stiff.Zero( );
  resid.Zero( );

  //node coordinates
  computeBasis();

  //zero mean shape functions
  for( p=0; p<nShape; p++ ) {
    for( q=0; q<numberNodes; q++ ) {

      for(r=0; r<nMixed; r++ ) {
        shpBar[p][q][r] = 0.0;
        rightHandSide[p][q][r] = 0.0;
      }

    }//end for q
  } // end for p


  //zero volume
  volume = 0.0;

  //zero projection matrix
  Proj.Zero( );
  ProjInv.Zero( );


  //gauss loop to compute and save shape functions
  int count = 0;

  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ ) {

        gaussPoint[0] = sg[i];
        gaussPoint[1] = sg[j];


        //save gauss point locations
        natCoorArray[0][count] = gaussPoint[0];
        natCoorArray[1][count] = gaussPoint[1];


        //get shape functions
        shape2dNine( gaussPoint, xl, shp, xsj );


        //save shape functions
        for( p=0; p<nShape; p++ ) {
          for( q=0; q<numberNodes; q++ )
            Shape[p][q][count] = shp[p][q];
        } // end for p


        //volume element to also be saved
        dvol[count] = ( wg[i]*wg[j] ) * xsj;


        //add to projection matrix
        interp[0] = 1.0;
        interp[1] = gaussPoint[0];
        interp[2] = gaussPoint[1];

        for( r=0; r<nMixed; r++ ) {
          for( s=0; s<nMixed; s++ )
            Proj(r,s) += ( interp[r]*interp[s] * dvol[count] );
        }//end for r

        volume += dvol[count];


        //add to mean shape functions
        for( p=0; p<nShape; p++ ) {
          for( q=0; q<numberNodes; q++ ) {

            for( s=0; s<nMixed; s++ )
              rightHandSide[p][q][s] += ( shp[p][q] * interp[s] * dvol[count] );

          }//end for q
        } // end for p


        //increment gauss point counter
        count++;

    } //end for j
  } // end for i



  //invert projection matrix
  //int Solve(const XC::Matrix &M, Matrix &res) const;
  Proj.Solve( Iden, ProjInv );

  //mean value of shape functions
  for( p=0; p<nShape; p++ ) {
    for( q=0; q<numberNodes; q++ ) {

      for(r=0; r<nMixed; r++ ) {
        for(s=0; s<nMixed; s++ )
          shpBar[p][q][r] += ( ProjInv(r,s) * rightHandSide[p][q][s] );
      }//end for r

    }//end for q
  }//end for p


  //gauss loop
  for( i=0; i<numberGauss; i++ ) {

    //extract gauss point location
    gaussPoint[0] = natCoorArray[0][i];
    gaussPoint[1] = natCoorArray[1][i];

    //extract shape functions from saved array
    for( p=0; p<nShape; p++ ) {
       for( q=0; q<numberNodes; q++ )
          shp[p][q]  = Shape[p][q][i];
    } // end for p


    //zero the strains
    strain.Zero( );

    // j-node loop to compute strain
    for( j=0; j<numberNodes; j++ )  {

      //compute B matrix

      BJ = computeBbar( j, gaussPoint, shp, shpBar );

      //nodal displacements
      const Vector &ul = theNodes[j]->getTrialDisp( );

      //compute the strain
      //strain += (BJ*ul);
      strain.addMatrixVector(1.0,  BJ,ul,1.0 );

    } // end for j



    //send the strain to the material
    success = const_cast<NDMaterial *>(physicalProperties[i])->setTrialStrain(strain);

    //compute the stress
    stress = physicalProperties[i]->getStress( );


    //multiply by volume element
    stress  *= dvol[i];

    if( tang_flag == 1 )
      {
        dd = physicalProperties[i]->getTangent( );
        dd *= dvol[i];
      } //end if tang_flag


    //residual and tangent calculations node loops

    jj = 0;
    for( j=0; j<numberNodes; j++ ) {

      BJ = computeBbar( j, gaussPoint, shp, shpBar );

      //transpose
      //BJtran = transpose( nstress, ndf, BJ );
      for(p=0; p<ndf; p++) {
        for(q=0; q<nstress; q++)
          BJtran(p,q) = BJ(q,p);
      }//end for p


      //residual
      //residJ = BJtran * stress;
      residJ.addMatrixVector(0.0,  BJtran,stress,1.0);

      //residual
      for( p=0; p<ndf; p++ )
        resid( jj + p ) += residJ(p);


      if( tang_flag == 1 ) {

        //BJtranD = BJtran * dd;
        BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0);

         kk = 0;
         for( k=0; k<numberNodes; k++ ) {

            BK = computeBbar( k, gaussPoint, shp, shpBar );


            //stiffJK =  BJtranD * BK;
            stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0);

            for( p=0; p<ndf; p++ )  {
               for( q=0; q<ndf; q++ )
                  stiff( jj+p, kk+q ) += stiffJK( p, q );
            } //end for p

            kk += ndf;
         }//end for k loop

      }//end if tang_flag

      jj += ndf;
    }//end for j loop


  }//end for i gauss loop


    return;
  }


//*************************************************************************
//! @brief compute local coordinates and basis
void XC::NineNodeMixedQuad::computeBasis(void) const
  {

    //nodal coordinates
    for(int i= 0;i<9;i++)
      {
        const XC::Vector &coorI = theNodes[i]->getCrds( );
        xl[0][i] = coorI(0);
        xl[1][i] = coorI(1);
      }  //end for i
  }

//*************************************************************************
//compute Bbar

const XC::Matrix &XC::NineNodeMixedQuad::computeBbar( int node,
                            const double natCoor[2],
                            const double shp[3][9],
                            double shpBar[3][9][3] ) const
  {

  static XC::Matrix Bbar(4,2);

  static double Bdev[3][2];

  static double BbarVol[3][2];

  static const double one3 = 1.0/3.0;

  static double interp[3];

  static double c0, c1;

  int i, j;

//---B Matrices in standard {1,2,3} mechanics notation---------
//
//                -                  -
//               |  2N,1    -N,2      |
// Bdev =  (1/3) |  -N,1    2N,2      |  (3x2)
//               |  -N,1    -N,2      |
//                -                  -
//
//                -                 -
//               |  N,1      N,2     |
// Bvol =  (1/3) |  N,1      N,2     |  (3x2)
//               |  N,1      N,2     |
//                -                 -
//
//                -                   -
//               |                     |
//               |    Bdev + Bvol      |
//   B       =   |                     |
//               |---------------------|   (4x2)
//               |   N,2     N,1       |
//                -                   -
//
//---------------------------------------------------------------


  Bbar.Zero( );

  //deviatoric
  Bdev[0][0] = 2.0*shp[0][node];
  Bdev[0][1] =    -shp[1][node];

  Bdev[1][0] =    -shp[0][node];
  Bdev[1][1] = 2.0*shp[1][node];

  Bdev[2][0] =    -shp[0][node];
  Bdev[2][1] =    -shp[1][node];


  //volume interpolation functions
  interp[0] = 1.0;
  interp[1] = natCoor[0];
  interp[2] = natCoor[1];

  //volumetric
  c0 = 0.0;
  c1 = 0.0;

  for(i=0; i<3; i++) {

    c0 += ( shpBar[0][node][i] * interp[i] );
    c1 += ( shpBar[1][node][i] * interp[i] );

  }//end for i

  //standard displacement formulation
  //c0 = shp[0][node];
  //c1 = shp[1][node];

  BbarVol[0][0] = c0;
  BbarVol[0][1] = c1;

  BbarVol[1][0] = c0;
  BbarVol[1][1] = c1;

  BbarVol[2][0] = c0;
  BbarVol[2][1] = c1;


  //extensional terms
  for( i=0; i<3; i++ ){
    for( j=0; j<2; j++ )
      Bbar(i,j) = one3*( Bdev[i][j] + BbarVol[i][j] );
  }//end for i


  //shear terms
  Bbar(3,0) = shp[1][node];
  Bbar(3,1) = shp[0][node];


  return Bbar;
  }

//**************************************************************************
//shape function routine for nine node quads

void XC::NineNodeMixedQuad::shape2dNine( double coor[2], const double x[2][9], double shp[3][9], double &xsj )
  {
  static const int nNode = 9;
  static const int ndm   = 2;

  static const int node1[] = { 0,1,1,0, 2,1,2,0, 2 };
  static const int node2[] = { 0,0,1,1, 0,2,1,2, 2 };
  int n1, n2;

  int i, j, k, q;

  static double xs[ndm][ndm];
  static double sx[ndm][ndm];

  double ss = coor[0];
  double tt = coor[1];
  double tempSS, tempTT;

  //shape functions and derivatives in natural coordinates
  for( q=0; q<nNode; q++ ) {

    n1 = node1[q];
    n2 = node2[q];

    tempSS = shape1d(1,n1,ss);
    tempTT = shape1d(1,n2,tt);

    //function
    shp[2][q] =  tempSS * tempTT;

    //ss-derivative
    shp[0][q] =  (shape1d(0,n1,ss)) * tempTT;

    //tt-derivative
    shp[1][q] =  tempSS * (shape1d(0,n2,tt));

  }//end for q


  // Construct jacobian and its inverse
  for( i=0; i<ndm; i++ ) {
    for( j=0; j<ndm; j++ ) {

      xs[i][j] = 0.0;

      for( k=0; k<nNode; k++ )
          xs[i][j] += ( x[i][k] * shp[j][k] );

    } //end for j
  }  // end for i


  xsj = xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];

  double xsjInv = 1.0/xsj;

  sx[0][0] =  xs[1][1] * xsjInv;
  sx[1][1] =  xs[0][0] * xsjInv;
  sx[0][1] = -xs[0][1] * xsjInv;
  sx[1][0] = -xs[1][0] * xsjInv;


  //form global derivatives
  double temp;
  for( i=0; i<nNode; i++ ) {
    temp      = shp[0][i]*sx[0][0] + shp[1][i]*sx[1][0];
    shp[1][i] = shp[0][i]*sx[0][1] + shp[1][i]*sx[1][1];
    shp[0][i] = temp;
  } // end for i


    return;
  }

//***********************************************************************
//1d quadratic shape functions

double XC::NineNodeMixedQuad::shape1d( int code, int node, double xi )
  {
    //one-dimensional quadratic shape functions
    //
    // o------o------o
    // 0      2      1
    //

    double result= 0.0;

    static const double oneHalf = 0.50;

    //shape functions
    if(code==1)
      {
        if(node==0)
          result = oneHalf*xi*(xi-1.0);
        if(node==1)
          result = oneHalf*xi*(xi+1.0);
        if(node==2)
          result = 1.0 - xi*xi;
      }
    //shape function derivatives
    else if(code==0)
      {
        if(node==0)
          result = oneHalf * ( 2.0*xi - 1.0 );
        if(node==1)
          result = oneHalf * ( 2.0*xi + 1.0 );
        if(node==2)
          result = -2.0*xi;
      }//end if
    return result;
  }

//! @brief Send object members through the communicator argument.
int XC::NineNodeMixedQuad::sendData(Communicator &comm)
  {
    int res= ElemWithMaterial<9,NDMaterialPhysicalProperties>::sendData(comm);
    res+= comm.sendDoubles(xl[0][0],xl[0][1],xl[0][2],xl[0][3],xl[0][4],xl[0][5],getDbTagData(),CommMetaData(8));
    res+= comm.sendDoubles(xl[0][6],xl[0][7],xl[0][8],xl[1][0],xl[1][1],xl[1][2],getDbTagData(),CommMetaData(9));
    res+= comm.sendDoubles(xl[1][3],xl[1][4],xl[1][5],xl[1][6],xl[1][7],xl[1][8],getDbTagData(),CommMetaData(10));
    res+= comm.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(13,14,15,16));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::NineNodeMixedQuad::recvData(const Communicator &comm)
  {
    int res= ElemWithMaterial<9,NDMaterialPhysicalProperties>::recvData(comm);
    res+= comm.receiveDoubles(xl[0][0],xl[0][1],xl[0][2],xl[0][3],xl[0][4],xl[0][5],getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(xl[0][6],xl[0][7],xl[0][8],xl[1][0],xl[1][1],xl[1][2],getDbTagData(),CommMetaData(9));
    res+= comm.receiveDoubles(xl[1][3],xl[1][4],xl[1][5],xl[1][6],xl[1][7],xl[1][8],getDbTagData(),CommMetaData(10));
    Ki= comm.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(13,14,15,16));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::NineNodeMixedQuad::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(17);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::NineNodeMixedQuad::recvSelf(const Communicator &comm)
  {
    inicComm(17);
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

