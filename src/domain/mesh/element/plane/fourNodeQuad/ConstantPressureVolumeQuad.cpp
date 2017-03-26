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

// $Revision: 1.15 $
// $Date: 2006/01/10 18:33:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/ConstantPressureVolumeQuad.cpp,v $

// Ed "C++" Love
//
// Constant Presssure/Volume Four Node Quadrilateral
// Plane Strain (NOT PLANE STRESS)


#include "ConstantPressureVolumeQuad.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <domain/domain/Domain.h>
#include <utility/handler/ErrorHandler.h>

#include "utility/actor/actor/ArrayCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"

//static data
double XC::ConstantPressureVolumeQuad::matrixData[64];
 XC::Matrix XC::ConstantPressureVolumeQuad::stiff(matrixData,8,8);
 XC::Vector XC::ConstantPressureVolumeQuad::resid(8);
 XC::Matrix XC::ConstantPressureVolumeQuad::mass(8,8);
 XC::Matrix XC::ConstantPressureVolumeQuad::damping(8,8);

//volume-pressure constants
double XC::ConstantPressureVolumeQuad::one3  = 1.0 / 3.0;
double XC::ConstantPressureVolumeQuad::two3  = 2.0 / 3.0;
double XC::ConstantPressureVolumeQuad::four3 = 4.0 / 3.0;
double XC::ConstantPressureVolumeQuad::one9  = 1.0 / 9.0;

//quadrature data

double XC::ConstantPressureVolumeQuad::sg[] = { -one_over_root3,
                                               one_over_root3,
                                               one_over_root3,
                                              -one_over_root3 };

double XC::ConstantPressureVolumeQuad::tg[] = { -one_over_root3,
                                              -one_over_root3,
                                               one_over_root3,
                                               one_over_root3 };

double XC::ConstantPressureVolumeQuad::wg[] = { 1.0, 1.0, 1.0, 1.0 };

//! @brief null constructor
XC::ConstantPressureVolumeQuad::ConstantPressureVolumeQuad(void)
  :QuadBase4N<NDMaterialPhysicalProperties>(ELE_TAG_ConstantPressureVolumeQuad,NDMaterialPhysicalProperties(9,nullptr)) {}


//! @brief full constructor
XC::ConstantPressureVolumeQuad::ConstantPressureVolumeQuad(int tag, int node1,int node2,int node3,int node4, NDMaterial &theMat)
  :QuadBase4N<NDMaterialPhysicalProperties>(tag, ELE_TAG_ConstantPressureVolumeQuad,node1,node2,node3,node4,NDMaterialPhysicalProperties(9,theMat,"AxiSymmetric2D")) {}

XC::Element* XC::ConstantPressureVolumeQuad::getCopy(void) const
  { return new ConstantPressureVolumeQuad(*this); }

//set domain
void XC::ConstantPressureVolumeQuad::setDomain(Domain *theDomain )
  {
    QuadBase4N<NDMaterialPhysicalProperties>::setDomain(theDomain);
    for(int i = 0;i<4;i++)
      {
        if(theNodes[i])
          {
            const XC::Vector &coor = theNodes[i]->getCrds();
            xl[0][i] = coor(0);
            xl[1][i] = coor(1);
          } // end if
      } //end for i
  }


//! @brief return number of dofs
int XC::ConstantPressureVolumeQuad::getNumDOF(void) const
  { return 8; }


