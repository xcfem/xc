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
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite XC::Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
# CLASS:             DPEPState (the base class for all Elasto-plastic state)     #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              08-03-2000                                                  #
# UPDATE HISTORY:    May 2004 Guanzhou changed Commit to be consistent           #
#                             with theory                                        #
#		     May 2004, Zhao Cheng splitted the elastic part	         #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: This class is used to hold all state parameters and internal#
#                    variables in an elasto-plastic constitutive model!          #
#                                                                                #
//================================================================================
*/

#ifndef EPState_CPP
#define EPState_CPP

#include "material/nD/Template3Dep/EPState.h"
#include <iostream>

//XC::stresstensor XC::EPState::TensorVar[ 4 ];
//XC::stresstensor XC::EPState::TensorVar_commit[ 4 ];
//XC::stresstensor XC::EPState::TensorVar_init[ 4 ];

//BJtensor  XC::EPState::Eep( 4, def_dim_4, 0.0 );
//BJtensor  XC::EPState::Eep_commit( 4, def_dim_4, 0.0 );
//BJtensor  XC::EPState::Eep_init( 4, def_dim_4, 0.0 );

//================================================================================
//Normal Constructor 1
//================================================================================

//ZC05/2004 XC::EPState::EPState(double               Eod,
//ZC05/2004                  double               Ed,
//ZC05/2004                  double               nu,
//ZC05/2004                  double               rho,
XC::EPState::EPState(
                 const XC::stresstensor  &stressp,
                 const XC::straintensor  &strainp,
                 const XC::straintensor  &Estrainp,
                 const XC::straintensor  &Pstrainp,
                 const XC::straintensor  &dEstrainp,
                 const XC::straintensor  &dPstrainp,
                 int                  NScalarp,
                 const double       * Scalarp,
                 int                  NTensorp,
                 const XC::stresstensor * Tensorp,
                 const XC::BJtensor       & Eepp,
                 const XC::stresstensor & Stress_commitp,
                 const XC::straintensor & Strain_commitp,
		 const XC::straintensor & ElasticStrain_commitp,
                 const double       * Scalar_commitp,
                 const XC::stresstensor * Tensor_commitp,
                 const XC::BJtensor       & Eep_commitp,
                 const XC::stresstensor & Stress_initp,
                 const XC::straintensor & Strain_initp,
                 const double       * Scalar_initp,
                 const XC::stresstensor * Tensor_initp,
                 const XC::BJtensor       & Eep_initp,
                 bool                 Convergedp,
//ZC05/2004           int                  Elasticflagp,
//ZC05/2004           double         Evp,
//ZC05/2004           double               nuhvp,
//ZC05/2004           double               Ghvp,
//ZC05/2004           double               eop,
//ZC05/2004           double               ecp,
//ZC05/2004           double               Lamp,
//ZC05/2004           double               pop,
                 double               ep,
                 double               psip,
		 int flag )
//ZC05/2004           double          ap
//ZC05/2004     )
//ZC05/2004: Eo(Eod), E_Young(Ed), nu_Poisson(nu), rho_mass_density(rho), CurrentStress(stressp),
//ZC05/2004  CurrentStrain(strainp), ElasticStrain(Estrainp), PlasticStrain(Pstrainp),
//ZC05/2004  dElasticStrain(dEstrainp), dPlasticStrain(dPstrainp), Eep(Eepp),
//ZC05/2004  Stress_commit(Stress_commitp), Strain_commit(Strain_commitp),
//ZC05/2004  Eep_commit(Eep_commitp), Stress_init(Stress_initp), Strain_init(Strain_initp),
//ZC05/2004  Eep_init(Eep_initp), Converged (Convergedp),
//ZC05/2004  Elasticflag(Elasticflagp),Ev(Evp),nuhv(nuhvp),Ghv(Ghvp),
//ZC05/2004  eo(eop), ec(ecp), Lambda(Lamp), po(pop), e(ep), psi(psip), a(ap)
: CurrentStress(stressp),
  CurrentStrain(strainp), ElasticStrain(Estrainp), PlasticStrain(Pstrainp),
  dElasticStrain(dEstrainp), dPlasticStrain(dPstrainp), Eep(Eepp),
  Stress_commit(Stress_commitp), Strain_commit(Strain_commitp), ElasticStrain_commit(ElasticStrain_commitp),
  Eep_commit(Eep_commitp), Stress_init(Stress_initp), Strain_init(Strain_initp),
  Eep_init(Eep_initp), Converged (Convergedp), e(ep), psi(psip), integratorFlag(flag)
{

      //Eo               = Eod;
      //E_Young          = Ed;
      //nu_Poisson       = nu;
      //rho_mass_density = rho;
      Delta_lambda = 0.0;

      NScalarVar = NScalarp;
      //ScalarVar = new double[ NScalarVar ];
      //if ( !ScalarVar ) {
      if( ( !Scalarp ) && (NScalarVar != 0) )
        {
          std::cerr << "XC::EPState::EPState   No initial values for scalar hardening vars, set to aero\n";
          //::exit(1);
        }
      else
        {
         for (int i = 0; i < NScalarVar; i++) {
            //std::cerr << Scalarp[i] << std::endl;
           ScalarVar[i] = Scalarp[i];
           ScalarVar_commit[i] = Scalar_commitp[i];
           ScalarVar_init[i] = Scalar_initp[i];
         }
      }

      NTensorVar = NTensorp;
      //TensorVar = new stresstensor[ NTensorVar ];
      //if ( !TensorVar ) {
      //   std::cerr << "XC::EPState::EPState insufficient memory for XC::Tensor hardening vars";
      //   ::exit(1);
      //}

      if ( (!Tensorp ) && ( NTensorVar)) {
         //ScalarVar = new double[ NScalarVar ];
         //if ( !ScalarVar ) {
  std::cerr << "XC::EPState::EPState   No initial values for tensorial hardening vars, set to zero\n";
         //::exit(1);
      }
      else {
         for (int i = 0; i < NTensorVar; i++) {
            //std::cerr << Tensorp[i];
            TensorVar[i] = Tensorp[i];
            TensorVar_commit[i] = Tensor_commitp[i];
            TensorVar_init[i] = Tensor_initp[i];
            //std::cerr << TensorVar[i];
            //TensorVar[i].null_indices();
         }
      }
}

