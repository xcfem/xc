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
//============================================================================
# COPYRIGHT (C):     :-))                                                     #
# PROJECT:           Object Oriented Finite XC::Element Program               #
# PURPOSE:           General platform for elaso-plastic constitutive model    #
#                    implementation                                           #
# CLASS:             DPEPState (the base class for all Elasto-plastic state)  #
#                                                                             #
# VERSION:                                                                    #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 ) #
# TARGET OS:         DOS || UNIX || . . .                                     #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                              #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                              #
#                                                                             #
#                                                                             #
# DATE:              08-03-2000                                               #
# UPDATE HISTORY:    May 2004 Guanzhou changed Commit to be consistent        #
#                             with theory                                     #
#		     May 2004, Zhao Cheng splitted the elastic part	      #
#                                                                             #
#                                                                             #
#                                                                             #
# SHORT EXPLANATION: This class is used to hold all state parameters and internal#
#                    variables in an elasto-plastic constitutive model!       #
#                                                                             #
//===========================================================================*/

#include "material/nD/Template3Dep/EPState.h"
#include <iostream>

//! @brief Default constructor
XC::state_vars::state_vars(void)
  : ScalarVar(MaxNScalarVar), TensorVar(MaxNTensorVar)
  {
    Eep= BJtensor( 4, def_dim_4, 0.0 ); // need to be initialized as 4th order BJtensor
  }

//! @brief Constructor
XC::state_vars::state_vars(
                 const stresstensor & Stressp,
                 const straintensor & Strainp,
		 const straintensor & ElasticStrainp)
  : Stress(Stressp), Strain(Strainp), ElasticStrain(ElasticStrainp),
    ScalarVar(MaxNScalarVar), TensorVar(MaxNTensorVar), Eep( 4, def_dim_4, 0.0)
  {}

//! @brief Constructor
XC::state_vars::state_vars(
                 const stresstensor & Stressp,
                 const straintensor & Strainp,
		 const straintensor & ElasticStrainp,
                 const std::vector<double> & Scalarp,
                 const std::vector<stresstensor> & Tensorp,
                 const BJtensor & Eepp)
  : Stress(Stressp), Strain(Strainp), ElasticStrain(ElasticStrainp),
    ScalarVar(Scalarp), TensorVar(Tensorp), Eep(Eepp)
  {}

//! @brief Constructor
XC::state_vars::state_vars(
                 const stresstensor  stressp,
                 const straintensor  strainp,
                 const straintensor  Estrainp,
                 const std::vector<double> &Scalarp,
                 const std::vector<stresstensor> &Tensorp)
  : Stress(stressp), Strain(strainp),
    ElasticStrain(Estrainp), //Guanzhou Mar2005
    ScalarVar(Scalarp), TensorVar(Tensorp)
  {
    Eep= BJtensor( 4, def_dim_4, 0.0 ); // need to be initialized as 4th order BJtensor
  }

//! @brief Constructor
XC::state_vars::state_vars(const std::vector<double> &Scalarp, const std::vector<stresstensor> &Tensorp)
  : ScalarVar(Scalarp), TensorVar(Tensorp)
  {
    Eep= BJtensor( 4, def_dim_4, 0.0 ); // need to be initialized as 4th order BJtensor
  }

const XC::stresstensor &XC::state_vars::getStress(void) const
  { return Stress; }

const XC::straintensor &XC::state_vars::getStrain(void) const
  { return Strain; }

const XC::straintensor &XC::state_vars::getElasticStrain(void) const
  { return ElasticStrain; }

const std::vector<double> &XC::state_vars::getScalarVar(void) const
  { return ScalarVar; }

const std::vector<XC::stresstensor> &XC::state_vars::getTensorVar(void) const
  { return TensorVar; }

const XC::BJtensor &XC::state_vars::getEep(void) const
  { return Eep; }

//! @brief Constructor
XC::trial_state_vars::trial_state_vars(void)
  : state_vars() {}

