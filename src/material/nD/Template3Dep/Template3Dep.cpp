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
/*
################################################################################
# COPYRIGHT (C):     :-))                                                      #
# PROJECT:           Object Oriented Finite XC::Element Program                    #
# PURPOSE:           General platform for elaso-plastic constitutive model     #
#                    implementation                                            #
# CLASS:             Template3Dep (the base class for all material point)     #
#                                                                              #
# VERSION:                                                                     #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
# TARGET OS:         DOS || UNIX || . . .                                      #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                               #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
#                                                                              #
#                                                                              #
# DATE:              08-03-2000                                                #
# UPDATE HISTORY:    09-12-2000                                                       #
#                     May 2004, Zhao Cheng splitting the elastic part                 #
#                    Oct. 2004 Zhao Cheng, small addition for u-p-U modeling   #
#                    Mar. 2005 Guanzhou updated constitutive driver to be      #
#                               compatible with global Newton-Raphson iterations#                                                         #
#                              BackwardEuler has been corrected                #
#                                                                              #
# SHORT EXPLANATION: This file contains the class implementation for           #
#                    Template3Dep.                                             #
#                                                                              #
################################################################################
*/

#define ITMAX 30
#define MAX_STEP_COUNT 30
#define NUM_OF_SUB_INCR 30
#define KK 1000.0

#include "utility/matrix/nDarray/Tensor.h"
#include "material/nD/Template3Dep/Template3Dep.h"
#include "material/nD/NDMaterialType.h"

//#include <material/nD/Template3Dep/ManzariDafaliasYieldSurface.h>
//#include <material/nD/Template3Dep/ManzariDafaliasPotentialSurface.h>
#include <material/nD/Template3Dep/YieldSurface.h>
#include <material/nD/Template3Dep/PotentialSurface.h>
#include <material/nD/Template3Dep/EvolutionLaw_S.h>
#include <material/nD/Template3Dep/EvolutionLaw_T.h>
#include <material/nD/Template3Dep/EPState.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>

#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/straint.h>
#include <utility/matrix/nDarray/BJtensor.h>
//** Include the Elastic Material Models here
#include <material/nD/elastic_isotropic/ElasticIsotropic3D.h>
#include <material/nD/ElasticCrossAnisotropic.h>
#include <material/nD/elastic_isotropic/PressureDependentElastic3D.h>

//! @brief Alloc tensorial variables evolution laws.
void XC::Template3Dep::allocELS(EvolutionLaw_S   *ELS1_,
                            EvolutionLaw_S   *ELS2_,
                            EvolutionLaw_S   *ELS3_,
			    EvolutionLaw_S   *ELS4_)
  {
    freeELS();
    // Evolution laws
    if( ELS1_ )
       ELS1 = ELS1_->getCopy();
    else
       ELS1= nullptr;

    if( ELS2_ )
       ELS2 = ELS2_->getCopy();
    else
       ELS2= nullptr;

    if( ELS3_ )
       ELS3 = ELS3_->getCopy();
    else
       ELS3= nullptr;

    if( ELS4_ )
       ELS4 = ELS4_->getCopy();
    else
       ELS4= nullptr;

  }

//! @brief Frees ELS memory.
void XC::Template3Dep::freeELS(void)
  {
    if(ELS1)
      {
        delete ELS1;
	ELS1= nullptr;
      }

    if(ELS2)
      {
        delete ELS2;
	ELS2= nullptr;
      }

    if(ELS3)
      {
        delete ELS3;
	ELS3= nullptr;
      }

    if(ELS4)
      {
        delete ELS4;
	ELS4= nullptr;
      }

  }

//! @brief Alloc tensorial variables evolution laws.
void XC::Template3Dep::allocELT(EvolutionLaw_T   *ELT1_,
                            EvolutionLaw_T   *ELT2_,
                            EvolutionLaw_T   *ELT3_,
                            EvolutionLaw_T   *ELT4_)
  {
    freeELT();
    if( ELT1_ )
       ELT1 = ELT1_->getCopy();
    else
       ELT1= nullptr;

    if( ELT2_ )
       ELT2 = ELT2_->getCopy();
    else
       ELT2= nullptr;

    if( ELT3_ )
       ELT3 = ELT3_->getCopy();
    else
       ELT3= nullptr;

    if( ELT4_ )
       ELT4 = ELT4_->getCopy();
    else
       ELT4= nullptr;
  }

//! @brief Frees memory.
void XC::Template3Dep::freeELT(void)
  {
    if(ELT1)
      {
        delete ELT1;
	ELT1= nullptr;
      }

    if(ELT2)
      {
        delete ELT2;
	ELT2= nullptr;
      }

    if(ELT3)
      {
        delete ELT3;
	ELT3= nullptr;
      }

    if(ELT4)
      {
        delete ELT4;
	ELT4= nullptr;
      }
  }

//! @brief Alloc objects members.
void XC::Template3Dep::alloc( NDMaterial &theElMat,
                            YieldSurface *YS_,
                            PotentialSurface *PS_,
                            EPState          *EPS_,
                            EvolutionLaw_S   *ELS1_,
                            EvolutionLaw_S   *ELS2_,
                            EvolutionLaw_S   *ELS3_,
                            EvolutionLaw_S   *ELS4_,
                            EvolutionLaw_T   *ELT1_,
                            EvolutionLaw_T   *ELT2_,
                            EvolutionLaw_T   *ELT3_,
                            EvolutionLaw_T   *ELT4_)
  {
    free();
    // Get a copy of the material
    theElasticMat = theElMat.getCopy();
    if(theElasticMat == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; failed to get copy of material.\n";
        exit(-1);
      }

    if( YS_ )
       YS = YS_->getCopy();
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; failed to construct the template3Dep.\n";
        exit(-1);
      }

    if( PS_ )
       PS = PS_->getCopy();
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; failed to construct the template3Dep\n";
        exit(-1);
      }

    if( EPS_ )
       EPS = EPS_->getCopy();
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; failed to construct the template3Dep\n";
        exit(-1);
      }
    allocELS(ELS1_,ELS2_,ELS3_,ELS4_);
    allocELT(ELT1_,ELT2_,ELT3_,ELT4_);
  }

//! @brief Frees memory.
void XC::Template3Dep::free(void)
  {
    if(theElasticMat)
      {
        delete theElasticMat;
        theElasticMat= nullptr;
      }
    if(YS)
      {
        delete YS;
	YS= nullptr;
      }

    if(PS)
      {
        delete PS;
	PS= nullptr;
      }

    if(EPS)
      {
        delete EPS;
	EPS= nullptr;
      }
    freeELS();
    freeELT();
  }

//! @brief Constructor
XC::Template3Dep::Template3Dep( int tag                       ,
                            NDMaterial             &theElMat,
                            YieldSurface     *YS_   ,
                            PotentialSurface *PS_   ,
                            EPState          *EPS_  ,
                            EvolutionLaw_S   *ELS1_ ,
                            EvolutionLaw_S   *ELS2_ ,
                            EvolutionLaw_S   *ELS3_ ,
                            EvolutionLaw_S   *ELS4_ ,
                            EvolutionLaw_T   *ELT1_ ,
                            EvolutionLaw_T   *ELT2_ ,
                            EvolutionLaw_T   *ELT3_ ,
                            EvolutionLaw_T   *ELT4_ )
  : XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)
  {
    alloc(theElMat,YS_,PS_,EPS_,ELS1_,ELS2_,ELS3_,ELS4_,ELT1_,ELT2_,ELT3_,ELT4_);

    //Initialize Eep using E-elastic
    BJtensor E  = ElasticStiffnessTensor();
    EPS->setEep( E );

}

//! @brief Constructor 0
XC::Template3Dep::Template3Dep( int tag                     ,
                            NDMaterial             &theElMat,
                            YieldSurface     *YS_ ,
                            PotentialSurface *PS_ ,
                            EPState          *EPS_)
  :XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)
  {
    alloc(theElMat,YS_,PS_,EPS_);
  }


//! @brief Constructor 1
XC::Template3Dep::Template3Dep(   int tag                     ,
                              NDMaterial       &theElMat,
                              YieldSurface     *YS_ ,
                              PotentialSurface *PS_ ,
                              EPState          *EPS_,
                              EvolutionLaw_S   *ELS1_ )
  : XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)

  { alloc(theElMat,YS_,PS_,EPS_,ELS1); }


//! @brief Constructor 2
XC::Template3Dep::Template3Dep(   int tag                     ,
                              NDMaterial       &theElMat,
                              YieldSurface     *YS_ ,
                              PotentialSurface *PS_ ,
                              EPState          *EPS_,
                              EvolutionLaw_T   *ELT1_ )
  :XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)   
  {
    alloc(theElMat,YS_,PS_,EPS_);
    allocELT(ELT1_);
  }

//! @brief Constructor 3
XC::Template3Dep::Template3Dep(   int tag,
                              NDMaterial       &theElMat,
                              YieldSurface     *YS_ ,
                              PotentialSurface *PS_ ,
                              EPState          *EPS_,
                              EvolutionLaw_S   *ELS1_,
                              EvolutionLaw_T   *ELT1_ )
  :XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)   
  {
    alloc(theElMat,YS_,PS_,EPS_);
    allocELS(ELS1_);
    allocELT(ELT1_);
  }

//! @brief Constructor 4
XC::Template3Dep::Template3Dep(   int tag                     ,
                              NDMaterial       &theElMat,
                              YieldSurface     *YS_ ,
                              PotentialSurface *PS_ ,
                              EPState          *EPS_,
                              EvolutionLaw_S   *ELS1_,
                              EvolutionLaw_S   *ELS2_,
                              EvolutionLaw_T   *ELT1_ )
  :XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)
  {
    alloc(theElMat,YS_,PS_,EPS_);
    allocELS(ELS1_,ELS2_);
    allocELT(ELT1_);
  }

//! @brief Constructor 5
XC::Template3Dep::Template3Dep(   int tag                     ,
                              NDMaterial       &theElMat,
                              YieldSurface     *YS_ ,
                              PotentialSurface *PS_ ,
                              EPState          *EPS_,
                              EvolutionLaw_S   *ELS1_,
                              EvolutionLaw_S   *ELS2_,
                              EvolutionLaw_T   *ELT1_,
                              EvolutionLaw_T   *ELT2_)
  :XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)
  {
    alloc(theElMat,YS_,PS_,EPS_);
    allocELS(ELS1_,ELS2_);
    allocELT(ELT1_,ELT2_);
  }

XC::Template3Dep::Template3Dep(int tag)
 : XC::NDMaterial(tag, ND_TAG_Template3Dep),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)
  {
    //YS = new DruckerPragerYieldSurface();
    //PS = new DruckerPragerPotentialSurface();
    //EPS = new EPState();
  }

//================================================================================
// Destructor
//================================================================================

XC::Template3Dep::~Template3Dep(void)
  { free(); }

//! @brief Copy constructor
XC::Template3Dep::Template3Dep(const Template3Dep &rval)
  : XC::NDMaterial(rval),
   theElasticMat(nullptr), YS(nullptr), PS(nullptr), EPS(nullptr),
   ELS1(nullptr), ELS2(nullptr),ELS3(nullptr), ELS4(nullptr),
   ELT1(nullptr), ELT2(nullptr), ELT3(nullptr), ELT4(nullptr)
  {
    alloc(*rval.theElasticMat,rval.YS,rval.PS,rval.EPS,rval.ELS1,rval.ELS2,rval.ELS3,rval.ELS4,rval.ELT1,rval.ELT2,rval.ELT3,rval.ELT4);
  }

//! @brief Assignment operator.
XC::Template3Dep &XC::Template3Dep::operator=(const Template3Dep &other)
  {
    NDMaterial::operator=(other);
    alloc(*other.theElasticMat,other.YS,other.PS,other.EPS,other.ELS1,other.ELS2,other.ELS3,other.ELS4,other.ELT1,other.ELT2,other.ELT3,other.ELT4);
    return *this;
  }



//! @brief  Routine used to generate elastic compliance BJtensor D for this material point
XC::BJtensor XC::Template3Dep::ElasticComplianceTensor(void) const
  {
    BJtensor ret( 4, def_dim_4, 0.0 );

    ret = ElasticStiffnessTensor().inverse();

    return ret;

//ZC05/2004     int eflag = (this->EPS)->getElasticflag();
//ZC05/2004     double Ey = this->EPS->getEo();
//ZC05/2004     double nuP =this->EPS->getnu();
//ZC05/2004
//ZC05/2004     // Zhao, 03-09-04
//ZC05/2004     // eflag = 0, Pressure independent Isotropic
//ZC05/2004     //       = 1, Pressure dendent Isotropic
//ZC05/2004     //       = 2, Pressure independent Cross Anisotropic
//ZC05/2004     //       = 3, Pressure dependent Cross Anisotropic
//ZC05/2004     //   default: 0, Pressure indendent Isotropic
//ZC05/2004
//ZC05/2004     // Pressure independent Isotropic
//ZC05/2004     if( eflag == 0 )
//ZC05/2004       {
//ZC05/2004         if(Ey == 0)
//ZC05/2004           {
//ZC05/2004             std::cerr << std::endl << "Ey = 0! Can't give you D!!" << std::endl;
//ZC05/2004             exit(1);
//ZC05/2004           }
//ZC05/2004
//ZC05/2004       // Kronecker delta XC::BJtensor
//ZC05/2004       BJtensor I2("I", 2, def_dim_2);
//ZC05/2004
//ZC05/2004       BJtensor I_ijkl = I2("ij")*I2("kl");
//ZC05/2004       //I_ijkl.null_indices();
//ZC05/2004       BJtensor I_ikjl = I_ijkl.transpose0110();
//ZC05/2004       BJtensor I_iljk = I_ijkl.transpose0111();
//ZC05/2004       BJtensor I4s = (I_ikjl+I_iljk)*0.5;
//ZC05/2004
//ZC05/2004       // Building compliance XC::BJtensor
//ZC05/2004       ret = (I_ijkl * (-nuP/Ey)) + (I4s * ((1.0+nuP)/Ey));
//ZC05/2004       }
//ZC05/2004
//ZC05/2004     // Pressure dependent Isotropic
//ZC05/2004     else if( eflag == 1)
//ZC05/2004       {
//ZC05/2004       double E = Ey;
//ZC05/2004       //std::cerr << " Eo= " << Ey;
//ZC05/2004         if(Ey == 0)
//ZC05/2004           {
//ZC05/2004             std::cerr << std::endl << "Ey = 0! Can't give you D!!" << std::endl;
//ZC05/2004             exit(1);
//ZC05/2004           }
//ZC05/2004
//ZC05/2004       //Update E
//ZC05/2004       stresstensor stc = (this->EPS)->getStress();
//ZC05/2004       double p = stc.p_hydrostatic();
//ZC05/2004       double po = EPS->getpo();
//ZC05/2004
//ZC05/2004       //std::cerr << " p = " <<  p;
//ZC05/2004
//ZC05/2004       //double po = 100.0; //kPa
//ZC05/2004 //BJ:  this is questionable to be re-examined!!!!!!!!!!!!!!!!!!!!
//ZC05/2004       if(p <= 0.5000*KK) p = 0.500*KK;
//ZC05/2004       E = Ey * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       //std::cerr << " Ec = " << Ey << std::endl;
//ZC05/2004
//ZC05/2004       // Kronecker delta XC::BJtensor
//ZC05/2004       BJtensor I2("I", 2, def_dim_2);
//ZC05/2004
//ZC05/2004       BJtensor I_ijkl = I2("ij")*I2("kl");
//ZC05/2004       //I_ijkl.null_indices();
//ZC05/2004       BJtensor I_ikjl = I_ijkl.transpose0110();
//ZC05/2004       BJtensor I_iljk = I_ijkl.transpose0111();
//ZC05/2004       BJtensor I4s = (I_ikjl+I_iljk)*0.5;
//ZC05/2004
//ZC05/2004       // Building compliance XC::BJtensor
//ZC05/2004       ret = (I_ijkl * (-nuP/E)) + (I4s * ((1.0+nuP)/E));
//ZC05/2004       }
//ZC05/2004
//ZC05/2004    // Pressure independent Anisotropic
//ZC05/2004     else if(eflag == 2)
//ZC05/2004       {
//ZC05/2004       // Form compliance matrix D
//ZC05/2004       double nu = nuP;
//ZC05/2004       double Ev = this->EPS->getEv();
//ZC05/2004       double Ghv = this->EPS->getGhv();
//ZC05/2004       double nuhv = this->EPS->getnuhv();
//ZC05/2004
//ZC05/2004       double A = 1.0/Ey;
//ZC05/2004       double B = 1.0/Ev;
//ZC05/2004       //std::cerr << "A " << A << " B " << B;
//ZC05/2004
//ZC05/2004       Matrix D(6,6);
//ZC05/2004       D(0,0) = D(1,1) = A;
//ZC05/2004       D(2,2) = B;
//ZC05/2004       D(0,1) = D(1,0) = -nu*A;
//ZC05/2004       D(0,2) = D(2,0) = D(1,2) = D(2,1) = -nuhv*B;
//ZC05/2004       //D(3,3) = (1.0+nu)*A;        // Bug found, 03-09-04
//ZC05/2004       //D(4,4) = D(5,5) = 0.5/Ghv;  // ...
//ZC05/2004       D(3,3) = 2.0*(1.0+nu)*A;
//ZC05/2004       D(4,4) = D(5,5) = 1.0/Ghv;
//ZC05/2004       //std::cerr << " C " << D;
//ZC05/2004
//ZC05/2004       //Convert Matric D to 4th order Elastic constants XC::BJtensor ret;
//ZC05/2004       ret.val(1,1,1,1) = D(0,0); ret.val(1,1,2,2) = D(0,1); ret.val(1,1,3,3) = D(0,2); // --> Sigma_xx
//ZC05/2004       ret.val(1,2,1,2) = D(3,3); ret.val(1,2,2,1) = D(3,3); // --> Sigma_xy
//ZC05/2004       ret.val(1,3,1,3) = D(4,4); ret.val(1,3,3,1) = D(4,4); // --> Sigma_xz
//ZC05/2004
//ZC05/2004       ret.val(2,1,1,2) = D(3,3); ret.val(2,1,2,1) = D(3,3); // --> Sigma_yx
//ZC05/2004       ret.val(2,2,1,1) = D(1,0); ret.val(2,2,2,2) = D(1,1); ret.val(2,2,3,3) = D(1,2); // --> Sigma_yy
//ZC05/2004       ret.val(2,3,2,3) = D(5,5); ret.val(2,3,3,2) = D(5,5); // --> Sigma_yz
//ZC05/2004
//ZC05/2004       ret.val(3,1,1,3) = D(4,4); ret.val(3,1,3,1) = D(4,4); // --> Sigma_zx
//ZC05/2004       ret.val(3,2,2,3) = D(5,5); ret.val(3,2,3,2) = D(5,5); // --> Sigma_zy
//ZC05/2004       ret.val(3,3,1,1) = D(2,0); ret.val(3,3,2,2) = D(2,1); ret.val(3,3,3,3) = D(2,2); // --> Sigma_zz
//ZC05/2004
//ZC05/2004       }
//ZC05/2004
//ZC05/2004     // Pressure dependent Anisotropic
//ZC05/2004     else if(eflag == 3)
//ZC05/2004       {
//ZC05/2004       // Form compliance matrix D
//ZC05/2004       double E = Ey;
//ZC05/2004       double nu = nuP;
//ZC05/2004       double Ev = this->EPS->getEv();
//ZC05/2004       double Ghv = this->EPS->getGhv();
//ZC05/2004       double nuhv = this->EPS->getnuhv();
//ZC05/2004       //std::cerr << " Eo= " << Ey;
//ZC05/2004         if(Ey == 0)
//ZC05/2004           {
//ZC05/2004             std::cerr << std::endl << "Ey = 0! Can't give you D!!" << std::endl;
//ZC05/2004             exit(1);
//ZC05/2004           }
//ZC05/2004
//ZC05/2004       //Update E
//ZC05/2004       stresstensor stc = (this->EPS)->getStress();
//ZC05/2004       double p = stc.p_hydrostatic();
//ZC05/2004       double po = EPS->getpo();
//ZC05/2004
//ZC05/2004       //std::cerr << " p = " <<  p;
//ZC05/2004
//ZC05/2004       //double po = 100.0; //kPa
//ZC05/2004       if(p <= 0.5000*KK) p = 0.500*KK;
//ZC05/2004       Ey = Ey * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       Ev = Ev * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       Ghv = Ghv * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       //std::cerr << " Ec = " << Ey << std::endl;
//ZC05/2004
//ZC05/2004       double A = 1.0/E;
//ZC05/2004       double B = 1.0/Ev;
//ZC05/2004       //std::cerr << "A " << A << " B " << B;
//ZC05/2004
//ZC05/2004       Matrix D(6,6);
//ZC05/2004       D(0,0) = D(1,1) = A;
//ZC05/2004       D(2,2) = B;
//ZC05/2004       D(0,1) = D(1,0) = -nu*A;
//ZC05/2004       D(0,2) = D(2,0) = D(1,2) = D(2,1) = -nuhv*B;
//ZC05/2004       D(3,3) = 2.0*(1.0+nu)*A;
//ZC05/2004       D(4,4) = D(5,5) = 1.0/Ghv;
//ZC05/2004       //std::cerr << " C " << D;
//ZC05/2004
//ZC05/2004       //Convert Matric D to 4th order Elastic constants XC::BJtensor ret;
//ZC05/2004       ret.val(1,1,1,1) = D(0,0); ret.val(1,1,2,2) = D(0,1); ret.val(1,1,3,3) = D(0,2); // --> Sigma_xx
//ZC05/2004       ret.val(1,2,1,2) = D(3,3); ret.val(1,2,2,1) = D(3,3); // --> Sigma_xy
//ZC05/2004       ret.val(1,3,1,3) = D(4,4); ret.val(1,3,3,1) = D(4,4); // --> Sigma_xz
//ZC05/2004
//ZC05/2004       ret.val(2,1,1,2) = D(3,3); ret.val(2,1,2,1) = D(3,3); // --> Sigma_yx
//ZC05/2004       ret.val(2,2,1,1) = D(1,0); ret.val(2,2,2,2) = D(1,1); ret.val(2,2,3,3) = D(1,2); // --> Sigma_yy
//ZC05/2004       ret.val(2,3,2,3) = D(5,5); ret.val(2,3,3,2) = D(5,5); // --> Sigma_yz
//ZC05/2004
//ZC05/2004       ret.val(3,1,1,3) = D(4,4); ret.val(3,1,3,1) = D(4,4); // --> Sigma_zx
//ZC05/2004       ret.val(3,2,2,3) = D(5,5); ret.val(3,2,3,2) = D(5,5); // --> Sigma_zy
//ZC05/2004       ret.val(3,3,1,1) = D(2,0); ret.val(3,3,2,2) = D(2,1); ret.val(3,3,3,3) = D(2,2); // --> Sigma_zz
//ZC05/2004
//ZC05/2004       }
//ZC05/2004     else
//ZC05/2004       {
//ZC05/2004       std::cerr << "Template3Dep::ElasticComplianceTensor failed to create elastic compliance XC::BJtensor. Eflag must be 0-3: " <<  eflag << std::endl;
//ZC05/2004       exit(-1);
//ZC05/2004
//ZC05/2004       }
//ZC05/2004
//ZC05/2004    return ret;
}