int XC::ConstantPressureVolumeQuad::update( )
  {
  // strains ordered  00, 11, 22, 01
  //            i.e.  11, 22, 33, 12
  //
  //            strain(0) =   eps_00
  //            strain(1) =   eps_11
  //            strain(2) =   eps_22
  //            strain(3) = 2*eps_01
  //
  //  same ordering for stresses but no 2

  int i,  k, l;
  int node;
  int success = 0;

  static double tmp_shp[3][4]; //shape functions

  static double shp[3][4][4]; //shape functions at each gauss point

  static double vol_avg_shp[3][4]; // volume averaged shape functions

  double xsj;  // determinant jacaobian matrix

  static XC::Matrix sx(2,2); // inverse jacobian matrix

  double dvol[4]; //volume elements

  double volume = 0.0; //volume of element

  double theta = 0.0; //average volume change (trace of strain)

  static XC::Vector strain(4); //strain in vector form

  double trace = 0.0; //trace of the strain

  static XC::Vector one(4); //rank 2 identity as a vector

  //one vector
  one(0) = 1.0;
  one(1) = 1.0;
  one(2) = 1.0;
  one(3) = 0.0;


  //zero stuff
  volume = 0.0;

  for( k = 0; k < 3; k++ ){
    for( l = 0; l < 4; l++ )
        vol_avg_shp[k][l] = 0.0;
  } //end for k


  //gauss loop to compute volume averaged shape functions

  for( i = 0; i < 4; i++ ){

    shape2d( sg[i], tg[i], xl, tmp_shp, xsj, sx );

    dvol[i] = wg[i] * xsj;  // multiply by radius for axisymmetry

    volume += dvol[i];

    for( k = 0; k < 3; k++ ){
      for( l = 0; l < 4; l++ ) {

        shp[k][l][i] = tmp_shp[k][l];

        vol_avg_shp[k][l] += tmp_shp[k][l] * dvol[i];

      } // end for l
    } //end for k

  } //end for i


  //compute volume averaged shape functions
  for( k = 0; k < 3; k++ ){
    for( l = 0; l < 4; l++ )
        vol_avg_shp[k][l] /= volume;
  } //end for k


  //compute theta
  theta = 0.0;
  for( i = 0; i < 4; i++ ) {

    strain.Zero( );

    //node loop to compute strain
    for( node = 0; node < 4; node++ ) {

      const XC::Vector &ul = theNodes[node]->getTrialDisp( );

      strain(0) += shp[0][node][i] * ul(0);

      strain(1) += shp[1][node][i] * ul(1);

      strain(2) = 0.0;  // not zero for axisymmetry

    } // end for node

    trace  =  strain(0) + strain(1) + strain(2);

    theta +=  trace * dvol[i];

  } // end for i
  theta /= volume;


  //compute strain in materials
  for( i = 0; i < 4; i++ ) {

    strain.Zero( );

    //node loop to compute strain
    for( node = 0; node < 4; node++ ) {

      const XC::Vector &ul = theNodes[node]->getTrialDisp( );

      strain(0) += shp[0][node][i] * ul(0);

      strain(1) += shp[1][node][i] * ul(1);

      strain(2) = 0.0; // not zero for axisymmetry

      strain(3) +=  shp[1][node][i] * ul(0)
                  + shp[0][node][i] * ul(1);

    } // end for node

    trace = strain(0) + strain(1) + strain(2);

    //strain -= (one3*trace)*one;
    strain.addVector(1.0,  one, -one3*trace );

    //strain += (one3*theta)*one;
    strain.addVector(1.0,  one, one3*theta );

    success += physicalProperties[i]->setTrialStrain( strain );

  } // end for i

  return success;
}

//print out element data
void XC::ConstantPressureVolumeQuad::Print( std::ostream &s, int flag )
{
  s << std::endl;
  s << "Four XC::Node Quad -- Mixed Pressure/Volume -- Plane Strain \n";
  s << "Element Number " << this->getTag() << std::endl;
  s << "Node 1 : " << theNodes.getTagNode(0) << std::endl;
  s << "Node 2 : " << theNodes.getTagNode(1) << std::endl;
  s << "Node 3 : " << theNodes.getTagNode(2) << std::endl;
  s << "Node 4 : " << theNodes.getTagNode(3) << std::endl;
  s << "Material XC::Information : \n ";

  physicalProperties.Print( s, flag );

  s << std::endl;
}

