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
//================================================================================
# COPYRIGHT (C):     :-))                                                        #
# PROJECT:           Object Oriented Finite Element Program                      #
# PURPOSE:           General platform for elaso-plastic constitutive model       #
#                    implementation                                              #
# CLASS:             ManzariDafaliasEvolutionLaw (evolution law for Manzari-Dafalias Model)   #
#                                                                                #
# VERSION:                                                                       #
# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
# TARGET OS:         DOS || UNIX || . . .                                        #
# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
#                                                                                #
#                                                                                #
# DATE:              08-03-2000                                                  #
# UPDATE HISTORY:                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
#                                                                                #
# SHORT EXPLANATION: The goal is to create a platform for efficient and easy     #
#                    implementation of any elasto-plastic constitutive model!     #
#                                                                                #
//================================================================================
*/

#ifndef ManzariDafaliasEvolutionLaw_H
#define ManzariDafaliasEvolutionLaw_H

#include <cmath>
#include "EvolutionLaw_S.h"

namespace XC {
//! @ingroup MatNDEL
//!
//! @brief Manzari-Dafalias evolution Law.
class ManzariDafaliasEvolutionLaw: public EvolutionLaw_S
  {
  // private to define the evolution law for Manzari-Dafalias critical state model
  public: 
 
    //Critical state parameters
    double Mc;
    double Me;
    double Lambda; // slope of e vs. ln p
    double ec_ref;// critical void ratio at reference mean effective stress p
    double p_ref; // critical void ratio at reference mean effective stress p

    // surface evolution parameters
    double kc_b;  // b stands for bounding surface, c compression
    double kc_d;  // d stands for dilatancy surface, c compression 
    double ke_b;  // d stands for bounding surface, e compression
    double ke_d;  // d stands for dilatancy surface, e compression 

    // Parameters to calculate plastic modulus 
    //double h;  // Could be calculated using ho and b_ij * n_ij
    double ho;
    double Cm;
    double eo;	 // initial void ratio

    //Dilatancy parameter
    //double D; //Moved to EPS's second scalar var
    double Ao;

    //Parameters to define the evolution of Fabric Tensor
    double Fmax;
    double Cf;
    double a;   //exponent for elastic modulus evolution  
    
 public:
    //ManzariDafaliasEvolutionLaw( );                   // default constructor---no parameters
    
    ManzariDafaliasEvolutionLaw( double Mcd = 1.14,   // default constructor
                    double Med = 1.14,
                    double Lamdad = 0.025,
                    double ec_refd = 0.8,
                    double p_refd = 100, //kPa
                    double kc_bd = 3.975,
                    double kc_dd = 4.200,
                    double ke_bd = 2.000,
                    double ke_dd = 0.070,
                    double hod = 1200.0,  // old 1200
                    double Cmd = 0.0,	 // old 0.0
                    //double eod = 0.65,
		    double Aod = 2.64,	 //old 2.64
                    double Fmaxd = 100,
                    double Cfd = 100, 
                    double ad = 0.6) : 
		    Mc (Mcd), Me(Med), Lambda(Lamdad), ec_ref(ec_refd), p_ref(p_refd),
		    kc_b(kc_bd), kc_d(kc_dd), ke_b(ke_bd), ke_d(ke_dd), ho(hod), Cm(Cmd), eo(0.0),
		    Ao(Aod), Fmax(Fmaxd), Cf(Cfd), a(ad) {} 
    
    ManzariDafaliasEvolutionLaw(const ManzariDafaliasEvolutionLaw &MDE );   // Copy constructor
    
    ManzariDafaliasEvolutionLaw *getCopy(void); // create a clone of itself
    
    void InitVars(EPState *EPS);    // Initialize all hardening vars called only once 
                                    // after material point is formed!
    
    void setInitD(EPState  *EPS);   // set initial D once current stress hits the y.s.
    
    double getKp( EPState *EPS, double dummy );     // calculating Kp 
    
    void UpdateAllVars( EPState *EPS, double dlamda );  // Evolve all vars
    //void UpdateAllTensorVar( EPState *EPS, double dlamda );  // Evolve all tensor vars

    void print();

    // some accessor functions
    double getMc() const;
    double getMe() const;
    double getLambda() const;
    double getec_ref() const;
    double getp_ref() const; 

    double getkc_b() const;  
    double getkc_d() const;  
    double getke_b() const;  
    double getke_d() const;  
    //double geth() const;  // Could be calculated using ho and b_ij * n_ij
    double getho() const;
    double getCm() const;
    double geteo() const;
    void seteo( double eod);

    //Dilatancy parameter
    double getAo() const;

    double getFmax() const;
    double getCf() const;
    double geta() const;


    //================================================================================
    // Overloaded Insertion Operator	  Zhaohui Added Aug. 13, 2000
    // prints Manzari-Dafalia EvolutionLaw's contents 
    //================================================================================
    friend std::ostream& operator<<(std::ostream &, const ManzariDafaliasEvolutionLaw &);
    double g_A(double theta, double e);	 // Interpolation function  by Agyris
    double g_WW(double theta, double e); // Interpolation function  by Willan-Warkne

    
  };
 
std::ostream& operator<<(std::ostream &os, const ManzariDafaliasEvolutionLaw & MDEL);

} // end of XC namespace


#endif

// test