//================================================================================
// Routine used to generate elastic stiffness XC::BJtensor D for this material point
//================================================================================
XC::BJtensor XC::Template3Dep::ElasticStiffnessTensor(void) const
  {
    BJtensor ret( 4, def_dim_4, 0.0 );

    if(theElasticMat->setTrialStrain( this->EPS->getElasticStrain() ) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; setTrialStrain failed in material with strain.";
      return -1;
    }

    ret = theElasticMat->getTangentTensor();
    return ret;

//ZC05/2004     int eflag = (this->EPS)->getElasticflag();
//ZC05/2004
//ZC05/2004     double Ey = this->EPS->getEo();
//ZC05/2004     double nu =this->EPS->getnu();
//ZC05/2004
//ZC05/2004     // Zhao, 03-09-04
//ZC05/2004     // eflag = 0, Pressure dependent Isotropic
//ZC05/2004     //       = 1, Pressure indendent Isotropic
//ZC05/2004     //       = 2, Pressure dependent Cross Anisotropic
//ZC05/2004     //       = 3, Pressure independent Cross Anisotropic
//ZC05/2004     //   default: 0, Pressure indendent Isotropic
//ZC05/2004     // Pressure dependent Isotropic
//ZC05/2004     if( eflag == 0) {
//ZC05/2004        double E = Ey;
//ZC05/2004        // Kronecker delta XC::BJtensor
//ZC05/2004        BJtensor I2("I", 2, def_dim_2);
//ZC05/2004
//ZC05/2004        BJtensor I_ijkl = I2("ij")*I2("kl");
//ZC05/2004
//ZC05/2004
//ZC05/2004        //I_ijkl.null_indices();
//ZC05/2004        BJtensor I_ikjl = I_ijkl.transpose0110();
//ZC05/2004        BJtensor I_iljk = I_ijkl.transpose0111();
//ZC05/2004        BJtensor I4s = (I_ikjl+I_iljk)*0.5;
//ZC05/2004
//ZC05/2004        // Building elasticity XC::BJtensor
//ZC05/2004        ret = I_ijkl*( E*nu / ( (1.0+nu)*(1.0 - 2.0*nu) ) ) + I4s*( E / (1.0 + nu) );
//ZC05/2004        //ret.null_indices();
//ZC05/2004     }
//ZC05/2004     else if( eflag == 1) {
//ZC05/2004
//ZC05/2004        //Update E
//ZC05/2004        stresstensor stc = (this->EPS)->getStress();
//ZC05/2004        double p = stc.p_hydrostatic();
//ZC05/2004        double po = EPS->getpo();
//ZC05/2004        //std::cerr << " p = " <<  p;
//ZC05/2004
//ZC05/2004        //double po = 100.0; //kPa
//ZC05/2004        if(p <= 0.5000*KK) p = 0.5000*KK;
//ZC05/2004        double E = Ey * pow(p/po/KK, 0.5);//0.5
//ZC05/2004        //std::cerr << " Eo = " << Ey ;
//ZC05/2004        //std::cerr << " Ec = " << E << std::endl;
//ZC05/2004
//ZC05/2004
//ZC05/2004        // Kronecker delta XC::BJtensor
//ZC05/2004        BJtensor I2("I", 2, def_dim_2);
//ZC05/2004
//ZC05/2004        BJtensor I_ijkl = I2("ij")*I2("kl");
//ZC05/2004
//ZC05/2004
//ZC05/2004        //I_ijkl.null_indices();
//ZC05/2004        BJtensor I_ikjl = I_ijkl.transpose0110();
//ZC05/2004        BJtensor I_iljk = I_ijkl.transpose0111();
//ZC05/2004        BJtensor I4s = (I_ikjl+I_iljk)*0.5;
//ZC05/2004
//ZC05/2004        // Building elasticity XC::BJtensor
//ZC05/2004        ret = I_ijkl*( E*nu / ( (1.0+nu)*(1.0 - 2.0*nu) ) ) + I4s*( E / (1.0 + nu) );
//ZC05/2004        //ret.null_indices();
//ZC05/2004     }
//ZC05/2004     else if( eflag == 3) {
//ZC05/2004       double E = Ey;
//ZC05/2004       double Ev = this->EPS->getEv();
//ZC05/2004       double Ghv = this->EPS->getGhv();
//ZC05/2004       double nuhv = this->EPS->getnuhv();
//ZC05/2004       //Update E
//ZC05/2004
//ZC05/2004       stresstensor stc = (this->EPS)->getStress();
//ZC05/2004       double p = stc.p_hydrostatic();
//ZC05/2004       double po = EPS->getpo();
//ZC05/2004
//ZC05/2004       //double po = 100.0; //kPa
//ZC05/2004       if(p <= 0.5000*KK) p = 0.500*KK;
//ZC05/2004       E = Ey * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       Ev = Ev * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       Ghv = Ghv * pow(p/po/KK, 0.5); //0.5
//ZC05/2004       //std::cerr << " Ec = " << Ey << std::endl;
//ZC05/2004       double A = 1.0/E;
//ZC05/2004       double B = 1.0/Ev;
//ZC05/2004       //std::cerr << "A " << A << " B " << B;
//ZC05/2004
//ZC05/2004       Matrix D(6,6);
//ZC05/2004       D(0,0) = D(1,1) = A;
//ZC05/2004       D(2,2) = B;
//ZC05/2004       D(0,1) = D(1,0) = -nu*A;
//ZC05/2004       D(0,2) = D(2,0) = D(1,2) = D(2,1) = -nuhv*B;
//ZC05/2004       D(3,3) = (1.0+nu)*A;
//ZC05/2004       D(4,4) = D(5,5) = 0.5/Ghv;
//ZC05/2004       //std::cerr << " C " << D;
//ZC05/2004
//ZC05/2004       // Do invertion once to get Elastic matrix D
//ZC05/2004       D.Invert( D );
//ZC05/2004
//ZC05/2004       //Convert Matric D to 4th order Elastic constants XC::BJtensor ret;
//ZC05/2004       ret.val(1,1,1,1) = D(0,0); ret.val(1,1,2,2) = D(0,1); ret.val(1,1,3,3) = D(0,2); // --> Sigma_xx
//ZC05/2004       ret.val(1,2,1,2) = D(3,3); ret.val(1,2,2,1) = D(3,3); // --> Sigma_xy
//ZC05/2004       ret.val(1,3,1,3) = D(4,4); ret.val(1,3,3,1) = D(4,4); // --> Sigma_xz
//ZC05/2004
//ZC05/2004       ret.val(2,1,1,2) = D(3,3); ret.val(2,1,2,1) = D(3,3); // --> Sigma_yx
//ZC05/2004       ret.val(2,2,1,1) = D(1,0); ret.val(2,2,2,2) = D(1,1); ret.val(2,2,3,3) = D(1,2); // --> Sigma_yy
//ZC05/2004       ret.val(2,3,2,3) = D(5,5); ret.val(2,3,3,2) = D(5,5); // --> Sigma_yz
//ZC05/2004
//ZC05/2004       ret.val(3,1,1,3) = D(4,4); ret.val(3,1,3,1) = D(4,4); // --> Sigma_zx
//ZC05/2004       ret.val(3,2,2,3) = D(5,5); ret.val(3,2,3,2) = D(5,5); // --> Sigma_zy
//ZC05/2004       ret.val(3,3,1,1) = D(2,0); ret.val(3,3,2,2) = D(2,1); ret.val(3,3,3,3) = D(2,2); // --> Sigma_zz
//ZC05/2004
//ZC05/2004     }
//ZC05/2004     else if( eflag == 2) {
//ZC05/2004       double Ev = this->EPS->getEv();
//ZC05/2004       double Ghv = this->EPS->getGhv();
//ZC05/2004       double nuhv = this->EPS->getnuhv();
//ZC05/2004
//ZC05/2004       double A = 1.0/Ey;
//ZC05/2004       double B = 1.0/Ev;
//ZC05/2004       //std::cerr << "A " << A << " B " << B;
//ZC05/2004
//ZC05/2004       Matrix D(6,6);
//ZC05/2004       D(0,0) = D(1,1) = A;
//ZC05/2004       D(2,2) = B;
//ZC05/2004       D(0,1) = D(1,0) = -nu*A;
//ZC05/2004       D(0,2) = D(2,0) = D(1,2) = D(2,1) = -nuhv*B;
//ZC05/2004       D(3,3) = (1.0+nu)*A;
//ZC05/2004       D(4,4) = D(5,5) = 0.5/Ghv;
//ZC05/2004       //std::cerr << " C " << D;
//ZC05/2004
//ZC05/2004       // Do invertion once to get Elastic matrix D
//ZC05/2004       D.Invert( D );
//ZC05/2004
//ZC05/2004       //Convert Matric D to 4th order Elastic constants XC::BJtensor ret;
//ZC05/2004       ret.val(1,1,1,1) = D(0,0); ret.val(1,1,2,2) = D(0,1); ret.val(1,1,3,3) = D(0,2); // --> Sigma_xx
//ZC05/2004       ret.val(1,2,1,2) = D(3,3); ret.val(1,2,2,1) = D(3,3); // --> Sigma_xy
//ZC05/2004       ret.val(1,3,1,3) = D(4,4); ret.val(1,3,3,1) = D(4,4); // --> Sigma_xz
//ZC05/2004
//ZC05/2004       ret.val(2,1,1,2) = D(3,3); ret.val(2,1,2,1) = D(3,3); // --> Sigma_yx
//ZC05/2004       ret.val(2,2,1,1) = D(1,0); ret.val(2,2,2,2) = D(1,1); ret.val(2,2,3,3) = D(1,2); // --> Sigma_yy
//ZC05/2004       ret.val(2,3,2,3) = D(5,5); ret.val(2,3,3,2) = D(5,5); // --> Sigma_yz
//ZC05/2004
//ZC05/2004       ret.val(3,1,1,3) = D(4,4); ret.val(3,1,3,1) = D(4,4); // --> Sigma_zx
//ZC05/2004       ret.val(3,2,2,3) = D(5,5); ret.val(3,2,3,2) = D(5,5); // --> Sigma_zy
//ZC05/2004       ret.val(3,3,1,1) = D(2,0); ret.val(3,3,2,2) = D(2,1); ret.val(3,3,3,3) = D(2,2); // --> Sigma_zz
//ZC05/2004
//ZC05/2004     }
//ZC05/2004     else
//ZC05/2004       {
//ZC05/2004       std::cerr << "Template3Dep::ElasticStiffnessTensor failed to create elastic compliance XC::BJtensor. Eflag must be 0-3: " <<  eflag << std::endl;
//ZC05/2004       exit(-1);
//ZC05/2004
//ZC05/2004       }
//ZC05/2004    return ret;
}

//================================================================================
int XC::Template3Dep::setTrialStrain(const XC::Vector &v)
{
    // Not yet implemented
    return 0;
}

//================================================================================
int XC::Template3Dep::setTrialStrain(const XC::Vector &v, const XC::Vector &r)
{
    // Not yet implemented
    return 0;
}

//================================================================================
int XC::Template3Dep::setTrialStrainIncr(const XC::Vector &v)
{
    // Not yet implemented
    return 0;
}

//================================================================================
int XC::Template3Dep::setTrialStrainIncr(const XC::Vector &v, const XC::Vector &r)
{
//================================================================================
    // Not yet implemented
    return 0;
}

//! @brief Return the material tangent stiffness.
const XC::Matrix& XC::Template3Dep::getTangent(void) const
  {
    static Matrix M;
    return M;
  }

//! @brief Return the material initial stiffness.
const XC::Matrix& XC::Template3Dep::getInitialTangent(void) const
  { return this->getInitialTangent(); }

//================================================================================
const XC::Vector& XC::Template3Dep::getStress(void) const
  {
    // Not yet implemented
    static Vector V;
    return V;
  }

//================================================================================
const XC::Vector& XC::Template3Dep::getStrain(void) const
  {
    // Not yet implemented
    static Vector V;
    return V;
  }

//================================================================================
// what is the trial strain? Initial strain?
int XC::Template3Dep::setTrialStrain(const Tensor &v)
  {
    //Guanzhou made it compatible with global iterations Mar2005
    Tensor tmp(v - EPS->getStrain());
    this->setTrialStrainIncr(tmp);
    return 0;
  }


//================================================================================
int XC::Template3Dep::setTrialStrain(const Tensor &v, const Tensor &r)
  {
    //Guanzhou made it compatible with global iterations Mar2005
    Tensor tmp(v - EPS->getStrain());
    this->setTrialStrainIncr(tmp);
    return 0;
  }

//================================================================================