//return stiffness matrix
const XC::Matrix &XC::ConstantPressureVolumeQuad::getTangentStiff(void) const
  {
    int tang_flag = 1; //get the tangent
    //do tangent and residual here
    formResidAndTangent( tang_flag );
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

const XC::Matrix& XC::ConstantPressureVolumeQuad::getInitialStiff(void) const
  {
    int i,  j,  k, l;
    int jj, kk;

  static double tmp_shp[3][4]; //shape functions

  static double shp[3][4][4]; //shape functions at each gauss point

  static double vol_avg_shp[3][4]; // volume averaged shape functions

  double xsj;  // determinant jacaobian matrix

  static XC::Matrix sx(2,2); // inverse jacobian matrix

  double dvol[4]; //volume elements

  double volume = 0.0; //volume of element

  static XC::Vector strain(4); //strain in vector form

  // static XC::Vector sigBar(4); //stress in vector form
  static XC::Vector sig(4); //mixed stress in vector form

  static XC::Matrix BJtran(2,4);
  static XC::Matrix BK(4,2);

  static XC::Matrix littleBJtran(2,1);
  static XC::Matrix littleBK(1,2);

  static XC::Matrix stiffJK(2,2); //nodeJ-nodeK 2x2 stiffness
  static XC::Vector residJ(2); //nodeJ residual

  static XC::Vector one(4); //rank 2 identity as a vector

  static XC::Matrix Pdev(4,4); //deviator projector

  //  static XC::Matrix dd(4,4);  //material tangent

  static XC::Matrix ddPdev(4,4);
  static XC::Matrix PdevDD(4,4);

  static double Pdev_dd_Pdev_data[16];
  static double Pdev_dd_one_data[4];
  static double one_dd_Pdev_data[4];
  static XC::Matrix Pdev_dd_Pdev(Pdev_dd_Pdev_data, 4, 4);
  static XC::Matrix Pdev_dd_one(Pdev_dd_one_data, 4, 1);
  static XC::Matrix one_dd_Pdev(one_dd_Pdev_data, 1,4);

  double bulk= 0.0;
  static XC::Matrix BJtranD(2,4);
  static XC::Matrix BJtranDone(2,1);

  static XC::Matrix littleBJoneD(2,4);
  static XC::Matrix littleBJtranBulk(2,1);

  //zero stiffness and residual
  stiff.Zero();

  //one vector
  one(0) = 1.0;
  one(1) = 1.0;
  one(2) = 1.0;
  one(3) = 0.0;

  //Pdev matrix
  Pdev.Zero( );

  Pdev(0,0) =  two3;
  Pdev(0,1) = -one3;
  Pdev(0,2) = -one3;

  Pdev(1,0) = -one3;
  Pdev(1,1) =  two3;
  Pdev(1,2) = -one3;

  Pdev(2,0) = -one3;
  Pdev(2,1) = -one3;
  Pdev(2,2) =  two3;

  Pdev(3,3) = 1.0;

  //zero stuff
  volume = 0.0;

  for( k = 0; k < 3; k++ ){
    for( l = 0; l < 4; l++ )
        vol_avg_shp[k][l] = 0.0;
  } //end for k


  //gauss loop to compute volume averaged shape functions

  for( i = 0; i < 4; i++ ){

    shape2d( sg[i], tg[i], xl, tmp_shp, xsj, sx );

    dvol[i] = wg[i] * xsj;  // multiply by radius for axisymmetry

    volume += dvol[i];

    for( k = 0; k < 3; k++ ){
      for( l = 0; l < 4; l++ ) {

        shp[k][l][i] = tmp_shp[k][l];

        vol_avg_shp[k][l] += tmp_shp[k][l] * dvol[i];

      } // end for l
    } //end for k

  } //end for i


  //compute volume averaged shape functions
  for( k = 0; k < 3; k++ ){
    for( l = 0; l < 4; l++ )
        vol_avg_shp[k][l] /= volume;
  } //end for k

  //residual and tangent calculations gauss loop
  for( i = 0; i < 4; i++ ) {

    static XC::Matrix dd(4,4);

    dd = physicalProperties[i]->getInitialTangent( );

    dd *= dvol[i];

    //Pdev_dd_Pdev = Pdev * dd * Pdev;
    Pdev_dd_Pdev.addMatrixTripleProduct(0.0,
                                        Pdev,
                                        dd,
                                        1.0);

    //Pdev_dd_one  = one3 * ( Pdev * dd * oneMatrix );
    PdevDD.addMatrixProduct(0.0, Pdev, dd, 1.0);
    Pdev_dd_one(0,0) = one3 * (PdevDD(0,0) + PdevDD(0,1) + PdevDD(0,2));
    Pdev_dd_one(1,0) = one3 * (PdevDD(1,0) + PdevDD(1,1) + PdevDD(1,2));
    Pdev_dd_one(2,0) = one3 * (PdevDD(2,0) + PdevDD(2,1) + PdevDD(2,2));
    Pdev_dd_one(3,0) = one3 * (PdevDD(3,0) + PdevDD(3,1) + PdevDD(3,2));

    //one_dd_Pdev  = one3 * ( oneTran * dd * Pdev );
    ddPdev.addMatrixProduct(0.0, dd, Pdev, 1.0);
    one_dd_Pdev(0,0) = one3 * (ddPdev(0,0) + ddPdev(1,0) + ddPdev(2,0));
    one_dd_Pdev(0,1) = one3 * (ddPdev(0,1) + ddPdev(1,1) + ddPdev(2,1));
    one_dd_Pdev(0,2) = one3 * (ddPdev(0,2) + ddPdev(1,2) + ddPdev(2,2));
    one_dd_Pdev(0,3) = one3 * (ddPdev(0,3) + ddPdev(1,3) + ddPdev(2,3));

    bulk = one9 * ( dd(0,0) + dd(0,1) + dd(0,2)
                    + dd(1,0) + dd(1,1) + dd(1,2)
                    + dd(2,0) + dd(2,1) + dd(2,2) );

    jj = 0;
    for( j = 0; j < 4; j++ ) {

      double BJ00 = shp[0][j][i];
      double BJ11 = shp[1][j][i];
      double BJ30 = shp[1][j][i];
      double BJ31 = shp[0][j][i];

      BJtran.Zero( );
      BJtran(0,0) = shp[0][j][i];
      BJtran(1,1) = shp[1][j][i];

      // BJ(2,0) for axi-symmetry

      BJtran(0,3) = shp[1][j][i];
      BJtran(1,3) = shp[0][j][i];

      //compute residual

      double ltBJ00 = vol_avg_shp[0][j];
      double ltBJ01 = vol_avg_shp[1][j];

      //BJtranD          =  BJtran * Pdev_dd_Pdev;
      // BJtranD.addMatrixProduct(0.0,  BJtran, Pdev_dd_Pdev, 1.0);

      //littleBJoneD     =  littleBJtran * one_dd_Pdev;
      // littleBJoneD.addMatrixProduct(0.0,  littleBJtran, one_dd_Pdev, 1.0);

      static double Adata[8];
      static XC::Matrix A(Adata, 2, 4);

      // A = BJtranD;
      // A += littleBJoneD;

      for(int colA = 0, loc = 0, colPdev = 0; colA<4; colA++, colPdev += 4) {
        double data3colA = Pdev_dd_Pdev_data[3+colPdev];
        Adata[loc++] = BJ00*Pdev_dd_Pdev_data[colPdev] + BJ30*data3colA + ltBJ00*one_dd_Pdev_data[colA];
        Adata[loc++] = BJ11*Pdev_dd_Pdev_data[1+colPdev] + BJ31*data3colA + ltBJ01*one_dd_Pdev_data[colA];
      }

      //BJtranDone       =  BJtran * Pdev_dd_one;
      // BJtranDone.addMatrixProduct(0.0,  BJtran, Pdev_dd_one, 1.0);

      //littleBJtranBulk =  bulk * littleBJtran;
      // littleBJtranBulk = littleBJtran;
      // littleBJtranBulk *= bulk;

      double B1, B2;
      // B1 = BJtranDone(0,0) + littleBJtranBulk(0,0);
      // B2 = BJtranDone(1,0) + littleBJtranBulk(1,0);

      B1 = BJ00*Pdev_dd_one_data[0] + BJ30*Pdev_dd_one_data[3] + ltBJ00 *bulk;
      B2 = BJ11*Pdev_dd_one_data[1] + BJ31*Pdev_dd_one_data[3] + ltBJ01 *bulk;

      int colkk, colkkP1;
      for( k = 0, kk=0, colkk =0, colkkP1 =8;
            k < 4;
            k++, kk += 2, colkk += 16, colkkP1 += 16 ) {

        double BK00 = shp[0][k][i];
        double BK11 = shp[1][k][i];
        double BK30 = shp[1][k][i];
        double BK31 = shp[0][k][i];

        double littleBK00 = vol_avg_shp[0][k];
        double littleBK01 = vol_avg_shp[1][k];

        //compute stiffness matrix

        stiff( jj,   kk   ) += Adata[0]*BK00 + Adata[6]*BK30 + B1 * littleBK00;
        stiff( jj+1, kk   ) += Adata[1]*BK00 + Adata[7]*BK30 + B2 * littleBK00;
        stiff( jj,   kk+1 ) += Adata[2]*BK11 + Adata[6]*BK31 + B1 * littleBK01;
        stiff( jj+1, kk+1 ) += Adata[3]*BK11 + Adata[7]*BK31 + B2 * littleBK01;

      } // end for k

      jj += 2;
    } // end for j
  } //end for i
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

//return mass matrix
const XC::Matrix& XC::ConstantPressureVolumeQuad::getMass(void) const
  {
    int tangFlag = 1;
    formInertiaTerms( tangFlag );
    if(isDead())
      mass*=dead_srf;
    return mass;
  }



int XC::ConstantPressureVolumeQuad::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::ConstantPressureVolumeQuad::addLoad - load type unknown for ele with tag: " << this->getTag() << std::endl;
    return -1;
  }

int XC::ConstantPressureVolumeQuad::addInertiaLoadToUnbalance(const XC::Vector &accel)
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

    // store computed RV fro nodes in resid vector
    int count = 0;

    for(i=0; i<numberNodes; i++)
      {
        const XC::Vector &Raccel = theNodes[i]->getRV(accel);
        for(int j=0; j<ndf; j++)
          resid(count++) = Raccel(i);
      }

    // create the load vector if one does not exist
    if(load.Nulo())
      load.reset(numberNodes*ndf);

    // add -M * RV(accel) to the load vector
    load.addMatrixVector(1.0, mass, resid, -1.0);
    return 0;
  }


