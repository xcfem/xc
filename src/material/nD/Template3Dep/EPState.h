// -*-c++-*-
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
# PROJECT:           Object Oriented Finite Element Program                    #
# PURPOSE:           General platform for elaso-plastic constitutive model     #
#                    implementation                                            #
# CLASS:             EPState (the base class for all Elasto-plastic state)     #
#                                                                              #
# VERSION:                                                                     #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
# TARGET OS:         DOS || UNIX || . . .                                      #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                               #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
#                                                                              #
#                                                                              #
# DATE:              08-03-2000                                                #
# UPDATE HISTORY:                                                              #
#		     May 2004, Zhao Cheng splitting the elastic part            #
#                                                                              #
#                                                                              #
#                                                                              #
#                                                                              #
# SHORT EXPLANATION: This class is used to hold all state parameters and       #
#                    internal variables in an elasto-plastic constitutive      #
#                    model.                                                    #
################################################################################
*/

#ifndef EPState_H
#define EPState_H

#include "utility/matrix/nDarray/stresst.h"
#include "utility/matrix/nDarray/straint.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include "utility/kernel/CommandEntity.h"

// Constants
#define MaxNScalarVar 4
#define MaxNTensorVar 4

namespace XC {
//! @ingroup EPMat3D
//!
//! @brief Base class for all elasto-plastic states.
//! 
//! This class is used to hold all state parameters and
//! internal variables in an elasto-plastic constitutive model. 
class EPState: public CommandEntity
  {
  public:
//ZC05/2004    // Elastic parameters
//ZC05/2004    double Eo;                    // Young's modulus when p = p_atmosphere  -- [in-plane] in terms of cross-anisotropic material
//ZC05/2004    double E_Young;            // Young's modulus -- [in-plane] in terms of cross-anisotropic material
//ZC05/2004    double nu_Poisson;            // Poisson's ratio -- [in-plane] in terms of cross-anisotropic materi          
//ZC05/2004    double rho_mass_density;      // Mass density               
            
    // Trial state
    stresstensor CurrentStress;   // Current trial stress  --total          
    straintensor CurrentStrain;    // Current trial strain  --total       
    stresstensor IterativeStress; // Iterative Stress           
    straintensor ElasticStrain;    // Current cumulative elastic strain      
    straintensor PlasticStrain;    // Current cumulative plastic strain      
    straintensor dElasticStrain;  // Current elastic strain increment       
    straintensor dPlasticStrain;  // Current plastic strain increment       
    BJtensor Eep;             // Current Elastic plastic stifness tensor
    
    int NScalarVar;               //Actual Number of internal scalar vars 
    std::vector<double> ScalarVar; // scalar variable array for scalar hardening vars 

    int NTensorVar;               //Actual Number of internal tensor vars 
    std::vector<stresstensor> TensorVar; // tensor variable array for tensor hardening vars 

    // Committed state
    stresstensor Stress_commit;   // Committed stress  --total             
    straintensor Strain_commit;   // Committed strain  --total             
    straintensor ElasticStrain_commit; //GZ Mar2005
    //straintensor PlasticStrain


    std::vector<double> ScalarVar_commit; // Committed scalar variable array for scalar hardening vars 
    std::vector<stresstensor> TensorVar_commit; // Committed tensor variable array for tensor hardening vars 
    BJtensor Eep_commit;      // Current Elastic plastic stifness tensor

    //Initial state
    stresstensor Stress_init;     // Initial stress  --total
    straintensor Strain_init;     // Initial strain  --total
    
    std::vector<double> ScalarVar_init; // initial scalar variable array for scalar hardening vars 
    std::vector<stresstensor> TensorVar_init; // initial tensor variable array for tensor hardening vars 
    BJtensor Eep_init;             // initial Elastic plastic stifness tensor

    bool Converged;      // Bool to indicate whether this is the converged EPState by current CDriver
    
