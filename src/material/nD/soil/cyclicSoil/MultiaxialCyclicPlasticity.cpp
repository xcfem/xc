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
** ****************************************************************** */
                                                                        
 
/*----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*
 |                                                                          | 
 |              MultiaxialCyclicPlasticity  NDMaterial                      |
 +                                                                          +
 |--------------------------------------------------------------------------|
 |                                                                          |
 +             Authors: Gang Wang  AND  Professor Nicholas Sitar            +
 |                                                                          |
 |             Department of Civil and Environmental Engineering            |
 +             University of California, Berkeley, CA 94720, USA            +
 |                                                                          |
 |             Email: wang@ce.berkeley.edu(G.W.)                            |
 +                                                                          + 
 |  Disclaimers:                                                            |
 |  (1) This is implementation of MultiaxialCyclicPlasticity for clays      |
 +      Model References:                                                   +
 |      Borja R.I, Amies, A.P. Multiaxial Cyclic Plasticity Model for       |
 |            Clays, ASCE J. Geotech. Eng. Vol 120, No 6, 1051-1070         |
 +      Montans F.J, Borja R.I. Implicit J2-bounding Surface Plasticity     +    
 |            using Prager's translation rule. Int. J. Numer. Meth. Engng.  |
 |            55:1129-1166, 2002                                            |
 +      Code References:                                                    +
 |      Ignacio Romero and Adrian Rodriguez Marek, Brick element model with |
 |            a Multiaxial Cyclic Plasticity Model, in GEOFEAP, UC Berkeley |
 +  (2) Questions regarding this code should be directed to Gang Wang       +
 |  (3) Documentation could be found at                                     |
 |        www.ce.berkeley.edu/~wang/papers/MultiaxialCyclicPlasticity.pdf   |
 +                                                                          +
 |  Development History:                                                    |
 |  First Draft   -- April 2004                                             |
 +  Rewrite       -- Nov   2004                                             + 
 |  Final Release --                                                        |
 |                                                                          | 
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
                              User Command

   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    nDMaterial XC::MultiaxialCyclicPlasticity $tag, $rho, $K, $G,
                                          $Su , $Ho , $h, $m, $beta, $KCoeff
   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   where: 
      tag   : tag for this material
      rho   : density
      K     : buck modulus
      G     : maximum (small strain) shear modulus
      Su    : undrained shear strength, size of bounding surface R=sqrt(8/3)*Su
      Ho    : linear kinematic hardening modulus of bounding surface
      h     : hardening parameter
      m     : hardening parameter
      beta  : integration parameter, usually beta=0.5
      KCoeff: coefficient of earth pressure, K0

  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#include <cmath>
#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity.h>
#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity3D.h> 
#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityAxiSymm.h>
#include <material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityPlaneStrain.h>
#include <domain/mesh/element/utils/Information.h>
#include "material/nD/NDMaterialType.h"

//this is mike's problem
XC::Tensor XC::MultiaxialCyclicPlasticity::rank2(2, def_dim_2, 0.0 );
XC::Tensor XC::MultiaxialCyclicPlasticity::rank4(2, def_dim_2, 0.0 );

//parameters
const double XC::MultiaxialCyclicPlasticity::one3   = 1.0 / 3.0;
const double XC::MultiaxialCyclicPlasticity::two3   = 2.0 / 3.0;
const double XC::MultiaxialCyclicPlasticity::four3  = 4.0 / 3.0;
const double XC::MultiaxialCyclicPlasticity::root23 = sqrt( 2.0 / 3.0 );
const double XC::MultiaxialCyclicPlasticity::infinity  = 1.0e12;

double XC::MultiaxialCyclicPlasticity::initialTangent[3][3][3][3];   //material tangent
double XC::MultiaxialCyclicPlasticity::IIdev[3][3][3][3]; //rank 4 deviatoric 
double XC::MultiaxialCyclicPlasticity::IbunI[3][3][3][3]; //rank 4 I bun I 

int XC::MultiaxialCyclicPlasticity::MaterialStageID = 2;   // classwide load stage tag, 
                                                       // elasto-plastic by default
int XC::MultiaxialCyclicPlasticity::IncrFormulationFlag=1;


//zero internal variables
void XC::MultiaxialCyclicPlasticity::initialize( ) 
{
  stress.Zero();
  strain.Zero();
  stress_n.Zero();
  strain_n.Zero();
  backs.Zero();                  // back stress fopr BS
  backs_n.Zero(); 
  so.Zero();                     // reversal deviatoric back stress
  so_n.Zero();
  

  // some flags

  flagjustunload=0;
  flagfirstload=0;
  icounter=0;
  iternum=0;
  plasticflag=0;
  plasticflag_n=0;
 
  // initialize s0 and kappa
  kappa = infinity;
  Psi   = 2 * shear;
  load  = 0.0; 
  //std::cerr<<"MCP::debug128: kappa = "<< kappa <<std::endl;
  X[1] = 2 * shear;
  X[2] = kappa;
  alp=0;
}


//null constructor
XC::MultiaxialCyclicPlasticity::MultiaxialCyclicPlasticity(int tag,int classtag)
  : XC::NDMaterial(tag,classtag), strain(3,3), stress(3,3), backs(3,3), so(3,3),
  strain_n(3,3), stress_n(3,3), backs_n(3,3), so_n(3,3)
{ 
  bulk        = 0.0;
  shear       = 0.0;
  bulk_K0     = 0.0;
  shear_K0    = 0.0;
  eta         = 0.0;

  density     = 0.0; 

  this->initialize( );     // or (*this).zero( ) 

  int i, j, k, l;

  //zero rank4 IIdev and IbunI 
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ )  {
      for( k = 0; k < 3; k++ ) {
            for( l = 0; l < 3; l++)  { 

              IbunI[i][j][k][l] = 0.0;

              IIdev[i][j][k][l] = 0.0;

            } // end for l
      } // end for k
    } // end for j
  } // end for i


  //form rank4 IbunI 

  IbunI [0][0] [0][0] = 1.0;
  IbunI [0][0] [1][1] = 1.0;
  IbunI [0][0] [2][2] = 1.0;
  IbunI [1][1] [0][0] = 1.0;
  IbunI [1][1] [1][1] = 1.0;
  IbunI [1][1] [2][2] = 1.0;
  IbunI [2][2] [0][0] = 1.0;
  IbunI [2][2] [1][1] = 1.0;
  IbunI [2][2] [2][2] = 1.0;

  //form rank4 IIdev

  IIdev [0][0] [0][0] =  two3; // 0.666667 
  IIdev [0][0] [1][1] = -one3; //-0.333333 
  IIdev [0][0] [2][2] = -one3; //-0.333333 
  IIdev [0][1] [0][1] = 0.5;
  IIdev [0][1] [1][0] = 0.5;
  IIdev [0][2] [0][2] = 0.5;
  IIdev [0][2] [2][0] = 0.5;
  IIdev [1][0] [0][1] = 0.5;
  IIdev [1][0] [1][0] = 0.5;
  IIdev [1][1] [0][0] = -one3; //-0.333333 
  IIdev [1][1] [1][1] =  two3; // 0.666667 
  IIdev [1][1] [2][2] = -one3; //-0.333333 
  IIdev [1][2] [1][2] = 0.5;
  IIdev [1][2] [2][1] = 0.5;
  IIdev [2][0] [0][2] = 0.5;
  IIdev [2][0] [2][0] = 0.5;
  IIdev [2][1] [1][2] = 0.5;
  IIdev [2][1] [2][1] = 0.5;
  IIdev [2][2] [0][0] = -one3; //-0.333333 
  IIdev [2][2] [1][1] = -one3; //-0.333333 
  IIdev [2][2] [2][2] =  two3; // 0.666667 
}


//null constructor
 XC::MultiaxialCyclicPlasticity:: MultiaxialCyclicPlasticity( ) : 
XC::NDMaterial( ), strain(3,3), stress(3,3), backs(3,3), so(3,3),
 strain_n(3,3), stress_n(3,3), backs_n(3,3), so_n(3,3)
{ 
  bulk        = 0.0;
  shear       = 0.0;
  bulk_K0     = 0.0;
  shear_K0    = 0.0;
  eta         = 0.0;

  density     = 0.0; 

  this->initialize( );     // or(*this).zero( ) 

  int i, j, k, l;

  //zero rank4 IIdev and IbunI 
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ )  {
      for( k = 0; k < 3; k++ ) {
            for( l = 0; l < 3; l++)  { 

              IbunI[i][j][k][l] = 0.0;

              IIdev[i][j][k][l] = 0.0;

            } // end for l
      } // end for k
    } // end for j
  } // end for i


  //form rank4 IbunI 

  IbunI [0][0] [0][0] = 1.0;
  IbunI [0][0] [1][1] = 1.0;
  IbunI [0][0] [2][2] = 1.0;
  IbunI [1][1] [0][0] = 1.0;
  IbunI [1][1] [1][1] = 1.0;
  IbunI [1][1] [2][2] = 1.0;
  IbunI [2][2] [0][0] = 1.0;
  IbunI [2][2] [1][1] = 1.0;
  IbunI [2][2] [2][2] = 1.0;

  //form rank4 IIdev

  IIdev [0][0] [0][0] =  two3; // 0.666667 
  IIdev [0][0] [1][1] = -one3; //-0.333333 
  IIdev [0][0] [2][2] = -one3; //-0.333333 
  IIdev [0][1] [0][1] = 0.5;
  IIdev [0][1] [1][0] = 0.5;
  IIdev [0][2] [0][2] = 0.5;
  IIdev [0][2] [2][0] = 0.5;
  IIdev [1][0] [0][1] = 0.5;
  IIdev [1][0] [1][0] = 0.5;
  IIdev [1][1] [0][0] = -one3; //-0.333333 
  IIdev [1][1] [1][1] =  two3; // 0.666667 
  IIdev [1][1] [2][2] = -one3; //-0.333333 
  IIdev [1][2] [1][2] = 0.5;
  IIdev [1][2] [2][1] = 0.5;
  IIdev [2][0] [0][2] = 0.5;
  IIdev [2][0] [2][0] = 0.5;
  IIdev [2][1] [1][2] = 0.5;
  IIdev [2][1] [2][1] = 0.5;
  IIdev [2][2] [0][0] = -one3; //-0.333333 
  IIdev [2][2] [1][1] = -one3; //-0.333333 
  IIdev [2][2] [2][2] =  two3; // 0.666667 
}


//full constructor
XC::MultiaxialCyclicPlasticity::MultiaxialCyclicPlasticity(int    tag,
                                                   int    classTag,
                                                   double rho,
                                                   double K,
                                                   double G,
                                                   double Su,
                                                   double Ho_kin,
                                                   double Parameter_h,
                                                   double Parameter_m,
                                                   double Parameter_beta,
                                                   double Kcoeff,
                                                   double viscosity)
  : XC::NDMaterial(tag, ND_TAG_MultiaxialCyclicPlasticity),
    strain(3,3),stress(3,3),backs(3,3), so(3,3),  
    strain_n(3,3),stress_n(3,3), backs_n(3,3), so_n(3,3)
{
  density     = rho; 
  bulk        = K;
  shear       = G;
  R           = sqrt(8.0/3.0)*Su;
  //std::cerr<<"MCP::235 R= "<<R <<std::endl; 
  Ho          = Ho_kin;        // kinematic hardening modulus
  h                          = Parameter_h; 
  m           = Parameter_m;
  beta        = Parameter_beta;
  eta         = viscosity;
 
  ///////////// compute modulus for stage1 K0 consolidation ////////////
  K0 = Kcoeff;
  // Poisson's ratio v=K0/(1+K0)
  double v = K0 /(1+K0); 
  double E = 9 * bulk * shear / (3*bulk+shear);
  // compute shear and bulk modulus for K0 consolidation, cf. Fung, p.141
  shear_K0 =  E /(2*(1+v));
  bulk_K0  =  E/(3*(1-2*v));
  ///////////////////////////////////////////////////////////////
  
  if(tag==200) { // to be a fluid
      v = 0.499;
      shear_K0 =  1;
      bulk_K0  *=  1000;
  }

  this->initialize( );

  int i, j, k, l;

  //zero rank4 IIdev and IbunI 
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ ) {
      for( k = 0; k < 3; k++ ) {
            for( l = 0; l < 3; l++)  { 

              IbunI[i][j][k][l] = 0.0;

              IIdev[i][j][k][l] = 0.0;

            } // end for l
      } // end for k
    } // end for j
  } // end for i


  //form rank4 IbunI 

  IbunI [0][0] [0][0] = 1.0;
  IbunI [0][0] [1][1] = 1.0;
  IbunI [0][0] [2][2] = 1.0;
  IbunI [1][1] [0][0] = 1.0;
  IbunI [1][1] [1][1] = 1.0;
  IbunI [1][1] [2][2] = 1.0;
  IbunI [2][2] [0][0] = 1.0;
  IbunI [2][2] [1][1] = 1.0;
  IbunI [2][2] [2][2] = 1.0;

  //form rank4 IIdev

  IIdev [0][0] [0][0] =  two3; // 0.666667 
  IIdev [0][0] [1][1] = -one3; //-0.333333 
  IIdev [0][0] [2][2] = -one3; //-0.333333 
  IIdev [0][1] [0][1] = 0.5;
  IIdev [0][1] [1][0] = 0.5;
  IIdev [0][2] [0][2] = 0.5;
  IIdev [0][2] [2][0] = 0.5;
  IIdev [1][0] [0][1] = 0.5;
  IIdev [1][0] [1][0] = 0.5;
  IIdev [1][1] [0][0] = -one3; //-0.333333 
  IIdev [1][1] [1][1] =  two3; // 0.666667 
  IIdev [1][1] [2][2] = -one3; //-0.333333 
  IIdev [1][2] [1][2] = 0.5;
  IIdev [1][2] [2][1] = 0.5;
  IIdev [2][0] [0][2] = 0.5;
  IIdev [2][0] [2][0] = 0.5;
  IIdev [2][1] [1][2] = 0.5;
  IIdev [2][1] [2][1] = 0.5;
  IIdev [2][2] [0][0] = -one3; //-0.333333 
  IIdev [2][2] [1][1] = -one3; //-0.333333 
  IIdev [2][2] [2][2] =  two3; // 0.666667 
}


//elastic constructor
 XC::MultiaxialCyclicPlasticity::MultiaxialCyclicPlasticity(  int tag, int classTag, 
                                               double rho, double K, double G ) 
:XC::NDMaterial(tag, classTag),
  strain(3,3),stress(3,3), backs(3,3),so(3,3),
  strain_n(3,3), stress_n(3,3),  backs_n(3,3), so_n(3,3)
{ 
  density     = rho; 
  bulk        = K;
  shear       = G; 
  bulk_K0     = K;
  shear_K0    = G; 
  eta         = 0.0;

  this->initialize( );

  int i, j, k, l;

  //zero rank4 IIdev and IbunI 
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ )  {
      for( k = 0; k < 3; k++ ) {
            for( l = 0; l < 3; l++)  { 

              IbunI[i][j][k][l] = 0.0;

              IIdev[i][j][k][l] = 0.0;

            } // end for l
      } // end for k
    } // end for j
  } // end for i


  //form rank4 IbunI 

  IbunI [0][0] [0][0] = 1.0;
  IbunI [0][0] [1][1] = 1.0;
  IbunI [0][0] [2][2] = 1.0;
  IbunI [1][1] [0][0] = 1.0;
  IbunI [1][1] [1][1] = 1.0;
  IbunI [1][1] [2][2] = 1.0;
  IbunI [2][2] [0][0] = 1.0;
  IbunI [2][2] [1][1] = 1.0;
  IbunI [2][2] [2][2] = 1.0;

  //form rank4 IIdev

  IIdev [0][0] [0][0] =  two3; // 0.666667 
  IIdev [0][0] [1][1] = -one3; //-0.333333 
  IIdev [0][0] [2][2] = -one3; //-0.333333 
  IIdev [0][1] [0][1] = 0.5;
  IIdev [0][1] [1][0] = 0.5;
  IIdev [0][2] [0][2] = 0.5;
  IIdev [0][2] [2][0] = 0.5;
  IIdev [1][0] [0][1] = 0.5;
  IIdev [1][0] [1][0] = 0.5;
  IIdev [1][1] [0][0] = -one3; //-0.333333 
  IIdev [1][1] [1][1] =  two3; // 0.666667 
  IIdev [1][1] [2][2] = -one3; //-0.333333 
  IIdev [1][2] [1][2] = 0.5;
  IIdev [1][2] [2][1] = 0.5;
  IIdev [2][0] [0][2] = 0.5;
  IIdev [2][0] [2][0] = 0.5;
  IIdev [2][1] [1][2] = 0.5;
  IIdev [2][1] [2][1] = 0.5;
  IIdev [2][2] [0][0] = -one3; //-0.333333 
  IIdev [2][2] [1][1] = -one3; //-0.333333 
  IIdev [2][2] [2][2] =  two3; // 0.666667 
}


XC::NDMaterial *XC::MultiaxialCyclicPlasticity::getCopy(const std::string &type) const
  {
    NDMaterial *retval= nullptr;
    if((type==strTypePlaneStress2D) || (type==strTypePlaneStress))
      std::cerr << "MultiaxialCyclicPlasticity type plane stress material is NOT available now....";
    else if((type==strTypePlaneStrain2D) || (type==strTypePlaneStrain))
      retval= new MultiaxialCyclicPlasticityPlaneStrain(this->getTag(), density, bulk, shear, sqrt(3.0/8.0)*R,
                      Ho, h, m, beta, K0, eta); 
    else if((type==strTypeAxiSymmetric2D) || (type==strTypeAxiSymmetric))
      retval= new MultiaxialCyclicPlasticityAxiSymm(this->getTag(), density, bulk, shear, sqrt(3.0/8.0)*R,
                      Ho, h, m, beta, K0, eta); 
    else if(((type==strTypeThreeDimensional)) || ((type==strType3D)))
      retval= new MultiaxialCyclicPlasticity3D(this->getTag(), density, bulk, shear, sqrt(3.0/8.0)*R,
                                             Ho, h, m, beta, K0, eta); 
    else if( ((type==strTypePlateFiber)) )
      std::cerr << "MultiaxialCyclicPlasticity type plate fiber material is NOT available now....";
    else // Handle other cases
      std::cerr << "MultiaxialCyclicPlasticity::getModel failed to get model: " << type << std::endl;
    return retval;
  }

//print out material data
void XC::MultiaxialCyclicPlasticity::Print( std::ostream &s, int flag ) const
{
  s << std::endl;;
  s << "MultiaxialCyclicPlasticity : "; 
  s << this->getType( )  << std::endl;;
  s << "K    =   " << bulk        << std::endl;;
  s << "Gmax =   " << shear       << std::endl;;
  s << "Rho  =   " << density     << std::endl;;
  s << "R    =   " << R           << std::endl;;
  s << "Ho   =   " << Ho          << std::endl;;
  s << "h    =   " << h           << std::endl;;
  s << "m    =   " << m           << std::endl;;
  s << "beta =   " << beta        << std::endl;;
  s << "eta  =   " << eta         << std::endl;;
  s << std::endl;;
}


void XC::MultiaxialCyclicPlasticity::elastic_integrator(void)
  {
    static Matrix dev_strain(3,3); //deviatoric strain  
    static Matrix dev_stress(3,3); //deviatoric stress

    // add
    double pressure= 0.0;  // 1/3 trace(stress) 
    double trace = 0.0; //trace of strain

    int i,j,k,l;
    int ii, jj; 

    if(IncrFormulationFlag==0)
      {
        trace = strain(0,0) + strain(1,1) + strain(2,2);
        //compute the deviatoric strains
        dev_strain = strain;

        for( i = 0; i < 3; i++ )
          dev_strain(i,i) -= ( one3*trace );

        //compute the trial deviatoric stresses
        dev_stress = dev_strain;
        dev_stress *= 2.0 * shear_K0;        
        // compute the trial pressure    
        pressure   = trace;
        pressure  *= bulk_K0;                                                 
      }

    static Matrix IncrStrain(3,3);
    static Matrix DevStress_n(3,3);
    static double pressure_n;

    if(IncrFormulationFlag==1)
      {
       // std::cerr<<"DP::elasticintegrator:stress_n"<<stress_n;
       // std::cerr<<"DP::elasticintegrator:strain_n"<<strain_n;

       IncrStrain= strain;
       IncrStrain-= strain_n;

       trace = IncrStrain(0,0) + IncrStrain(1,1) + IncrStrain(2,2);

       dev_strain = IncrStrain;
       for( i = 0; i < 3; i++ )  dev_strain(i,i) -= ( one3*trace );
       pressure_n  = one3 * (stress_n(0,0)+stress_n(1,1)+stress_n(2,2));
       DevStress_n = stress_n;
  
       for( i = 0; i < 3; i++ )  DevStress_n(i,i) -= pressure_n;

       // Delta_S = 2*shear*Delta_e
       // Delta_p = bulk * Delta_theta
  
       // incremental formulation, NOTE: now dev_strain and trace are INCREMENTAL strains
       dev_stress = dev_strain;
       dev_stress *= 2.0 * shear_K0;
       dev_stress += DevStress_n;
       pressure = trace;
       pressure *= bulk_K0;
       pressure += pressure_n;
     }  
  
    // add on bulk part of stress, compute TOTAL stress at t=n+1
    stress = dev_stress;
    for( i = 0; i < 3; i++ )
      stress(i,i) += pressure;


    //compute the tangent 
    for( ii = 0; ii < 6; ii++ )
      {
        for( jj = 0; jj < 6; jj++ )
          {
            index_map( ii, i, j );
            index_map( jj, k, l );

            //elastic terms
            tangent[i][j][k][l]  = bulk_K0 * IbunI[i][j][k][l];

            tangent[i][j][k][l] += (2.0*shear_K0) * IIdev[i][j][k][l];

            //minor symmetries 
            tangent [j][i][k][l] = tangent[i][j][k][l];
            tangent [i][j][l][k] = tangent[i][j][k][l];
            tangent [j][i][l][k] = tangent[i][j][k][l];

          } // end for jj
      } // end for ii
    flagfirstload=0; // prepare for MCP integrator
    return;
  } 




// set up for initial elastic
void XC::MultiaxialCyclicPlasticity::doInitialTangent(void) const
{
  int ii,jj,i,j,k,l;

  //compute the deviatoric strains
  for( ii = 0; ii < 6; ii++ ) {
    for( jj = 0; jj < 6; jj++ )  {

          index_map( ii, i, j );
          index_map( jj, k, l );

          //elastic terms
          initialTangent[i][j][k][l]  = bulk * IbunI[i][j][k][l];
          initialTangent[i][j][k][l] += (2.0*shear) * IIdev[i][j][k][l];

          //minor symmetries 
          initialTangent [j][i][k][l] = initialTangent[i][j][k][l];
          initialTangent [i][j][l][k] = initialTangent[i][j][k][l];
          initialTangent [j][i][l][k] = initialTangent[i][j][k][l];

    } // end for jj
  } // end for ii

  return;
} 



//matrix_index ---> BJtensor indices i,j
void XC::MultiaxialCyclicPlasticity::index_map( int matrix_index, int &i, int &j ) const
{
  switch( matrix_index+1 ) { //add 1 for standard XC::BJtensor indices

    case 1 :
      i = 1; 
      j = 1;
      break;
 
    case 2 :
      i = 2;
      j = 2; 
      break;

    case 3 :
      i = 3;
      j = 3;
      break;

    case 4 :
      i = 1;
      j = 2;
      break;

    case 5 :
      i = 2;
      j = 3;
      break;

    case 6 :
      i = 3;
      j = 1;
      break;


    default :
      i = 1;
      j = 1;
      break;

  } //end switch

i--; //subtract 1 for C-indexing
j--;

return; 
}


XC::NDMaterial *XC::MultiaxialCyclicPlasticity::getCopy(void) const
  {
    std::cerr << "MultiaxialCyclicPlasticity::getCopy -- subclass responsibility\n"; 
    exit(-1);
    return 0;
  }

const std::string &XC::MultiaxialCyclicPlasticity::getType(void) const
  {
    std::cerr << "MultiaxialCyclicPlasticity::getType -- subclass responsibility\n";
    exit(-1);
    return strTypeError;
  }

int XC::MultiaxialCyclicPlasticity::getOrder(void) const
  {
    std::cerr << "MultiaxialCyclicPlasticity::getOrder -- subclass responsibility\n";
    exit(-1);
    return 0;
  }


int 
XC::MultiaxialCyclicPlasticity::commitState( ) 
{
 
  stress_n   = stress;     // april 2, 2004
  strain_n   = strain;     // april 2, 2004

  backs_n    = backs;
  so_n       = so; 
 
  Psi   = X[1];
  kappa = X[2];

  plasticflag_n=plasticflag;
  if(plasticflag==2 ){
      plasticflag_n=1;
  }

  iternum=0;   // reset global newton iteration counter
  return 0;

}


int XC::MultiaxialCyclicPlasticity::revertToLastCommit( ) 
  { return 0; }


int XC::MultiaxialCyclicPlasticity::revertToStart( )
  {
    this->initialize( );
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::MultiaxialCyclicPlasticity::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendDoubles(K0,bulk_K0,shear_K0,bulk,shear,getDbTagData(),CommMetaData(1));
    res+= comm.sendDoubles(density,R,Ho,h,m,beta,getDbTagData(),CommMetaData(2));
    res+= comm.sendInts(flagjustunload,flagfirstload,icounter,iternum,getDbTagData(),CommMetaData(3));
    res+= comm.sendInts(plasticflag,plasticflag_n,EleTag,getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(eta,kappa,Psi,X[0],X[1],X[2],getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(alp,load,getDbTagData(),CommMetaData(6));
    res+= comm.sendMatrix(strain,getDbTagData(),CommMetaData(7));
    res+= comm.sendMatrix(stress,getDbTagData(),CommMetaData(8));
    res+= comm.sendMatrix(backs,getDbTagData(),CommMetaData(9));
    res+= comm.sendMatrix(so,getDbTagData(),CommMetaData(10));
    res+= comm.sendMatrix(strain_n,getDbTagData(),CommMetaData(11));
    res+= comm.sendMatrix(stress_n,getDbTagData(),CommMetaData(12));
    res+= comm.sendMatrix(backs_n,getDbTagData(),CommMetaData(13));
    res+= comm.sendMatrix(so_n,getDbTagData(),CommMetaData(14));
    size_t conta= 15;
    for(size_t i=0;i<3;i++)
      for(size_t j=0;j<3;j++)
         for(size_t k=0;k<3;k++)
            res+= comm.sendDoubles(tangent[i][j][k][0],tangent[i][j][k][1],tangent[i][j][k][2],getDbTagData(),CommMetaData(conta++));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MultiaxialCyclicPlasticity::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveDoubles(K0,bulk_K0,shear_K0,bulk,shear,getDbTagData(),CommMetaData(1));
    res+= comm.receiveDoubles(density,R,Ho,h,m,beta,getDbTagData(),CommMetaData(2));
    res+= comm.receiveInts(flagjustunload,flagfirstload,icounter,iternum,getDbTagData(),CommMetaData(3));
    res+= comm.receiveInts(plasticflag,plasticflag_n,EleTag,getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(eta,kappa,Psi,X[0],X[1],X[2],getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(alp,load,getDbTagData(),CommMetaData(6));
    res+= comm.receiveMatrix(strain,getDbTagData(),CommMetaData(7));
    res+= comm.receiveMatrix(stress,getDbTagData(),CommMetaData(8));
    res+= comm.receiveMatrix(backs,getDbTagData(),CommMetaData(9));
    res+= comm.receiveMatrix(so,getDbTagData(),CommMetaData(10));
    res+= comm.receiveMatrix(strain_n,getDbTagData(),CommMetaData(11));
    res+= comm.receiveMatrix(stress_n,getDbTagData(),CommMetaData(12));
    res+= comm.receiveMatrix(backs_n,getDbTagData(),CommMetaData(13));
    res+= comm.receiveMatrix(so_n,getDbTagData(),CommMetaData(14));
    size_t conta= 15;
    for(size_t i=0;i<3;i++)
      for(size_t j=0;j<3;j++)
         for(size_t k=0;k<3;k++)
            res+= comm.receiveDoubles(tangent[i][j][k][0],tangent[i][j][k][1],tangent[i][j][k][2],getDbTagData(),CommMetaData(conta++));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::MultiaxialCyclicPlasticity::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(96);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::MultiaxialCyclicPlasticity::recvSelf(const Communicator &comm)
  {
    inicComm(88);
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

int XC::MultiaxialCyclicPlasticity::updateParameter(int responseID, Information &info)
{

  // not finished
  this-> MaterialStageID = responseID;
   return 0;
}


// GEOFEAP subroutine model34(d,eps,sig,cc,cee,hn,h1,nhv,isw,n)
void XC::MultiaxialCyclicPlasticity::plastic_integrator()
{ 

  int unloadflag;

  // toleraces

  static double ZERO      = 1.0e-16; 

  static double zerotol   = 1.0e-10;    // tol for zeroloading...||de||<zerotol
  static double tolforload= 1.0e-10;     // tol to determine loading/unloading
  static double tolforX   = 1.0e-6;       // tol for compute norm g1g2 in iteration X[1], X[2]
  
  double twomu; 
  int ii,jj;              // for loop iterators
  int i,j,k,l;

  static XC::Matrix de(3,3);   //incremental deviatoric strain  
  static XC::Matrix  s(3,3);   //deviatoric stress
 // add
  static double p;          // pressure, 1/3 trace(stress) 
  static double e;          // incr. vol.strain, trace(incremental strains)

  static XC::Matrix IncrStrain(3,3); // Frank let all XC::Matrix be static
  static XC::Matrix s_n(3,3);        // dev. stress at t_n 
  static double p_n;             // pressure at t_n
  //static XC::Matrix soinit(3,3);   // save s0_n here
  double normchi=0;
  static XC::Matrix strial(3,3);
  static XC::Matrix chitri(3,3);
  static XC::Matrix alpha_n(3,3); // backstress of loading surface at t_n
  static double  Psi_split; // Psi for strain split step
  static XC::Matrix temp6(3,3);
  static double dottemp6;
  double norm = 0;
    
  double Hn =0;
  double Hn1=0;
  double ftrial;
  double temp;
  double normde= 0;
  double fn= 0;
  int zeroloadflag;
  int showdebugInfo;

  showdebugInfo=0;

  XC::Vector debugInfo(10);

  debugInfo.Zero();
  for( i = 0; i < 10; i++ )  debugInfo(i) = -1.0;

  unloadflag=0;
  zeroloadflag=0;
  load = 0;
  icounter=0;
  flagjustunload = 0;
  X[1]=0; X[2]=0;

  iternum++;

  // (0) calculate input incr. strain: (de, e)
  //     retrieve stress and stress of t_n step(s_n, p_n)


  IncrStrain = strain;
  IncrStrain -= strain_n;       // incremental
  e  = IncrStrain(0,0) + IncrStrain(1,1) + IncrStrain(2,2);
  de = IncrStrain;
  for( i = 0; i < 3; i++ )  de(i,i) -= ( one3*e );
 
  // compute p and s 
  p_n = one3 * (stress_n(0,0)+stress_n(1,1)+stress_n(2,2));
  s_n = stress_n;

  for( i = 0; i < 3; i++ )  s_n(i,i) -= p_n;
 
  flagfirstload++;

  // initialize so_n if it is a very first call of this routine
  if(flagfirstload==1){
      //std::cerr<<"MCP::firstload"<<std::endl;
      so_n=s_n;
      so=so_n;
      backs_n=s_n;
      kappa=infinity;
      Psi=2*shear;
      plasticflag=0;
      X[1]=2*shear;
      X[2]=infinity;
      debugInfo(1)=1;
      goto LABEL10;
  }

  so=so_n;


  // normde=||de||
  normde=0;
  for( i = 0; i < 3; i++ ){
      for( j = 0; j < 3; j++ ) {
              normde += de(i,j)*de(i,j);
      }
  } //end for i 

  if(normde >= 0.0) {
      normde=sqrt(normde); 
  } else {
      std::cerr<<"MCP:1061:problem!!"<<std::endl;
  }

  if(normde<=zerotol)
    {
      X[1] = 2*shear;
      X[2] = infinity;
      plasticflag=0;
      load = 1000; 
      zeroloadflag=1;
      debugInfo(1)=2;
      //std::cerr<<"MCP::small strain incr" <<std::endl;
      goto LABEL10;
    }
  MCPparameter(9) = normde;



  //(1) Solve Kappa_n from s_n 
  //    assign plasticflag_n

   double kappasave;

   kappasave=kappa;  // this kappa is obtained from previous solution of X[2]
   //int plasticflagsave= plasticflag_n;

   fn=0;

   chitri=s_n-backs_n;
   normchi=0;
   for( i = 0; i < 3; i++ ){
       for( j = 0; j < 3; j++ ) {
               normchi += chitri(i,j)*chitri(i,j);
       }
    } //end for i 

    if(normchi >= 0) {
      normchi =sqrt(normchi); 
    } else {
      normchi = 0;
      std::cerr<<"Problem!!"<<std::endl;
    }

    fn= normchi - R;

    if((fn >=0)&&(plasticflag_n!=1))
    {
            std::cerr<<"MCP::995:strange, fn="<<fn<<" plasticflag_n="<<plasticflag_n<<std::endl;
            showdebugInfo=1;
    }

    if(fn >=0 ){
        kappa=0;
            plasticflag_n=1;
            //std::cerr<<"MCP::tn on yield surface"<<std::endl;
    }
    else {

            plasticflag_n=0;
            // compute kappa from s_n, backs_n, so
            double n1 =0;
            double n2 =0;
            double t1dott2=0;

            n1=0.0;

            for( i = 0; i < 3; i++ ){
                    for( j = 0; j < 3; j++ ) {
                            temp=s_n(i,j)-backs_n(i,j);
                        n1 += temp*temp;
                    }
            } //end for i 
            
            if(n1 >= 0.0) {
                    n1=sqrt(n1); 
            }

            n2=0.0;
            for( i = 0; i < 3; i++ ){
                    for( j = 0; j < 3; j++ ) {
                temp=s_n(i,j)-so_n(i,j);
                            n2 += temp*temp ;
                    }
            } //end for i 

            if(n2 >= 0.0) {
                    n2=sqrt(n2); 
            }

            if(n2==0){ // just unload
                    kappa=infinity;
            } else{
                    t1dott2=0;
                    for( i = 0; i < 3; i++ ){
                            for( j = 0; j < 3; j++ ) {
                                    t1dott2 += (s_n(i,j)-backs_n(i,j))*(s_n(i,j)-so_n(i,j));
                            }
                    } //end for i 
                t1dott2=t1dott2/(n1*n2);


                // find kappa directly for t_n    cf Montans Eq. (15)
                    //kappa =-n1/n2*t1dott2+sqrt(R*R/n2/n2-n1*n1/n2/n2*(1-t1dott2*t1dott2));
                                  
 
                    // just another formula, cf Borja   Eq. (29), better than Montans!!
                    // because Montans requires n1,n2!=0, borja only requires n2!=0
                    // which is just unload case for n2=0

                    temp=t1dott2*n1*n2,
                    kappa =sqrt(temp*temp+n2*n2*(R*R-normchi*normchi))-t1dott2*n1*n2;
                    kappa *=1.0/(n2*n2);

                    if((fabs(kappa-kappasave)>1e-6)&&((fabs(kappasave)<infinity))){
                       // std::cerr<<"MCP:992 big error in last computed X[2]" <<std::endl;
                    //        std::cerr<<"MCP:992 kappa-kappaS   ="<<kappa-kappasave   <<std::endl;        
                    //        std::cerr<<"MCP:992 kappa="<<kappa<<" kappaS="<<kappasave   <<std::endl;
                    }
                    
            }
               
    }


   


    /// note: two options, (1) calculate tn (2) not, use previous value
    /// looks (2) is better!!!
    
    kappa=kappasave;
    //plasticflag_n=plasticflagsave;


    //(2) compute alpha_n

    for( i = 0; i < 3; i++ ){
            for( j = 0; j < 3; j++ ) {
                    alpha_n(i,j)= so_n(i,j)+ (backs_n(i,j)-so_n(i,j)) /(1+kappa);
            }
    } //end for i 





    // (3) compute for Psi for t_n

    Hn=h*pow(kappa,m)+Ho; 

    Psi=2.0*shear*(1.0-shear/(shear+Hn/3.0));

    // in general Psi is not equal X[1]. since the latter is secant of
    // step n-> n+1, and Psi is secant of step n only

    
    Psi_split=2.0*shear*(1.0-shear/(shear+((1-beta)*Hn+beta*Ho)/3.0)); // Gang
    //Psi_split=2.0*shear/(1.0+3.0*shear*((1-beta)/Hn+beta/Ho)); // Borja



    // (4) check for loading/unloading

  load=0; temp6.Zero(); temp=0;
  for( i = 0; i < 3; i++ ){
      for( j = 0; j < 3; j++ ) {
        temp6(i,j) = s_n(i,j)-alpha_n(i,j);
        load += temp6(i,j)*de(i,j);
        temp += temp6(i,j)*temp6(i,j);
      }
  } //end for i 

  load *= 1.0/(sqrt(temp));


   if(load < tolforload*normde)  // unloading
    {   
      if(plasticflag_n==1){
              debugInfo(2)=1;
              //std::cerr<<"MCP1091::unload checked!! from plastic, load="<<load<<" normde="<<normde<<std::endl;
              //std::cerr<<"MCP1091::plasticflag_n="<<plasticflag_n<<std::endl;
              //std::cerr<<"MCP1091::fn="<<fn<<" R="<<R<<" load="<<load<<std::endl;
      } else {
              debugInfo(2)=2;
              //std::cerr<<"MCP1095::unload checked!! within B.S.,  load="<<load<<" normde="<<normde<<std::endl;
              //std::cerr<<"MCP1095::plasticflag_n="<<plasticflag_n<<std::endl;
              //std::cerr<<"MCP1095::fn="<<fn<<" R="<<R<<" load="<<load<<std::endl;
      }
            
            kappa = infinity;
            Psi   = 2*shear;
            so = s_n; 
            flagjustunload = 1;   // not used
            unloadflag = 1;  
            X[1] = 2*shear;
            X[2] = infinity;
            //unloadcounter +=1;
            plasticflag=0;     // force it go to plasticflag=0 tangent
            goto LABEL10;
            //goto LABEL6;
    }


  // (5) set plasticflag


 // begin: check for plastic loading if LAST loading is plastic
  if(plasticflag_n==1){    
            strial = s_n + 2.0 * shear * de; 
      chitri = strial;
      chitri-= backs_n;

      // check yield
      normchi = 0;
      for( i = 0; i < 3; i++ ){
              for( j = 0; j < 3; j++ ) {
                    normchi += chitri(i,j)*chitri(i,j);
              }
            } //end for i 
      
      // compute normchi
      if(normchi >= 0){
              normchi = sqrt(normchi);
      } else {
              std::cerr<<"MCP XC::1060::Problem, normchi<0"<<std::endl;
      }

      ftrial=normchi-R;
      if(ftrial > 0){
              plasticflag=1;
              debugInfo(3)=1;
      } else {
              debugInfo(3)=2;
              std::cerr<<"MCP1419::unload from plastic" <<std::endl;
              // update plasticflag
              plasticflag = 0;
              kappa = infinity;
              Psi   = 2*shear;
              so    = s_n;   
              X[1] = 2*shear;
              X[2] = infinity;
              flagjustunload = 1;   // not used
              unloadflag = 1;
              goto LABEL10;
              //goto LABEL6;
      }
  } else {  // last loading is NOT plastic, assign plasticflag=0 or 2
      debugInfo(3)=3;
      chitri  = Psi*de;
      chitri += s_n;
      chitri -= backs_n;
 
      normchi=0;
      for( i = 0; i < 3; i++ ){
              for( j = 0; j < 3; j++ ) {
               normchi += chitri(i,j)*chitri(i,j);
              }
            } //end for i 
      
      // compute normchi
      if(normchi >= 0){
              normchi =sqrt(normchi);
      } else {
              std::cerr<<"MCP XC::1089::Problem, normchi<0 normchi="<< normchi<<std::endl;
      }   

      plasticflag=0;

      ftrial=normchi-R;
      if(ftrial >= 0) {   
                           //Psi_split is calculated in the very beginning
                    chitri  = Psi_split*de;
                    chitri += s_n;
                    chitri -= backs_n;
                    
                    normchi=0;
                    for( i = 0; i < 3; i++ ){
                            for( j = 0; j < 3; j++ ) {  
                                    normchi += chitri(i,j)*chitri(i,j);
                            }
                    } //end for i 

                    // compute normchi
                    if(normchi >= 0){
                            normchi =sqrt(normchi);
                    } else {
                            std::cerr<<"MCP XC::1111::Problem, normchi<0"<<std::endl;
                    }   

                    ftrial=normchi-R;
                    if(ftrial>=0){            
                            debugInfo(3)=4;
                            plasticflag=2;
                    } else {
                            debugInfo(3)=5;  // it is really an awkward situation when it comes here....
                            plasticflag=0;   // diverge if it set to 1
                    }
      } //end if ftrial>0
  }
   
  // end: check for plastic loading




  // (6) if loading within B.S., solve  Psi_n+1, kappa_n+1 (i.e. X[1] and X[2])
 
//LABEL6:
  if(plasticflag==0){
    // initialize

    // converge better
    X[1] = Psi ;         // stiff at time n
    X[2] = kappa;         // kappa at time n


    //X[1] = 2*shear ;        // stiff at time n
    //X[2] = infinity;         // kappa at time n
    
    double g1, g2;
    double aa, bb, cc, dd;        

    Hn  = h* pow(kappa, m) + Ho;
    Hn1 = h* pow(X[2],m) + Ho;
    // define g1, g2
    //g1= 2.0*shear-X[1]-3.0*shear*X[1]*((1-beta)/Hn + beta/Hn1);  // borja
    g1=X[1]-2.0*shear*(1.0-shear/(shear+((1-beta)*Hn+beta*Hn1)/3.0));    // Gang

    temp6.Zero();

    //temp6 = s-backs + X[1]*de + X[2]*(s+X[1]*de-so);  
    for( i = 0; i < 3; i++ ){
            for( j = 0; j < 3; j++ ) {
                    temp6(i,j) = s_n(i,j)-backs_n(i,j) + X[1]*de(i,j) + X[2]*(s_n(i,j)+X[1]*de(i,j)-so(i,j));  
            }
     } //end for i 


    dottemp6=0;
    for( i = 0; i < 3; i++ ){
            for( j = 0; j < 3; j++ ) {
          dottemp6 += temp6(i,j)*temp6(i,j);
            }
     } //end for i 

     if(dottemp6 >=0){
             g2=R-sqrt(dottemp6);
     } else {
             std::cerr<<"MCP1244::problem, dottemp6<0 "<<dottemp6<<std::endl;
             g2=R;
     }

     norm = g1*g1+g2*g2;
          if(norm >= 0){
             norm = sqrt(norm);
     } else { 
             std::cerr<<"MCP::problem, norm<0 "<<norm<<std::endl;
             norm = 0.0; 
     }

    
     icounter = 0;
   // begin iterations to solve Psi_n+1, Kappa_n+1 !!!!         
     while((norm>tolforX)&&(icounter<60)&&(X[2]>0))
     //while((icounter<30))
     { 
      //   compute Jacobian 
      //   aa = dg1/dPsi    bb=dg1/dKappa
      //   cc = dg2/dPsi    dd=dg2/dKappa
      //  
      //       | aa  bb |
      //   J = |        |
      //       | cc  dd |



            //Hn  = h* pow(kappa, m) + Ho;
            Hn1 = h* pow(X[2],m) + Ho;


            //aa = -1.0 - 3.0*shear*((1.0-beta)/Hn + beta/Hn1); // borja
            //bb = 3.0 * shear * X[1] * beta * m * h * pow(X[2],m-1.0) / pow(Hn1,2); // borja

            // reformulate 
            aa = 1.0;                                                 // Gang
            temp=shear + ((1.0-beta)*Hn+beta*Hn1)/3.0;

            bb= -2.0/3.0*beta*shear*shear/(temp*temp)*h*m*pow(X[2],m-1.0);  // Gang
        // we will have problem if X[2]=0, then bb=NaN. Problem: pow(X[2],m-1)


                 if(sqrt(dottemp6) > ZERO) {
                    cc = 0;
                    for( i = 0; i < 3; i++ ){
                            for( j = 0; j < 3; j++ ) {
                               cc += temp6(i,j)*de(i,j);
                            }
                    } 
                    cc *= -(1+X[2])/sqrt(dottemp6);

                    dd = 0;
                    for( i = 0; i < 3; i++ ){
                            for( j = 0; j < 3; j++ ) {
                     dd += temp6(i,j)*(s_n(i,j)+X[1]*de(i,j)-so(i,j));
                            }
                     }  
                dd *= -1.0/sqrt(dottemp6);
            } else {
                    std::cerr<<"MCP:: singularity in Jacobian, dottemp6="<<dottemp6<<std::endl;
                //std::cerr<<"MCP:: icounter="<<icounter<<std::endl;
                    std::cerr<<"MCP:: X[1]="<<X[1]<<" X[2]="<<X[2]<<std::endl;
                    std::cerr<<"MCP:: plasticflag_n="<<plasticflag_n<<" kappa="<<kappa<<" Psi="<<Psi <<std::endl;
                    cc = 0;
                    dd = 0;
            }

            if(fabs(aa*dd-cc*bb)>=ZERO){
                    X[1] += -1.0 /(aa*dd - cc*bb)*( dd*g1 - bb*g2);
                    X[2] += -1.0 /(aa*dd - cc*bb)*(-cc*g1 + aa*g2);
            } else {
                    std::cerr<<"MCP:: Fatal error: infinite Jacobian"  <<std::endl;
                // remarks: arrive here maybe simply X[2] < 0, s.t. pow() gives NaN -1.#IND
                    // so we must exit the loop once X[2]<0 is found
            std::cerr<<"MCP::pow()="<<pow(X[2],m)<<std::endl;
                    std::cerr<<"MCP::X[2]="<<X[2]<<std::endl;  // Gang

                    //exit(1);
            }
 
            if(X[2]<=0) {
                    icounter = 100; // go out of the loop
               //  exit the loop
            }

        // re-evaluate
            //Hn  = h* pow(kappa, m) + Ho;
            Hn1 = h* pow(X[2],m) + Ho;


                 // update g1, g2
        // g1= 2.0*shear-X[1]-3.0*shear*X[1]*((1.0-beta)/Hn + beta/Hn1);   // borja

            g1=X[1]-2.0*shear*(1.0-shear/(shear+((1.0-beta)*Hn+beta*Hn1)/3.0));    // Gang


                 temp6.Zero();
 
            for( i = 0; i < 3; i++ ){
                    for( j = 0; j < 3; j++ ) {
                            temp6(i,j) = s_n(i,j)-backs_n(i,j) + X[1]*de(i,j) + X[2]*(s_n(i,j)+X[1]*de(i,j)-so(i,j));  
                    }
            }


            dottemp6 = 0;
            for( i = 0; i < 3; i++ ){
                    for( j = 0; j < 3; j++ ) {
                            dottemp6 += temp6(i,j)*temp6(i,j);
                    }
             } //end for i 

            if(dottemp6 >=0){
                     g2=R-sqrt(dottemp6);
            } else {
                     // remark: come here wrong because X[2]=0
             std::cerr<<"MCP1353::aa="<<aa<<" bb="<<bb<<" cc="<<cc<<" dd="<<dd<<std::endl;
                     std::cerr<<"MCP1353::X[2]="<<X[2]<<std::endl;
                     std::cerr<<"MCP1353::pow(X[2],m-1)"<<pow(X[2],m-1)<<std::endl;
                     icounter=1353;
            }
                  norm = g1*g1+g2*g2;
            if(norm > 0){
                    norm = sqrt(norm);
            } else { 
                    norm = 0.0; 
            }
 
            icounter +=1;
    
     } // end of while loop
 
     // begin: check kappa converged to a positive value
    debugInfo(4)=1;

    if((norm > tolforX)&&(X[2]!=0)){
     std::cerr<<std::endl <<std::endl <<"MCP::X[1] X[2] is not converged!! norm = "<<norm <<" icounter="<<icounter<<std::endl;
     std::cerr<<"MCP::plasticflag_n= "<<plasticflag_n <<" plasticflag="<< plasticflag<<std::endl;
     std::cerr<<"MCP::X[2] ="<< X[2]<<std::endl <<std::endl;
     std::cerr<<"MCP::debugInfo= "<<debugInfo<<std::endl;
     showdebugInfo=1;
    }

    if(X[2]<=0.0)
     {   debugInfo(4)=2;
             
             //std::cerr<< std::endl;
             //std::cerr<<"MCP1299::WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
             //std::cerr<<"MCP1299::norm = "<<norm <<" icounter="<<icounter<<std::endl;
             //std::cerr<<"MCP1299::kappa= "<<kappa<<" Psi ="<<Psi<<std::endl;
             //std::cerr<<"MCP1299::Negative value for X[2] (kappa_n+1) "<< X[2]<<" set it to 0 "<<std::endl <<std::endl;
                     
 
         X[2]=0.0;
             
             if(unloadflag == 1){
                     debugInfo(4)=3;
                     plasticflag=0;
                     X[1] = 2*shear;
             } else {
                    chitri = Psi_split*de;
                    chitri += s_n;
                    chitri -= backs_n;
                    normchi=0;
                    for( i = 0; i < 3; i++ ){
                            for( j = 0; j < 3; j++ ) {
                                    normchi += chitri(i,j)*chitri(i,j);
                            }
                    } //end for i 

                    // compute normchi
                    if(normchi >= 0){
                            normchi =sqrt(normchi);
                    } else {
                            std::cerr<<"MCP XC::1111::Problem, normchi<0"<<std::endl;
                    }   

                    ftrial=normchi-R;
                    if(ftrial>=0){
                            debugInfo(4)=4;
                            plasticflag=2;
                            X[1]=Psi_split; //Psi_split=2.0*shear*(1-shear/(shear+((1-beta)*Hn+beta*Ho)/3.0)); // Gang
                    } else {
                            debugInfo(4)=X[2];
                            plasticflag=0;  // diverge if it set to 1
                            // question: should plasticflag=0?? or 1?? for this case?
                            X[1]=2.0*shear*(1.0-shear/(shear+((1.0-beta)*Hn+beta*Ho)/3.0));
                            //X[1]=Psi;  // cannot converge if use this
                    }
             }
                     
    } //end if X[2]<0
     
 
 } //end if(plasticflag==0)



 // (7) if strain split, calculate ratio alp and secant modulus Psi_split

 if(plasticflag==2){
               // we are here when the loading is partially inside BS, partially on BS
               // alp is defined as epstotal=eps(inside)*alp+eps(outside)*(1-alp)
               // value of Psi between kappa(t=n) inside BS and kappa=0 on BS         
            debugInfo(4)=6;
    
                         double chichi;
                    double chide;
                    chichi=0; chide=0;
                    for( i = 0; i < 3; i++ ){
                            for( j = 0; j < 3; j++ ) {
                                    chichi += (s_n(i,j)-backs_n(i,j))*(s_n(i,j)-backs_n(i,j));
                                    chide  += (s_n(i,j)-backs_n(i,j))*de(i,j);
                            } // end for j
                    } //end for i 

                    // compute strain splitter
                    alp = (-chide+sqrt(chide*chide+normde*normde*(R*R-chichi)));
                    alp = alp /(Psi_split *normde*normde); 

 
                double insidesqrt;
                    insidesqrt=chide*chide+normde*normde*(R*R-chichi);

                    if((alp > 1.0)||(insidesqrt<0)||(alp < 0.0) ){
                            debugInfo(4)=7;
                            std::cerr<<"MCP:XC::1394::WRONG!!! alpha="<<alp<<" chichi-R*R="<<chichi-R*R<<std::endl;
                            std::cerr<<"MCP::debugInfo= "<<debugInfo<<std::endl;
                            // remark: will have problem if chichi>R*R
                            alp=0;
                            plasticflag=1;
                            showdebugInfo=1;
                    }

 }

    
 //(8) Compute consistent tangent, consistency parameter, and update stresses

////////////////////////////// TANGENT /////////////////////

LABEL10:

   debugInfo(0)=plasticflag_n;
   debugInfo(6)=normde;
   debugInfo(7)=plasticflag;
   debugInfo(8)=X[1];
   debugInfo(9)=X[2];

 if(plasticflag==0)  // bounding surface mapping rule
 {  

          debugInfo(5)=1;
          // update cauchy stresses using incremental strain
              twomu=X[1];

              s  = s_n;
              s += twomu * de;
              p = p_n + bulk  * e;
          // add on bulk part of stress, compute TOTAL stress at t=n+1
              stress = s; 
              for( i = 0; i < 3; i++ )  stress(i,i)= s(i,i) + p;
              
              backs=backs_n;
              // check stress_n+1 is on the yield surface
              
              temp6=s-backs;
              temp=0;
              for( i = 0; i < 3; i++ ){
                      for( j = 0; j < 3; j++ ) {
                              temp += temp6(i,j)*temp6(i,j);
                      } // end for j
              } //end for i 

             
              if(sqrt(temp)-R>=0){
                      if(unloadflag!=1){
                              plasticflag=1;
                      }

                    // Remark:
                    // a big problem for this update is, upon unload from plastic, the 
                    // predicted de is still very high, may cause this >=0. And if you
                    // just simply let plasticflag = 1, it is not unload at all. Continue
                    // to yield at plastic
                // std::cerr<<"MCP1535::plasticflag=0: |S|-R = " << sqrt(temp)-R<<std::endl;
              }


              // a consistent tangent

          // compute tangent again
          for( ii = 0; ii < 6; ii++ ) {
                      for( jj = 0; jj < 6; jj++ )  {
                              index_map( ii, i, j );
                              index_map( jj, k, l );
                              tangent[i][j][k][l]  = bulk * IbunI[i][j][k][l];
                              tangent[i][j][k][l] += twomu * IIdev[i][j][k][l];
                              //minor symmetries 
                              tangent [j][i][k][l] = tangent[i][j][k][l];
                              tangent [i][j][l][k] = tangent[i][j][k][l];
                              tangent [j][i][l][k] = tangent[i][j][k][l];
                      }
              }
   } 

   if(plasticflag > 0) // plasticflag = 1,2   plastic loading   pp.15
   { 
        
        if((X[1]==2.0*shear)&&(unloadflag!=1)&&(zeroloadflag!=1)){
                      std::cerr<<"MCP::warning...WHY X[1]=2G at plasticflag>0 and not unload?"<<std::endl;
                      std::cerr<<"MCP::debugInfo= "<<debugInfo<<std::endl;
                      showdebugInfo=1;
            }


            twomu   = 2.0 * shear;
            if(plasticflag==1)
            {        debugInfo(5)=2;
                    alp = 0.0;
                    strial  = s_n;
                    strial += twomu*de;
                    chitri  = strial;
                    chitri -= backs_n;
                    Psi_split = 0; // just give a number
            }//end plasticfl=1
 
            if(plasticflag==2)
            {
                    debugInfo(5)=3;
                    strial  = s_n;
                    strial += alp * Psi_split * de;
                strial += (1-alp)*twomu*de;
                    chitri  = strial;
                    chitri -= backs_n;
            } // end if plasticflag=2

            normchi = 0.0;
            // double check yield
            for( i = 0; i < 3; i++ ){
                    for( j = 0; j < 3; j++ ) {
                            normchi += chitri(i,j)*chitri(i,j);
                    } // end for j
            } //end for i 
            if(normchi >= 0) {
                    normchi = sqrt(normchi);
            } 
            else {
                    std::cerr<<"MCP1873:: normchi = " << normchi <<std::endl;
                std::cerr<<"MCP:: plastic loading with zero stresses, problem" <<std::endl; 
            }

            ftrial =  normchi - R; 

            if(ftrial < ZERO)
            {   // Remark: come here, means, unload, but goes to wrong place
                    std::cerr<<"MCP1472:: ftrial = " <<ftrial<<std::endl;
                    std::cerr<<"MCP1472:: strange! set ftrial=0, plasticflag="<<plasticflag <<std::endl;
                    std::cerr<<"MCP1372:: plasticflag_n="<<plasticflag_n<<" fn="<<fn<<" normde="<<normde<<" Psi_split="<<Psi_split<<std::endl;
                    std::cerr<<"MCP1372:: load="<<load<<std::endl;
                    showdebugInfo=1;
            }

            // compute consistency parameter
            double gamma;
            gamma =  ftrial /(twomu + 2.0*Ho/3.0);
            // leave stress slightly outside yield surface
        gamma *= (1.0 - 1e-08); // add July 2, 2004 !!!!!! very important add !!!!!
                            
            // update plastic variable for backstresses
            
            backs  = backs_n;
            backs +=  two3 * Ho * gamma * chitri/ normchi;

        s  = s_n + alp * Psi_split * de;
            s += twomu * ((1-alp) * de - gamma * chitri/normchi );  


            // check stress_n+1 is on the yield surface
                  temp6=s-backs;
            temp=0;
            for( i = 0; i < 3; i++ ){
                    for( j = 0; j < 3; j++ ) {
                            temp += temp6(i,j)*temp6(i,j);
                            } // end for j
            } //end for i 

             
            if((sqrt(temp)-R>(1.0e-3)*R)||(sqrt(temp)-R<0)){
           std::cerr<<"MCP1690::plastic: alp   = " << alp <<std::endl;
               std::cerr<<"MCP1690::plastic: |S|-R = " << sqrt(temp)-R<<std::endl;
               std::cerr<<"MCP::debugInfo= "<<debugInfo<<std::endl;
               showdebugInfo=1;
               // Remark: dangerous if <0... 
            }

            //s  = s_n;
            //s += twomu * ( de - gamma * chitri/normchi);  
            p = p_n + bulk  * e;
            
            stress = s;
            for( i = 0; i < 3; i++ )
                    stress(i,i) += p;   // add on pressure part

 
            // remark: this update is very important, since when commit
            // we assign Psi=X[1], kappa=X[2]
            if(Ho==0){
                    X[1] = 0; 
            } else 
            {        
                    X[1] = 2.0 * shear /(1.0 + 3.0*shear/Ho);
            }
        X[2] = 0;   // kappa = 0


        //compute the tangent
            double theta1 = 1.0 - 2.0 *shear *gamma/normchi;
            double theta2 = 1.0/(1.0+Ho/3.0/shear) - (1.0-theta1);

            double NbunN;

            for( ii = 0; ii < 6; ii++ ) {
                    for( jj = 0; jj < 6; jj++ )  {

                            index_map( ii, i, j );
                            index_map( jj, k, l );

                            NbunN  = chitri(i,j)*chitri(k,l)/(normchi*normchi); 

                            // elasto-plastic term  (1-alp) * C_ep
                            // ****** Elasto-plastic consistent modulus, from Simo & Hughes Box 3.2
                            // ---------------------------------------------------------------------
                //        C_ep = K IbunI + 2 shear( theta1* IIdev - theta2 * NbunN)
                            // _____________________________________________________________________

                ///std::cerr<<"MCP tangent! changed July 18"<<std::endl;
                            tangent[i][j][k][l]  = (1-alp)* bulk * IbunI[i][j][k][l];
                            tangent[i][j][k][l] += (1-alp)* 2.0*shear*theta1 * IIdev[i][j][k][l];
                            tangent[i][j][k][l] -= (1-alp)* 2.0*shear*theta2 * NbunN;
                            
                            // bounding surface terms alp * C_bound
                            //****** Consistent modulus from Bounding Surface Plasticity
                            //----------------------------------------------------------------------- 
                            //       C_bound = K IbunI + Psi_split IIdev
                            //_______________________________________________________________________

                            tangent[i][j][k][l] += alp * bulk * IbunI[i][j][k][l];
                tangent[i][j][k][l] += alp * Psi_split * IIdev[i][j][k][l];
              
                            // tangent = alp * C_bound + (1-alp) * C_ep;

                            //minor symmetries 
                            tangent[j][i][k][l] = tangent[i][j][k][l];
                            tangent[i][j][l][k] = tangent[i][j][k][l];
                            tangent[j][i][l][k] = tangent[i][j][k][l];

                    } // end for jj
            } // end for ii
   } // end if(plasticflag)



   if(showdebugInfo==1){
      std::cerr<<"END OF XC::INTEGRATOR::debugInfo= "<<debugInfo<<std::endl;   
   }
      //if((iternum>20)&&(this->getEleTag()==151)){
       //std::cerr<<"MCP::debugInfo= "<<debugInfo<<std::endl;
       //std::cerr<<"MCP::plasticflag= "<<plasticflag<<std::endl;
      //}
}  // end of this subroutine


XC::Vector XC::MultiaxialCyclicPlasticity::MCPparameter(10);




XC::Vector &XC::MultiaxialCyclicPlasticity::getMCPparameter(void)
{
 //std::cerr<<"getMCPparameter"<<std::endl;
 MCPparameter(0) = plasticflag;
 MCPparameter(1) = X[1];
 MCPparameter(2) = X[2]; // kappa
 MCPparameter(3) = alp;
 MCPparameter(4) = stress(0,1);
 MCPparameter(5) = backs(0,1) ;


 double norm = 0;
 double p = one3 * (stress(0,0)+stress(1,1)+stress(2,2));
 XC::Matrix s = stress;
 int i; 
 for(i = 0; i < 3; i++ )  s(i,i) -= p;
 
 for(i = 0; i < 3; i++ ){
   for(int j = 0; j < 3; j++ ) {
     norm   += (s(i,j)-backs(i,j)) * (s(i,j)-backs(i,j));
   } // end for j
 } //end for i 

 MCPparameter(6) = sqrt(norm);
 MCPparameter(7) = load;

 norm=0;
 for(i = 0; i < 3; i++ ){
    for(int j = 0; j < 3; j++ ) {
       norm   += (strain(i,j)) * (strain(i,j));
    } // end for j
  } //end for i 

 MCPparameter(8) = norm;
 // MCPparameter(9) = normde;   get directly from subroutine plastic_integrator
 return MCPparameter;

}