int XC::Template3Dep::setTrialStrainIncr(const Tensor &v)
  {

    //std::cerr << "\nBE: " << std::endl;
    //EPState StartEPS( *(this->getEPS()) );
    //stresstensor start_stress = StartEPS.getStress_commit(); //Guanzhou Mar2005
    //std::cerr << "start_stress 0 " << start_stress;

    //EPState tmp_EPS = BackwardEulerEPState(v);
    //if( tmp_EPS.getConverged() ) {
    //     //setTrialEPS( tmp_EPS );
    //     setEPS( tmp_EPS );
    //   //double p = (tmp_EPS.getStress()).p_hydrostatic();
    //   //double ec = (tmp_EPS.getec()) - (tmp_EPS.getLam()) * log( p / (tmp_EPS.getpo()) );
    //   //double  st = (tmp_EPS.getStrain()).Iinvariant1();
    //   //double pl_s = (tmp_EPS.getPlasticStrain()).Iinvariant1();
    //   //double dpl_s = (tmp_EPS.getdPlasticStrain()).Iinvariant1();
    //   //std::cerr << "ec " << ec << " e " << tmp_EPS.gete() << " psi " << (tmp_EPS.gete() - ec) << " strain " << st << " t_pl " << pl_s << " d_pl " << dpl_s << "\n";
    //     return 0;
    //}

    //std::cerr << std::endl;
    //setEPS( StartEPS );
    //int number_of_subincrements = 5;
    //Cascading subdividing in case that some incr_step is too big

    //int loop = 0;
    //while( !tmp_EPS.getConverged()  && (loop < 1) ) {
    //
    //   setEPS( StartEPS );
    //   EPState startEPS( *(this->getEPS()) );
    //   stresstensor start_stress = startEPS.getStress();
    //   std::cerr << " Step Start Stress:" << start_stress << std::endl;
    //
    //   loop += 1;
    //   std::cerr << "\n "<< loop << "th Sub-BE, total subdivision: " << 10*loop*NUM_OF_SUB_INCR << std::endl;
    //   tmp_EPS = BESubIncrementation(v, 10*loop*NUM_OF_SUB_INCR);
    //   if( tmp_EPS.getConverged() ) {
    //       //setTrialEPS( tmp_EPS );
    //       setEPS( tmp_EPS );
    //       return 0;
    //   }
    //   //else {
    //   //    std::cerr << "\n2nd Sub BE: " << 3*NUM_OF_SUB_INCR << std::endl;
    //   //     tmp_EPS = BESubIncrementation(v, 3*NUM_OF_SUB_INCR);
    //   //
    //   //    if( tmp_EPS.getConverged() ) {
    //   //       setEPS( tmp_EPS );
    //   //        return 0;
    //   //     }
    //   //     else
    //   //        return 1;
    //   //}
    //}

    //// for testing MD model only for no BE
    //EPState tmp_EPS = FESubIncrementation(v, NUM_OF_SUB_INCR);
    EPState *thisEPState = this->getEPS();
    EPState tmp_EPS;
    const straintensor &strainTensor= static_cast<const straintensor &>(v);
    if( thisEPState->getIntegratorFlag() == 0 )
      tmp_EPS = ForwardEulerEPState(strainTensor);
    else if( thisEPState->getIntegratorFlag() == 1 )
      tmp_EPS = BackwardEulerEPState(strainTensor);
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; error when getting integrator flag from EPState! \n";
        exit(1);
      }

    //EPState tmp_EPS = BackwardEulerEPState(v);
    setEPS( tmp_EPS );
    //setEPS( StartEPS );
    return 0;
}

//================================================================================
int XC::Template3Dep::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r)
{
    //EPS->setStrain(v + EPS->getStrain() );  //ZC10/26/2004
    this->setTrialStrainIncr(v);              //ZC10/26/2004
    return 0;
}

//================================================================================
const XC::BJtensor &XC::Template3Dep::getTangentTensor(void) const
  {
//    Tensor Eep = EPS->getEep();
//    return Eep;
    return EPS->trialStateVars.Eep;
  }

//================================================================================
const XC::stresstensor &XC::Template3Dep::getStressTensor(void) const
  {
    //std::cerr << *EPS;
    //stresstensor tmp;
    //tmp =  EPS->getStress();
    //std::cerr << "EPS->getStress() " << EPS->getStress() << std::endl;

    //Something funny!!! happened here when returning EPS->getStress()
    // This function will return wrong Stress.
    return EPS->getStress();
  }


//================================================================================
const XC::straintensor &XC::Template3Dep::getStrainTensor(void) const
  { return EPS->getStrain(); }

//================================================================================
const XC::straintensor &XC::Template3Dep::getPlasticStrainTensor(void) const
  { return EPS->getPlasticStrain(); }

//================================================================================
double XC::Template3Dep::getpsi(void)
{
     //this function cannot be moved,
     //leave here for compiling..., Zhao04/22/04
     return 0.05;
}

//================================================================================
int XC::Template3Dep::commitState(void)
{
    int err;
    theElasticMat->commitState();
    err = getEPS()->commitState();
    return err;
}

//================================================================================
int XC::Template3Dep::revertToLastCommit(void)
{
  int err;
  theElasticMat->revertToLastCommit();
  err = EPS->revertToLastCommit();
  return err;
}

//! @brief Revert the material to its initial state.
int XC::Template3Dep::revertToStart(void)
  {
    int err= NDMaterial::revertToStart();
    err+= theElasticMat->revertToStart();
    err+= EPS->revertToStart();
    return err;
  }

//! @brief Virtual constructor.
XC::NDMaterial * XC::Template3Dep::getCopy(void) const
  { return new Template3Dep(*this); }

