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
//ShellBData.cpp

#include "ShellBData.h"
#include <cstdlib>
#include "utility/matrix/Matrix.h"



//! @brief Initialize the pointerspointers to the shear matrices.
void XC::ShellBData::shear_matrix_ptrs_setup(void)
  {
    //shear matrix pointers
    if(GammaB1pointer.empty())
      {
        GammaB1pointer= std::vector<Matrix>(4);  //four matrix pointers
        GammaB1pointer[0]= Matrix(1,3);  //
        GammaB1pointer[1]= Matrix(1,3);  //    four
        GammaB1pointer[2]= Matrix(1,3);  //  1x3 matrices
        GammaB1pointer[3]= Matrix(1,3);  //
      } //end if B1

    if(GammaD1pointer.empty())
      {
        GammaD1pointer= std::vector<Matrix>(4);
        GammaD1pointer[0]= Matrix(1,3);
        GammaD1pointer[1]= Matrix(1,3);
        GammaD1pointer[2]= Matrix(1,3);
        GammaD1pointer[3]= Matrix(1,3);
      } //end if D1

    if(GammaA2pointer.empty())
      {
        GammaA2pointer= std::vector<Matrix>(4);
        GammaA2pointer[0]= Matrix(1,3);
        GammaA2pointer[1]= Matrix(1,3);
        GammaA2pointer[2]= Matrix(1,3);
        GammaA2pointer[3]= Matrix(1,3);
      } //end if A2

    if(GammaC2pointer.empty())
      {
        GammaC2pointer= std::vector<Matrix>(4);
        GammaC2pointer[0]= Matrix(1,3);
        GammaC2pointer[1]= Matrix(1,3);
        GammaC2pointer[2]= Matrix(1,3);
        GammaC2pointer[3]= Matrix(1,3);
      } //end if C2
  }


void XC::ShellBData::bhat_setup(void)
  {
    if(Bhat.empty())
      {
        Bhat= std::vector<Matrix>(4);
        Bhat[0]= Matrix(2,3);
        Bhat[1]= Matrix(2,3);
        Bhat[2]= Matrix(2,3);
        Bhat[3]= Matrix(2,3);
      } //end if Bhat
  }

//! @brief Constructor
XC::ShellBData::ShellBData(void)
  : GammaB1pointer(0), GammaD1pointer(0),GammaA2pointer(0),GammaC2pointer(0), Bhat(0)
  {
    shear_matrix_ptrs_setup();
    bhat_setup();
  }

const XC::Matrix &XC::ShellBData::getGammaB1(const size_t &node) const
  { return GammaB1pointer[node]; }
XC::Matrix &XC::ShellBData::getGammaB1(const size_t &node)
  { return GammaB1pointer[node]; }
const XC::Matrix &XC::ShellBData::getGammaD1(const size_t &node) const
  { return GammaD1pointer[node]; }
XC::Matrix &XC::ShellBData::getGammaD1(const size_t &node)
  { return GammaD1pointer[node]; }
const XC::Matrix &XC::ShellBData::getGammaA2(const size_t &node) const
  { return GammaA2pointer[node]; }
XC::Matrix &XC::ShellBData::getGammaA2(const size_t &node)
  { return GammaA2pointer[node]; }
const XC::Matrix &XC::ShellBData::getGammaC2(const size_t &node) const
  { return GammaC2pointer[node]; }
XC::Matrix &XC::ShellBData::getGammaC2(const size_t &node)
  { return GammaC2pointer[node]; }
const XC::Matrix &XC::ShellBData::getBhat(const size_t &j) const
  { return Bhat[j]; }
XC::Matrix &XC::ShellBData::getBhat(const size_t &j)
  { return Bhat[j]; }

void XC::ShellBData::ZeroBHat(void)
  {
    Bhat[0].Zero();
    Bhat[1].Zero();
    Bhat[2].Zero();
    Bhat[3].Zero();
  }

void XC::ShellBData::DivideBHat(const double &vol)
  {
    Bhat[0].operator/=(vol);
    Bhat[1].operator/=(vol);
    Bhat[2].operator/=(vol);
    Bhat[3].operator/=(vol);
  }