    double Delta_lambda; //Guanzhou Mar2005

//ZC05/2004    // Flag to indicate if elastic portion is pressure dependent isotropic, pressure independent isotropic, pressure 
//ZC05/2004    // independent cross-anisotropic or pressure dependentcross-anisotropic 
//ZC05/2004    // 1 == pressure dependent isotropic (default case, for soil)
//ZC05/2004    // 2 == pressure independent isotropic
//ZC05/2004    // 3 == pressure independent cross-anisotropic
//ZC05/2004    // 4 == pressure dependent cross-anisotropic
//ZC05/2004
//ZC05/2004    int Elasticflag;
//ZC05/2004
//ZC05/2004    //Parameters for elastic cross-anistropic material
//ZC05/2004    double Ev;            // Ev: Young's modulus in a vertical direction -- [out-of-plane]
//ZC05/2004    double nuhv;   // nuhv: Poisson's ratio for strain in the vertical direction due to a horizontal direct stress -- [out-of-plane]
//ZC05/2004    double Ghv;          // Ghv: Modulus of shear deformation in a vertical plane -- [out-of-plane]
//ZC05/2004
//ZC05/2004    double eo;           // Initial void ratio Joey 02-11-03
//ZC05/2004    double ec;           // Void ratio at critical state at po Joey 02-11-03
//ZC05/2004    double Lambda;       // Slope of critical state line  Joey 02-11-03
//ZC05/2004    double po;           // Reference pressure (100 kPa by default)  Joey 02-11-03
    double e;            // Void ratio  Joey 02-11-03
    double psi;          // State parameter  Joey 02-18-03
    int integratorFlag; //Guanzhou Mar2005
//ZC05/2004    double a;            // Exponent in E = Eo (p/p_atm)^a for nonlinear elastic model Joey 02-11-03
    
  public:
    //Normal Constructor--no parameters
    EPState(void);
    ~EPState(void);
    
//ZC05/2004     //Normal Constructor1
//ZC05/2004     EPState(double              Eod,
//ZC05/2004             double              Ed,
//ZC05/2004             double              nu,
//ZC05/2004              double              rho,
//ZC05/2004             const stresstensor &stressp,
//ZC05/2004             const straintensor &strainp,
//ZC05/2004             const straintensor &Estrainp,
//ZC05/2004             const straintensor &Pstrainp,
//ZC05/2004             const straintensor &dEstrainp,
//ZC05/2004             const straintensor &dPstrainp,
//ZC05/2004             int                 NScalarp,
//ZC05/2004              const std::vector<double> &Scalarp,
//ZC05/2004             int                 NTensorp,
//ZC05/2004              const std::vector<stresstensor> &Tensorp,
//ZC05/2004              const tensor       &Eepp,
//ZC05/2004           const stresstensor &Stress_commitp,
//ZC05/2004           const straintensor &Strain_commitp,    
//ZC05/2004           const std::vector<double> &Scalar_commitp,
//ZC05/2004           const std::vector<stresstensor> &Tensor_commitp, 
//ZC05/2004           const tensor       &Eep_commitp,
//ZC05/2004           const stresstensor &Stress_initp,    
//ZC05/2004           const straintensor &Strain_initp,   
//ZC05/2004           const const std::vector<double> &Scalar_initp,
//ZC05/2004           const std::vector<stresstensor> &Tensor_initp,
//ZC05/2004           const tensor       &Eep_initp, 
//ZC05/2004           bool                Convergedp,
//ZC05/2004       int                 Elasticflagp = 0,
//ZC05/2004       double              Evp   = 0.0,
//ZC05/2004       double              nuhvp = 0.0,
//ZC05/2004       double              Ghvp = 0.0,
//ZC05/2004       double              eop = 0.85, 
//ZC05/2004       double              ecp = 0.80, 
//ZC05/2004       double              Lam = 0.025, 
//ZC05/2004       double              pop = 100.0,
//ZC05/2004       double              ep  = 0.85, 
//ZC05/2004       double              psip = 0.05, 
//ZC05/2004       double              ap  = 0.5
//ZC05/2004       );
//ZC05/2004 
//ZC05/2004 
//ZC05/2004     //Normal Constructor11
//ZC05/2004     EPState(double              Eod,
//ZC05/2004             double              Ed,
//ZC05/2004             double              nu,
//ZC05/2004             double              rho,
//ZC05/2004             const stresstensor  stressp,       
//ZC05/2004             const straintensor  strainp, 
//ZC05/2004             const straintensor  Estrainp,
//ZC05/2004             const straintensor  Pstrainp,
//ZC05/2004       int                 NScalarp,
//ZC05/2004       const std::vector<double> &Scalarp,
//ZC05/2004       int                 NTensorp,
//ZC05/2004       const std::vector<stresstensor> &Tensorp, 
//ZC05/2004       int                 Elasticflagp = 0,
//ZC05/2004       double     Evp   = 0.0,
//ZC05/2004       double              nuhvp = 0.0,
//ZC05/2004       double              Ghvp = 0.0,
//ZC05/2004       double              eop = 0.85,
//ZC05/2004       double              ecp = 0.80,
//ZC05/2004       double              Lam = 0.025, 
//ZC05/2004       double              pop = 100.0,
//ZC05/2004       double              ap = 0.5
//ZC05/2004       );
//ZC05/2004 
//ZC05/2004     //Normal Constructor2
//ZC05/2004     EPState(double              Eod,    
//ZC05/2004             double              Ed,
//ZC05/2004             double              nu,
//ZC05/2004       double              rho,
//ZC05/2004             int                 NScalarp,
//ZC05/2004       const std::vector<double> &Scalarp,
//ZC05/2004       int                 NTensorp,
//ZC05/2004       const std::vector<stresstensor> &Tensorp,
//ZC05/2004       int                 Elasticflagp = 0,
//ZC05/2004       double     Evp   = 0.0,
//ZC05/2004       double              nuhvp = 0.0,
//ZC05/2004       double              Ghvp = 0.0,
//ZC05/2004       double              eop = 0.85,
//ZC05/2004       double              ecp = 0.80,
//ZC05/2004       double              Lam = 0.025, 
//ZC05/2004       double              pop = 100.0,
//ZC05/2004       double              ap = 0.5
//ZC05/2004       );