//================================================================================
XC::NDMaterial *XC::Template3Dep::getCopy(const std::string &code) const
  {
    Template3Dep *tmp= nullptr;
    if((code==strTypeTemplate3Dep))
      {
        tmp= new Template3Dep(*this);
        tmp->getEPS()->setInit();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to get model: " <<  code << std::endl;
    return tmp;
  }

//================================================================================
const std::string &XC::Template3Dep::getType(void) const
  { return strTypeTemplate3Dep; }

//================================================================================
//??What is the Order????????? might be the

int XC::Template3Dep::getOrder(void) const
  { return 6; }

//================================================================================
int XC::Template3Dep::sendSelf(Communicator &comm)
  {
    // Not yet implemented
    return 0;
  }

//================================================================================
int XC::Template3Dep::recvSelf(const Communicator &)
  {
    // Not yet implemented
    return 0;
  }

//================================================================================
void
XC::Template3Dep::Print(std::ostream &s, int flag) const
{
     s << (*this);
}



//private utilities

//================================================================================
// Set new EPState
//================================================================================

void XC::Template3Dep::setEPS( EPState & rval)
  { EPS->operator=(rval); }

//================================================================================
// Get the Elastic XC::Material //ZC05/2004
//================================================================================
XC::NDMaterial* XC::Template3Dep::getElMat() const
{
    return theElasticMat;
}


//================================================================================
// Get the Yield Surface
//================================================================================
XC::YieldSurface * XC::Template3Dep::getYS() const
  { return YS; }


//================================================================================
// Get the Potential Surface
//================================================================================
XC::PotentialSurface * XC::Template3Dep::getPS() const
  { return PS; }

//================================================================================
// Get the XC::EPState
//================================================================================
XC::EPState * XC::Template3Dep::getEPS() const
{
    return EPS;
}


//================================================================================
// Get the 1st Salar evolution law
//================================================================================

XC::EvolutionLaw_S * XC::Template3Dep::getELS1() const
  { return ELS1; }

//================================================================================
// Get the 2ndst Salar evolution law
//================================================================================
XC::EvolutionLaw_S * XC::Template3Dep::getELS2() const
  { return ELS2; }

//================================================================================
// Get the 2ndst Salar evolution law
//================================================================================
XC::EvolutionLaw_S * XC::Template3Dep::getELS3() const
  { return ELS3; }
//================================================================================
// Get the 2ndst Salar evolution law
//================================================================================
XC::EvolutionLaw_S * XC::Template3Dep::getELS4() const
  { return ELS4; }


//================================================================================
// Get the 1st tensorial evolution law
//================================================================================

XC::EvolutionLaw_T * XC::Template3Dep::getELT1() const
  { return ELT1; }

//================================================================================
// Get the 2nd tensorial evolution law
//================================================================================

XC::EvolutionLaw_T * XC::Template3Dep::getELT2() const
  { return ELT2; }
//================================================================================
// Get the 3rd tensorial evolution law
//================================================================================

XC::EvolutionLaw_T * XC::Template3Dep::getELT3() const
  { return ELT3; }
//================================================================================
// Get the 4th tensorial evolution law
//================================================================================

XC::EvolutionLaw_T * XC::Template3Dep::getELT4() const
  { return ELT4; }


//================================================================================
// Predictor XC::EPState by Forward, Backward, MidPoint Methods...
//================================================================================

XC::EPState XC::Template3Dep::PredictorEPState(straintensor & strain_increment)
  {

    EPState Predictor = ForwardEulerEPState( strain_increment);
    //EPState Predictor = SemiBackwardEulerEPState( strain_increment, material_point);
    return Predictor;

  }

//================================================================================
// New XC::EPState using Forward Euler Algorithm
//================================================================================
XC::EPState XC::Template3Dep::ForwardEulerEPState( const straintensor &strain_increment)
  {
    // Volumetric strain
    //double st_vol = strain_increment.p_hydrostatic();
    double st_vol = strain_increment.Iinvariant1(); //- = compressive
    //std::cerr << st_vol << " st_vol1 " << st_vol1 << "\n";

    //EPState forwardEPS( *(material_point->getEPS()) );
    EPState forwardEPS( *(this->getEPS()) );
    //std::cerr <<"start eps: " <<   forwardEPS;
    //std::cerr << "\nForwardEulerEPState  strain_increment " << strain_increment << std::endl;

    // Building elasticity XC::BJtensor
    BJtensor E    = ElasticStiffnessTensor();
    //BJtensor Eep  = ElasticStiffnessTensor();
    BJtensor Eep  = E;
    BJtensor D    = ElasticComplianceTensor();
    E.null_indices();
    D.null_indices();

    //Checking E and D
    //BJtensor ed = E("ijpq") * D("pqkl");
    //double edd = ed.trace(); // = 3.

    straintensor strain_incr = strain_increment;
    strain_incr.null_indices();
    stresstensor stress_increment(E("ijpq") * strain_incr("pq"));
    stress_increment.null_indices();
    //std::cerr << " stress_increment: " << stress_increment << std::endl;

    EPState startEPS( *(getEPS()) );
    stresstensor start_stress = startEPS.getStress();
    start_stress.null_indices();
    //std::cerr << "===== start_EPS =====: " << startEPS;

    double f_start = 0.0;
    double f_pred  = 0.0;

    EPState IntersectionEPS( startEPS );

    EPState ElasticPredictorEPS( startEPS );
    stresstensor elastic_predictor_stress = start_stress + stress_increment;
    ElasticPredictorEPS.setStress( elastic_predictor_stress );
    //std::cerr << " Elastic_predictor_stress: " << elastic_predictor_stress << std::endl;

    f_start = this->getYS()->f( &startEPS );
    //::printf("\n##############  f_start = %.10e  ",f_start);
    //std::cerr << "\nFE f_start = " << f_start;

    f_pred =  this->getYS()->f( &ElasticPredictorEPS );
    //::printf("##############  f_pred = %.10e\n\n",f_pred);
    //std::cerr << "  FE f_pred = " << f_pred << std::endl;

    stresstensor intersection_stress = start_stress; // added 20april2000 for forward euler
    stresstensor elpl_start_stress = start_stress;
    stresstensor true_stress_increment = stress_increment;
    straintensor El_strain_increment;

    if( (f_start <= 0) && ((f_pred <= 0) || (f_start > f_pred)) )
      {
        //Updating elastic strain increment
        straintensor estrain = ElasticPredictorEPS.getElasticStrain();
        straintensor tstrain = ElasticPredictorEPS.getStrain();
        estrain = estrain + strain_incr;
        tstrain = tstrain + strain_incr;
        ElasticPredictorEPS.setElasticStrain( estrain );
        ElasticPredictorEPS.setStrain( tstrain );
        ElasticPredictorEPS.setdElasticStrain( strain_incr );

        //Evolve parameters like void ratio(e) according to elastic strain and elastic stress--for MD model specifically
        //double Delta_lambda = 0.0;
        //material_point.EL->UpdateVar( &ElasticPredictorEPS, 1);
        // Update E_Young and e according to current stress state before evaluate ElasticStiffnessTensor
        if( getELT1() ) {
                      //getELT1()->updateEeDm(&ElasticPredictorEPS, st_vol, 0.0);
                      getELT1()->updateEeDm(&ElasticPredictorEPS, -st_vol, 0.0);
              }

        //std::cerr <<" strain_increment.Iinvariant1() " << strain_increment.Iinvariant1() << std::endl;
        ElasticPredictorEPS.setEep(E);
        return ElasticPredictorEPS;
      }

    if( f_start <= 0 && f_pred > 0 )
      {
        intersection_stress =
           yield_surface_cross( start_stress, elastic_predictor_stress);
        //std::cerr  << "    start_stress: " <<  start_stress << std::endl;
        //std::cerr  << "    Intersection_stress: " <<  intersection_stress << std::endl;

        IntersectionEPS.setStress( intersection_stress );
        //intersection_stress.reportshort("Intersection stress \n");

        elpl_start_stress = intersection_stress;
        //elpl_start_stress.reportshortpqtheta("elpl start stress AFTER \n");

        true_stress_increment = elastic_predictor_stress - elpl_start_stress;
        //true_stress_increment.null_indices();

      stresstensor EstressIncr = intersection_stress- start_stress;

        //forwardEPS.setStress( elpl_start_stress );
  //Should only count on that elastic portion, not st_vol...
        if( getELT1() ) {
            El_strain_increment = D("ijpq") * EstressIncr("pq");
       double st_vol_El_incr = El_strain_increment.Iinvariant1();

      //std::cerr << " FE crossing update... ";
      getELT1()->updateEeDm(&IntersectionEPS, -st_vol_El_incr, 0.0);
      //std::cerr << " FE crossing update... ";
      getELT1()->updateEeDm(&forwardEPS, -st_vol_El_incr, 0.0);
  }

      }


    //forwardEPS.setStress( elpl_start_stress );

    //std::cerr <<"elpl start eps: " <<   forwardEPS;
    //double f_cross =  this->getYS()->f( &forwardEPS );
    //std::cerr << " #######  f_cross = " << f_cross << "\n";

    //set the initial value of D once the current stress hits the y.s. for Manzari-Dafalias Model
    //if( f_start <= 0 && f_pred > 0 )
    //    material_point.EL->setInitD(&forwardEPS);
    //std::cerr << " inside ConstitutiveDriver after setInitD " << forwardEPS;


    //  pulling out some XC::BJtensor and double definitions
    //BJtensor dFods( 2, def_dim_2, 0.0);
    //BJtensor dQods( 2, def_dim_2, 0.0);
    stresstensor dFods;
    stresstensor dQods;
    //  stresstensor s;  // deviator
    BJtensor H( 2, def_dim_2, 0.0);
    BJtensor temp1( 2, def_dim_2, 0.0);
    BJtensor temp2( 2, def_dim_2, 0.0);
    double lower = 0.0;
    BJtensor temp3( 2, def_dim_2, 0.0);

    double Delta_lambda = 0.0;
    double h_s[4]       = {0.0, 0.0, 0.0, 0.0};
    double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
    stresstensor h_t[4];
    stresstensor xi_t[4];
    double hardMod_     = 0.0;

    //double Dq_ast   = 0.0;
    //double q_ast_entry = 0.0;
    //double q_ast = 0.0;

    stresstensor plastic_stress;
    straintensor plastic_strain;
    stresstensor elastic_plastic_stress;
    // ::printf("\n...... felpred = %lf\n",felpred);

    if( f_pred >= 0 ) {


        //dFods = getYS()->dFods( &forwardEPS );
        //dQods = getPS()->dQods( &forwardEPS );
        dFods = getYS()->dFods( &IntersectionEPS );
        dQods = getPS()->dQods( &IntersectionEPS );

        //std::cerr << "dF/ds" << dFods << std::endl;
        //std::cerr << "dQ/ds" << dQods << std::endl;

        // XC::Tensor H_kl  ( eq. 5.209 ) W.F. Chen
        H = E("ijkl")*dQods("kl");       //E_ijkl * R_kl
        H.null_indices();
        temp1 = dFods("ij") * E("ijkl"); // L_ij * E_ijkl
        temp1.null_indices();
        temp2 = temp1("ij")*dQods("ij"); // L_ij * E_ijkl * R_kl
        temp2.null_indices();
        lower = temp2.trace();

        // Evaluating the hardening modulus: sum of  (df/dq*) * qbar

  hardMod_ = 0.0;
  //Of 1st scalar internal vars
  if( getELS1() ) {
     h_s[0]  = getELS1()->h_s(&IntersectionEPS, getPS());
           xi_s[0] = getYS()->xi_s1( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[0] * xi_s[0];
  }

  //Of 2nd scalar internal vars
  if( getELS2() ) {
     h_s[1]  = getELS2()->h_s( &IntersectionEPS, getPS());
           xi_s[1] = getYS()->xi_s2( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[1] * xi_s[1];
  }

  //Of 3rd scalar internal vars
  if( getELS3() ) {
     h_s[2]  = getELS3()->h_s( &IntersectionEPS, getPS());
           xi_s[2] = getYS()->xi_s3( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[2] * xi_s[2];
  }

  //Of 4th scalar internal vars
  if( getELS4() ) {
     h_s[3]  = getELS4()->h_s( &IntersectionEPS, getPS());
           xi_s[3] = getYS()->xi_s4( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[3] * xi_s[3];
  }

  //Of tensorial internal var
  // 1st tensorial var
  if( getELT1() ) {
     h_t[0]  = getELT1()->h_t(&IntersectionEPS, getPS());
     xi_t[0] = getYS()->xi_t1( &IntersectionEPS );
           BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 2nd tensorial var
  if( getELT2() ) {
     h_t[1]  = getELT2()->h_t( &IntersectionEPS, getPS());
       xi_t[1] = getYS()->xi_t2( &IntersectionEPS );
           BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 3rd tensorial var
  if( getELT3() ) {
     h_t[2]  = getELT3()->h_t( &IntersectionEPS, getPS());
     xi_t[2] = getYS()->xi_t3( &IntersectionEPS );
           BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 4th tensorial var
  if( getELT4() ) {
     h_t[3]  = getELT4()->h_t(&IntersectionEPS, getPS());
     xi_t[3] = getYS()->xi_t4( &IntersectionEPS );
           BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // Subtract accumulated hardMod_ from lower
        lower = lower - hardMod_;
        //std::cerr << "FE : lower " << lower << " hardMod_ " << hardMod_ << std::endl;
        //Calculating Kp according to Kp = - (df/dq*) * qbar
        //double Kp = material_point.EL->getKp(&forwardEPS, norm_dQods);
        //Kp = 0.0;
        //std::cerr << std::endl << ">>>>>>>>>   Lower = " << lower << std::endl;
        //lower = lower + Kp;
        //std::cerr << std::endl << ">>>>>>>>>    Kp = " << Kp << std::endl;

        //std::cerr << " stress_increment "<< stress_increment << std::endl;
        //std::cerr << " true_stress_increment "<< true_stress_increment << std::endl;

        temp3 = dFods("ij") * true_stress_increment("ij"); // L_ij * E_ijkl * d e_kl (true ep strain increment)
        temp3.null_indices();
        //std::cerr << " temp3.trace() -- true_stress_incr " << temp3.trace() << std::endl;
  //GZ  temp3 = temp1("ij")*strain_incr("ij");
  //GZ  temp3.null_indices();
        //std::cerr << " temp3.trace() " << temp3.trace() << std::endl;
        Delta_lambda = (temp3.trace())/lower;
        //std::cerr << "FE: Delta_lambda " <<  Delta_lambda << std::endl;
        if(Delta_lambda<0.0) Delta_lambda=0.0;

        plastic_stress = H("kl") * Delta_lambda;
        plastic_strain = dQods("kl") * Delta_lambda; // plastic strain increment
        plastic_stress.null_indices();
        plastic_strain.null_indices();
        //std::cerr << " Delta_lambda " << Delta_lambda << "plastic_stress =   " << plastic_stress << std::endl;
        //std::cerr << "plastic_stress =   " << plastic_stress << std::endl;
        //std::cerr << "plastic_strain =   " << plastic_strain << std::endl;
        //std::cerr << "plastic_strain I1= " << plastic_strain.Iinvariant1() << std::endl;
        //std::cerr << "plastic_strain vol " << Delta_lambda * ( forwardEPS.getScalarVar( 2 ) )<< std::endl; ;
        //std::cerr << "  q=" << Delta_lambda * dQods.q_deviatoric()<< std::endl;
        //plastic_stress.reportshort("plastic stress (with delta_lambda)\n");

        elastic_plastic_stress = elastic_predictor_stress - plastic_stress;
        //elastic_plastic_stress.reportshortpqtheta("FE elastic plastic stress \n");

        //calculating elatic strain increment
        //stresstensor dstress_el = elastic_plastic_stress - start_stress;
        //straintensor elastic_strain = D("ijpq") * dstress_el("pq");
        straintensor elastic_strain = strain_incr - plastic_strain;  // elastic strain increment
        //std::cerr << "elastic_strain I1=" << elastic_strain.Iinvariant1() << std::endl;
        //std::cerr << "elastic_strain " << elastic_strain << std::endl;
        //std::cerr << "strain increment I1=" << strain_increment.Iinvariant1() << std::endl;
        //std::cerr << "strain increment    " << strain_increment << std::endl;

        straintensor estrain = forwardEPS.getElasticStrain(); //get old elastic strain
        straintensor pstrain = forwardEPS.getPlasticStrain(); //get old plastic strain
        //straintensor pstrain = forwardEPS.getStrain_commit() - estrain; //get old plastic strain

        straintensor tstrain = forwardEPS.getStrain();        //get old total strain
        pstrain = pstrain + plastic_strain;
        estrain = estrain + elastic_strain;
        tstrain = tstrain + elastic_strain + plastic_strain;

        //Setting de_p, de_e, total plastic, elastic strain, and  total strain
        forwardEPS.setdPlasticStrain( plastic_strain );
        forwardEPS.setdElasticStrain( elastic_strain );
        forwardEPS.setPlasticStrain( pstrain );
        forwardEPS.setElasticStrain( estrain );
        forwardEPS.setStrain( tstrain );

        //================================================================
       //Generating Eep using  dQods at the intersection point
        dFods = getYS()->dFods( &IntersectionEPS );
        dQods = getPS()->dQods( &IntersectionEPS );

  BJtensor upperE1 = E("pqkl")*dQods("kl");
        upperE1.null_indices();
  BJtensor upperE2 = dFods("ij")*E("ijmn");
        upperE2.null_indices();

  //BJtensor upperE = upperE1("pq") * upperE1("mn");  // Bug found, Zhao Cheng Jan13, 2004
  BJtensor upperE = upperE1("pq") * upperE2("mn");
        upperE.null_indices();

        /*//temp2 = upperE2("ij")*dQods("ij"); // L_ij * E_ijkl * R_kl
        temp2.null_indices();
        lower = temp2.trace();


  // Evaluating the hardening modulus: sum of  (df/dq*) * qbar

  hardMod_ = 0.0;
  //Of 1st scalar internal vars
  if( getELS1() ) {
     h_s[0]  = getELS1()->h_s( &IntersectionEPS, getPS());
           xi_s[0] = getYS()->xi_s1( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[0] * xi_s[0];
  }

  //Of 2nd scalar internal vars
  if( getELS2() ) {
     h_s[1]  = getELS2()->h_s( &IntersectionEPS, getPS());
           xi_s[1] = getYS()->xi_s2( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[1] * xi_s[1];
  }

  //Of 3rd scalar internal vars
  if( getELS3() ) {
     h_s[2]  = getELS3()->h_s( &IntersectionEPS, getPS());
           xi_s[2] = getYS()->xi_s3( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[2] * xi_s[2];
  }

  //Of 4th scalar internal vars
  if( getELS4() ) {
     h_s[3]  = getELS4()->h_s( &IntersectionEPS, getPS());
           xi_s[3] = getYS()->xi_s4( &IntersectionEPS );
        hardMod_ = hardMod_ + h_s[3] * xi_s[3];
  }

  //Of tensorial internal var
  // 1st tensorial var
  if( getELT1() ) {
     h_t[0]  = getELT1()->h_t(&IntersectionEPS, getPS());
     xi_t[0] = getYS()->xi_t1( &IntersectionEPS );
           BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 2nd tensorial var
  if( getELT2() ) {
     h_t[1]  = getELT2()->h_t( &IntersectionEPS, getPS());
     xi_t[1] = getYS()->xi_t2( &IntersectionEPS );
           BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 3rd tensorial var
  if( getELT3() ) {
     h_t[2]  = getELT3()->h_t(&IntersectionEPS, getPS());
     xi_t[2] = getYS()->xi_t3( &IntersectionEPS );
           BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 4th tensorial var
  if( getELT4() ) {
     h_t[3]  = getELT4()->h_t( &IntersectionEPS, getPS());
     xi_t[3] = getYS()->xi_t4( &IntersectionEPS );
           BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // Subtract accumulated hardMod_ from lower

  lower = lower - hardMod_;
  */

        BJtensor Ep = upperE*(1./lower);

  // elastoplastic constitutive XC::BJtensor
  double h_L = 0.0; // Bug fixed Joey 07-21-02 added h(L) function
  if( Delta_lambda > 0 ) h_L = 1.0;
  //std::cerr << " h_L = " << h_L << "\n";
        //Eep =  Eep - Ep*h_L;  // Bug found, Zhao Cheng Jan13, 2004
  Eep =  E - Ep*h_L;

       //std::cerr <<" after calculation---Eep.rank()= " << Eep.rank() <<std::endl;
  //Eep.printshort(" IN template ");

        //--// before the surface is been updated !
        //--//        f_Final = Criterion.f(elastic_plastic_stress);
        //--
        //--        q_ast_entry = Criterion.kappa_get(elastic_plastic_stress);
        //--
        //--//        h_  = h(elastic_plastic_stress);
        //--        Dq_ast = Delta_lambda * h_ * just_this_PP;
        //--//        if(Dq_ast < 0.0 ) Dq_ast = 0.0;
        //--//        Dq_ast = fabs(Delta_lambda * h_ * just_this_PP); // because of softening response...
        //--//::printf(" h_=%.6e  q_ast_entry=%.6e  Dq_ast=%.6e   Delta_lambda=%.6e\n", h_, q_ast_entry, Dq_ast, Delta_lambda);
        //--
        //--        current_lambda_set(Delta_lambda);
        //--
        //--        q_ast = q_ast_entry + Dq_ast;
        //--        Criterion.kappa_set( elastic_plastic_stress, q_ast);
        //--//::fprintf(stdout," Criterion.kappa_get(elastic_plastic_stress)=%.8e\n",Criterion.kappa_get(elastic_plastic_stress));
        //--//::fprintf(stderr," Criterion.kappa_get(elastic_plastic_stress)=%.8e\n",Criterion.kappa_get(elastic_plastic_stress));
        //--
        //--
        //--//::fprintf(stdout," ######## predictor --> q_ast_entry=%.8e Dq_ast=%.8e q_ast=%.8e\n",q_ast_entry, Dq_ast, q_ast);
        //--//::fprintf(stderr," ######## predictor --> q_ast_entry=%.8e Dq_ast=%.8e q_ast=%.8e\n",q_ast_entry, Dq_ast, q_ast);
        //--
        //--//::fprintf(stdout,"ForwardEulerStress IN Criterion.kappa_get(start_stress)=%.8e\n",Criterion.kappa_get(start_stress));
        //--//::fprintf(stderr,"ForwardEulerStress IN Criterion.kappa_get(start_stress)=%.8e\n",Criterion.kappa_get(start_stress));
        //--

        //new XC::EPState in terms of stress
        forwardEPS.setStress(elastic_plastic_stress);
        //std::cerr <<"inside constitutive driver: forwardEPS "<< forwardEPS;

        forwardEPS.setEep(Eep);
        //forwardEPS.getEep().printshort(" after set");

        //Before update all the internal vars
        double f_forward =  getYS()->f( &forwardEPS );
        //std::cerr << "\n************  Before f_forward = " <<  f_forward << "\n";

        //Evolve the surfaces and hardening vars
  int NS = forwardEPS.getNScalarVar();
  int NT = forwardEPS.getNTensorVar();

  double dS= 0.0;
  double S = 0.0;

  int ii;
  for(ii = 1; ii <= NS; ii++) {
              dS = Delta_lambda * h_s[ii-1] ;       // Increment to the scalar internal var
              S  = forwardEPS.getScalarVar(ii); //Guanzhou Mar2005    // Get the old value of the scalar internal var
              forwardEPS.setScalarVar(ii, S + dS ); // Update internal scalar var
  }

  stresstensor dT;
  stresstensor T;
  stresstensor new_T;

  for(ii = 1; ii <= NT; ii++) {
        dT = h_t[ii-1]*Delta_lambda  ;       // Increment to the XC::BJtensor internal var
              T  = forwardEPS.getTensorVar(ii); //Guanzhou Mar2005    // Get the old value of the XC::BJtensor internal var
              new_T = T + dT;
              forwardEPS.setTensorVar(ii, new_T );
        }

        // Update E_Young and e according to current stress state
  //straintensor pl_strain_increment;
    if( getELT1() )
      {
        //pl_strain_increment = strain_increment - El_strain_increment;
        const double st_vol_pl = plastic_strain.Iinvariant1();
        //int err = getELT1()->updateEeDm(&forwardEPS, st_vol_pl, Delta_lambda);
        //std::cerr << "pl_vol= " << st_vol_pl << "|update before FE \n";
        //std::cerr << " FE Pl update...";
        // D > 0 compressive -> Iinv > 0  -> de < 0 correct!
        int err= 0;
        err= getELT1()->updateEeDm(&forwardEPS, st_vol_pl, Delta_lambda);
      }

        //BJtensor tempx  = plastic_strain("ij") * plastic_strain("ij");
        //double tempxd = tempx.trace();
        //double e_eq  = pow( 2.0 * tempxd / 3.0, 0.5 );
        ////std::cerr << "e_eq = " << e_eq << std::endl;
        //
        //double dalfa1 =  e_eq * 10;
        //double alfa1  = forwardEPS.getScalarVar(1);



      //std::cerr << "UpdateAllVars " << forwardEPS<< std::endl;

        //After update all the internal vars
        f_forward =  getYS()->f( &forwardEPS );
        //std::cerr << "\n************  After f_forward = " <<  f_forward << "\n";


    }

    //::fprintf(stderr,"ForwardEulerStress EXIT Criterion.kappa_get(start_stress)=%.8e\n",Criterion.kappa_get(start_stress));
    //forwardEPS.setConverged(true);

    //double p = (forwardEPS.getStress()).p_hydrostatic();
    //double ec = (forwardEPS.getec()) - (forwardEPS.getLam()) * log( p / (forwardEPS.getpo()) );
    //double st = (forwardEPS.getStrain()).Iinvariant1();
    //double pl_s = (forwardEPS.getPlasticStrain()).Iinvariant1();
    //double dpl_s = (forwardEPS.getdPlasticStrain()).Iinvariant1();
    //std::cerr << "P FE p=" << p << " ec " << ec << " e " << forwardEPS.gete() << " psi " << (forwardEPS.gete() - ec) << " strain " << st << " t_pl " << pl_s << " d_pl " << dpl_s << "\n";
    forwardEPS.Delta_lambda = Delta_lambda;

    return forwardEPS;
}

//================================================================================
// Starting XC::EPState using Semi Backward Euler Starting Point
//================================================================================
XC::EPState XC::Template3Dep::SemiBackwardEulerEPState( const straintensor &strain_increment)
  {
    stresstensor start_stress;
    EPState SemibackwardEPS( *(this->getEPS()) );
    start_stress = SemibackwardEPS.getStress();

    // building elasticity XC::BJtensor
    //BJtensor E = Criterion.StiffnessTensorE(Ey,nu);
    BJtensor E  = ElasticStiffnessTensor();
    // building compliance XC::BJtensor
    //  BJtensor D = Criterion.ComplianceTensorD(Ey,nu);

    //  pulling out some XC::BJtensor and double definitions
    BJtensor dFods(2, def_dim_2, 0.0);
    BJtensor dQods(2, def_dim_2, 0.0);
    BJtensor temp2(2, def_dim_2, 0.0);
    double lower = 0.0;
    double Delta_lambda = 0.0;

    EPState E_Pred_EPS( *(this->getEPS()) );

    straintensor strain_incr = strain_increment;
    stresstensor stress_increment(E("ijkl")*strain_incr("kl"));
    stress_increment.null_indices();
    // stress_increment.reportshort("stress Increment\n");


    stresstensor plastic_stress;
    stresstensor elastic_predictor_stress;
    stresstensor elastic_plastic_stress;
    //..  double Felplpredictor = 0.0;

    double h_s[4]       = {0.0, 0.0, 0.0, 0.0};
    double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
    stresstensor h_t[4];
    stresstensor xi_t[4];
    double hardMod_ = 0.0;

    double S        = 0.0;
    double dS       = 0.0;
    stresstensor T;
    stresstensor dT;
    //double Dq_ast   = 0.0;
    //double q_ast_entry = 0.0;
    //double q_ast = 0.0;

    elastic_predictor_stress = start_stress + stress_increment;
    //..  elastic_predictor_stress.reportshort("ELASTIC PREDICTOR stress\n");
    E_Pred_EPS.setStress( elastic_predictor_stress );

    //  double f_start = Criterion.f(start_stress);
    //  ::printf("SEMI BE##############  f_start = %.10e\n",f_start);
    double f_pred =  getYS()->f( &E_Pred_EPS );
    //::printf("SEMI BE##############  f_pred = %.10e\n",f_pred);

    // second of alternative predictors as seen in MAC page 170-171
    if( f_pred >= 0.0 )
    {
        //el_or_pl_range(1); // set to 1 ( plastic range )
        // PREDICTOR FASE
        //..     ::printf("\n\npredictor part  step_counter = %d\n\n", step_counter);

        dFods = getYS()->dFods( &E_Pred_EPS );
        dQods = getPS()->dQods( &E_Pred_EPS );
        //.... dFods.print("a","dF/ds  ");
        //.... dQods.print("a","dQ/ds  ");

        temp2 = (dFods("ij")*E("ijkl"))*dQods("kl");
        temp2.null_indices();
        lower = temp2.trace();

        // Evaluating the hardening modulus: sum of  (df/dq*) * qbar

   //Of scalar internal var
  hardMod_ = 0.0;

  //Of 1st scalar internal vars
  if( getELS1() ) {
     h_s[0]  = getELS1()->h_s( &E_Pred_EPS, getPS());
           xi_s[0] = getYS()->xi_s1( &E_Pred_EPS );
        hardMod_ = hardMod_ + h_s[0] * xi_s[0];
  }

  //Of 2nd scalar internal vars
  if( getELS2() ) {
     h_s[1]  = getELS2()->h_s( &E_Pred_EPS, getPS());
           xi_s[1] = getYS()->xi_s2( &E_Pred_EPS );
        hardMod_ = hardMod_ + h_s[1] * xi_s[1];
  }

  //Of 3rd scalar internal vars
  if( getELS3() ) {
     h_s[2]  = getELS3()->h_s(&E_Pred_EPS, getPS());
           xi_s[2] = getYS()->xi_s3( &E_Pred_EPS );
        hardMod_ = hardMod_ + h_s[2] * xi_s[2];
  }

  //Of 4th scalar internal vars
  if( getELS4() ) {
     h_s[3]  = getELS4()->h_s( &E_Pred_EPS, getPS());
           xi_s[3] = getYS()->xi_s4( &E_Pred_EPS );
        hardMod_ = hardMod_ + h_s[3] * xi_s[3];
  }

  //Of tensorial internal var
  // 1st tensorial var
  if( getELT1() ) {
     h_t[0]  = getELT1()->h_t(&E_Pred_EPS, getPS());
     xi_t[0] = getYS()->xi_t1( &E_Pred_EPS );
           BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 2nd tensorial var
  if( getELT2() ) {
     h_t[1]  = getELT2()->h_t(&E_Pred_EPS, getPS());
     xi_t[1] = getYS()->xi_t2( &E_Pred_EPS );
           BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 3rd tensorial var
  if( getELT3() ) {
     h_t[2]  = getELT3()->h_t(&E_Pred_EPS, getPS());
     xi_t[2] = getYS()->xi_t3( &E_Pred_EPS );
           BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // 4th tensorial var
  if( getELT4() ) {
     h_t[3]  = getELT4()->h_t(&E_Pred_EPS, getPS());
     xi_t[3] = getYS()->xi_t4( &E_Pred_EPS );
           BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
       hardMod_ = hardMod_ + hm.trace();
  }

  // Subtract accumulated hardMod_ from lower
        lower = lower - hardMod_;

        //h_s  = material_point.ELS1->h_s( &E_Pred_EPS, material_point.PS );
        //xi_s = material_point.YS->xi_s1( &E_Pred_EPS );
        //hardMod_ = h_s * xi_s;
        //lower = lower - hardMod_;

   ////Of tensorial internal var
  //h_t  = material_point.ELT1->h_t(&E_Pred_EPS, material_point.PS);
        //xi_t = material_point.YS->xi_t1( &E_Pred_EPS );
        //BJtensor hm = h_t("ij") * xi_t("ij");
  //hardMod_ = hm.trace();
        //lower = lower - hardMod_;


        Delta_lambda = f_pred/lower;
        if( Delta_lambda < 0.0 )
          {
            //::fprintf(stderr,"\nP!\n");
          }
        plastic_stress = (E("ijkl")*dQods("kl"))*(-Delta_lambda);
        plastic_stress.null_indices();
        //.. plastic_stress.reportshort("plastic stress predictor II\n");
        //.. elastic_predictor_stress.reportshort("elastic predictor stress \n");
        elastic_plastic_stress = elastic_predictor_stress + plastic_stress;
        elastic_plastic_stress.null_indices();

  SemibackwardEPS.setStress( elastic_plastic_stress );

        ////q_ast_entry = Criterion.kappa_get(elastic_plastic_stress);
        //S  = SemibackwardEPS.getScalarVar(1); // Get the old value of the internal var
        //h_s  = material_point.ELS1->h_s( &SemibackwardEPS, material_point.PS );
  //dS = Delta_lambda * h_s ;   // Increment to the internal scalar var
        //h_t  = material_point.ELT1->h_t( &SemibackwardEPS, material_point.PS );
  //dT = Delta_lambda * h_t ;   // Increment to the internal tensorial var

        //Evolve the surfaces and hardening vars
  int NS = SemibackwardEPS.getNScalarVar();
  int NT = SemibackwardEPS.getNTensorVar();

  int ii;
  for(ii = 1; ii <= NS; ii++) {
              dS = Delta_lambda * h_s[ii-1] ;       // Increment to the scalar internal var
              S  = SemibackwardEPS.getScalarVar(ii);     // Get the old value of the scalar internal var
              SemibackwardEPS.setScalarVar(ii, S + dS ); // Update internal scalar var
  }


        ////current_lambda_set(Delta_lambda);
        ////q_ast = q_ast_entry + Dq_ast;
        ////Criterion.kappa_set( elastic_plastic_stress, q_ast);
        //SemibackwardEPS.setScalarVar(1, S + dS );

  //stresstensor new_T = T + dT;
        //SemibackwardEPS.setTensorVar(1, new_T );

  stresstensor new_T;

  for(ii = 1; ii <= NT; ii++) {
        dT = h_t[ii-1] * Delta_lambda;            // Increment to the XC::BJtensor internal var
              T  = SemibackwardEPS.getTensorVar(ii);     // Get the old value of the XC::BJtensor internal var
              new_T = T + dT;
              SemibackwardEPS.setTensorVar(ii, new_T );
        }


        //return elastic_plastic_stress;
        return SemibackwardEPS;
    }
    return E_Pred_EPS;
}





//================================================================================
// New XC::EPState using Backward Euler Algorithm
//================================================================================
XC::EPState XC::Template3Dep::BackwardEulerEPState( const straintensor &strain_increment)
  {
  //!!!!!!!! Guanzhou rewrote the code according to Prof. Boris Jeremic, May2004

  // Volumetric strain
  //double st_vol = strain_increment.p_hydrostatic();
  double st_vol = strain_increment.Iinvariant1(); //- = compressive

  //EPState to be returned, it can be elastic or elastic-plastic XC::EPState
  EPState backwardEPS( * (this->getEPS()) );
  EPState startEPS( *(this->getEPS()) );

  stresstensor start_stress = startEPS.getStress(); //Guanzhou Mar2005

  //Output for plotting
  //std::cerr.precision(5);
  //std::cerr.width(10);
  //std::cerr << " strain_increment " << strain_increment << "\n";

  //std::cerr.precision(5);
  //std::cerr.width(10);
  //std::cerr << "start_stress " <<  start_stress;

  // Pulling out some XC::BJtensor and double definitions
  BJtensor I2("I", 2, def_dim_2);
  BJtensor I_ijkl("I", 4, def_dim_4);
  I_ijkl = I2("ij")*I2("kl");
  I_ijkl.null_indices();
  BJtensor I_ikjl("I", 4, def_dim_4);
  I_ikjl = I_ijkl.transpose0110();

  //double Ey = Criterion.E();
  //double nu = Criterion.nu();
  //BJtensor E = StiffnessTensorE(Ey,nu);
  BJtensor E  = ElasticStiffnessTensor();
  // BJtensor D = ComplianceTensorD(Ey,nu);
  // stresstensor REAL_start_stress = start_stress;

  BJtensor dFods(2, def_dim_2, 0.0);
  BJtensor dQods(2, def_dim_2, 0.0);
  //  BJtensor dQodsextended(2, def_dim_2, 0.0);
  //  BJtensor d2Qodqast(2, def_dim_2, 0.0);
  BJtensor temp2(2, def_dim_2, 0.0);
  double lower = 0.0;
  double Delta_lambda = 0.0; // Lambda
  double delta_lambda = 0.0; // dLambda

  double Felplpredictor    = 0.0;
  //Kai  double absFelplpredictor = 0.0;
  //  double Ftolerance = pow(d_macheps(),(1.0/2.0))*1000000.00; //FORWARD no iterations
  //double Ftolerance = pow( d_macheps(), 0.5)*1.00;

  //GZ out double Ftolerance = pow( d_macheps(), 0.5)*1000*KK;  //Zhaohui UCD 10e6 for Pa, kg and m 1000 for kPa, ton and m

  double Ftolerance = 1.0e-8; //Guanzhou gives an absolute tolerance

  //std::cerr << Ftolerance << std::endl;
  //  double Ftolerance = pow(d_macheps(),(1.0/2.0))*1.0;
  //  double entry_kappa_cone = Criterion.kappa_cone_get();
  //  double entry_kappa_cap  = Criterion.kappa_cap_get();

  BJtensor aC(2, def_dim_2, 0.0);
  stresstensor BEstress;
  stresstensor residual;
  BJtensor d2Qoverds2( 4, def_dim_4, 0.0);
  BJtensor T( 4, def_dim_4, 0.0);
  BJtensor Tinv( 4, def_dim_4, 0.0);

  double Fold = 0.0;
  BJtensor temp3lower;
  BJtensor temp5;
  double temp6 = 0.0;
  double upper = 0.0;

  stresstensor dsigma;
  //XC::stresstensor Dsigma;
  stresstensor sigmaBack;
  straintensor dPlasticStrain; // delta plastic strain
  straintensor PlasticStrain;  // Total plastic strain
  straintensor incrPlasticStrain;

  //double dq_ast = 0.0;       // iterative change in internal variable (kappa in this case)
  //double Dq_ast = 0.0;       // incremental change in internal variable (kappa in this case)
  //double q_ast  = 0.0;       // internal variable (kappa in this case)
  //double q_ast_entry  = 0.0; //internal variable from previous increment (kappa in this case)

  int step_counter = 0;
  //int MAX_STEP_COUNT = ;
  //  int MAX_STEP_COUNT = 0;
  //int flag = 0;

  straintensor strain_incr = strain_increment;
  strain_incr.null_indices();
  stresstensor stress_increment(E("ijkl")*strain_incr("kl"));
  stress_increment.null_indices();

  //XC::stresstensor Return_stress; //  stress to be returned can be either predictor
                              // or elastic plastic stress.

  EPState ElasticPredictorEPS( startEPS );
  stresstensor elastic_predictor_stress = start_stress + stress_increment;

  ElasticPredictorEPS.setStress( elastic_predictor_stress );
  //  elastic_predictor_stress.reportshortpqtheta("\n . . . .  ELASTIC PREDICTOR stress");

  //std::cerr.precision(5);
  //std::cerr.width(10);
  //std::cerr << "elastic predictor " <<  elastic_predictor_stress << std::endl;

  stresstensor elastic_plastic_predictor_stress;
  EPState EP_PredictorEPS( startEPS );

  //double f_start = material_point.YS->f( &startEPS );
  //std::cerr << " ************** f_start = " << f_start;
  //::fprintf(stdout,"tst##############  f_start = %.10e\n",f_start);
  // f_pred = Criterion.f(elastic_predictor_stress);
  //::fprintf(stdout,"tst##############  f_pred = %.10e\n",f_pred);
  //double f_start =  getYS()->f( &startEPS );
  //std::cerr << "\n  ************  Enter Backward   f_star **** " << f_start;

  double f_pred =  getYS()->f( &ElasticPredictorEPS );
  //double f_pred =  getYS()->f( &backwardEPS );
  //std::cerr << "  **BE f_pred **** " << f_pred << std::endl;
  //int region = 5;

  //double h_s      = 0.0;
  //double xi_s     = 0.0;
  double hardMod_ = 0.0;

  double h_s[4]       = {0.0, 0.0, 0.0, 0.0};
  double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
  stresstensor h_t[4];
  stresstensor xi_t[4];

  //region
  //check for the region than proceede
  //region = Criterion.influence_region(elastic_predictor_stress);
  //if( region == 1 )  // apex gray region
  //  {
  //    double pc_ = pc();
  //    elastic_plastic_predictor_stress =
  //      elastic_plastic_predictor_stress.pqtheta2stress(pc_, 0.0, 0.0);
  //    return  elastic_plastic_predictor_stress;
  //  }

  if( f_pred <= Ftolerance ) //Guanzhou changed
  {

      //Updating elastic strain increment
      straintensor estrain = ElasticPredictorEPS.getElasticStrain(); //Guanzhou
      straintensor tstrain = ElasticPredictorEPS.getStrain(); //Guanzhou
      estrain = estrain + strain_increment;
      tstrain = tstrain + strain_increment;
      ElasticPredictorEPS.setElasticStrain( estrain );
      ElasticPredictorEPS.setStrain( tstrain );
      ElasticPredictorEPS.setdElasticStrain( strain_increment );
      //std::cerr<< "Elastic:  Total strain" << tstrain << std::endl;

      if( getELT1() ) {
         getELT1()->updateEeDm(&ElasticPredictorEPS, -st_vol, 0.0);
      }

      //Set Elasto-Plastic stiffness XC::BJtensor
      ElasticPredictorEPS.setEep(E);
      ElasticPredictorEPS.setConverged(true);
      //E.printshort(" BE -- Eep ");

      backwardEPS = ElasticPredictorEPS;
      //std::cerr <<  "\n backwardEPS e " <<  backwardEPS.gete();

      //double p = (backwardEPS.getStress()).p_hydrostatic();
      //double ec = (backwardEPS.getec()) - (backwardEPS.getLam()) * log( p / (backwardEPS.getpo()) );
      //double st = (backwardEPS.getStrain()).Iinvariant1();
      //double pl_s = (backwardEPS.getPlasticStrain()).Iinvariant1();
      //double dpl_s = (backwardEPS.getdPlasticStrain()).Iinvariant1();
      //std::cerr << "E ec " << ec << " e " << backwardEPS.gete() << " psi " << (backwardEPS.gete() - ec) << " strain " << st << " t_pl " << pl_s << " d_pl " << dpl_s << "\n";
      return backwardEPS;

      //std::cerr <<  "\nbackwardEPS" <<  backwardEPS;
      //std::cerr <<  "\nElasticPredictorEPS " <<  ElasticPredictorEPS;

  }
  if( f_pred > Ftolerance )
  {

      //Let's put strict backwardEuler here
      //Starting point by applying one Forward Euler step
      EP_PredictorEPS = PredictorEPState( strain_incr);
      //EP_PredictorEPS = ElasticPredictorEPS;

      //std::cerr << " ----------Predictor Stress" << EP_PredictorEPS.getStress();
      //Setting the starting XC::EPState with the starting internal vars in XC::EPState

      //MP->setEPS( EP_PredictorEPS );

      Felplpredictor =  getYS()->f(&EP_PredictorEPS);
      //std::cerr <<  " BE: F_elplpredictor " << Felplpredictor << std::endl;


      //Kai     absFelplpredictor = fabs(Felplpredictor);
      if( fabs(Felplpredictor) <= Ftolerance )
      {
      //Forward Euler will do.
         backwardEPS = EP_PredictorEPS;
               return backwardEPS; //Guanzhou
         //Return_stress = elastic_plastic_predictor_stress;
         //Guanzhou out flag = 1;
      }

      //GZ out aC    = getPS()->dQods( &EP_PredictorEPS );
      //GZ out dFods = getYS()->dFods( &EP_PredictorEPS );
      //GZ out dQods = getPS()->dQods( &EP_PredictorEPS );
      //GZ out
      //GZ out temp2 = (dFods("ij")*E("ijkl"))*dQods("kl");
      //GZ out temp2.null_indices();
      //GZ out lower = temp2.trace();
      //GZ out
      //GZ out //Guanzhou added internal evolution Mar2005
      //GZ out hardMod_ = 0.0;
      //GZ out XC::BJtensor Hh(2, def_dim_2, 0.0);
      //GZ out //Of 1st scalar internal vars
      //GZ out if( getELS1() ) {
      //GZ out         h_s[0]  = getELS1()->h_s( &EP_PredictorEPS, getPS());
      //GZ out              xi_s[0] = getYS()->xi_s1( &EP_PredictorEPS );
      //GZ out           hardMod_ = hardMod_ + h_s[0] * xi_s[0];
      //GZ out }
      //GZ out
      //GZ out //Of 2nd scalar internal vars
      //GZ out if( getELS2() ) {
      //GZ out         h_s[1]  = getELS2()->h_s( &EP_PredictorEPS, getPS());
      //GZ out              xi_s[1] = getYS()->xi_s2( &EP_PredictorEPS );
      //GZ out           hardMod_ = hardMod_ + h_s[1] * xi_s[1];
      //GZ out }
      //GZ out
      //GZ out //Of 3rd scalar internal vars
      //GZ out if( getELS3() ) {
      //GZ out         h_s[2]  = getELS3()->h_s( &EP_PredictorEPS, getPS());
      //GZ out              xi_s[2] = getYS()->xi_s3( &EP_PredictorEPS );
      //GZ out           hardMod_ = hardMod_ + h_s[2] * xi_s[2];
      //GZ out }
      //GZ out
      //GZ out //Of 4th scalar internal vars
      //GZ out if( getELS4() ) {
      //GZ out         h_s[3]  = getELS4()->h_s( &EP_PredictorEPS, getPS());
      //GZ out              xi_s[3] = getYS()->xi_s4( &EP_PredictorEPS );
      //GZ out           hardMod_ = hardMod_ + h_s[3] * xi_s[3];
      //GZ out }
      //GZ out
      //GZ out //Of tensorial internal var
      //GZ out // 1st tensorial var
      //GZ out if( getELT1() ) {
      //GZ out         h_t[0]  = getELT1()->h_t( &EP_PredictorEPS, getPS());
      //GZ out         xi_t[0] = getYS()->xi_t1( &EP_PredictorEPS );
      //GZ out              BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
      //GZ out          hardMod_ = hardMod_ + hm.trace();
      //GZ out }
      //GZ out
      //GZ out // 2nd tensorial var
      //GZ out if( getELT2() ) {
      //GZ out         h_t[1]  = getELT2()->h_t( &EP_PredictorEPS, getPS());
      //GZ out         xi_t[1] = getYS()->xi_t2( &EP_PredictorEPS );
      //GZ out              BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
      //GZ out          hardMod_ = hardMod_ + hm.trace();
      //GZ out }
      //GZ out
      //GZ out // 3rd tensorial var
      //GZ out if( getELT3() ) {
      //GZ out         h_t[2]  = getELT3()->h_t( &EP_PredictorEPS, getPS());
      //GZ out         xi_t[2] = getYS()->xi_t3( &EP_PredictorEPS );
      //GZ out              BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
      //GZ out          hardMod_ = hardMod_ + hm.trace();
      //GZ out }
      //GZ out
      //GZ out // 4th tensorial var
      //GZ out if( getELT4() ) {
      //GZ out         h_t[3]  = getELT4()->h_t( &EP_PredictorEPS, getPS());
      //GZ out         xi_t[3] = getYS()->xi_t4( &EP_PredictorEPS );
      //GZ out              BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
      //GZ out          hardMod_ = hardMod_ + hm.trace();
      //GZ out }
      //GZ out
      //GZ out
      //GZ out lower = lower - hardMod_;
      //GZ out
      //GZ out
      //GZ out Delta_lambda = f_pred/lower; //Guanzhou Mar2005
      //GZ out //::printf("  Delta_lambda = f_pred/lower = %.8e\n", Delta_lambda);
      //GZ out ////     Delta_lambda = Felplpredictor/lower;
      //GZ out ////::printf("  Delta_lambda = Felplpredictor/lower =%.8e \n", Delta_lambda);
      //GZ out
      //GZ out elastic_plastic_predictor_stress = elastic_predictor_stress - E("ijkl")*aC("kl")*Delta_lambda;
      //GZ out elastic_plastic_predictor_stress.null_indices();
      //GZ out EP_PredictorEPS.setStress( elastic_plastic_predictor_stress );
      //GZ out
      //GZ out //Guanzhou, update internal
      //GZ out
      //GZ out int NS = EP_PredictorEPS.getNScalarVar();
      //GZ out int NT = EP_PredictorEPS.getNTensorVar();
      //GZ out
      //GZ out double dS = 0;
      //GZ out double S  = 0;
      //GZ out //double new_S = 0;
      //GZ out
      //GZ out XC::stresstensor dT;
      //GZ out XC::stresstensor Tv;
      //GZ out XC::stresstensor new_T;
      //GZ out
      //GZ out int ii;
      //GZ out for(ii = 1; ii <= NS; ii++) {
      //GZ out         dS = Delta_lambda * h_s[ii-1] ;             // Increment to the scalar internal var
      //GZ out              S  = EP_PredictorEPS.getScalarVar(ii);      // Get the old value of the scalar internal var
      //GZ out              EP_PredictorEPS.setScalarVar(ii, S + dS );  // Update internal scalar var
      //GZ out }
      //GZ out
      //GZ out for(ii = 1; ii <= NT; ii++) {
      //GZ out         dT = h_t[ii-1] * Delta_lambda;            // Increment to the XC::BJtensor internal var
      //GZ out              Tv  = EP_PredictorEPS.getTensorVar(ii);     // Get the old value of the XC::BJtensor internal var
      //GZ out              new_T = Tv + dT;
      //GZ out              EP_PredictorEPS.setTensorVar(ii, new_T );  // Update tensorial scalar var
      //GZ out }
      //GZ out
      //GZ out Felplpredictor =  getYS()->f(&EP_PredictorEPS);

  //Guanzhou out Mar2005 //Zhaohui modified, sometimes give much better convergence rate
  //Guanzhou out Mar2005       elastic_plastic_predictor_stress = EP_PredictorEPS.getStress();
  //Guanzhou out Mar2005 //std::cerr << "elastic_plastic_predictor_stress" << elastic_plastic_predictor_stress;

        //std::cerr.precision(5);
        //std::cerr.width(10);
        //std::cerr << " " << EP_PredictorEPS.getStress().p_hydrostatic() << " ";

        //std::cerr.precision(5);
        //std::cerr.width(10);
        //std::cerr << EP_PredictorEPS.getStress().q_deviatoric()<< " ";

        //std::cerr.precision(5);
        //std::cerr.width(10);
        //std::cerr << Delta_lambda << std::endl;

  //elastic_plastic_predictor_stress.reportshort("......elastic_plastic_predictor_stress");
        //::printf("  F(elastic_plastic_predictor_stress) = %.8e\n", Criterion.f(elastic_plastic_predictor_stress));

        //h_s  = MP->ELS1->h_s( &EP_PredictorEPS, MP->PS );
        ////h_  = h(elastic_plastic_predictor_stress);
        ////  Dq_ast = Criterion.kappa_get(elastic_plastic_predictor_stress);

  //q_ast_entry = Criterion.kappa_get(elastic_plastic_predictor_stress);
        //Dq_ast = Delta_lambda * h_ * just_this_PP;
        //q_ast = q_ast_entry + Dq_ast;

  //Zhaohui comments: internal vars are alreary evolued in ForwardEulerEPS(...), not necessary here!!!
  //..dS = Delta_lambda * h_s ;   // Increment to the internal var
        //..S  = EP_PredictorEPS.getScalarVar(1); // Get the old value of the internal var
  //..new_S = S + dS;
  //..std::cerr << "Internal Var : " << new_S << std::endl;
  //..EP_PredictorEPS.setScalarVar(1, new_S); // Get the old value of the internal var

  //::fprintf(stdout," ######## predictor --> Dq_ast=%.8e q_ast=%.8e\n", Dq_ast,        q_ast);
        //::fprintf(stderr," ######## predictor --> Dq_ast=%.8e q_ast=%.8e\n", Dq_ast,        q_ast);

        //Criterion.kappa_set( sigmaBack, q_ast);  //????
        //current_lambda_set(Delta_lambda);     //????

        //::printf("  Delta_lambda = %.8e\n", Delta_lambda);
        //::printf("step = pre iteracija  #############################--   q_ast = %.10e \n", q_ast);
        //::printf("step = pre iteracija  posle predictora  ###########--   Dq_ast = %.10e \n",Dq_ast);
        //**********
        //**********
        //::printf("\nDelta_lambda  before BE = %.10e \n", Delta_lambda );

        //========================== main part of iteration =======================
        //      while( absFelplpredictor > Ftolerance &&
        double Felplold = 0.0;
        Delta_lambda = EP_PredictorEPS.Delta_lambda;
        while  (( fabs(fabs(Felplpredictor) - fabs(Felplold)) > 1.0e-6 )
                && ( fabs(fabs(Felplpredictor) ) > 1.0e-5 )
                && (step_counter < MAX_STEP_COUNT) )// if more iterations than prescribed
        //while( ( fabs(fabs(Felplpredictor) ) > Ftolerance ) && (step_counter < MAX_STEP_COUNT) )// if more iterations than prescribed
        //out07may97      do
        {
          //std::cerr << "Iteration " << step_counter << " F " << Felplpredictor << " delta " << Delta_lambda << std::endl;
          aC = getPS()->dQods( &EP_PredictorEPS );
          BEstress = elastic_predictor_stress - stresstensor(E("ijkl")*aC("kl")*Delta_lambda);
          //BEstress.reportshort("......BEstress ");
          /////          BEstress = elastic_plastic_predictor_stress - E("ijkl")*aC("kl")*Delta_lambda;
          BEstress.null_indices();
          //          Felplpredictor = Criterion.f(BEstress);
          //          ::printf("\nF_backward_Euler BE = %.10e \n", Felplpredictor);

          //Guanzhou out Mar2005 residual = elastic_plastic_predictor_stress - BEstress;
          residual = EP_PredictorEPS.getStress() - BEstress;

          //residual.reportshortpqtheta("\n......residual ");
          //          double ComplementaryEnergy = (residual("ij")*D("ijkl")*residual("ij")).trace();
          //::printf("\n Residual ComplementaryEnergy = %.16e\n", ComplementaryEnergy);

          /////          residual = elastic_predictor_stress - BEstress;


          //d2Qoverds2 = Criterion.d2Qods2(elastic_plastic_predictor_stress);
          d2Qoverds2 = getPS()->d2Qods2( &EP_PredictorEPS );
          //d2Qoverds2.print();

              T = I_ikjl + E("ijkl")*d2Qoverds2("klmn")*Delta_lambda;
          T.null_indices();

          Tinv = T.inverse();

              //Guanzhou out Mar2005
          //Guanzhou out Mar2005 //Z Cheng add: H
              //Guanzhou out Mar2005 XC::BJtensor H( 2, def_dim_2, 0.0);
              //Guanzhou out Mar2005 H = dQods;

          //dFods = Criterion.dFods(elastic_plastic_predictor_stress);
          //dQods = Criterion.dQods(elastic_plastic_predictor_stress);
          dFods = getYS()->dFods( &EP_PredictorEPS );
          dQods = getPS()->dQods( &EP_PredictorEPS );

          //Guanzhou Mar2005
          BJtensor H( 2, def_dim_2, 0.0);
          H = dQods;

          //Fold = Criterion.f(elastic_plastic_predictor_stress);
          Fold = getYS()->f( &EP_PredictorEPS );

              lower = 0.0; // this is old temp variable used here again :-)
          //h_  = h(elastic_plastic_predictor_stress);
          //xi_ = xi(elastic_plastic_predictor_stress);

              //h_s  = MP->ELS1->h_s( &EP_PredictorEPS, MP->PS );
          //xi_s = MP->YS->xi_s1( &EP_PredictorEPS );
          //hardMod_ = h_s * xi_s;

          // Evaluating the hardening modulus: sum of  (df/dq*) * qbar

              hardMod_ = 0.0;
          BJtensor Hh(2, def_dim_2, 0.0);
              //Of 1st scalar internal vars

             if( getELS1() ) {
                       h_s[0]  = getELS1()->h_s( &EP_PredictorEPS, getPS());
                     xi_s[0] = getYS()->xi_s1( &EP_PredictorEPS );
                  hardMod_ = hardMod_ + h_s[0] * xi_s[0];
                       Hh = Hh + getPS()->d2Qodsds1( &EP_PredictorEPS ) *h_s[0] *Delta_lambda;
              }

              //Of 2nd scalar internal vars
              if( getELS2() ) {
                       h_s[1]  = getELS2()->h_s( &EP_PredictorEPS, getPS());
                     xi_s[1] = getYS()->xi_s2( &EP_PredictorEPS );
                  hardMod_ = hardMod_ + h_s[1] * xi_s[1];
                       Hh = Hh + getPS()->d2Qodsds2( &EP_PredictorEPS ) *h_s[1] *Delta_lambda;
              }

              //Of 3rd scalar internal vars
              if( getELS3() ) {
                       h_s[2]  = getELS3()->h_s( &EP_PredictorEPS, getPS());
                     xi_s[2] = getYS()->xi_s3( &EP_PredictorEPS );
                  hardMod_ = hardMod_ + h_s[2] * xi_s[2];
                       Hh = Hh + getPS()->d2Qodsds3( &EP_PredictorEPS ) *h_s[2] *Delta_lambda;
              }

              //Of 4th scalar internal vars
              if( getELS4() ) {
                       h_s[3]  = getELS4()->h_s( &EP_PredictorEPS, getPS());
                     xi_s[3] = getYS()->xi_s4( &EP_PredictorEPS );
                  hardMod_ = hardMod_ + h_s[3] * xi_s[3];
                       Hh = Hh + getPS()->d2Qodsds4( &EP_PredictorEPS ) *h_s[3] *Delta_lambda;
              }

              //Of tensorial internal var
              // 1st tensorial var
              if( getELT1() ) {
                       h_t[0]  = getELT1()->h_t( &EP_PredictorEPS, getPS());
                       xi_t[0] = getYS()->xi_t1( &EP_PredictorEPS );
                     BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
                 hardMod_ = hardMod_ + hm.trace();
                       Hh = Hh + (getPS()->d2Qodsdt1( &EP_PredictorEPS ))("ijmn") *(h_t[0])("mn") *Delta_lambda;
              }

              // 2nd tensorial var
              if( getELT2() ) {
                       h_t[1]  = getELT2()->h_t( &EP_PredictorEPS, getPS());
                       xi_t[1] = getYS()->xi_t2( &EP_PredictorEPS );
                     BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
                 hardMod_ = hardMod_ + hm.trace();
                       Hh = Hh + (getPS()->d2Qodsdt2( &EP_PredictorEPS ))("ijmn") *(h_t[1])("mn") *Delta_lambda;
              }

              // 3rd tensorial var
              if( getELT3() ) {
                       h_t[2]  = getELT3()->h_t( &EP_PredictorEPS, getPS());
                       xi_t[2] = getYS()->xi_t3( &EP_PredictorEPS );
                     BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
                 hardMod_ = hardMod_ + hm.trace();
                       Hh = Hh + (getPS()->d2Qodsdt3( &EP_PredictorEPS ))("ijmn") *(h_t[2])("mn") *Delta_lambda;
              }

              // 4th tensorial var
              if( getELT4() ) {
                       h_t[3]  = getELT4()->h_t( &EP_PredictorEPS, getPS());
                       xi_t[3] = getYS()->xi_t4( &EP_PredictorEPS );
                     BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
                 hardMod_ = hardMod_ + hm.trace();
                       Hh = Hh + (getPS()->d2Qodsdt4( &EP_PredictorEPS ))("ijmn") *(h_t[3])("mn") *Delta_lambda;
              }

              // Subtract accumulated hardMod_ from lower
          //lower = lower - hardMod_;

              //hardMod_ = hardMod_ * just_this_PP;
          //::printf("\n BackwardEulerStress ..  hardMod_ = %.10e \n", hardMod_ );
          //outfornow          d2Qodqast = d2Qoverdqast(elastic_plastic_predictor_stress);
          //outfornow          dQodsextended = dQods + d2Qodqast * Delta_lambda * h_;
          //outfornow          temp3lower = dFods("mn")*Tinv("ijmn")*E("ijkl")*dQodsextended("kl");
          // temp3lower = dFods("mn")*Tinv("ijmn")*E("ijkl")*dQods("kl"); // Bug found, Z Cheng, Jan 2004
              Hh.null_indices();

              H = H + Hh;

          temp3lower = dFods("mn")*Tinv("ijmn")*E("ijkl")*H("kl");
          temp3lower.null_indices();
          lower = temp3lower.trace();
          lower = lower - hardMod_;
    //std::cerr << " 1st hardMod_ " <<  hardMod_ << "\n";

          temp5 = (residual("ij") * Tinv("ijmn")) * dFods("mn");
          temp6 = temp5.trace();
    //The same as the above but more computation
          //temp5 = dFods("mn") * residual("ij") * Tinv("ijmn");
          //temp6 = temp5.trace();

          upper = Fold - temp6;

    //================================================================================
    //dlambda
          delta_lambda = upper / lower;
          //std::cerr << "upper " << upper << " lower " << lower << std::endl;
          Delta_lambda = Delta_lambda + delta_lambda;
          if( Delta_lambda < 0.0 ) Delta_lambda = 0.0;//Guanzhou

    //Guanzhou corrected May2005 // Zhaohui_____10-01-2000 not sure xxxxxxxxxxxxxxxxxxx
      dPlasticStrain = dQods("kl") * delta_lambda;
      incrPlasticStrain = incrPlasticStrain + dPlasticStrain;
      //Updating elastic strain increment
      straintensor estrain = EP_PredictorEPS.getElasticStrain(); //Guanzhou
      straintensor tstrain = EP_PredictorEPS.getStrain(); //Guanzhou
      straintensor pstrain = tstrain - estrain;
      pstrain = pstrain + incrPlasticStrain;
      estrain = tstrain - pstrain;
      EP_PredictorEPS.setElasticStrain( estrain );
      EP_PredictorEPS.setPlasticStrain( pstrain );
      EP_PredictorEPS.setdPlasticStrain( incrPlasticStrain );
      straintensor incrElasticStrain= (-1.0)*incrPlasticStrain;
      EP_PredictorEPS.setdElasticStrain( incrElasticStrain );
      EP_PredictorEPS.Delta_lambda = Delta_lambda;

      //std::cerr<< "Elastic:  Total strain" << tstrain << std::endl;

      int err;
      if( getELT1() ) {
              double pl_st_vol = pstrain.Iinvariant1(); //Joey 02-17-03
              // D > 0 compressive -> Iinv > 0  -> de < 0 correct!
        err = getELT1()->updateEeDm(&EP_PredictorEPS, pl_st_vol, Delta_lambda);
      }

          //::printf(" >> %d  Delta_lambda = %.8e", step_counter, Delta_lambda);
          // stari umesto dQodsextended za stari = dQods
          //outfornow          dsigma =
          //outfornow            ((residual("ij")*Tinv("ijmn"))+
          //outfornow            ((E("ijkl")*dQodsextended("kl"))*Tinv("ijmn")*Delta_lambda) )*-1.0;
          //::printf("    Delta_lambda = %.8e\n", Delta_lambda);
          //dsigma = ( (residual("ij")*Tinv("ijmn") )+
          //         ( (E("ijkl")*dQods("kl"))*Tinv("ijmn")*delta_lambda) )*(-1.0); //*-1.0???  // Bug found, Z Cheng, Jan 2004
          dsigma = ( (residual("ij")*Tinv("ijmn") )+
                   ( (E("ijkl")*H("kl"))*Tinv("ijmn")*delta_lambda) )*(-1.0); //Guanzhou fixed Apr2005
          dsigma.null_indices();

          sigmaBack = EP_PredictorEPS.getStress() + dsigma;
          EP_PredictorEPS.setStress(sigmaBack);

        //dsigma.reportshortpqtheta("\n......dsigma ");
          //::printf("  .........   in NR loop   delta_lambda = %.16e\n", delta_lambda);
          //::printf("  .........   in NR loop   Delta_lambda = %.16e\n", Delta_lambda);

    //dq_ast = delta_lambda * h_ * just_this_PP;
          //Dq_ast += dq_ast;

    //dS = delta_lambda * h_s ;   // Increment to the internal var
          //S  = EP_PredictorEPS.getScalarVar(1); // Get the old value of the internal var
    //new_S = S + dS;
          //EP_PredictorEPS.setScalarVar(1, new_S);

          //Evolve the surfaces and hardening vars
            int NS = EP_PredictorEPS.getNScalarVar();
            int NT = EP_PredictorEPS.getNTensorVar();

            double dS = 0;
              double S  = 0;
      //double new_S = 0;

              stresstensor dT;
              stresstensor T;
              stresstensor new_T;

    //if( delta_lambda < 0) delta_lambda = 0;
            int ii;
            for(ii = 1; ii <= NS; ii++) {
             dS = delta_lambda * h_s[ii-1] ;      //Guanzhou fixed Apr2005       // Increment to the scalar internal var
             S  = EP_PredictorEPS.getScalarVar(ii);      //Guanzhou fixed Apr2005// Get the old value of the scalar internal var
             EP_PredictorEPS.setScalarVar(ii, S + dS );  // Update internal scalar var
            }

            for(ii = 1; ii <= NT; ii++) {
                       dT = h_t[ii-1] * delta_lambda;      //Guanzhou fixed Apr2005      // Increment to the XC::BJtensor internal var
                     T  = EP_PredictorEPS.getTensorVar(ii);     //Guanzhou fixed Apr2005// Get the old value of the XC::BJtensor internal var
                     new_T = T + dT;
                     EP_PredictorEPS.setTensorVar(ii, new_T );  // Update tensorial scalar var
        }

        Felplold = Felplpredictor;
        Felplpredictor = getYS()->f( &EP_PredictorEPS );

    //=======          Dq_ast = Delta_lambda * h_ * just_this_PP;
          //q_ast = q_ast_entry + Dq_ast;

    //::fprintf(stdout," ######## step = %3d --> Dq_ast=%.8e q_ast=%.8e\n",
          //             step_counter,         Dq_ast,        q_ast);
          //::fprintf(stderr," ######## step = %3d --> Dq_ast=%.8e q_ast=%.8e\n",
          //             step_counter,         Dq_ast,        q_ast);

    //current_lambda_set(Delta_lambda);
          //....          elastic_plastic_predictor_stress.reportshort("elplpredstress");
          //....          dsigma.reportshort("dsigma");

          //sigmaBack.reportshortpqtheta("\n before======== SigmaBack");
    //Guanzhou out Mar2005 sigmaBack = elastic_plastic_predictor_stress + dsigma;
          //sigmaBack.deviator().reportshort("\n after ======== SigmaBack");
          //sigmaBack.reportshortpqtheta("\n after ======== SigmaBack");

    //Guanzhou out Mar2005 //temp trick
    //Guanzhou out Mar2005    if  (sigmaBack.p_hydrostatic() > 0)
    //Guanzhou out Mar2005    {
    //Guanzhou out Mar2005       //======          sigmaBack = elastic_predictor_stress + Dsigma;
    //Guanzhou out Mar2005       //sigmaBack.reportshortpqtheta("BE................  NR sigmaBack   ");
    //Guanzhou out Mar2005       //sigmaBack.reportAnim();
    //Guanzhou out Mar2005       //::fprintf(stdout,"Anim BEpoint0%d   = {Sin[theta]*q, p, Cos[theta]*q} \n",step_counter+1);
    //Guanzhou out Mar2005       ////::fprintf(stdout,"Anim BEpoint0%dP = Point[BEpoint0%d] \n",step_counter+1,step_counter+1);
    //Guanzhou out Mar2005       //::fprintf(stdout,"Anim   \n");
    //Guanzhou out Mar2005
    //Guanzhou out Mar2005 //Criterion.kappa_set( sigmaBack, q_ast) ;
    //Guanzhou out Mar2005       EP_PredictorEPS.setStress( sigmaBack );
    //Guanzhou out Mar2005
    //Guanzhou out Mar2005 //Felplpredictor = Criterion.f(sigmaBack);
    //Guanzhou out Mar2005       //Kai          absFelplpredictor = fabs(Felplpredictor);
    //Guanzhou out Mar2005       //::printf("  F_bE=%.10e (%.10e)\n", Felplpredictor,Ftolerance);
    //Guanzhou out Mar2005 Felplpredictor = getYS()->f( &EP_PredictorEPS );
    //Guanzhou out Mar2005       //::printf(" F_BE: step=%5d  F= %.10e (%.10e)\n", step_counter, Felplpredictor, Ftolerance);
    //Guanzhou out Mar2005    }
    //Guanzhou out Mar2005    else
    //Guanzhou out Mar2005    {
    //Guanzhou out Mar2005       sigmaBack= sigmaBack.pqtheta2stress(0.1, 0.0, 0.0);
    //Guanzhou out Mar2005       Felplpredictor = 0;
    //Guanzhou out Mar2005       backwardEPS.setStress(sigmaBack);
    //Guanzhou out Mar2005       backwardEPS.setConverged(true);
    //Guanzhou out Mar2005       return backwardEPS;
    //Guanzhou out Mar2005    }

          //double tempkappa1 = kappa_cone_get();
          //double tempdFodeta = dFoverdeta(sigmaBack);
          //double tempdetaodkappa = detaoverdkappa(tempkappa1);
          //::printf("    h_=%.6e  xi_=%.6e, dFodeta=%.6e, detaodkappa=%.6e, hardMod_=%.6e\n",
          //     h_, xi_,tempdFodeta, tempdetaodkappa,  hardMod_);
          //::printf("   upper = %.6e    lower = %.6e\n", upper, lower);
          //::printf(" q_ast_entry=%.6e  Dq_ast=%.6e   Delta_lambda=%.6e\n",
          //      q_ast_entry, Dq_ast, Delta_lambda);

          // now prepare new step
          //Guanzhou out Mar2005 elastic_plastic_predictor_stress = sigmaBack;

    //Output for plotting
    //std::cerr.precision(5);
          //std::cerr.width(10);
          //std::cerr << " " << sigmaBack.p_hydrostatic() << " ";
          //std::cerr << " " << sigmaBack.p_hydrostatic() << " ";

    //std::cerr.precision(5);
          //std::cerr.width(10);
          //std::cerr << sigmaBack.q_deviatoric() << " ";
          //std::cerr << sigmaBack.q_deviatoric() << " ";

    //std::cerr.precision(5);
          //std::cerr.width(10);
          //std::cerr << " Felpl " << Felplpredictor;
          //std::cerr << " Felpl " << Felplpredictor;

    //std::cerr.precision(5);
          //std::cerr.width(10);
          //std::cerr << " tol " << Ftolerance << std::endl;
          //std::cerr << " tol " << Ftolerance << " " << step_counter << std::endl;

    //::printf("         ...........................  end of step %d\n", step_counter);// getchar();
          step_counter++;

          //int err = 0;
          //if( getELT1() ) {
    //   double pl_st_vol = dPlasticStrain.Iinvariant1(); //Joey 02-17-03           Iinvariant1 > 0 for compression
          //   std::cerr << " Pl update " << pl_st_vol << "\n";
    //   err = getELT1()->updateEeDm(&EP_PredictorEPS, - pl_st_vol, delta_lambda); // D > 0 --> contraction
    //}

        }

        //std::cerr << " " << sigmaBack.p_hydrostatic() << " ";
        //std::cerr << sigmaBack.q_deviatoric() << " ";
        //std::cerr << " Felpl " << Felplpredictor;
        //std::cerr << " tol " << Ftolerance << " " << step_counter << std::endl;

        //// Update E_Young and e according to current stress state before evaluate ElasticStiffnessTensor
        if( step_counter >= MAX_STEP_COUNT  )
        {
           //std::clog << "Template3Dep::BackwardEulerEPState   Step_counter > MAX_STEP_COUNT %d iterations", MAX_STEP_COUNT ;
           //Guanzhou
           std::cerr << "Template3Dep::BackwardEuler(), failed to converge in " << step_counter << "steps!!!" << '\n';
           EP_PredictorEPS.setConverged( false );
           exit(1);
     //::exit(1);
        }

        //Guanzhou out int err = 0;
        //Guanzhou out if( getELT1() ) {
             //Guanzhou out         double pl_st_vol = PlasticStrain.Iinvariant1(); //Joey 02-17-03
             //Guanzhou out         // D > 0 compressive -> Iinv > 0  -> de < 0 correct!
        //Guanzhou out            err = getELT1()->updateEeDm(&EP_PredictorEPS, pl_st_vol, Delta_lambda);
          //Guanzhou out }

  //out07may97      while( absFelplpredictor > Ftolerance &&
        //out07may97              step_counter <= MAX_STEP_COUNT  ); // if more iterations than prescribed

        //**********
        //**********
        //**********
        //**********

       // already set everything
  // Need to generate Eep and set strains and stresses
        //if( ( flag !=1) && (step_counter < MAX_STEP_COUNT) )
        //Guanzhou out Mar2005 if( ( flag !=1) ) {
          //Guanzhou out Mar2005

           //Return_stress = elastic_plastic_predictor_stress;
           //Criterion.kappa_set( Return_stress, q_ast) ;

           // Generating Consistent Stiffness XC::Tensor Eep
           BJtensor I2("I", 2, def_dim_2);
           BJtensor I_ijkl = I2("ij")*I2("kl");
           I_ijkl.null_indices();
           BJtensor I_ikjl = I_ijkl.transpose0110();


           dQods = getPS()->dQods( &EP_PredictorEPS ); // this is m_ij
           BJtensor temp2 = E("ijkl")*dQods("kl");
           temp2.null_indices();
           dFods = getYS()->dFods( &EP_PredictorEPS ); // this is n_ij
           d2Qoverds2 = getPS()->d2Qods2( &EP_PredictorEPS );

           BJtensor T = I_ikjl + E("ijkl")*d2Qoverds2("klmn")*Delta_lambda;
       //BJtensor tt = E("ijkl")*d2Qoverds2("klmn")*Delta_lambda;
     //tt.printshort("temp tt");
     //T = I_ikjl + tt;
           T.null_indices();
           BJtensor Tinv = T.inverse();

           BJtensor R = Tinv("ijmn")*E("ijkl");
           R.null_indices();

     BJtensor Hh(2, def_dim_2, 0.0);
     //Hh.Reset_to(0.0);
     BJtensor H(2, def_dim_2, 0.0);
     H =dQods;

    //Of 1st scalar internal vars
    if( getELS1() ) {
       h_s[0]  = getELS1()->h_s( &EP_PredictorEPS, getPS());
             xi_s[0] = getYS()->xi_s1( &EP_PredictorEPS );
          hardMod_ = hardMod_ + h_s[0] * xi_s[0];
       Hh = Hh + getPS()->d2Qodsds1( &EP_PredictorEPS ) *h_s[0] *Delta_lambda;
    }

    //Of 2nd scalar internal vars
    if( getELS2() ) {
       h_s[1]  = getELS2()->h_s( &EP_PredictorEPS, getPS());
             xi_s[1] = getYS()->xi_s2( &EP_PredictorEPS );
          hardMod_ = hardMod_ + h_s[1] * xi_s[1];
       Hh = Hh + getPS()->d2Qodsds2( &EP_PredictorEPS ) *h_s[1] *Delta_lambda;
    }

    //Of 3rd scalar internal vars
    if( getELS3() ) {
       h_s[2]  = getELS3()->h_s( &EP_PredictorEPS, getPS());
             xi_s[2] = getYS()->xi_s3( &EP_PredictorEPS );
          hardMod_ = hardMod_ + h_s[2] * xi_s[2];
       Hh = Hh + getPS()->d2Qodsds3( &EP_PredictorEPS ) *h_s[2] *Delta_lambda;
    }

    //Of 4th scalar internal vars
    if( getELS4() ) {
       h_s[3]  = getELS4()->h_s( &EP_PredictorEPS, getPS());
             xi_s[3] = getYS()->xi_s4( &EP_PredictorEPS );
          hardMod_ = hardMod_ + h_s[3] * xi_s[3];
       Hh = Hh + getPS()->d2Qodsds4( &EP_PredictorEPS ) *h_s[3] *Delta_lambda;
    }

    //Of tensorial internal var
    // 1st tensorial var
    if( getELT1() ) {
       h_t[0]  = getELT1()->h_t( &EP_PredictorEPS, getPS());
       xi_t[0] = getYS()->xi_t1( &EP_PredictorEPS );
             BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
         hardMod_ = hardMod_ + hm.trace();
       Hh = Hh + (getPS()->d2Qodsdt1( &EP_PredictorEPS ))("ijmn") *(h_t[0])("mn") *Delta_lambda;
    }

    // 2nd tensorial var
    if( getELT2() ) {
       h_t[1]  = getELT2()->h_t( &EP_PredictorEPS, getPS());
       xi_t[1] = getYS()->xi_t2( &EP_PredictorEPS );
             BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
         hardMod_ = hardMod_ + hm.trace();
       Hh = Hh + (getPS()->d2Qodsdt2( &EP_PredictorEPS ))("ijmn") *(h_t[1])("mn") *Delta_lambda;
    }

    // 3rd tensorial var
    if( getELT3() ) {
       h_t[2]  = getELT3()->h_t( &EP_PredictorEPS, getPS());
       xi_t[2] = getYS()->xi_t3( &EP_PredictorEPS );
             BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
         hardMod_ = hardMod_ + hm.trace();
       Hh = Hh + (getPS()->d2Qodsdt3( &EP_PredictorEPS ))("ijmn") *(h_t[2])("mn") *Delta_lambda;
    }

    // 4th tensorial var
    if( getELT4() ) {
       h_t[3]  = getELT4()->h_t( &EP_PredictorEPS, getPS());
       xi_t[3] = getYS()->xi_t4( &EP_PredictorEPS );
             BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
         hardMod_ = hardMod_ + hm.trace();
       Hh = Hh + (getPS()->d2Qodsdt4( &EP_PredictorEPS ))("ijmn") *(h_t[3])("mn") *Delta_lambda;
    }

     H = H + Hh;


     // BJtensor temp3lower = dFods("ot")*R("otpq")*dQods("pq");  // Bug found, Z Cheng, Jan 2004
     BJtensor temp3lower = dFods("ot")*R("otpq")*H("pq");
         temp3lower.null_indices();

         double lower = temp3lower.trace();

         lower = lower - hardMod_;  // h
     //std::cerr << " 2nd hardMod_ " <<  hardMod_ << "\n";


         //BJtensor upper = R("pqkl")*dQods("kl")*dFods("ij")*R("ijmn");
         // BJtensor upper11 = R("pqkl")*dQods("kl");  // Bug found, Z Cheng, Jan 2004
     BJtensor upper11 = R("pqkl")*H("kl");
         BJtensor upper22 = dFods("ij")*R("ijmn");
     upper11.null_indices();
     upper22.null_indices();
         BJtensor upper = upper11("pq")*upper22("mn");

     upper.null_indices();
         BJtensor Ep = upper*(1./lower);
         BJtensor Eep =  R - Ep; // elastoplastic constitutive XC::BJtensor

         //std::cerr << "Eep :\n" << Eep;

           //Set Elasto-Plastic stiffness XC::BJtensor
           EP_PredictorEPS.setEep(Eep);
           EP_PredictorEPS.setConverged(true);

     //GZ out //set plastic strain and total strain
           //GZ out incrPlasticStrain = dQods("kl")*Delta_lambda;
           //GZ out incrPlasticStrain.null_indices();
           //GZ out
           //GZ out PlasticStrain = startEPS.getStrain() - startEPS.getElasticStrain();
           //GZ out
           //GZ out PlasticStrain = PlasticStrain + incrPlasticStrain;
           //GZ out
           //GZ out EP_PredictorEPS.setPlasticStrain(PlasticStrain);
           //GZ out
           //GZ out XC::straintensor elastic_strain = strain_increment - incrPlasticStrain;  // elastic strain increment
           //GZ out XC::straintensor estrain = startEPS.getElasticStrain(); //get old elastic strain
           //GZ out //Guanzhou XC::straintensor pstrain = EP_PredictorEPS.getPlasticStrain(); //get old plastic strain
           //GZ out
           //GZ out XC::straintensor tstrain = startEPS.getStrain();        //get old total strain
           //GZ out //Guanzhou pstrain = pstrain + PlasticStrain;
           //GZ out estrain = estrain + elastic_strain;
           //GZ out tstrain = tstrain + strain_increment;
           //GZ out //std::cerr<< "Plastic:  Total strain" << tstrain <<std::endl;
           //GZ out
           //GZ out //Setting de_p, de_e, total plastic, elastic strain, and  total strain
           //GZ out EP_PredictorEPS.setdPlasticStrain( incrPlasticStrain );
           //GZ out EP_PredictorEPS.setdElasticStrain( elastic_strain );
           //GZ out //EP_PredictorEPS.setPlasticStrain( pstrain );
           //GZ out EP_PredictorEPS.setElasticStrain( estrain );
           //GZ out EP_PredictorEPS.setStrain( tstrain );

     backwardEPS = EP_PredictorEPS;

     //double f_backward =  getYS()->f( &backwardEPS );
           //std::cerr << "\n************  Exit Backward = " <<  f_backward << "\n";

  }

  //return Return_stress;
  //Guanzhou out backwardEPS = EP_PredictorEPS;

  //double p = (backwardEPS.getStress()).p_hydrostatic();
  //double ec = (backwardEPS.getec()) - (backwardEPS.getLam()) * log( p / (backwardEPS.getpo()) );
  //double st = (backwardEPS.getStrain()).Iinvariant1();
  //double pl_s = (backwardEPS.getPlasticStrain()).Iinvariant1();
  //double dpl_s = (backwardEPS.getdPlasticStrain()).Iinvariant1();
  //std::cerr << "P BE p=" << p << " ec " << ec << " e " << backwardEPS.gete() << " psi " << (backwardEPS.gete() - ec) << " strain " << st << " t_pl " << pl_s << " d_pl " << dpl_s << "\n";
  //std::cerr << "Backward XC::EPState: \n" << backwardEPS;
  return backwardEPS;
}




//================================================================================
// New XC::EPState using Forward Euler Subincrement Euler Algorithm
//================================================================================
XC::EPState XC::Template3Dep::FESubIncrementation( const XC::straintensor & strain_increment,
                                           int number_of_subincrements)
{

    EPState old_EPS( *(this->getEPS()) );
    EPState FESI_EPS( *(this->getEPS()) );
    //XC::NDMaterial MP( material_point );
    //XC::NDMaterial *MP = this->getCopy();
    //std::cerr << "in FESubIncrementation MP " << MP;

    stresstensor back_stress;
    stresstensor begin_stress = this->getEPS()->getStress();
    //XC::stresstensor begin_stress = start_stress;
    //::fprintf(stderr,"{");

    double sub = 1./( (double) number_of_subincrements );
    //XC::stresstensor elastic_subincremental_stress = stress_increment * sub;

    straintensor tempp = strain_increment;
    straintensor elastic_subincremental_strain(tempp * sub);
    straintensor total_strain = elastic_subincremental_strain;
    //elastic_subincremental_stress.reportshort("SUB INCREMENT in stresses\n");
    //std::cerr << "INCREMENT strain " << strain_increment << std::endl; ;
    //std::cerr << "SUB INCREMENT strain " << elastic_subincremental_strain << std::endl; ;

    for( int steps=0 ; steps < number_of_subincrements ; steps++ ){

        //start_stress.reportshort("START stress\n");
        FESI_EPS = ForwardEulerEPState( elastic_subincremental_strain);

  // Update the XC::EPState in XC::Template3Dep
  this->setEPS( FESI_EPS );

        back_stress = FESI_EPS.getStress();
  //std::cerr.unsetf(ios::showpos);
  //std::cerr << setw(4);
        //std::cerr << "Step No. " << steps << "  ";

  //std::cerr.setPrecision(SCIENTIFIC);
  //std::cerr.precision(3);

  // std::cerr
  // std::cerr.setf(ios::showpos);
  // std::cerr.precision(3);

  //std::cerr << setw(7);
  //std::cerr << "p " << back_stress.p_hydrostatic() << "  ";
  //std::cerr << setw(7);
  //std::cerr << "q " << back_stress.q_deviatoric() << "  ";
  //std::cerr << setw(7);
  //std::cerr << " theta " << back_stress.theta() << "  ";
  //std::cerr << setw(7);
  //std::cerr << "alfa1 " << FESI_EPS.getScalarVar(1) << "  ";
  //std::cerr << setw(7);
  //std::cerr << "f = " << getYS()->f( &FESI_EPS ) << std::endl;

        //begin_stress = back_stress;
        //total_strain = total_strain + elastic_subincremental_strain;

     }

     //    ::fprintf(stderr,"}");
     this->setEPS( old_EPS );
     return FESI_EPS;

}



//================================================================================
// New XC::EPState using Backward Euler Subincrement Euler Algorithm
//================================================================================
XC::EPState XC::Template3Dep::BESubIncrementation( const XC::straintensor & strain_increment,
                                           int number_of_subincrements)
  {
    EPState old_EPS( *(this->getEPS()) );
    EPState BESI_EPS( *(this->getEPS()) );
    straintensor strain_incr = strain_increment;
    //XC::NDMaterial MP( material_point );
    //XC::NDMaterial *MP = getCopy();
    //std::cerr << "in FESubIncrementation MP " << MP;

    stresstensor back_stress;
    stresstensor begin_stress = old_EPS.getStress();
    //XC::stresstensor begin_stress = getEPS()->getStress();
    //XC::stresstensor begin_stress = start_stress;
    //::fprintf(stderr,"{");

    double sub = 1./( (double) number_of_subincrements );
    //XC::stresstensor elastic_subincremental_stress = stress_increment * sub;

    straintensor elastic_subincremental_strain(strain_incr * sub);
    straintensor total_strain = elastic_subincremental_strain;
    //elastic_subincremental_stress.reportshort("SUB INCREMENT in stresses\n");

    for( int steps=0 ; steps < number_of_subincrements ; steps++ ){

        //start_stress.reportshort("START stress\n");
        BESI_EPS = BackwardEulerEPState( elastic_subincremental_strain);
        if( BESI_EPS.getConverged() )
          this->setEPS( BESI_EPS );
  else {
          //this->setEPS( BESI_EPS );
          std::cerr << "Template3Dep::BESubIncrementation  failed to converge at " << steps << "th(of "
     << number_of_subincrements << "step sub-BackwardEuler Algor.\n";
    //exit(1);
          //std::cerr << "Template3Dep::BESubIncrementation  failed to converge using %d step sub-BackwardEuler Algor.", number_of_subincrements ;
    //exit(1);

        //back_stress = BESI_EPS.getStress();
  //std::cerr.unsetf(ios::showpos);
  //std::cerr << setw(4);
        //std::cerr << "Step No. " << steps << "  ";
  //
  //std::cerr.setf(ios::scientific);
  //std::cerr.setf(ios::showpos);
  //std::cerr.precision(3);
  //std::cerr << setw(7);
  //std::cerr << " back-stress  p " << back_stress.p_hydrostatic() << "  ";
  //std::cerr << setw(7);
  //std::cerr << "q " << back_stress.q_deviatoric() << "  ";
  //std::cerr << setw(7);
  //std::cerr << "alfa1 " << BESI_EPS.getScalarVar(1) << "  ";
  //std::cerr << setw(7);
  //std::cerr << "f = " << MP->YS->f( &BESI_EPS ) << "  "<< std::endl;


    //  std::cerr.setf(ios::scientific);
    // std::cerr.setf(ios::showpos);
    // std::cerr.precision(3);
  std::cerr.width(7);
  std::cerr << "\n intraStep: begin-stress  p " << begin_stress.p_hydrostatic() << "  ";
  std::cerr.width(7);
  std::cerr << "q " << begin_stress.q_deviatoric() << "  ";
  std::cerr.width(7);
  std::cerr << "alfa1 " << old_EPS.getScalarVar(1) << "  ";
  //std::cerr.width(7);
  //std::cerr << "f = " << MP->YS->f( &BESI_EPS ) << "  "<< std::endl;

  std::cerr << "strain increment " << strain_incr << std::endl;

  //fflush(std::cerr);
  break;
  //exit(1);
        //begin_stress = back_stress;
        //total_strain = total_strain + elastic_subincremental_strain;
  }

     }

     //    ::fprintf(stderr,"}");
     //this->setEPS( old_EPS );
     return BESI_EPS;

  }



////================================================================================
//// Routine used to generate elastic stiffness XC::BJtensor E
////================================================================================
//BJtensor XC::Template3Dep::ElasticStiffnessTensor( double E, double nu) const
//  {
//    BJtensor ret( 4, def_dim_4, 0.0 );
//
//    BJtensor I2("I", 2, def_dim_2);
//    BJtensor I_ijkl = I2("ij")*I2("kl");
//    I_ijkl.null_indices();
//    BJtensor I_ikjl = I_ijkl.transpose0110();
//    BJtensor I_iljk = I_ijkl.transpose0111();
//    BJtensor I4s = (I_ikjl+I_iljk)*0.5;
//
//    // Building elasticity XC::BJtensor
//    ret = (I_ijkl*((E*nu*2.0)/(2.0*(1.0+nu)*(1-2.0*nu)))) + (I4s*(E/((1.0+nu))));
//
//    return ret;
//  }
//
//
////================================================================================
//// Routine used to generate elastic compliance XC::BJtensor D
////================================================================================
//
//BJtensor XC::Template3Dep::ElasticComplianceTensor( double E, double nu) const
//  {
//    if(E == 0) {
//      std::cerr << std::endl << "Ey = 0! Can't give you D!!" << std::endl;
//      exit(1);
//    }
//
//    BJtensor ret( 4, def_dim_4, 0.0 );
//    //BJtensor ret;
//
//    BJtensor I2("I", 2, def_dim_2);
//    BJtensor I_ijkl = I2("ij")*I2("kl");
//    I_ijkl.null_indices();
//    BJtensor I_ikjl = I_ijkl.transpose0110();
//    BJtensor I_iljk = I_ijkl.transpose0111();
//    BJtensor I4s = (I_ikjl+I_iljk)*0.5;
//
//    // Building compliance XC::BJtensor
//    ret = (I_ijkl * (-nu/E)) + (I4s * ((1.0+nu)/E));
//
//    return ret;
//  }
//

//================================================================================
// trying to find intersection point
// according to M. Crisfield's book
// "Non-linear Finite XC::Element XC::Analysis of Solids and Structures "
// chapter 6.6.1 page 168.
//================================================================================
XC::stresstensor XC::Template3Dep::yield_surface_cross(const XC::stresstensor & start_stress,
                                               const XC::stresstensor & end_stress)
  {
    // Bounds
    double x1 = 0.0;
    double x2 = 1.0;

  // accuracy
  double const TOL = 1.0e-9;
  //std::cerr << "start_stress "<< start_stress;
  //std::cerr << "end_stress " << end_stress;
  //end_stress.reportshortpqtheta("end stress");

  double a = zbrentstress( start_stress, end_stress, x1, x2, TOL ); // Defined below
  // ::printf("\n****\n a = %lf \n****\n",a);

  stresstensor delta_stress(end_stress - start_stress);
  stresstensor intersection_stress(start_stress + stresstensor(delta_stress * a));
  //***  intersection_stress.reportshort("FINAL Intersection stress\n");

  return intersection_stress;

}


//================================================================================
// Routine used by yield_surface_cross to
// find the XC::stresstensor at cross point
//================================================================================

double XC::Template3Dep::zbrentstress(const XC::stresstensor & start_stress,
                                  const XC::stresstensor & end_stress,
                                  double x1, double x2, double tol)
{
  double EPS = d_macheps();

  int iter;
  double a=x1;
  double b=x2;
  double c=0.0;
  double d=0.0;
  double e=0.0;
  double min1=0.0;
  double min2=0.0;
  double fa=func(start_stress, end_stress, a);
  double fb=func(start_stress, end_stress, b);
  //std::cerr << "fb = " << fb;

  double fc=0.0;
  double p=0.0;
  double q=0.0;
  double r=0.0;
  double s=0.0;
  double tol1=0.0;
  double xm=0.0;
  //::printf("\n############# zbrentstress iterations --\n");
  if(fb*fa > 0.0)
    {
      ::printf("\a\nRoot must be bracketed in ZBRENTstress\n");
      ::exit(1);
    }
  fc=fb;
  for( iter=1 ; iter<=ITMAX ; iter++ )
  {
      //::printf("iter No. = %d  ;  b = %16.10lf\n", iter, b);
      if(fb*fc > 0.0)
        {
          c=a;
          fc=fa;
          e=d=b-a;
        }
      if(fabs(fc) < fabs(fb))
        {
          a=b;
          b=c;
          c=a;
          fa=fb;
          fb=fc;
          fc=fa;
        }
      tol1=2.0*EPS*fabs(b)+0.5*tol;
      xm=0.5*(c-b);
      if(fabs(xm) <= tol1 || fb == 0.0) return b;
      if(fabs(e) >= tol1 && fabs(fa) > fabs(fb))
        {
          s=fb/fa;
          if(a == c)
            {
              p=2.0*xm*s;
              q=1.0-s;
            }
          else
            {
              q=fa/fc;
              r=fb/fc;
              p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
              q=(q-1.0)*(r-1.0)*(s-1.0);
            }
          if(p > 0.0)  q = -q;
          p=fabs(p);
          min1=3.0*xm*q-fabs(tol1*q);
          min2=fabs(e*q);
          if(2.0*p < (min1 < min2 ? min1 : min2))
            {
              e=d;
              d=p/q;
            }
          else
            {
              d=xm;
              e=d;
            }
        }
      else
        {
          d=xm;
          e=d;
        }
      a=b;
      fa=fb;
      if(fabs(d) > tol1)
        b += d;
      else
        b += (xm > 0.0 ? fabs(tol1) : -fabs(tol1));
      fb=func(start_stress, end_stress, b);
  }
  //::printf("\a\nMaximum number of iterations exceeded in zbrentstress\n");
  return 0.0; // this just to full the compiler because of the warnings
}


//================================================================================
// routine used by zbrentstress, takes an alfa and returns the
// yield function value for that alfa
//================================================================================
double XC::Template3Dep::func(const stresstensor & start_stress,
                          const stresstensor & end_stress,
                          double alfa )
  {
    //EPState *tempEPS = getEPS()->getCopy();
    EPState tempEPS( (*this->getEPS()) );
    stresstensor delta_stress= end_stress - start_stress;
    stresstensor intersection_stress(start_stress + delta_stress*alfa);

    tempEPS.setStress(intersection_stress);

    //std::cerr << "*tempEPS" << *tempEPS;

    double f = getYS()->f( &tempEPS );
    return f;
  }

//================================================================================
// Overloaded Insertion Operator
// prints an XC::EPState's contents
//================================================================================
std::ostream& XC::operator<<(std::ostream &os, const XC::Template3Dep & MP)
{
    os << std::endl << "Template3Dep: " << std::endl;
    os << "\ttag: " << MP.getTag() << std::endl;
    os << "=================================================================" << std::endl;
    MP.getYS()->print();
    MP.getPS()->print();
    MP.getEPS()->print();

    os << std::endl << "Scalar Evolution Laws: " << std::endl;
    if( MP.ELS1 ){
       os << "\nFor 1st scalar var:\n";
       MP.ELS1->print();
    }

    if( MP.ELS2 ){
       os << "\nFor 2nd scalar var:\n";
       MP.ELS2->print();
    }

    if( MP.ELS3 ){
       os << "\nFor 3rd scalar var:\n";
       MP.ELS3->print();
    }

    if( MP.ELS4 ){
       os << "\nFor 4th scalar var:\n";
       MP.ELS4->print();
    }


    os << std::endl << "Tensorial Evolution Laws: " << std::endl;
    if( MP.ELT1 ){
       os << "\nFor 1st tensorial var:\n";
       MP.ELT1->print();
    }
    if( MP.ELT2 ){
       os << "\nFor 2nd tensorial var:\n";
       MP.ELT2->print();
    }
    if( MP.ELT3 ){
       os << "\nFor 3rd tensorial var:\n";
       MP.ELT3->print();
    }
    if( MP.ELT4 ){
       os << "\nFor 4th tensorial var:\n";
       MP.ELT4->print();
    }

    os << std::endl;
    return os;
}


//Guanzhou added, Apr2005
//EPState XC::Template3Dep::NewBackwardEuler( const XC::straintensor &strain_increment)
//{
//  EPState backwardEPS( *(this->getEPS()) );
//  EPState startEPS( *(this->getEPS()) );
//
//  double tol1 = 1e-8;
//  double tol2 = 1e-8;
//
//  int counter = 0;
//  double Delta_lambda = 0.0;
//
//  BJtensor E  = ElasticStiffnessTensor();
//
//  straintensor strain_incr = strain_increment;
//  straintensor total_strain = backwardEPS.getStrain();
//  total_strain = total_strain + strain_incr;
//  backwardEPS.setStrain(total_strain);//watch out, iteration is carried out with constant total strain!!!
//
//  straintensor plastic_strain = backwardEPS.getPlasticStrain();
//  straintensor trial_incr = total_strain - plastic_strain;
//
//  stresstensor elastic_predictor = E("ijkl") * trial_incr("kl");
//  elastic_predictor.null_indices();
//
//  backwardEPS.setStress(elastic_predictor);
//
//  BJtensor dFods(2, def_dim_2, 0.0);
//  BJtensor dQods(2, def_dim_2, 0.0);
//  BJtensor d2Qoverds2( 4, def_dim_4, 0.0);
//
//  dQods = getPS()->dQods( &backwardEPS );
//
//  straintensor plastic_strain_residual =
//          plastic_strain * (-1.0) + startEPS.getPlasticStrain() + dQods("kl") * Delta_lambda;
//
//  //double hardMod_ = 0.0;
//  Vector h_s(4);
//  //double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
//  stresstensor h_t[4];
//  //XC::stresstensor xi_t[4];
//
//  //BJtensor Hh(2, def_dim_2, 0.0);
//  //Of 1st scalar internal vars
//  if( getELS1() ) {
//          h_s[0]  = getELS1()->h_s( &backwardEPS, getPS());
//               //xi_s[0] = getYS()->xi_s1( &backwardEPS );
//            //hardMod_ = hardMod_ + h_s[0] * xi_s[0];
//  }
//
//  //Of 2nd scalar internal vars
//  if( getELS2() ) {
//          h_s[1]  = getELS2()->h_s( &backwardEPS, getPS());
//               //xi_s[1] = getYS()->xi_s2( &backwardEPS );
//            //hardMod_ = hardMod_ + h_s[1] * xi_s[1];
//  }
//
//  //Of 3rd scalar internal vars
//  if( getELS3() ) {
//          h_s[2]  = getELS3()->h_s( &backwardEPS, getPS());
//               //xi_s[2] = getYS()->xi_s3( &backwardEPS );
//            //hardMod_ = hardMod_ + h_s[2] * xi_s[2];
//  }
//
//  //Of 4th scalar internal vars
//  if( getELS4() ) {
//          h_s[3]  = getELS4()->h_s( &backwardEPS, getPS());
//               //xi_s[3] = getYS()->xi_s4( &backwardEPS );
//            //hardMod_ = hardMod_ + h_s[3] * xi_s[3];
//  }
//
//  //Of tensorial internal var
//  // 1st tensorial var
//  if( getELT1() ) {
//          h_t[0]  = getELT1()->h_t( &backwardEPS, getPS());
//          //xi_t[0] = getYS()->xi_t1( &backwardEPS );
//               //BJtensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
//           //hardMod_ = hardMod_ + hm.trace();
//  }
//
//  // 2nd tensorial var
//  if( getELT2() ) {
//          h_t[1]  = getELT2()->h_t( &backwardEPS, getPS());
//          //xi_t[1] = getYS()->xi_t2( &backwardEPS );
//               //BJtensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
//           //hardMod_ = hardMod_ + hm.trace();
//  }
//
//  // 3rd tensorial var
//  if( getELT3() ) {
//          h_t[2]  = getELT3()->h_t( &backwardEPS, getPS());
//          //xi_t[2] = getYS()->xi_t3( &backwardEPS );
//               //BJtensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
//           //hardMod_ = hardMod_ + hm.trace();
//  }
//
//  // 4th tensorial var
//  if( getELT4() ) {
//          h_t[3]  = getELT4()->h_t( &backwardEPS, getPS());
//          //xi_t[3] = getYS()->xi_t4( &backwardEPS );
//               //BJtensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
//           //hardMod_ = hardMod_ + hm.trace();
//  }
//
//  int NS = backwardEPS.getNScalarVar();
//  int NT = backwardEPS.getNTensorVar();
//
//  Vector internal_variable_residual(NS+NT);
//
//  stresstensor T;
//  stresstensor new_T;
//  stresstensor dT;
//
//  BJtensor
//
//  int ii;
//  for(ii = 1; ii <= NS; ii++) {
//          internal_variable_residual[ii-1] =
//                backwardEPS.getScalarVar(ii)*(-1.0) + startEPS.getScalarVar(ii) +
//                Delta_lambda * h_s[ii-1] ;
//  }
//
//  for(ii = 1; ii <= NT; ii++) {
//          new_T = backwardEPS.getTensorVar(ii)*(-1.0);
//        T = startEPS.getTensorVar(ii);
//          dT = h_t[ii-1] * Delta_lambda;
//        internal_variable_residual[NS+ii-1] =
//                new_T.determinant() + T.determinant() + dT.determinant();
//  }
//
//  double f = getYS()->f( &backwardEPS );
//  double residual = internal_variable_residual.Norm();
//
//  if( (fabs(f) <= tol1) && residual <= tol2 ) {
//
//        straintensor elastic_strain = backwardEPS.getElasticStrain();
//        elastic_strain = elastic_strain + strain_incr;
//          backwardEPS.setElasticStrain(elastic_strain);
//        backwardEPS.setEep(E);
//        backwardEPS.setConverged(true);
//        //if( getELT1() ) {
//        //         getELT1()->updateEeDm(&backwardEPS, -st_vol, 0.0);
//              //}
//        return backwardEPS;
//
//  } else {
//
//        while d2Qoverds2 = getPS()->d2Qods2( &backwardEPS );
//
//          return backwardEPS;
//  }
//
//}
//