//! @brief Constructor
XC::trial_state_vars::trial_state_vars(const std::vector<double> &Scalarp, const std::vector<stresstensor> &Tensorp)
  : state_vars(Scalarp,Tensorp) {}

//! @brief Constructor.
XC::trial_state_vars::trial_state_vars(const stresstensor &Stress,
				       const straintensor &Strain,    
				       const straintensor &ElasticStrain,
				       const straintensor &Pstrainp)
  : state_vars(Stress, Strain, ElasticStrain),
    PlasticStrain(Pstrainp) {}

//! @brief Constructor.
XC::trial_state_vars::trial_state_vars(const stresstensor &Stress,
				       const straintensor &Strain,    
				       const straintensor &ElasticStrain,
				       const straintensor &Pstrainp,
				       const straintensor &dEstrainp,
				       const straintensor &dPstrainp,
				       const std::vector<double> &Scalar,
				       const std::vector<stresstensor> &Tensor, 
				       const XC::BJtensor &Eep)
  : state_vars(Stress, Strain, ElasticStrain, Scalar, Tensor, Eep),
    PlasticStrain(Pstrainp), dElasticStrain(dEstrainp),
    dPlasticStrain(dPstrainp) {}

void XC::trial_state_vars::set(const state_vars &initStateVars)
  { state_vars::operator=(initStateVars); }

void XC::trial_state_vars::init(const state_vars &initStateVars)
  {
    set(initStateVars);
    straintensor Strain0;
    ElasticStrain= Strain0;
    PlasticStrain= Strain0;
    dElasticStrain= Strain0;
    dPlasticStrain= Strain0;
  }