    //Normal Constructor1
    EPState(const stresstensor &stressp,
            const straintensor &strainp,
            const straintensor &Estrainp,
            const straintensor &Pstrainp,
            const straintensor &dEstrainp,
            const straintensor &dPstrainp,
            int                 NScalarp,
            const std::vector<double> &Scalarp,
            int                 NTensorp,
            const std::vector<stresstensor> &Tensorp,
            const BJtensor     &Eepp,
            const stresstensor &Stress_commitp,
            const straintensor &Strain_commitp,    
            const straintensor & ElasticStrain_commitp,
	    const std::vector<double> &Scalar_commitp,
            const std::vector<stresstensor> &Tensor_commitp, 
            const BJtensor       &Eep_commitp,
            const stresstensor &Stress_initp,    
            const straintensor &Strain_initp,   
            const std::vector<double> &Scalar_initp,
            const std::vector<stresstensor> &Tensor_initp,
            const BJtensor       &Eep_initp, 
            bool                Convergedp, 
	    double              ep  = 0.85,
	    double              psip = 0.05,
	    int flag = 0 );

    //Normal Constructor11
    EPState(const stresstensor  stressp,       
            const straintensor  strainp, 
            const straintensor  Estrainp,
            const straintensor  Pstrainp,
            int                 NScalarp,
            const std::vector<double> &Scalarp,
            int                 NTensorp,
            const std::vector<stresstensor> &Tensorp, 
	    double              ep = 0.85,
	    double              psip = 0.05, 
	    int flag = 0); //Guanzhou

    //Normal Constructor2
    EPState(int                 NScalarp,
            const std::vector<double> &Scalarp,
            int                 NTensorp,
            const std::vector<stresstensor> &Tensorp );

    EPState *getCopy(void); //create a clone of itself

//ZC05/2004    int getElasticflag() const;

//ZC05/2004    double getE() const;
//ZC05/2004    double getEo() const;
//ZC05/2004    double getEv() const;
//ZC05/2004    double getnu() const;
//ZC05/2004    double getnuhv() const;
//ZC05/2004    double getGhv() const;
//ZC05/2004    double getrho() const;

    int getNScalarVar() const;
    int getNTensorVar() const;
    bool getConverged() const;

//ZC05/2004    // Added Joey 02-12-03
//ZC05/2004    double geteo() const;
//ZC05/2004    double getec() const;
//ZC05/2004    double getLam() const;
    double gete() const;
    double getpsi() const; //state parameter
    int getIntegratorFlag() const; //Guanzhou Mar 2005
//ZC05/2004    double getpo() const;
//ZC05/2004    double geta() const;

