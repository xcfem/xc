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
//# PURPOSE:           stress tensor with all necessery functions                #
//# CLASS:             stresstensor                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Alireza Tabarrei                                          #
//# PROGRAMMER(S):     Alireza Tabarrei                                          #
//#                                                                              #
//#                                                                              #
//# DATE:              June 2004																																																 #
//# UPDATE HISTORY:    																																																									 #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef COSSERATSTRESSTENSOR_H
#define COSSERATSTRESSTENSOR_H

#include "utility/matrix/nDarray/BJtensor.h"
#include <iostream>

namespace XC {
class Material_Model;

//! @ingroup Matrix
//
class Cosseratstresstensor : public BJtensor
{
  public:
    friend class Material_Model;

  public:
    // just send appropriate arguments to the base constructor
    Cosseratstresstensor(int rank_of_tensor=2, double initval=0.0); // default constructor
    Cosseratstresstensor( double *values );
    Cosseratstresstensor( double initvalue );

    Cosseratstresstensor(const Cosseratstresstensor & x );
    Cosseratstresstensor(const BJtensor & x); // copy-initializer
    Cosseratstresstensor(const nDarray & x); // copy-initializer

    //~Cosseratstresstensor( );
    

    Cosseratstresstensor operator=(const Cosseratstresstensor & rval);// Cosseratstresstensor assignment
    Cosseratstresstensor operator=(const BJtensor & rval);// tensor assignment to Cosseratstresstensor
    Cosseratstresstensor operator=(const nDarray & rval);// nDarray assignment to Cosseratstresstensor

    Cosseratstresstensor deep_copy(void);
    //..    Cosseratstresstensor * p_deep_copy(void);

    //ini  // use "from" and initialize already allocated stress tensor from "from" values
    //ini      void Initialize( const Cosseratstresstensor & from );

    //___// operator() overloading for 3D Gauss points!
    //___    Cosseratstresstensor & operator()(short ir, short is, short it,
    //___                              short tr, short ts, short tt  );
    

    double Iinvariant1( ) const;
    double Iinvariant2( ) const;
    double Iinvariant3( ) const;

    double Jinvariant1( ) const;
    double Jinvariant2( ) const;
    double Jinvariant3( ) const;

    Cosseratstresstensor deviator( ) const;
    Cosseratstresstensor principal( ) const;

    double sigma_octahedral( ) const;
    double tau_octahedral( ) const;

    double ksi( )     const;
    double xi( )      const;
    double ro( )      const;
    double rho( )      const;
    double theta()   const;
    double thetaPI( ) const;

    double p_hydrostatic( ) const;
    double q_deviatoric( ) const;

    BJtensor dpoverds( void  ) const;
    BJtensor dqoverds( void ) const;
    BJtensor dthetaoverds( void ) const;
    BJtensor d2poverds2( void ) const;
    BJtensor d2qoverds2( void  ) const;
    BJtensor d2thetaoverds2( void ) const;
	     	          


    //--    Cosseratstresstensor yield_surface_cross(Cosseratstresstensor & end_stress,
    //--                                     Material_Model & YC);

    Cosseratstresstensor pqtheta2stress( double, double, double );

    void report(const std::string &) const;
    void reportshort(const std::string &) const;
    void reportshortpqtheta(const std::string &) const;
    void reportSHORTpqtheta(const std::string &) const;
    void reportSHORTs1s2s3(const std::string &) const;
    void reportKLOTpqtheta(const std::string &) const;
    void reportshortI1J2J3(const std::string &) const;
    void reportAnim(void) const;
    void reportTensor(const std::string &) const;

    //================================================================================
    // Overloaded Insertion Operator	  ZHaohui Added Aug. 13, 2000
    // prints an Cosseratstresstensor's contents 
    //================================================================================
    friend std::ostream &operator<<(std::ostream &os, const Cosseratstresstensor & rhs);

    //  // routine used by root finder, takes an alfa and returns the
    //  // yield function value for that alfa
    //    public:
    //      double func( Cosseratstresstensor & start_stress,
    //                   Cosseratstresstensor & end_stress,
    //                   Material_Model & YC,
    //                   double alfa );
    //  
    //  
    //  //..// polynomial root solver friend functions definitions
    //  //..public:
    //  //..friend void laguer(complex *, int , complex *, double , int );
    //  //..friend void zroots(complex *, int , complex *, int );
    //  //..
    //  
    // zero of function
    friend double zbrentstress(Cosseratstresstensor   & start_stress,
                             Cosseratstresstensor   & end_stress,
                             Material_Model & YC,
                             double x1, double x2, double tol);
  
    //  friend double zbrent(double x1, double x2, double tol);
    //  
    //  
  };
std::ostream &operator<<(std::ostream &os, const Cosseratstresstensor & rhs);
} // end of XC namespace

#endif