//================================================================================
//Normal Constructor 11
//================================================================================

//ZC05/XC::2004EPState::EPState(double              Eod,
//ZC05/2004                 double              Ed,
//ZC05/2004                 double              nu,
//ZC05/2004                 double              rho,
XC::EPState::EPState(
                 const XC::stresstensor  stressp,
                 const XC::straintensor  strainp,
                 const XC::straintensor  Estrainp,
                 const XC::straintensor  Pstrainp,
                 int                 NScalarp,
                 const double       *Scalarp,
                 int                 NTensorp,
                 const XC::stresstensor *Tensorp,
		 double             ep,
		 double             psip,
		 int flag )
//ZC05/2004                 int                 Elasticflagp,
//ZC05/2004                 double              Evp,
//ZC05/2004                 double              nuhvp,
//ZC05/2004                 double              Ghvp,
//ZC05/2004                 double              eop,
//ZC05/2004                 double              ecp,
//ZC05/2004                 double              Lamp,
//ZC05/2004                 double              pop,
//ZC05/2004                 double              ap
//ZC05/2004     )
//: Eo(Eod), E_Young(Ed), nu_Poisson(nu), rho_mass_density(rho),
//  CurrentStress(stressp), CurrentStrain(strainp), ElasticStrain(Estrainp),
//  PlasticStrain(Pstrainp), Stress_commit(stressp), Strain_commit(strainp),
//  Stress_init(stressp), Strain_init(strainp),
//  Elasticflag(Elasticflagp),Ev(Evp),nuhv(nuhvp),Ghv(Ghvp),
//  eo(eop), ec(ecp), Lambda(Lamp),po(pop), e(eop), a(ap)
{

//ZC05/2004Eo  = Eod;
//ZC05/2004E_Young  = Ed;
//ZC05/2004nu_Poisson  = nu;
//ZC05/2004rho_mass_density  = rho;

CurrentStress  = stressp;
CurrentStrain  = strainp;
ElasticStrain  = Estrainp;
ElasticStrain_commit = Estrainp;//Guanzhou Mar2005
Delta_lambda = 0.0;

PlasticStrain  = Pstrainp;
Stress_commit  = stressp;
Strain_commit  = strainp;

Stress_init  = stressp;
Strain_init  = strainp;
integratorFlag = flag;//Guanzhou March2005

//ZC05/2004Elasticflag  = Elasticflagp;
//ZC05/2004Ev  = Evp;
//ZC05/2004nuhv  = nuhvp;
//ZC05/2004Ghv  = Ghvp;

//ZC05/2004eo  = eop;
//ZC05/2004ec  = ecp;
//ZC05/2004Lambda  = Lamp;
//ZC05/2004po  = pop;
//ZC05/2004e  = eop;
//ZC05/2004a  = ap;

e = ep;    //ZC
psi = psip;  //ZC

      //Eo               = Eod;
      //E_Young          = Ed;
      //nu_Poisson       = nu;
      //rho_mass_density = rho;

      //CurrentStress    = stressp;

      //std::cerr << "stressp " << stressp;
      //CurrentStress.null_indices();

      //CurrentStrain    =  strainp;
      //ElasticStrain    =  Estrainp;
      //PlasticStrain    =  Pstrainp;
      //Eep = Eepp;
      Eep = BJtensor( 4, def_dim_4, 0.0 ); // need to be initialized as 4th order XC::BJtensor

      //std::cerr << "strainp " << strainp;
      //CurrentStrain.null_indices();

      NScalarVar = NScalarp;
      //ScalarVar = new double[ NScalarVar ];
      //if ( !ScalarVar ) {
      if ( ( !Scalarp ) && (NScalarVar != 0) ) {
  std::cerr << "XC::EPState::EPState   No initial values for scalar hardening vars, set to zero\n";
         //::exit(1);
      }
      else {
         for (int i = 0; i < NScalarVar; i++) {
            //std::cerr << Scalarp[i] << std::endl;
          ScalarVar[i] = Scalarp[i];
          ScalarVar_commit[i] = Scalarp[i];
          ScalarVar_init[i] = Scalarp[i];
         }
      }


      NTensorVar = NTensorp;
      //TensorVar = new stresstensor[ NTensorVar ];
      //if ( !TensorVar ) {
      //   std::cerr << "XC::EPState::EPState insufficient memory for XC::Tensor hardening vars";
      //   ::exit(1);
      //}

      if ( ( !Tensorp ) && ( NTensorVar != 0 ) ) {
         //ScalarVar = new double[ NScalarVar ];
         //if ( !ScalarVar ) {
  std::cerr << "XC::EPState::EPState   No initial values for tensorial hardening vars, set to zero\n";
         //::exit(1);
      }
      else {
         for (int i = 0; i < NTensorVar; i++) {
          //std::cerr << Tensorp[i] << std::endl;
          //std::cerr << TensorVar[i] << std::endl;
          TensorVar[i] = Tensorp[i];
          TensorVar_commit[i] = Tensorp[i];
          TensorVar_init[i] = Tensorp[i];
          //std::cerr << TensorVar[i] << std::endl;
          //TensorVar[i].null_indices();
         }
     }

     Converged = false;
//ZC05/2004     psi = e - ec;
}

//================================================================================
//Normal Constructor 2
//================================================================================