//! @brief compute standard Bshear matrix
const XC::Matrix &XC::ShellBData::computeBshear(const size_t &node, const double shp[3][4] ) const
  {
    static Matrix Bshear(2,3);

//---Bshear XC::Matrix in standard {1,2,3} mechanics notation------
//
//             -                -
//   Bshear= | +N,1      0    +N |  (2x3)
//            | +N,2     -N     0 |
//             -                -
//
//  two(2) shear strains, one(1) displacement and two(2) rotations for plate
//-----------------------------------------------------------------------

    Bshear.Zero( );

    Bshear(0,0)=  shp[0][node];
    Bshear(0,2)=  shp[2][node];
    Bshear(1,0)=  shp[1][node];
    Bshear(1,1)= -shp[2][node];

    return Bshear;

  }
//! @brief compute Bbar shear matrix
const XC::Matrix &XC::ShellBData::computeBbarShear(const size_t &node,const double &L1,const double &L2,const Matrix &Jinv) const
  {
      static Matrix Bshear(2,3);
      static Matrix BshearNat(2,3);

      static Matrix JinvTran(2,2);  // J-inverse-transpose

      static Matrix Gamma1(1,3);
      static Matrix Gamma2(1,3);

      static Matrix temp1(1,3);
      static Matrix temp2(1,3);


      //JinvTran= transpose( 2, 2, Jinv );
      JinvTran(0,0)= Jinv(0,0);
      JinvTran(1,1)= Jinv(1,1);
      JinvTran(0,1)= Jinv(1,0);
      JinvTran(1,0)= Jinv(0,1);


      // compute BShear from Bbar interpolation

      //Gamma1= ( 1.0 - L2 )*( *GammaB1pointer[node] )  +
      //         ( 1.0 + L2 )*( *GammaD1pointer[node] )  ;
      temp1= GammaB1pointer[node];
      temp1*= ( 1.0 - L2 );

      temp2= GammaD1pointer[node];
      temp2*= ( 1.0 + L2 );

      Gamma1 = temp1;
      Gamma1 += temp2;


      //Gamma2= ( 1.0 - L1 )*( *GammaA2pointer[node] )  +
      //         ( 1.0 + L1 )*( *GammaC2pointer[node] )  ;
      temp1= GammaA2pointer[node];
      temp1*= ( 1.0 - L1 );

      temp2= GammaC2pointer[node];
      temp2*= ( 1.0 + L1 );

      Gamma2= temp1;
      Gamma2+= temp2;

      //don't forget the 1/2
      //Gamma1 *= 0.50;
      //Gamma2 *= 0.50;


      BshearNat(0,0)= Gamma1(0,0);
      BshearNat(0,1)= Gamma1(0,1);
      BshearNat(0,2)= Gamma1(0,2);

      BshearNat(1,0)= Gamma2(0,0);
      BshearNat(1,1)= Gamma2(0,1);
      BshearNat(1,2)= Gamma2(0,2);


      //strain XC::BJtensor push on BshearNat
      //Bshear= ( JinvTran * BshearNat );
      Bshear.addMatrixProduct(0.0,  JinvTran,BshearNat,0.5 );
      //note the 1/2 -----------------------------------^

      return Bshear;
  }

void XC::ShellBData::computeB(const double shp[3][4],const double &L1,const double &L2,const Matrix &J0inv,const double &dv)
  {
    for(size_t node= 0;node<4;node++)
      {
        //compute B shear matrix for this node
        //Bhat[node] += (  dv * computeBshear(node,shp)  );
        Bhat[node].addMatrix(1.0, computeBshear(node,shp),dv );

        //compute B-bar shear matrix for this node
        //Bhat[node] -= ( dv *
        //          computeBbarShear( node, sg[i], tg[i], J0inv )
        //                );
        Bhat[node].addMatrix(1.0, computeBbarShear(node,L1,L2,J0inv),-dv);
      } //end for node
  }