//! @brief get residual
const XC::Vector& XC::ConstantPressureVolumeQuad::getResistingForce(void) const
  {
    int tang_flag = 0; //don't get the tangent
    formResidAndTangent( tang_flag );

    // subtract external loads
    if(!load.Nulo())
      resid-= load;
    if(isDead())
      resid*=dead_srf;
    return resid;
  }


//! @brief get residual with inertia terms
const XC::Vector& XC::ConstantPressureVolumeQuad::getResistingForceIncInertia(void) const
  {
    int tang_flag = 0; //don't get the tangent
    static Vector res(8);

    //do tangent and residual here
    formResidAndTangent( tang_flag );

    //inertia terms
    formInertiaTerms( tang_flag );
    res = resid;

    // subtract external loads
    if(!load.Nulo())
      res-= load;

    // add the damping forces if rayleigh damping
    if(!rayFactors.Nulos())
      res += this->getRayleighDampingForces();
    if(isDead())
      res*=dead_srf;
    return res;
  }

//*****************************************************************************
//form inertia terms

void   XC::ConstantPressureVolumeQuad::formInertiaTerms( int tangFlag ) const
{

  static const int ndm = 2;

  static const int ndf = 2;

  static const int numberNodes = 4;

  static const int numberGauss = 4;

  static const int nShape = 3;

  static const int massIndex = nShape - 1;

  double xsj;  // determinant jacaobian matrix

  double dvol; //volume element

  static double shp[nShape][numberNodes];  //shape functions at a gauss point

  static XC::Vector momentum(ndf);

  static XC::Matrix sx(ndm,ndm);

  int i, j, k, p;
  int jj, kk;

  double temp, rho, massJK;


  //zero mass
  mass.Zero( );

  //gauss loop
  for( i = 0; i < numberGauss; i++ ) {

    //get shape functions
    shape2d( sg[i], tg[i], xl, shp, xsj, sx );

    //volume element
    dvol = wg[i] * xsj;

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

      temp = shp[massIndex][j] * dvol;

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

      else
        for( p = 0; p < ndf; p++ )
          resid( jj+p ) += ( temp * momentum(p) );

      jj += ndf;

    } // end for j loop


  } //end for i gauss loop



}

