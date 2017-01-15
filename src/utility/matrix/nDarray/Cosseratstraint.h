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
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           strain tensor with all necessery functions                #
//# CLASS:             Cosseratstraintensor                                      #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Alireza Tabarrei, Boris Jeremic                           #
//# PROGRAMMER(S):     Alireza Tabarrei,Boris Jeremic                            #
//#                                                                              #
//#                                                                              #
//# DATE:              July 25 '93                                               #
//# UPDATE HISTORY:    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC,        #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/
//
#ifndef COSSERATSTRAINTENSOR_HH
#define COSSERATSTRAINTENSOR_HH

#include "utility/matrix/nDarray/BJtensor.h"

namespace XC {
class Cosseratstraintensor : public BJtensor
{
  public: // just send appropriate arguments to the base constructor

//    Cosseratstraintensor (int rank_of_tensor=2, double initval=0.00000003141528);
    Cosseratstraintensor (int rank_of_tensor=2, double initval=0.0);
// default constructor           // this is just PI/10^8 to check default constructor

    Cosseratstraintensor ( double *values );

    Cosseratstraintensor ( double initvalue );

    Cosseratstraintensor(const Cosseratstraintensor & x );
    Cosseratstraintensor(const BJtensor & x); // copy-initializer
    Cosseratstraintensor(const nDarray & x); // copy-initializer

    //~Cosseratstraintensor( );
    
    Cosseratstraintensor operator=(const Cosseratstraintensor & rval); // Cosseratstraintensor assignment
    Cosseratstraintensor operator=(const BJtensor & rval);// tensor assignment to Cosseratstraintensor
    Cosseratstraintensor operator=(const nDarray & rval);// nDarray assignment to Cosseratstraintensor

    Cosseratstraintensor deep_copy(void);
//..    Cosseratstraintensor * p_deep_copy(void);

//ini  // use "from" and initialize already allocated strain tensor from "from" values
//ini      void Initialize( const Cosseratstraintensor & from );

//___// operator() overloading for 3D Gauss points!
//___    Cosseratstraintensor & operator()(short ir, short is, short it,
//___                              short tr, short ts, short tt  );
 
    double Iinvariant1( ) const;
    double Iinvariant2( ) const;
    double Iinvariant3( ) const;

    double Jinvariant1( ) const;
    double Jinvariant2( ) const;
    double Jinvariant3( ) const;

    double equivalent( ) const;	  //Zhaohui added 09-02-2000

    Cosseratstraintensor deviator( ) const;
    Cosseratstraintensor principal( ) const;

    double sigma_octahedral( ) const;
    double tau_octahedral( ) const;

    double ksi( ) const;
    double ro( ) const;
    double theta( ) const;
    double thetaPI( ) const;

    double p_hydrostatic( ) const;
    double q_deviatoric( ) const;


    Cosseratstraintensor pqtheta2strain( double, double, double );
    Cosseratstraintensor evoleq2strain( double, double );

    void report(const std::string &) const;
    void reportshort(const std::string &) const;

//..// polinomial root solver friend functions definitions
//..public:
//..friend void laguer(complex *, int , complex *, double , int );
//..friend void zroots(complex *, int , complex *, int );
//..
};
} // end of XC namespace

#endif