//ZC05/XC::2004EPState::EPState(double              Eod,
//ZC05/2004                 double              Ed,
//ZC05/2004                 double              nu,
//ZC05/2004                 double              rho,
XC::EPState::EPState(
           int                 NScalarp,
           const double       *Scalarp,
           int                 NTensorp,
           const XC::stresstensor *Tensorp
//ZC05/2004           int                 Elasticflagp,
//ZC05/2004           double        Evp,
//ZC05/2004           double              nuhvp,
//ZC05/2004           double              Ghvp,
//ZC05/2004           double              eop,
//ZC05/2004           double              ecp,
//ZC05/2004           double              Lamp,
//ZC05/2004           double              pop,
//ZC05/2004           double         ap
     )
//: Eo(Eod), E_Young(Ed), nu_Poisson(nu), rho_mass_density(rho),
//  Elasticflag(Elasticflagp),Ev(Evp),nuhv(nuhvp),Ghv(Ghvp),
//  eo(eop), ec(ecp), Lambda(Lamp), po(pop), e(eop), a(ap)
{

//ZC05/2004Eo = Eod;
//ZC05/2004E_Young = Ed;
//ZC05/2004nu_Poisson = nu;
//ZC05/2004rho_mass_density = rho;
//ZC05/2004Elasticflag = Elasticflagp;
//ZC05/2004Ev = Evp;
//ZC05/2004nuhv = nuhvp;
//ZC05/2004Ghv = Ghvp;
//ZC05/2004eo = eop;
//ZC05/2004ec = ecp;
//ZC05/2004Lambda = Lamp;
//ZC05/2004po = pop;
//ZC05/2004e = eop;
//ZC05/2004a = a;
      Delta_lambda = 0.0;

      e = 0.85;    //ZC
      psi  = 0.05; //ZC
      integratorFlag = 0;//ForwardEuler assumed

      //Eo               = Eod;
      //E_Young          = Ed;
      //nu_Poisson       = nu;
      //rho_mass_density = rho;

      //CurrentStress    = stressp;
      //std::cerr << "CurrentStress " << CurrentStress;
      //CurrentStress.null_indices();
      //CurrentStrain    =  strainp;
      //ElasticStrain    =  Estrainp;
      //PlasticStrain    =  Pstrainp;
      //dElasticStrain   =  dEstrainp;
      //dPlasticStrain   =  dPstrainp;

      Eep = BJtensor( 4, def_dim_4, 0.0 ); // need to be initialized as 4th order XC::BJtensor
      //std::cerr << "strainp " << strainp;
      //CurrentStrain.null_indices();

      NScalarVar  =  NScalarp;
      //ScalarVar = new double[ NScalarVar ];
      //if ( !ScalarVar ) {
      //   std::cerr << "XC::EPState::EPState insufficient memory for Scalar hardening vars";
      //   ::exit(1);
      //}
      if ( (!Scalarp) && (NScalarVar != 0) ) {
  std::cerr << "XC::EPState::EPState   No initial values for scalar hardening vars, set to zero\n";
         //::exit(1);
      }
      else {
         for (int i = 0; i < NScalarVar; i++) {
            //std::cerr << Scalarp[i] << std::endl;
           ScalarVar[i] = Scalarp[i];
           ScalarVar_commit[i] = Scalarp[i];
           ScalarVar_init[i] = Scalarp[i];
         }
      }

      NTensorVar = NTensorp;
      //TensorVar = new stresstensor[ NTensorVar ];
      //if ( !TensorVar ) {
      //   std::cerr << "XC::EPState::EPState insufficient memory for XC::Tensor hardening vars";
      //   ::exit(1);
      //}
      if ( (!Scalarp) && ( NTensorVar != 0 )) {
         std::cerr << "XC::EPState::EPState   No initial values for tensorial hardening vars, set to zero\n";
         //::exit(1);
      }
      else {
         for (int i = 0; i < NTensorVar; i++) {
           //std::cerr << Tensorp[i];
           TensorVar[i] = Tensorp[i];
           TensorVar_commit[i] = Tensorp[i];
           TensorVar_init[i] = Tensorp[i];
           //std::cerr << TensorVar[i];
           //TensorVar[i].null_indices();
         }
      }

      Converged = false;
//ZC05/2004      psi = e - ec;
}


//================================================================================
//Normal Constructor --no parameters
//================================================================================

XC::EPState::EPState( )
//ZC05/2004: Eo(30000.0), E_Young(30000.0), nu_Poisson(0.3), rho_mass_density(0.0),
//ZC05/2004  Converged(false),
//ZC05/2004  Elasticflag(0),Ev(0.0),nuhv(0.0),Ghv(0.0),
//ZC05/2004  eo(0.85), ec(0.80), Lambda(0.025), po(100.0), e(0.85), psi(0.05), a(0.5)
: Converged(false), e(0.85), psi(0.05) //ZC05/2004

{

      //Eo               = 30000.0;
      //E_Young          = 30000.0;
      //nu_Poisson       = 0.3;
      //rho_mass_density = 0.0;
      Eep = BJtensor( 4, def_dim_4, 0.0 );
      integratorFlag = 0;//ForwardEuler assumed
      Delta_lambda = 0.0;

      NScalarVar = MaxNScalarVar;
      for (int i =0; i < NScalarVar; i++) {
      	 ScalarVar_commit[i] = 0.0;
         ScalarVar[i] = 0.0;
	 ScalarVar_init[i] = 0.0;
      }
      
      NTensorVar = MaxNTensorVar;
      for (int j =0; j < NTensorVar; j++) {
         TensorVar[j] = XC::stresstensor(0.0);
         TensorVar_commit[j] = XC::stresstensor(0.0);
         TensorVar_init[j] = XC::stresstensor(0.0);
      }

      //Converged = false;

}

//================================================================================
//create a clone of itself
//================================================================================