//*********************************************************************

//form residual and tangent
void XC::ConstantPressureVolumeQuad::formResidAndTangent( int tang_flag ) const
{
  // strains ordered  00, 11, 22, 01
  //            i.e.  11, 22, 33, 12
  //
  //            strain(0) =   eps_00
  //            strain(1) =   eps_11
  //            strain(2) =   eps_22
  //            strain(3) = 2*eps_01
  //
  //  same ordering for stresses but no 2

  int i,  j,  k, l;
  int jj, kk;

  static double tmp_shp[3][4]; //shape functions

  static double shp[3][4][4]; //shape functions at each gauss point

  static double vol_avg_shp[3][4]; // volume averaged shape functions

  double xsj;  // determinant jacaobian matrix

  static XC::Matrix sx(2,2); // inverse jacobian matrix

  double dvol[4]; //volume elements

  double volume = 0.0; //volume of element

  double pressure = 0.0; //constitutive pressure

  static XC::Vector strain(4); //strain in vector form

  // static XC::Vector sigBar(4); //stress in vector form
  static XC::Vector sig(4); //mixed stress in vector form

  double trace = 0.0; //trace of the strain

  static XC::Matrix BJtran(2,4);
  static XC::Matrix BK(4,2);

  static XC::Matrix littleBJtran(2,1);
  static XC::Matrix littleBK(1,2);

  static XC::Matrix stiffJK(2,2); //nodeJ-nodeK 2x2 stiffness
  static XC::Vector residJ(2); //nodeJ residual

  static XC::Vector one(4); //rank 2 identity as a vector

  static XC::Matrix Pdev(4,4); //deviator projector

  //  static XC::Matrix dd(4,4);  //material tangent

  static XC::Matrix ddPdev(4,4);
  static XC::Matrix PdevDD(4,4);

  static double Pdev_dd_Pdev_data[16];
  static double Pdev_dd_one_data[4];
  static double one_dd_Pdev_data[4];
  static XC::Matrix Pdev_dd_Pdev(Pdev_dd_Pdev_data, 4, 4);
  static XC::Matrix Pdev_dd_one(Pdev_dd_one_data, 4, 1);
  static XC::Matrix one_dd_Pdev(one_dd_Pdev_data, 1,4);

  double bulk= 0.0;
  static XC::Matrix BJtranD(2,4);
  static XC::Matrix BJtranDone(2,1);

  static XC::Matrix littleBJoneD(2,4);
  static XC::Matrix littleBJtranBulk(2,1);

  //zero stiffness and residual
  if( tang_flag == 1 )
    stiff.Zero();
  else
    resid.Zero();

  //one vector
  one(0) = 1.0;
  one(1) = 1.0;
  one(2) = 1.0;
  one(3) = 0.0;

  //Pdev matrix
  Pdev.Zero( );

  Pdev(0,0) =  two3;
  Pdev(0,1) = -one3;
  Pdev(0,2) = -one3;

  Pdev(1,0) = -one3;
  Pdev(1,1) =  two3;
  Pdev(1,2) = -one3;

  Pdev(2,0) = -one3;
  Pdev(2,1) = -one3;
  Pdev(2,2) =  two3;

  Pdev(3,3) = 1.0;

  //zero stuff
  volume = 0.0;

  for( k = 0; k < 3; k++ ){
    for( l = 0; l < 4; l++ )
        vol_avg_shp[k][l] = 0.0;
  } //end for k


  //gauss loop to compute volume averaged shape functions

  for( i = 0; i < 4; i++ ){

    shape2d( sg[i], tg[i], xl, tmp_shp, xsj, sx );

    dvol[i] = wg[i] * xsj;  // multiply by radius for axisymmetry

    volume += dvol[i];

    for( k = 0; k < 3; k++ ){
      for( l = 0; l < 4; l++ ) {

        shp[k][l][i] = tmp_shp[k][l];

        vol_avg_shp[k][l] += tmp_shp[k][l] * dvol[i];

      } // end for l
    } //end for k

  } //end for i


  //compute volume averaged shape functions
  for( k = 0; k < 3; k++ ){
    for( l = 0; l < 4; l++ )
        vol_avg_shp[k][l] /= volume;
  } //end for k

  //compute pressure if residual calculation
  if(tang_flag != 1) {
    pressure = 0.0;
    for( i = 0; i < 4; i++ ) {

      const XC::Vector &sigBar = physicalProperties[i]->getStress( );

      pressure +=  one3 * ( sigBar(0) + sigBar(1) + sigBar(2) ) * dvol[i];

    } // end for i

    pressure /= volume;
  } // end if != tang_flag

  //residual and tangent calculations gauss loop

  for( i = 0; i < 4; i++ ) {

    if( tang_flag == 1 ) {    // compute matrices for stiffness calculation

      static XC::Matrix dd(4,4);
      dd = physicalProperties[i]->getTangent( );

      dd *= dvol[i];

      //Pdev_dd_Pdev = Pdev * dd * Pdev;
      Pdev_dd_Pdev.addMatrixTripleProduct(0.0,
                                          Pdev,
                                          dd,
                                          1.0);

      //Pdev_dd_one  = one3 * ( Pdev * dd * oneMatrix );
      PdevDD.addMatrixProduct(0.0, Pdev, dd, 1.0);
      Pdev_dd_one(0,0) = one3 * (PdevDD(0,0) + PdevDD(0,1) + PdevDD(0,2));
      Pdev_dd_one(1,0) = one3 * (PdevDD(1,0) + PdevDD(1,1) + PdevDD(1,2));
      Pdev_dd_one(2,0) = one3 * (PdevDD(2,0) + PdevDD(2,1) + PdevDD(2,2));
      Pdev_dd_one(3,0) = one3 * (PdevDD(3,0) + PdevDD(3,1) + PdevDD(3,2));

      //one_dd_Pdev  = one3 * ( oneTran * dd * Pdev );
      ddPdev.addMatrixProduct(0.0, dd, Pdev, 1.0);
      one_dd_Pdev(0,0) = one3 * (ddPdev(0,0) + ddPdev(1,0) + ddPdev(2,0));
      one_dd_Pdev(0,1) = one3 * (ddPdev(0,1) + ddPdev(1,1) + ddPdev(2,1));
      one_dd_Pdev(0,2) = one3 * (ddPdev(0,2) + ddPdev(1,2) + ddPdev(2,2));
      one_dd_Pdev(0,3) = one3 * (ddPdev(0,3) + ddPdev(1,3) + ddPdev(2,3));

      bulk = one9 * ( dd(0,0) + dd(0,1) + dd(0,2)
                      + dd(1,0) + dd(1,1) + dd(1,2)
                      + dd(2,0) + dd(2,1) + dd(2,2) );

    } else { // compute stress for residual calculation
      //stress for equilibrium
      const Vector &sigBar = physicalProperties[i]->getStress( );
      trace = sigBar(0) + sigBar(1) + sigBar(2);
      sig  = sigBar;

      //sig -= (one3*trace)*one;
      sig.addVector(1.0,  one, -one3*trace );
      sig.addVector(1.0,  one, pressure );

      //multilply by volume elements and compute
      sig *= dvol[i];
    }

    //residual and tangent loop over nodes
    jj = 0;
    for( j = 0; j < 4; j++ ) {

      /********** expanding for efficiency the matrix operations that use these
      BJ.Zero( );
      BJ(0,0) = shp[0][j][i];
      BJ(1,1) = shp[1][j][i];

      // BJ(2,0) for axi-symmetry

      BJ(3,0) = shp[1][j][i];
      BJ(3,1) = shp[0][j][i];

      littleBJ(0,0) = vol_avg_shp[0][j];
      littleBJ(0,1) = vol_avg_shp[1][j];

      // BJtran = this->transpose( 4, 2, BJ );
      for(p=0; p<2; p++) {
        for(q=0; q<4; q++)
          BJtran(p,q) = BJ(q,p);
      }//end for p

      for(p=0; p<2; p++) {
        for(q=0; q<1; q++)
          littleBJtran(p,q) = littleBJ(q,p);
      }//end for p

      **********************************************************************/


      double BJ00 = shp[0][j][i];
      double BJ11 = shp[1][j][i];
      double BJ30 = shp[1][j][i];
      double BJ31 = shp[0][j][i];

      BJtran.Zero( );
      BJtran(0,0) = shp[0][j][i];
      BJtran(1,1) = shp[1][j][i];

      // BJ(2,0) for axi-symmetry

      BJtran(0,3) = shp[1][j][i];
      BJtran(1,3) = shp[0][j][i];

      //compute residual

      if( tang_flag == 1 ) { //stiffness matrix

        double ltBJ00 = vol_avg_shp[0][j];
        double ltBJ01 = vol_avg_shp[1][j];

        //BJtranD          =  BJtran * Pdev_dd_Pdev;
        // BJtranD.addMatrixProduct(0.0,  BJtran, Pdev_dd_Pdev, 1.0);

        //littleBJoneD     =  littleBJtran * one_dd_Pdev;
        // littleBJoneD.addMatrixProduct(0.0,  littleBJtran, one_dd_Pdev, 1.0);

        static double Adata[8];
        static XC::Matrix A(Adata, 2, 4);

        // A = BJtranD;
        // A += littleBJoneD;

        for(int colA = 0, loc = 0, colPdev = 0; colA<4; colA++, colPdev += 4) {
          double data3colA = Pdev_dd_Pdev_data[3+colPdev];
          Adata[loc++] = BJ00*Pdev_dd_Pdev_data[colPdev] + BJ30*data3colA + ltBJ00*one_dd_Pdev_data[colA];
          Adata[loc++] = BJ11*Pdev_dd_Pdev_data[1+colPdev] + BJ31*data3colA + ltBJ01*one_dd_Pdev_data[colA];
        }

        //BJtranDone       =  BJtran * Pdev_dd_one;
        // BJtranDone.addMatrixProduct(0.0,  BJtran, Pdev_dd_one, 1.0);

        //littleBJtranBulk =  bulk * littleBJtran;
        // littleBJtranBulk = littleBJtran;
        // littleBJtranBulk *= bulk;

        double B1, B2;
        // B1 = BJtranDone(0,0) + littleBJtranBulk(0,0);
        // B2 = BJtranDone(1,0) + littleBJtranBulk(1,0);

        B1 = BJ00*Pdev_dd_one_data[0] + BJ30*Pdev_dd_one_data[3] + ltBJ00 *bulk;
        B2 = BJ11*Pdev_dd_one_data[1] + BJ31*Pdev_dd_one_data[3] + ltBJ01 *bulk;

        int colkk, colkkP1;
        for( k = 0, kk=0, colkk =0, colkkP1 =8;
              k < 4;
              k++, kk += 2, colkk += 16, colkkP1 += 16 ) {

          /**************************************************************
           REPLACING THESE LINES WITH THE 4 BELOW COMMENT FOR EFFICIENCY
           BK.Zero( );
           BK(0,0) = shp[0][k][i];
           BK(1,1) = shp[1][k][i];

           // BK(2,0) for axi-symmetry

           BK(3,0) = shp[1][k][i];
           BK(3,1) = shp[0][k][i];
          **************************************************************/

          double BK00 = shp[0][k][i];
          double BK11 = shp[1][k][i];
          double BK30 = shp[1][k][i];
          double BK31 = shp[0][k][i];

          double littleBK00 = vol_avg_shp[0][k];
          double littleBK01 = vol_avg_shp[1][k];

          //compute stiffness matrix

          // stiffJK =  ( BJtranD + littleBJoneD ) * BK
          //        +  ( BJtranDone + littleBJtranBulk ) * littleBK;

          /**************************************************************
            REPLACING THESE LINES WITH THE 4 BELOW COMMENT FOR EFFICIENCY
          //stiffJK.addMatrixProduct(0.0, A, BK, 1.0);

          //stiff( jj,   kk   ) += stiffJK(0,0) + B1 * littleBK00;
          //stiff( jj+1, kk   ) += stiffJK(1,0) + B2 * littleBK00;
          //stiff( jj,   kk+1 ) += stiffJK(0,1) + B1 * littleBK01;
          //stiff( jj+1, kk+1 ) += stiffJK(1,1) + B2 * littleBK01;
          ***************************************************************/

          // matrixData[  colkk +   jj] += Adata[0]*BK00 + Adata[6]*BK30 + B1 * littleBK00;
          // matrixData[  colkk + jj+1] += Adata[1]*BK00 + Adata[7]*BK30 + B2 * littleBK00;
          // matrixData[colkkP1 +   jj] += Adata[2]*BK11 + Adata[6]*BK31 + B1 * littleBK01;
          // matrixData[colkkP1 + jj+1] += Adata[3]*BK11 + Adata[7]*BK31 + B2 * littleBK01;
          stiff( jj,   kk   ) += Adata[0]*BK00 + Adata[6]*BK30 + B1 * littleBK00;
          stiff( jj+1, kk   ) += Adata[1]*BK00 + Adata[7]*BK30 + B2 * littleBK00;
          stiff( jj,   kk+1 ) += Adata[2]*BK11 + Adata[6]*BK31 + B1 * littleBK01;
          stiff( jj+1, kk+1 ) += Adata[3]*BK11 + Adata[7]*BK31 + B2 * littleBK01;

        } // end for k

      } else { // residual calculation

        //residJ = BJtran * sig;
        residJ.addMatrixVector(0.0,  BJtran, sig, 1.0);
        resid( jj   ) += residJ(0);
        resid( jj+1 ) += residJ(1);
      }

      jj += 2;
    } // end for j

  } //end for i


  return;
}