//! @brief Normal Constructor 1
XC::EPState::EPState(const trial_state_vars &trialSt,
                 int                  NScalarp,
                 int                  NTensorp,
		 const state_vars &commitSt,
		 const state_vars &initSt,
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
//ZC05/2004: Eo(Eod), E_Young(Ed), nu_Poisson(nu), rho_mass_density(rho),
//ZC05/2004  PlasticStrain(Pstrainp),
//ZC05/2004  dElasticStrain(dEstrainp), dPlasticStrain(dPstrainp), Eep(Eepp),
//ZC05/2004  Converged (Convergedp),
//ZC05/2004  Elasticflag(Elasticflagp),Ev(Evp),nuhv(nuhvp),Ghv(Ghvp),
//ZC05/2004  eo(eop), ec(ecp), Lambda(Lamp), po(pop), e(ep), psi(psip), a(ap)
: NScalarVar(NScalarp), NTensorVar(NTensorp),
  trialStateVars(trialSt), commitStateVars(commitSt), initStateVars(initSt),
  Converged (Convergedp), Delta_lambda(0.0),
  e(ep), psi(psip),
  integratorFlag(flag)
  {
      //Eo               = Eod;
      //E_Young          = Ed;
      //nu_Poisson       = nu;
      //rho_mass_density = rho;
  }


//! @brief Normal Constructor 11
//ZC05/2004EPState::EPState(double              Eod,
//ZC05/2004                 double              Ed,
//ZC05/2004                 double              nu,
//ZC05/2004                 double              rho,
XC::EPState::EPState(const trial_state_vars &trialSt,
                 const straintensor  Pstrainp,
                 int                 NScalarp,
                 int                 NTensorp,
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
//  ElasticStrain(Estrainp),
//  PlasticStrain(Pstrainp),
//  Elasticflag(Elasticflagp),Ev(Evp),nuhv(nuhvp),Ghv(Ghvp),
//  eo(eop), ec(ecp), Lambda(Lamp),po(pop), e(eop), a(ap)
  : NScalarVar(NScalarp), NTensorVar(NTensorp),
    trialStateVars(trialSt),
    commitStateVars(trialSt),
    initStateVars(trialSt)
  {

    //ZC05/2004Eo  = Eod;
    //ZC05/2004E_Young  = Ed;
    //ZC05/2004nu_Poisson  = nu;
    //ZC05/2004rho_mass_density  = rho;

    Delta_lambda = 0.0;

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


    //std::cerr << "stressp " << stressp;
    //Eep = Eepp;

    Converged = false;
    //ZC05/2004     psi = e - ec;
  }


//Normal Constructor 2
XC::EPState::EPState(
           int                 NScalarp,
           const std::vector<double> &Scalarp,
           int                 NTensorp,
           const std::vector<stresstensor> &Tensorp
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
  : NScalarVar(NScalarp), NTensorVar(NTensorp),
    trialStateVars(Scalarp, Tensorp),
    commitStateVars(Scalarp, Tensorp),
    initStateVars(Scalarp, Tensorp)
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

    Converged = false;
    //ZC05/2004      psi = e - ec;
  }

//! @brief Default constructor
XC::EPState::EPState(void)
//ZC05/2004: Eo(30000.0), E_Young(30000.0), nu_Poisson(0.3), rho_mass_density(0.0),
//ZC05/2004  Converged(false),
//ZC05/2004  Elasticflag(0),Ev(0.0),nuhv(0.0),Ghv(0.0),
//ZC05/2004  eo(0.85), ec(0.80), Lambda(0.025), po(100.0), e(0.85), psi(0.05), a(0.5)
  : NScalarVar(MaxNScalarVar),
    NTensorVar(MaxNTensorVar),
    trialStateVars(),
    commitStateVars(),
    initStateVars(),
    Converged(false), e(0.85), psi(0.05) //ZC05/2004    
  {
    //Eo               = 30000.0;
    //E_Young          = 30000.0;
    //nu_Poisson       = 0.3;
    //rho_mass_density = 0.0;
    integratorFlag = 0;//ForwardEuler assumed
    Delta_lambda = 0.0;
  }


//! @brief Virtual constructor.
XC::EPState* XC::EPState::getCopy(void)
  { return new EPState(*this); }

//! @brief Destructor
XC::EPState::~EPState(void)
  {}


//ZC05/2004 
//ZC05/2004 int XC::EPState::getElasticflag(void) const {
//ZC05/2004       return Elasticflag;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getE() const {
//ZC05/2004       return E_Young;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 // Ev: Young's modulus in a vertical direction -- [out-of-plane]
//ZC05/2004 double XC::EPState::getEv() const {
//ZC05/2004       return Ev;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getEo() const {
//ZC05/2004       return Eo;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getnu() const {
//ZC05/2004       return nu_Poisson;
//ZC05/2004 }
//ZC05/2004 
//ZC05/2004 
//ZC05/2004 // nuhv: Poisson's ratio for strain in the vertical direction due to a horizontal direct stress -- [out-of-plane]
//ZC05/2004 double XC::EPState::getnuhv() const {
//ZC05/2004       return nuhv;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 // Ghv: Modulus for shear deformation in a vertical direction plane-- [out-of-plane]
//ZC05/2004 double XC::EPState::getGhv() const {
//ZC05/2004       return Ghv;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getrho() const {
//ZC05/2004       return rho_mass_density;
//ZC05/2004 };


int XC::EPState::getNScalarVar() const
  { return NScalarVar; }


int XC::EPState::getNTensorVar() const
  { return NTensorVar; }


bool XC::EPState::getConverged() const
  { return Converged; }

//ZC05/2004 
//ZC05/2004 double XC::EPState::geteo() const {
//ZC05/2004       return eo;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getec() const {
//ZC05/2004       return ec;
//ZC05/2004 }


double XC::EPState::gete() const
  { return e; }

double XC::EPState::getpsi() const
  { return psi; }

int XC::EPState::getIntegratorFlag() const
  { return integratorFlag; }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getLam() const
//  {
//ZC05/2004       return Lambda;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::getpo() const
//  {
//ZC05/2004       return po;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 double XC::EPState::geta() const
//  {
//ZC05/2004       return a;
//ZC05/2004 }


const XC::stresstensor &XC::EPState::getStress(void) const
  { return trialStateVars.Stress; }


const XC::stresstensor &XC::EPState::getStress_commit(void) const
  { return commitStateVars.Stress; }


const XC::stresstensor &XC::EPState::getStress_init(void) const
  { return initStateVars.Stress; }

const XC::stresstensor &XC::EPState::getIterativeStress() const
  { return trialStateVars.IterativeStress; }


const XC::straintensor &XC::EPState::getStrain() const
  { return trialStateVars.Strain; }


const XC::straintensor &XC::EPState::getStrain_commit() const
  { return commitStateVars.Strain; }


const XC::straintensor &XC::EPState::getStrain_init() const
  { return initStateVars.Strain; }


const XC::straintensor &XC::EPState::getElasticStrain() const
  { return trialStateVars.ElasticStrain; }

//GZ Mar2005
const XC::straintensor &XC::EPState::getElasticStrain_commit() const
  { return commitStateVars.ElasticStrain; }


const XC::straintensor &XC::EPState::getPlasticStrain() const
  { return trialStateVars.PlasticStrain; }


const XC::straintensor &XC::EPState::getdElasticStrain() const
  { return trialStateVars.dElasticStrain; }


const XC::straintensor &XC::EPState::getdPlasticStrain() const
  { return trialStateVars.dPlasticStrain; }


const XC::BJtensor &XC::EPState::getEep() const
  { return trialStateVars.Eep; }


const XC::BJtensor &XC::EPState::getEep_commit() const
  { return commitStateVars.Eep; }


const XC::BJtensor &XC::EPState::getEep_init() const
  { return initStateVars.Eep; }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setElasticflag( int efd ) {
//ZC05/2004       Elasticflag = efd;
//ZC05/2004 }


//ZC05/2004 
//ZC05/2004 void XC::EPState::setEo( double Eod ) {
//ZC05/2004       Eo = Eod;
//ZC05/2004 }


//ZC05/2004 
//ZC05/2004 void XC::EPState::setE( double Ey ) {
//ZC05/2004       E_Young = Ey;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setEv( double Evd ) {
//ZC05/2004       Ev = Evd;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setGhv( double Ghvd ) {
//ZC05/2004       Ghv = Ghvd;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setnu( double nud ) {
//ZC05/2004       nu_Poisson = nud;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setnuhv( double nuhvd ) {
//ZC05/2004       nuhv = nuhvd;
//ZC05/2004 }


void XC::EPState::setStress(const stresstensor &newstress )
  { trialStateVars.Stress = newstress; }


void XC::EPState::setIterativeStress(const stresstensor &newstress )
  { trialStateVars.IterativeStress = newstress; }



void XC::EPState::setStrain(const straintensor &newstrain )
  { trialStateVars.Strain = newstrain; }


void XC::EPState::setStress_commit(const stresstensor &newstress )
  { commitStateVars.Stress = newstress; }


void XC::EPState::setStrain_commit(const straintensor &newstrain )
  { commitStateVars.Strain = newstrain; }

void XC::EPState::setElasticStrain_commit(const straintensor &newstrain )
  { commitStateVars.ElasticStrain = newstrain; }



void XC::EPState::setStress_init(const stresstensor &newstress )
  { initStateVars.Stress = newstress; }


void XC::EPState::setStrain_init(const straintensor &newstrain )
  { initStateVars.Strain = newstrain; }


void XC::EPState::setElasticStrain(const straintensor &newEstrain )
  { trialStateVars.ElasticStrain = newEstrain; }


void XC::EPState::setPlasticStrain(const straintensor &newPstrain )
  { trialStateVars.PlasticStrain = newPstrain; }



void XC::EPState::setdElasticStrain(const straintensor &newdEstrain )
  { trialStateVars.dElasticStrain = newdEstrain; }


void XC::EPState::setdPlasticStrain(const straintensor &newdPstrain )
  { trialStateVars.dPlasticStrain = newdPstrain; }


void XC::EPState::setEep(const BJtensor &newEep )
  { trialStateVars.Eep = newEep; }


void XC::EPState::setConverged( bool b)
  { Converged = b; }

//ZC05/2004 
//ZC05/2004 void XC::EPState::seteo( double eod ) {
//ZC05/2004       eo = eod;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setec( double ecd ) {
//ZC05/2004       ec = ecd;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setLam( double Lamd ) {
//ZC05/2004       Lambda = Lamd;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::setpo( double pod ) {
//ZC05/2004       po = pod;
//ZC05/2004 }

//ZC05/2004 
//ZC05/2004 void XC::EPState::seta( double ad ) {
//ZC05/2004       a = ad;
//ZC05/2004 }


void XC::EPState::sete( double ed )
  { e = ed; }


void XC::EPState::setpsi( double psid )
  { psi = psid; }


// Return the nth Scalar Var.... Starting from 1!!

double XC::EPState::getScalarVar( int WhichOne) const
  {
    if( WhichOne <= getNScalarVar() )
      return trialStateVars.ScalarVar[ WhichOne - 1 ];
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Out of ScalarVar's range "
		  <<  getNScalarVar() << std::endl;
	exit(1);
      }
    return 0.0;
  }



//! @brief Return the nth Tensor Var.... Starting from 1!!
const XC::stresstensor &XC::EPState::getTensorVar(int WhichOne) const
  {
    static stresstensor retval;
    if( WhichOne <= getNTensorVar() )
       return trialStateVars.TensorVar[ WhichOne - 1 ];
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Out of ScalarVar's range "
		  <<  getNScalarVar() << std::endl;
	exit(1);
      }
    return retval;
  }


// Return Scalar pointer

const std::vector<double> &XC::EPState::getScalarVar() const
  { return trialStateVars.ScalarVar; }


void XC::EPState::setNScalarVar(int rval)
  { NScalarVar = rval; }


void XC::EPState::setNTensorVar(int rval)
  { NTensorVar = rval; }


const std::vector<double> & XC::EPState::getScalarVar_commit(void) const
  { return commitStateVars.ScalarVar; }


double XC::EPState::getScalarVar_commit( int i) const
  { return commitStateVars.ScalarVar[i-1]; }


const std::vector<double> & XC::EPState::getScalarVar_init(void) const
  { return initStateVars.ScalarVar; }


double XC::EPState::getScalarVar_init(int i) const
  { return initStateVars.ScalarVar[i]; }


// Return Tensor pointer

const std::vector<XC::stresstensor> &XC::EPState::getTensorVar(void) const
  { return trialStateVars.TensorVar; }


const std::vector<XC::stresstensor> &XC::EPState::getTensorVar_commit() const
  { return commitStateVars.TensorVar; }


const XC::stresstensor &XC::EPState::getTensorVar_commit(int i) const
  { return commitStateVars.TensorVar[i-1]; }


const XC::stresstensor &XC::EPState::getTensorVar_init(int i) const
  { return initStateVars.TensorVar[i-1]; }


const std::vector<XC::stresstensor> & XC::EPState::getTensorVar_init() const
  { return initStateVars.TensorVar; }


// set nth Scalar Var.... Starting from 1!!

void XC::EPState::setScalarVar(int WhichOne, double rval)
  {
    if( WhichOne <= getNScalarVar() )
       trialStateVars.ScalarVar[ WhichOne - 1 ] = rval;
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Out of ScalarVar's range "
		  <<  getNScalarVar() << std::endl;
	exit(1);
      }
  }