//! @brief Virtual constructor.
XC::EPState* XC::EPState::getCopy(void)
  {

//ZC05/2004      EPState * eps = new  EPState(this->getEo(),
//ZC05/2004                 this->getE(),
//ZC05/2004                 this->getnu(),
//ZC05/2004                 this->getrho(),
      EPState * eps = new  EPState(
                 this->getStress(),
                 this->getStrain(),
                 this->getElasticStrain(),
                 this->getPlasticStrain(),
                 this->getdElasticStrain(),
                 this->getdPlasticStrain(),
                 this->getNScalarVar(),
                 this->getScalarVar(),
                 this->getNTensorVar(),
                 this->getTensorVar(),
                 this->getEep(),
                 this->getStress_commit(),
                 this->getStrain_commit(),
		 this->getElasticStrain_commit(),
                 this->getScalarVar_commit(),
                 this->getTensorVar_commit(),
                 this->getEep_commit(),
                 this->getStress_init(),
                 this->getStrain_init(),
                 this->getScalarVar_init(),
                 this->getTensorVar_init(),
                 this->getEep_init(),
                 this->getConverged(),
//ZC05/2004                 this->getElasticflag(),
//ZC05/2004                 this->getEv(),
//ZC05/2004                 this->getnuhv(),
//ZC05/2004                 this->getGhv(),
//ZC05/2004                 this->geteo(),
//ZC05/2004                 this->getec(),
//ZC05/2004                 this->getLam(),
//ZC05/2004                 this->getpo(),
                 this->gete(),
                 this->getpsi(),
		 this->getIntegratorFlag()
//ZC05/2004                 this->geta()
           );
     eps->Delta_lambda = this->Delta_lambda;
     return eps;
}


//================================================================================
// Copy constructor
//================================================================================
XC::EPState::EPState( const EPState &rhs )
  {

//ZC05/2004       Eo               = rhs.getEo();
//ZC05/2004       E_Young          = rhs.getE();
//ZC05/2004       nu_Poisson       = rhs.getnu();
//ZC05/2004       rho_mass_density = rhs.getrho();
      CurrentStress    = rhs.getStress();
      CurrentStrain    = rhs.getStrain();
      ElasticStrain    = rhs.getElasticStrain();
      PlasticStrain    = rhs.getPlasticStrain();
      dElasticStrain   = rhs.getdElasticStrain();
      dPlasticStrain   = rhs.getdPlasticStrain();

      Stress_commit = rhs.getStress_commit();
      Strain_commit = rhs.getStrain_commit();
      ElasticStrain_commit = rhs.getElasticStrain_commit();
      Stress_init   = rhs.getStress_init();
      Strain_init   = rhs.getStrain_init();
      //std::cerr << Eep.rank() << " ";
      //Eep.printshort("before copy con ");
      Eep = rhs.getEep();
      Eep_commit = rhs.getEep_commit();
      Eep_init = rhs.getEep_init();
      //std::cerr << Eep.rank() << " ";
      //Eep.printshort("after copy con ");

      NScalarVar  =  rhs.getNScalarVar();
      //ScalarVar = new double[ NScalarVar ];
      //if ( !ScalarVar ) {
      //   std::cerr << "XC::EPState::EPState insufficient memory for Scalar hardening vars";
      //   ::exit(1);
      //}
      int i;
      for (i = 0; i < NScalarVar; i++) {
   ScalarVar[i] = rhs.ScalarVar[ i ];
   ScalarVar_commit[i] = rhs.ScalarVar_commit[ i ];
   ScalarVar_init[i] = rhs.ScalarVar_init[ i ];
      }
      NTensorVar = rhs.getNTensorVar();
      //TensorVar = new stresstensor[ NTensorVar ];
      //if ( !TensorVar ) {
      //   std::cerr << "XC::EPState::EPState insufficient memory for XC::Tensor hardening vars";
      //   ::exit(1);
      //}
      for (i = 0; i < NTensorVar; i++) {
   TensorVar[i] = rhs.TensorVar[ i ];
   TensorVar_commit[i] = rhs.TensorVar_commit[ i ];
   TensorVar_init[i] = rhs.TensorVar_init[ i ];
   //std::cerr << TensorVar[i];
    //TensorVar[i].null_indices();
      }

      Converged = rhs.getConverged();

//ZC05/2004       Elasticflag = rhs.getElasticflag();
//ZC05/2004       Ev        = rhs.getEv();
//ZC05/2004       nuhv      = rhs.getnuhv();
//ZC05/2004       Ghv       = rhs.getGhv();

//ZC05/2004       eo        = rhs.geteo();
//ZC05/2004       ec        = rhs.getec();
//ZC05/2004       Lambda    = rhs.getLam();
//ZC05/2004       po        = rhs.getpo();
       e         = rhs.gete();
       psi       = rhs.getpsi();

       Delta_lambda = rhs.Delta_lambda; //Guanzhou

       integratorFlag = rhs.getIntegratorFlag();
//ZC05/2004       a         = rhs.geta();

}

//================================================================================
// Destructor
//================================================================================
XC::EPState::~EPState() {

    //if ( ScalarVar )
    //  delete [] ScalarVar;
    //if ( TensorVar )
    //  delete TensorVar;

    //if ( ScalarVar_commit )
    //  delete [] ScalarVar_commit;
    //if ( TensorVar_commit )
    //  delete TensorVar_commit;

    //if ( ScalarVar_init )
    //  delete [] ScalarVar_init;
    //if ( TensorVar_init )
    //  delete TensorVar_init;

}