    const stresstensor &getStress(void) const;
    const stresstensor &getIterativeStress() const;
    const straintensor &getStrain() const;
    const straintensor &getElasticStrain() const;
    const straintensor &getPlasticStrain() const;
    const straintensor &getdElasticStrain() const;
    const straintensor &getdPlasticStrain() const;
    const BJtensor &getEep(void) const;

    //Get committed state vars
    const stresstensor &getStress_commit() const;
    const straintensor &getStrain_commit() const;
    const straintensor &getElasticStrain_commit() const;
    const std::vector<double> &getScalarVar_commit() const;
    double getScalarVar_commit(int i) const;
    const std::vector<stresstensor> &getTensorVar_commit() const;
    stresstensor getTensorVar_commit(int i) const;
    const BJtensor &getEep_commit() const;

    //Get initial state vars
    const stresstensor &getStress_init() const;
    const straintensor &getStrain_init() const;
    const std::vector<double> & getScalarVar_init() const;
    double getScalarVar_init(int i) const;
    const std::vector<stresstensor> &getTensorVar_init() const;
    stresstensor getTensorVar_init(int i) const;
    const BJtensor &getEep_init() const;

//ZC05/2004    void setElasticflag( int efd );
//ZC05/2004    void setEo( double Eod );
//ZC05/2004    void setE( double Ey );
//ZC05/2004    void setnu( double nud );

//ZC05/2004    void setEv( double Evd );
//ZC05/2004    void setGhv( double Ghvd );
//ZC05/2004    void setnuhv( double nud );

    void setStress( const stresstensor &newstress );
    void setIterativeStress( const stresstensor &newstress );
    void setStrain( const straintensor &newstrain );

    void setStress_commit( const stresstensor &newstress );
    void setStrain_commit( const straintensor &newstrain );
    void setElasticStrain_commit( const straintensor &newstrain );

    void setStress_init( const stresstensor &newstress );
    void setStrain_init( const straintensor &newstrain );

    void setElasticStrain( const straintensor &newstrain );
    void setPlasticStrain( const straintensor &newstrain );
    void setdElasticStrain( const straintensor &newstrain );
    void setdPlasticStrain( const straintensor &newstrain );
    void setEep(const BJtensor &);
    void setConverged( bool b);

//ZC05/2004    // Added Joey 02-12-03
//ZC05/2004    void seteo( double eod );
//ZC05/2004    void setec( double ecd );
//ZC05/2004    void setLam( double Lamd );
    void sete( double ed );
    void setpsi( double psid );
//ZC05/2004    void setpo( double pod );
//ZC05/2004    void seta( double ad );

    // WhichOne starts from 1 and ends up to  NScalarVar
    double getScalarVar( int WhichOne) const;
    stresstensor getTensorVar(int WhichOne) const;

    // return the pointers
    const std::vector<double> &getScalarVar() const;
    const std::vector<stresstensor> &getTensorVar() const;

    // WhichOne starts from 1 and ends up to NTensorVar
    void setNScalarVar(int rval);
    void setScalarVar(int WhichOne, double rval);
    void setScalarVar_commit(int WhichOne, double rval);
    void setScalarVar_init(int WhichOne, double rval);

    void setNTensorVar(int rval);
    void setTensorVar(int WhichOne, const stresstensor &rval);
    void setTensorVar_commit(int WhichOne, const stresstensor &rval);
    void setTensorVar_init(int WhichOne, const stresstensor &rval);

    void setScalarVar(const std::vector<double> &rval);
    void setTensorVar(const std::vector<stresstensor> &rval);
    void setInit();

    //added for OpenSees _Zhaohui 02-10-2000
    int commitState ();
    int revertToLastCommit ();
    int revertToStart ();

    void print();

    //================================================================================
    // Overloaded Insertion Operator
    // prints an EPState's contents 
    //================================================================================
    friend std::ostream & operator<<(std::ostream &, const EPState &);
  };
std::ostream & operator<<(std::ostream &, const EPState &);
} // end of XC namespace


#endif

