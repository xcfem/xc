// -*-c++-*-
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
/*
################################################################################
# COPYRIGHT (C):     :-))                                                      #
# PROJECT:           Object Oriented Finite Element Program                    #
# PURPOSE:           General platform for elaso-plastic constitutive model     #
#                    implementation                                            #
# CLASS:             Template3Dep (the base class for all material point)      #
#                                                                              #
# VERSION:                                                                     #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
# TARGET OS:         DOS || UNIX || . . .                                      #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                               #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
#                                                                              #
#                                                                              #
# DATE:              08-03-2000                                                #
# UPDATE HISTORY:    09-12-2000                                                #
#       May 2004, Zhao Cheng splitting the elastic part        #
#                                                                              #
#                                                                              #
# SHORT EXPLANATION: The Template3Dep class is used to hold specific           #
#                    yield surface, potential surface, Evolution law(s)        #
#                    and EPState of a 3D elasto-plastic material model for one #
#                    gauss point!! It is worthwhile noting that one model may  #
#                    have multiple evolution law.  Each evlotuion law is       #
#                    used to evolve one internal var.                          #
#                                                                              #
#                                                                              #
################################################################################
*/

#ifndef Template3Dep_H
#define Template3Dep_H

#include <material/nD/NDMaterial.h>
#include "EPState.h"

namespace XC {
class YieldSurface;
class PotentialSurface;
class EvolutionLaw_S;
class EvolutionLaw_T;
class Tensor;

//! @ingroup NDMat
//!
//! @defgroup EPMat3D Three-dimensional elasto-plastic material.
//!
//! @ingroup EPMat3D
//!
//! @brief The Template3Dep class is used to hold specific
//! yield surface, potential surface, Evolution law(s)
//! and EPState of a 3D elasto-plastic material model for one
//! gauss point!! It is worthwhile noting that one model may
//! have multiple evolution law.  Each evlotuion law is
//! used to evolve one internal var.
class Template3Dep: public NDMaterial
  {
   private:
    NDMaterial *theElasticMat;
    YieldSurface *YS;
    PotentialSurface *PS;
    EPState EPS;
                               
    //Scalar variable evolution laws (currently at most 4  allowed)
    EvolutionLaw_S *ELS1; 
    EvolutionLaw_S *ELS2; 
    EvolutionLaw_S *ELS3; 
    EvolutionLaw_S *ELS4; 
    void allocELS(const EvolutionLaw_S *ELS1_= nullptr,
		  const EvolutionLaw_S *ELS2_= nullptr,
		  const EvolutionLaw_S *ELS3_= nullptr,
		  const EvolutionLaw_S *ELS4_= nullptr );
    void freeELS1(void);
    void freeELS2(void);
    void freeELS3(void);
    void freeELS4(void);
    void freeELS(void);
    
    //Tensorial variable evolution laws (currently at most 4  allowed)
    EvolutionLaw_T *ELT1; 
    EvolutionLaw_T *ELT2; 
    EvolutionLaw_T *ELT3; 
    EvolutionLaw_T *ELT4; 
    void allocELT(const EvolutionLaw_T *ELT1_= nullptr,
		  const EvolutionLaw_T *ELT2_= nullptr,
		  const EvolutionLaw_T *ELT3_= nullptr,
		  const EvolutionLaw_T *ELT4_= nullptr );
    void freeELT1(void);
    void freeELT2(void);
    void freeELT3(void);
    void freeELT4(void);
    void freeELT(void);

    void alloc( const NDMaterial *,
		const YieldSurface *,
		const PotentialSurface *,
		const EPState &,
		const EvolutionLaw_S *ELS1_= nullptr,
		const EvolutionLaw_S *ELS2_= nullptr,
		const EvolutionLaw_S *ELS3_= nullptr,
		const EvolutionLaw_S *ELS4_= nullptr,
		const EvolutionLaw_T *ELT1_= nullptr,
		const EvolutionLaw_T *ELT2_= nullptr,
		const EvolutionLaw_T *ELT3_= nullptr,
		const EvolutionLaw_T *ELT4_= nullptr );
    void free(void);
  public:
    Template3Dep(int tag= 0);
    // constructor
    Template3Dep(int tag,
                  const NDMaterial &,
                  const YieldSurface &,        
                  const PotentialSurface &PS,
                  const EPState &EPS_,
		  const EvolutionLaw_S &ELS1_, 
		  const EvolutionLaw_S &ELS2_, 
		  const EvolutionLaw_S &ELS3_, 
		  const EvolutionLaw_S &ELS4_, 
		  const EvolutionLaw_T &ELT1_,
		  const EvolutionLaw_T &ELT2_,
		  const EvolutionLaw_T &ELT3_,
		  const EvolutionLaw_T &ELT4_);
    
    // Constructor0
    // If no evolution law is provided, then there will be no hardening or softening!
    Template3Dep(int tag, const NDMaterial &, const YieldSurface &, const PotentialSurface &PS_, const EPState &EPS_);

    // Constructor1
    // Only one scalar evolution law is provided!
    Template3Dep(int tag, const NDMaterial &, const YieldSurface &,const PotentialSurface &, const EPState &, const EvolutionLaw_S &);

    // Constructor2
    // Only one tensorial evolution law is provided!
    Template3Dep(int tag, const NDMaterial &, const YieldSurface &,const PotentialSurface &, const EPState &, const EvolutionLaw_T &);