//================================================================================
//Overloading the assignment sign
//================================================================================
const XC::EPState &XC::EPState::operator=(const XC::EPState &rhs ) {

      if ( this != &rhs ) {
//ZC05/2004          Eo               = rhs.getEo();
//ZC05/2004          E_Young          = rhs.getE();
//ZC05/2004          nu_Poisson       = rhs.getnu();
//ZC05/2004          rho_mass_density = rhs.getrho();

         CurrentStress    = rhs.getStress();
         //std::cerr << "Current stress " << CurrentStress;
         CurrentStrain    = rhs.getStrain();
         //std::cerr << "strainp " << strainp;
         ElasticStrain    = rhs.getElasticStrain();
         PlasticStrain    = rhs.getPlasticStrain();
         dElasticStrain   = rhs.getdElasticStrain();
         dPlasticStrain   = rhs.getdPlasticStrain();

         Stress_commit = rhs.getStress_commit();
         Strain_commit = rhs.getStrain_commit();
	 ElasticStrain_commit = rhs.getElasticStrain_commit();
         Stress_init   = rhs.getStress_init();
         Strain_init   = rhs.getStrain_init();

         Eep              = rhs.getEep();
         Eep_commit = rhs.getEep_commit();
         Eep_init = rhs.getEep_init();

         NScalarVar  =  rhs.getNScalarVar();
         //ScalarVar = new double[ NScalarVar ];
         //if ( !ScalarVar ) {
         //   std::cerr << "XC::EPState::operator= insufficient memory for Scalar hardening vars";
         //   ::exit(1);
         //}
         int i;
         for (i = 0; i < NScalarVar; i++) {
            ScalarVar[i] = rhs.ScalarVar[i];
            ScalarVar_commit[i] = rhs.ScalarVar_commit[i];
            ScalarVar_init[i] = rhs.ScalarVar_init[i];
         }

         NTensorVar = rhs.getNTensorVar();
         //TensorVar = new stresstensor[ NTensorVar ];
         //if ( !TensorVar ) {
         //   std::cerr << "XC::EPState::operator= insufficient memory for XC::Tensor hardening vars";
         //   ::exit(1);
         //}
         for (i = 0; i < NTensorVar; i++) {
             TensorVar[i] = rhs.TensorVar[i];
             TensorVar_commit[i] = rhs.TensorVar_commit[i];
             TensorVar_init[i] = rhs.TensorVar_init[i];
             //TensorVar[i].null_indices();
         }

         Converged = rhs.getConverged();
	 integratorFlag = rhs.getIntegratorFlag();
 	 Delta_lambda = rhs.Delta_lambda;

//ZC05/2004          Elasticflag = rhs.getElasticflag();
//ZC05/2004          Ev        = rhs.getEv();
//ZC05/2004          nuhv      = rhs.getnuhv();
//ZC05/2004          Ghv       = rhs.getGhv();

//ZC05/2004          eo        = rhs.geteo();
//ZC05/2004          ec        = rhs.getec();
//ZC05/2004          Lambda    = rhs.getLam();
//ZC05/2004          po        = rhs.getpo();
         e         = rhs.gete();
	 psi       = rhs.getpsi();
//ZC05/2004          a         = rhs.geta();

      }

      return *this;

}

//ZC05/2004 //================================================================================
//ZC05/2004 int XC::EPState::getElasticflag(void) const {
//ZC05/2004       return Elasticflag;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getE() const {
//ZC05/2004       return E_Young;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 // Ev: Young's modulus in a vertical direction -- [out-of-plane]
//ZC05/2004 double XC::EPState::getEv() const {
//ZC05/2004       return Ev;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getEo() const {
//ZC05/2004       return Eo;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getnu() const {
//ZC05/2004       return nu_Poisson;
//ZC05/2004 }
//ZC05/2004 
//ZC05/2004 //================================================================================
//ZC05/2004 // nuhv: Poisson's ratio for strain in the vertical direction due to a horizontal direct stress -- [out-of-plane]
//ZC05/2004 double XC::EPState::getnuhv() const {
//ZC05/2004       return nuhv;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 // Ghv: Modulus for shear deformation in a vertical direction plane-- [out-of-plane]
//ZC05/2004 double XC::EPState::getGhv() const {
//ZC05/2004       return Ghv;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getrho() const {
//ZC05/2004       return rho_mass_density;
//ZC05/2004 };

//================================================================================
int XC::EPState::getNScalarVar() const {
      return NScalarVar;
}

//================================================================================
int XC::EPState::getNTensorVar() const {
      return NTensorVar;
}