//shape function routine for four node quads
void XC::ConstantPressureVolumeQuad::shape2d( double ss, double tt,
                                            const double x[2][4],
                                            double shp[3][4],
                                            double &xsj,
                                            Matrix &sx )
{

  int i, j, k;

  double temp;

  static const double s[] = { -0.5,  0.5, 0.5, -0.5 };
  static const double t[] = { -0.5, -0.5, 0.5,  0.5 };

  static double xs[2][2];

  //  static XC::Matrix xs(2,2);

  for( i = 0; i < 4; i++ ) {
      shp[2][i] = ( 0.5 + s[i]*ss )*( 0.5 + t[i]*tt );
      shp[0][i] = s[i] * ( 0.5 + t[i]*tt );
      shp[1][i] = t[i] * ( 0.5 + s[i]*ss );
  } // end for i


  // Construct jacobian and its inverse

  for( i = 0; i < 2; i++ ) {
    for( j = 0; j < 2; j++ ) {

      double value = 0;
      for( k = 0; k < 4; k++ )
        value +=  x[i][k] * shp[j][k];
      // xs(i,j) +=  x[i][k] * shp[j][k];
      xs[i][j] = value;

    } //end for j
  }  // end for i

  xsj = xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];

  sx(0,0) =  xs[1][1] / xsj;
  sx(1,1) =  xs[0][0] / xsj;
  sx(0,1) = -xs[0][1] / xsj;
  sx(1,0) = -xs[1][0] / xsj;

  //form global derivatives

  for( i = 0; i < 4; i++ ) {
    temp      = shp[0][i]*sx(0,0) + shp[1][i]*sx(1,0);
    shp[1][i] = shp[0][i]*sx(0,1) + shp[1][i]*sx(1,1);
    shp[0][i] = temp;
  } // end for i

  return;
}

//! @brief Send object members through the channel being passed as parameter.
int XC::ConstantPressureVolumeQuad::sendData(CommParameters &cp)
  {
    int res= QuadBase4N<NDMaterialPhysicalProperties>::sendData(cp);
    res+= cp.sendDoubles(xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(8));
    res+= cp.sendDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ConstantPressureVolumeQuad::recvData(const CommParameters &cp)
  {
    int res= QuadBase4N<NDMaterialPhysicalProperties>::recvData(cp);
    res+= cp.receiveDoubles(xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(8));
    res+= cp.receiveDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ConstantPressureVolumeQuad::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ConstantPressureVolumeQuad::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
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