    // Constructor 3
    // One scalar evolution law and one tensorial evolution law are provided!
    Template3Dep(int tag, const NDMaterial &, const YieldSurface &,const PotentialSurface &, const EPState &, const EvolutionLaw_S &, const EvolutionLaw_T &);
    
    // Constructor 4
    // Two scalar evolution laws and one tensorial evolution law are provided!
    Template3Dep(int tag, const NDMaterial &, const YieldSurface &,const PotentialSurface &, const EPState &, const EvolutionLaw_S &, const EvolutionLaw_S &, const EvolutionLaw_T &);
    
    // Constructor 5
    // Two scalar evolution laws and two tensorial evolution laws are provided!
    Template3Dep(int tag, const NDMaterial &, const YieldSurface &,const PotentialSurface &, const EPState &, const EvolutionLaw_S &, const EvolutionLaw_S &, const EvolutionLaw_T &, const EvolutionLaw_T &);

    Template3Dep(const Template3Dep &);
    Template3Dep &operator=(const Template3Dep &);
    // For parallel processing
    virtual ~Template3Dep(void);

    // methods to set state and retrieve state using Matrix and Vector classes
    int setTrialStrain(const Vector &v);
    int setTrialStrain(const Vector &v, const Vector &r);
    int setTrialStrainIncr(const Vector &v) ;
    int setTrialStrainIncr(const Vector &v, const Vector &r) ;
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;

    const Vector &getStress(void) const;
    const Vector &getStrain(void) const;

    // methods to set and retrieve state using the Tensor class    
    int setTrialStrain(const Tensor &v) ;
    int setTrialStrain(const Tensor &v, const Tensor &r) ;    
    int setTrialStrainIncr(const Tensor &v) ;
    int setTrialStrainIncr(const Tensor &v, const Tensor &r) ;
    const Tensor &getTangentTensor(void) const;
    const stresstensor &getStressTensor(void) const;
    const straintensor &getStrainTensor(void) const;
    const straintensor &getPlasticStrainTensor(void) const; //Added Joey Aug. 13, 2001
    double getpsi(void); //Added Joey 02-18-03

    const EPState &getEPS() const;
    void setEPS(const EPState &eps);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    const std::string &getType(void) const;
    int getOrder(void) const ;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag =0) const;

    //Private Utility method
  //private:
    
     //These are from formerly CDriver
     EPState ForwardEulerEPState( const straintensor &strain_increment);
     
     EPState SemiBackwardEulerEPState( const straintensor &strain_increment);
     
     EPState FESubIncrementation( const straintensor &strain_increment,
                                  int number_of_subincrements);

     EPState BackwardEulerEPState( const straintensor &strain_increment);

     EPState BESubIncrementation( const straintensor & strain_increment,
                                  int number_of_subincrements);                                                 
  private:
                                                        
    //========================================================================
    // this one is intended to shell the previous three and to decide 
    // ( according to the data stored in Material_Model object ) 
    // which constitutive tensor to return ( forward ( non-constistent
    // or backward ( consistent ) or . . . 
    
    //virtual tensor ConstitutiveTensor(stresstensor   & final_stress, 
    //                                 stresstensor   & start_stress,
    //                                 straintensor   & strain_increment,
    //                                 Material_Model & Criterion,
    //                                 double           just_this_PP );
    

    //========================================================================
    // trying to find intersection point
    // according to M. Crisfield's book
    // "Non-linear Finite Element Analysis of Solids and Structures "
    // chapter 6.6.1 page 168.
    //========================================================================
    EPState PredictorEPState(const straintensor &strain_increment);

    stresstensor yield_surface_cross(const stresstensor & start_stress,
                                     const stresstensor & end_stress);

    double zbrentstress(const stresstensor & start_stress,
                        const stresstensor & end_stress,
                        double x1, double x2, double tol);

    double func( const stresstensor &start_stress,
                 const stresstensor &end_stress,
                 double alfa );
    
   public: 
    BJtensor ElasticComplianceTensor(void) const;
    BJtensor ElasticStiffnessTensor(void) const;

    void freeElMat(void);
    NDMaterial *getElMat() const;
    void setElMat(const NDMaterial &);
    
    void freeYS(void);
    YieldSurface *getYS() const;
    void setYS(const YieldSurface &);
    
    void freePS(void);
    PotentialSurface *getPS() const;
    void setPS(const PotentialSurface &);

    //EPState &getEPS(void);
    
    //get scalar evolution laws
    EvolutionLaw_S *getELS1() const;
    void setELS1(const EvolutionLaw_S &);
    EvolutionLaw_S *getELS2() const;
    void setELS2(const EvolutionLaw_S &);
    EvolutionLaw_S *getELS3() const;    
    void setELS3(const EvolutionLaw_S &);
    EvolutionLaw_S *getELS4() const;
    void setELS4(const EvolutionLaw_S &);

    //get tensorial evolution laws
    EvolutionLaw_T *getELT1() const;
    void setELT1(const EvolutionLaw_T &);
    EvolutionLaw_T *getELT2() const;
    void setELT2(const EvolutionLaw_T &);
    EvolutionLaw_T *getELT3() const;
    void setELT3(const EvolutionLaw_T &);
    EvolutionLaw_T *getELT4() const;
    void setELT4(const EvolutionLaw_T &);
    

    friend std::ostream& operator<<(std::ostream &os, const Template3Dep & MP);
  };

//! @brief Printing stuff.
std::ostream& operator<<(std::ostream &os, const Template3Dep & MP);
} // end of XC namespace


#endif