//================================================================================
bool XC::EPState::getConverged() const {
      return Converged;
}

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::geteo() const {
//ZC05/2004       return eo;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getec() const {
//ZC05/2004       return ec;
//ZC05/2004 }

//================================================================================
double XC::EPState::gete() const {
      return e;
}

//================================================================================
double XC::EPState::getpsi() const {
       return psi;
}

int XC::EPState::getIntegratorFlag() const {
	return integratorFlag;
}

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getLam() const {
//ZC05/2004       return Lambda;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::getpo() const {
//ZC05/2004       return po;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 double XC::EPState::geta() const {
//ZC05/2004       return a;
//ZC05/2004 }

//================================================================================
const XC::stresstensor &XC::EPState::getStress(void) const
  { return CurrentStress; }

//================================================================================
const XC::stresstensor &XC::EPState::getStress_commit(void) const
  { return Stress_commit; }

//================================================================================
const XC::stresstensor &XC::EPState::getStress_init(void) const
  { return Stress_init; }

//================================================================================
const XC::stresstensor &XC::EPState::getIterativeStress() const
  { return IterativeStress; }

//================================================================================
const XC::straintensor &XC::EPState::getStrain() const
  { return CurrentStrain; }


//================================================================================
const XC::straintensor &XC::EPState::getStrain_commit() const
  { return Strain_commit; }

//================================================================================
const XC::straintensor &XC::EPState::getStrain_init() const
  { return Strain_init; }

//================================================================================
const XC::straintensor &XC::EPState::getElasticStrain() const
  { return ElasticStrain; }

//GZ Mar2005
const XC::straintensor &XC::EPState::getElasticStrain_commit() const
  { return ElasticStrain_commit; }

//================================================================================
const XC::straintensor &XC::EPState::getPlasticStrain() const
  { return PlasticStrain; }

//================================================================================
const XC::straintensor &XC::EPState::getdElasticStrain() const
  { return dElasticStrain; }


//================================================================================
const XC::straintensor &XC::EPState::getdPlasticStrain() const
  { return dPlasticStrain; }


//================================================================================
const XC::BJtensor &XC::EPState::getEep() const
  { return Eep; }

//================================================================================
const XC::BJtensor &XC::EPState::getEep_commit() const
  { return Eep_commit; }

//================================================================================
const XC::BJtensor &XC::EPState::getEep_init() const
  { return Eep_init; }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setElasticflag( int efd ) {
//ZC05/2004       Elasticflag = efd;
//ZC05/2004 }


//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setEo( double Eod ) {
//ZC05/2004       Eo = Eod;
//ZC05/2004 }


//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setE( double Ey ) {
//ZC05/2004       E_Young = Ey;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setEv( double Evd ) {
//ZC05/2004       Ev = Evd;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setGhv( double Ghvd ) {
//ZC05/2004       Ghv = Ghvd;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setnu( double nud ) {
//ZC05/2004       nu_Poisson = nud;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setnuhv( double nuhvd ) {
//ZC05/2004       nuhv = nuhvd;
//ZC05/2004 }

//================================================================================
void XC::EPState::setStress(const XC::stresstensor &newstress ) {
      CurrentStress = newstress;
}

//================================================================================
void XC::EPState::setIterativeStress(const XC::stresstensor &newstress ) {
      IterativeStress = newstress;
}


//================================================================================
void XC::EPState::setStrain(const XC::straintensor &newstrain ) {

      CurrentStrain = newstrain;

}

//================================================================================
void XC::EPState::setStress_commit(const XC::stresstensor &newstress ) {
      Stress_commit = newstress;
}

//================================================================================
void XC::EPState::setStrain_commit(const XC::straintensor &newstrain ) {

      Strain_commit = newstrain;

}

void XC::EPState::setElasticStrain_commit(const XC::straintensor &newstrain ) {

      ElasticStrain_commit = newstrain;

}


//================================================================================
void XC::EPState::setStress_init(const XC::stresstensor &newstress ) {
      Stress_init = newstress;
}

//================================================================================
void XC::EPState::setStrain_init(const XC::straintensor &newstrain ) {

      Strain_init = newstrain;

}

//================================================================================
void XC::EPState::setElasticStrain(const XC::straintensor &newEstrain ) {

      ElasticStrain = newEstrain;

}

//================================================================================
void XC::EPState::setPlasticStrain(const XC::straintensor &newPstrain ) {

      PlasticStrain = newPstrain;

}


//================================================================================
void XC::EPState::setdElasticStrain(const XC::straintensor &newdEstrain ) {

      dElasticStrain = newdEstrain;

}

//================================================================================
void XC::EPState::setdPlasticStrain(const XC::straintensor &newdPstrain ) {

      dPlasticStrain = newdPstrain;

}

//================================================================================
void XC::EPState::setEep(const XC::BJtensor &newEep )  {

     Eep = newEep;

}

//================================================================================
void XC::EPState::setConverged( bool b) {
     Converged = b;
}

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::seteo( double eod ) {
//ZC05/2004       eo = eod;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setec( double ecd ) {
//ZC05/2004       ec = ecd;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setLam( double Lamd ) {
//ZC05/2004       Lambda = Lamd;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::setpo( double pod ) {
//ZC05/2004       po = pod;
//ZC05/2004 }

//ZC05/2004 //================================================================================
//ZC05/2004 void XC::EPState::seta( double ad ) {
//ZC05/2004       a = ad;
//ZC05/2004 }

//================================================================================
void XC::EPState::sete( double ed ) {
      e = ed;
}

//================================================================================
void XC::EPState::setpsi( double psid ) {
      psi = psid;
}

//================================================================================
// Return the nth Scalar Var.... Starting from 1!!
//================================================================================
double XC::EPState::getScalarVar( int WhichOne) const {

      if ( WhichOne <= getNScalarVar() )
         return ScalarVar[ WhichOne - 1 ];
      else
      {
  std::cerr << "XC::EPState::getScalarVar Out of ScalarVar's range: " <<  getNScalarVar()  << std::endl;
         return 0.0;
   exit(1);
      }

      return 0.0;

}


//================================================================================
// Return the nth XC::Tensor Var.... Starting from 1!!
//================================================================================
XC::stresstensor XC::EPState::getTensorVar(int WhichOne) const
  {

      XC::stresstensor st;

      if ( WhichOne <= getNTensorVar() )
         return TensorVar[ WhichOne - 1 ];
      else
      {
  std::cerr << "XC::EPState::getTensorVar No. %d: Out of Tensortial Var's range " << WhichOne << " out of " <<  getNTensorVar() << std::endl;
   exit(1);
      }

      return st;
  }

//================================================================================
// Return Scalar pointer
//================================================================================
double * XC::EPState::getScalarVar()
  { return ScalarVar; }

//================================================================================
void XC::EPState::setNScalarVar(int rval) {

      NScalarVar = rval;

}

//================================================================================
void XC::EPState::setNTensorVar(int rval) {

      NTensorVar = rval;

}


//================================================================================
double * XC::EPState::getScalarVar_commit( ) {

      return ScalarVar_commit;

}

//================================================================================
double XC::EPState::getScalarVar_commit( int i) {

      return ScalarVar_commit[i-1];

}

//================================================================================
double * XC::EPState::getScalarVar_init() {

      return ScalarVar_init;

}

//================================================================================
double XC::EPState::getScalarVar_init(int i) {

      return ScalarVar_init[i];

}

//================================================================================
// Return XC::Tensor pointer
//================================================================================
XC::stresstensor * XC::EPState::getTensorVar() {

      return TensorVar;

}

//================================================================================
XC::stresstensor * XC::EPState::getTensorVar_commit() {

      return TensorVar_commit;

}

//================================================================================
XC::stresstensor XC::EPState::getTensorVar_commit(int i) {

      return TensorVar_commit[i-1];

}

//================================================================================
XC::stresstensor XC::EPState::getTensorVar_init(int i) {

      return TensorVar_init[i-1];

}

//================================================================================
XC::stresstensor * XC::EPState::getTensorVar_init() {

      return TensorVar_init;

}

//================================================================================
// set nth Scalar Var.... Starting from 1!!
//================================================================================
void XC::EPState::setScalarVar(int WhichOne, double rval) {

      if ( WhichOne <= getNScalarVar() )
         ScalarVar[ WhichOne - 1 ] = rval;
      else
      {
  std::cerr << "XC::EPState::setScalarVar Out of ScalarVar's range " << getNScalarVar() << std::endl;
         //std::cerr << " Out of ScalarVar's range!";
   exit(1);
      }
}
void XC::EPState::setScalarVar_commit(int WhichOne, double rval) {

      if ( WhichOne <= getNScalarVar() )
         ScalarVar_commit[ WhichOne - 1 ] = rval;
      else
      {
  std::cerr << "XC::EPState::setScalarVar Out of ScalarVar's range " <<  getNScalarVar() << std::endl;
         //std::cerr << " Out of ScalarVar's range!";
   exit(1);
      }
}

void XC::EPState::setScalarVar_init(int WhichOne, double rval) {

      if ( WhichOne <= getNScalarVar() )
         ScalarVar_init[ WhichOne - 1 ] = rval;
      else
      {
  std::cerr << "XC::EPState::setScalarVar Out of ScalarVar's range " <<  getNScalarVar() << std::endl;
         //std::cerr << " Out of ScalarVar's range!";
   exit(1);
      }
}


//================================================================================
// set nth XC::Tensor Var.... Starting from 1!!
//================================================================================
void XC::EPState::setTensorVar(int WhichOne, const XC::stresstensor &rval) {

      if ( WhichOne <= getNTensorVar() )
         TensorVar[ WhichOne - 1 ] = rval;
      else
      {
  std::cerr << "XC::EPState::setTensorVar Out of XC::Tensor Var's range " <<  getNTensorVar() << std::endl;
   exit(1);
      }

}
//================================================================================
void XC::EPState::setTensorVar_commit(int WhichOne, const XC::stresstensor &rval) {

      if ( WhichOne <= getNTensorVar() )
         TensorVar_commit[ WhichOne - 1 ] = rval;
      else
      {
  std::cerr << "XC::EPState::setTensorVar Out of XC::Tensor Var's range " <<  getNTensorVar()<< std::endl;
   exit(1);
      }

}
//================================================================================
void XC::EPState::setTensorVar_init(int WhichOne, const XC::stresstensor &rval) {

      if ( WhichOne <= getNTensorVar() )
         TensorVar_init[ WhichOne - 1 ] = rval;
      else
      {
  std::cerr << "XC::EPState::setTensorVar Out of XC::Tensor Var's range " <<  getNTensorVar() << std::endl;
   exit(1);
      }

}


//================================================================================
// set all Scalar Vars ..No boundary checking!
//================================================================================
void XC::EPState::setScalarVar(double *rval) {

      if ( !rval ) {
  std::cerr << "XC::EPState::setScalarVar No scalar vars supplied\n";
         ::exit(1);
      }
      for (int i = 0; i < getNScalarVar(); i++) {
         //std::cerr << Scalarp[i] << std::endl;
   ScalarVar[i] = rval[i];
      }

}

//================================================================================
// set all Scalar Vars
//================================================================================
void XC::EPState::setTensorVar(const XC::stresstensor *rval) {

      if ( !rval ) {
         std::cerr << "XC::EPState::setTensorVar No tensorial vars supplied\n";
         ::exit(1);
      }
      for (int i = 0; i < getNTensorVar(); i++) {
   TensorVar[i] = rval[i];
    TensorVar[i].null_indices();
      }

}

//================================================================================
void XC::EPState::print() {
      std::cerr << *this;

}


//================================================================================
// Set all state variables to initials

void XC::EPState::setInit() {

      XC::stresstensor Stress0;
      XC::straintensor Strain0;

      CurrentStress   = Stress_init;
      CurrentStrain   = Strain_init;
      ElasticStrain   = Strain0;
      PlasticStrain   = Strain0;
      dElasticStrain  = Strain0;
      dPlasticStrain  = Strain0;
      Eep = Eep_init;

      Stress_commit   = Stress_init;
      Strain_commit   = Strain_init;
      Eep_commit = Eep_init;
      ElasticStrain_commit = Strain0;

    int i;
      for (i = 0; i < NScalarVar; i++) {
          ScalarVar[i] = ScalarVar_init[i];
          ScalarVar_commit[i] = ScalarVar_init[i];
      }

      for (i = 0; i < NTensorVar; i++) {
         TensorVar[i] = TensorVar_init[i];
         TensorVar_commit[i] = TensorVar_init[i];
      }

      Converged = false;
}

//================================================================================
int XC::EPState::commitState ()
  {


//      std::cerr << "\n\n\n\n int XC::EPState::commitState ()\n\n\n\n";


      int err = 0;
      // commit the variables state
//Guanzhou out 5-6-2004      CurrentStress   = Stress_init;
//Guanzhou out 5-6-2004      CurrentStrain   = Strain_init;
//Guanzhou out 5-6-2004      Eep = Eep_init;

      Stress_commit   = CurrentStress;
      Strain_commit   = CurrentStrain;
      Eep_commit = Eep;


      //GZ Mar2005
      ElasticStrain_commit = ElasticStrain;

    int i;
      for (i = 0; i < NScalarVar; i++) {
          //ScalarVar[i] = ScalarVar_init[i];
          ScalarVar_commit[i] = ScalarVar[i];
      }

      for (i = 0; i < NTensorVar; i++) {
         //TensorVar[i] = TensorVar_init[i];
         TensorVar_commit[i] = TensorVar[i];
      }

      return err;

  }

//================================================================================
int XC::EPState::revertToLastCommit () {
      int err = 0;
      // revert the variables state  to last committed
      CurrentStress   = Stress_commit;
      CurrentStrain   = Strain_commit;
      Eep = Eep_commit;

      ElasticStrain = ElasticStrain_commit;

    int i;
      for (i = 0; i < NScalarVar; i++) {
          //ScalarVar[i] = ScalarVar_init[i];
          ScalarVar[i] = ScalarVar_commit[i];
      }

      for (i = 0; i < NTensorVar; i++) {
         //TensorVar[i] = TensorVar_init[i];
         TensorVar[i] = TensorVar_commit[i];
      }

      return err;

}

//================================================================================
int XC::EPState::revertToStart () {

      int err = 0;
      // revert the variables state to the initials
      CurrentStress   = Stress_init;
      CurrentStrain   = Strain_init;
      Eep = Eep_init;

      Stress_commit   = Stress_init;
      Strain_commit   = Strain_init;
      Eep_commit = Eep_init;
      //ElasticStrain_commit = Strain0;

    int i;
      for (i = 0; i < NScalarVar; i++) {
          ScalarVar[i] = ScalarVar_init[i];
          ScalarVar_commit[i] = ScalarVar_init[i];
      }

      for (i = 0; i < NTensorVar; i++) {
         TensorVar[i] = TensorVar_init[i];
         TensorVar_commit[i] = TensorVar_init[i];
      }

      return err;
}





//================================================================================
// Overloaded Insertion Operator
// prints an XC::EPState's contents
//================================================================================
std::ostream &XC::operator<<(std::ostream &os, const XC::EPState & EPS)
//ostream & operator<<(ostream &os, const XC::EPState & EPS)
    {
      //        os.setf( ios::showpos | ios::scientific);
      os.precision(4);
        os.width(10);
//ZC05/2004         os << std::endl << "Elastic plastic state parameters: "  << std::endl;
//ZC05/2004 
//ZC05/2004   int ef = EPS.getElasticflag();
//ZC05/2004   os << "\tElastic Flag = " << ef << ";";
//ZC05/2004   if (ef == 1)
//ZC05/2004      os << " pressure dependent isotropic material (default case, for soil)." << std::endl;
//ZC05/2004   else if (ef == 2)
//ZC05/2004      os << " pressure independent isotropic material." << std::endl;
//ZC05/2004   else if (ef == 3)
//ZC05/2004      os << " pressure independent cross-anisotropic material." << std::endl;
//ZC05/2004   else if (ef == 4)
//ZC05/2004      os << " pressure dependent cross-anisotropic material." << std::endl;
//ZC05/2004   else
//ZC05/2004      os << " elastic portion code not correct. Flag must be 1, 2, 3 or 4." << std::endl;


//ZC05/2004         os << "\tEo = " << EPS.getEo() << ";";
//ZC05/2004         os << " E_Young = " << EPS.getE() << ";";
//ZC05/2004         //os.width(10);
//ZC05/2004   os << " nu_Poisson = " << EPS.getnu() << ";";
//ZC05/2004         os << " \tE_v = " << EPS.getEv() << ";";
//ZC05/2004         os << " nu_hv = " << EPS.getnuhv() << ";";
//ZC05/2004   os << " G_hv = " << EPS.getGhv() << ";";
//ZC05/2004   os << " rho = " << EPS.getrho() << std::endl;
//ZC05/2004 
//ZC05/2004         os << "\teo = " << EPS.geteo() << ";";
//ZC05/2004         os << " ec = " << EPS.getec() << ";";
//ZC05/2004         os << " Lambda = " << EPS.getLam() << ";";
//ZC05/2004         os << " po = " << EPS.getpo() << ";";
  os << " e = " << EPS.gete() << std::endl;
  os << " psi = " << EPS.getpsi() << std::endl;
//ZC05/2004         os << " a = " << EPS.geta() << ";";

  if ( EPS.getConverged() )
     os << "\tConverged = ok! ";
  else
     os << "\tConverged = no! " << std::endl;

        //os.width(10);
        os << std::endl << "\tCurrent Stress:" << EPS.getStress() << std::endl;
        os << "\tIterati Stress:" << EPS.getIterativeStress() << std::endl;

  os << "\tCurrent Strain:" << EPS.getStrain() << std::endl;
  os << "\tElasticStrain :" << EPS.getElasticStrain() << std::endl;
  os << "\tPlasticStrain :" << EPS.getPlasticStrain() << std::endl;
  os << "\tdElasticStrain:" << EPS.getdElasticStrain() << std::endl;
  os << "\tdPlasticStrain:" << EPS.getdPlasticStrain() << std::endl;
  os << "\tEep.rank():" << EPS.getEep().rank() << std::endl;

  //        os.unsetf( ios::showpos );
  int NS = EPS.getNScalarVar();
  int NT = EPS.getNTensorVar();

  os << std::endl << "\tNScalarVar = " << NS << std::endl;

    int i;
        for (i = 0; i < NS; i++) {
            os << "\tNo." << i+1 << " " << EPS.ScalarVar[i] << "; ";
  }
        os << std::endl << std::endl;

        os << "\tNTensorVar = " << NT;
        for (i = 0; i < NT; i++) {
    //           os.unsetf( ios::showpos);
           os << std::endl << "\tNo." << i+1 << " tensorial var:";
     //           os.setf( ios::showpos);
           os << EPS.TensorVar[i];
        }

        os << std::endl;
        return os;
    }

#endif