void XC::EPState::setScalarVar_commit(int WhichOne, double rval)
  {

      if(WhichOne <= getNScalarVar())
         commitStateVars.ScalarVar[ WhichOne - 1 ] = rval;
      else
        {
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; Out of ScalarVar's range "
		    <<  getNScalarVar() << std::endl;
          exit(1);
        }
  }

void XC::EPState::setScalarVar_init(int WhichOne, double rval)
  {
      if( WhichOne <= getNScalarVar() )
         initStateVars.ScalarVar[ WhichOne - 1 ] = rval;
      else
        {
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; Out of ScalarVar's range "
		    <<  getNScalarVar() << std::endl;
          exit(1);
        }
  }



//! @brief set nth Tensor Var.... Starting from 1!!
void XC::EPState::setTensorVar(int WhichOne, const stresstensor &rval)
  {
      if( WhichOne <= getNTensorVar() )
         trialStateVars.TensorVar[ WhichOne - 1 ] = rval;
      else
        {
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; Out of TensorVars's range "
		    <<  getNScalarVar() << std::endl;
          exit(1);
        }
  }

void XC::EPState::setTensorVar_commit(int WhichOne, const stresstensor &rval)
  {
    if ( WhichOne <= getNTensorVar() )
       commitStateVars.TensorVar[ WhichOne - 1 ] = rval;
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Out of TensorVars's range "
		  <<  getNScalarVar() << std::endl;
	exit(1);
      }
  }

void XC::EPState::setTensorVar_init(int WhichOne, const stresstensor &rval)
  {
    if ( WhichOne <= getNTensorVar() )
      initStateVars.TensorVar[ WhichOne - 1 ] = rval;
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Out of TensorVars's range "
		  <<  getNScalarVar() << std::endl;
	exit(1);
      }
  }

//! @brief set all Scalar Vars ..No boundary checking!
void XC::EPState::setScalarVar(const std::vector<double> &rval)
  { trialStateVars.ScalarVar= rval; }


//! @brief set all Scalar Vars
void XC::EPState::setTensorVar(const std::vector<stresstensor> &rval)
  { trialStateVars.TensorVar= rval; }


void XC::EPState::print()
  { std::cerr << *this; }



//! @brief Set all state variables to initials
void XC::EPState::setInit(void)
  {
    straintensor Strain0;

    trialStateVars.init(initStateVars);

    commitStateVars= initStateVars;
    commitStateVars.ElasticStrain = Strain0;

    Converged = false;
  }

//! @brief Commit the material state.
int XC::EPState::commitState(void)
  {
//      std::cerr << "\n\n\n\n int EPState::commitState ()\n\n\n\n";
    int err = 0;
    // commit the variables state
//Guanzhou out 5-6-2004      CurrentStress   = initStateVars.Stress;
//Guanzhou out 5-6-2004      CurrentStrain   = initStateVars.Strain;
//Guanzhou out 5-6-2004      Eep = initStateVars.Eep;
    commitStateVars= trialStateVars;

    //trialStateVars= initStateVars;
    return err;
  }

//! @brief revert to the last commited state.
int XC::EPState::revertToLastCommit(void)
  {
    int err = 0;
    // revert the variables state  to last committed
    trialStateVars.set(commitStateVars);

    return err;
  }

//! @brief Revert the material to its initial state.
int XC::EPState::revertToStart(void)
  {
    int err = 0;
    // revert the variables state to the initials
    trialStateVars.set(initStateVars);
    commitStateVars= initStateVars;
    //commitStateVars.ElasticStrain = Strain0;
    return err;
  }


//! @brief prints an XC::EPState's contents
std::ostream &XC::operator<<(std::ostream &os, const EPState & EPS)
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

        for (int i = 0; i < NS; i++) {
            os << "\tNo." << i+1 << " " << EPS.trialStateVars.ScalarVar[i] << "; ";
  }
        os << std::endl << std::endl;

        os << "\tNTensorVar = " << NT;
        for(int i = 0; i < NT; i++) {
    //           os.unsetf( ios::showpos);
           os << std::endl << "\tNo." << i+1 << " tensorial var:";
     //           os.setf( ios::showpos);
           os << EPS.trialStateVars.TensorVar[i];
        }

        os << std::endl;
        return os;
    }


