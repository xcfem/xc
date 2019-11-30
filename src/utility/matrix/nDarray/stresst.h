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
//# PURPOSE:           stress tensor with all necessary functions                #
//# CLASS:             stresstensor                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              July 22 '93                                               #
//# UPDATE HISTORY:    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                    13 septembar '96 added reportAnim  :)                     #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef STRESSTENSOR_H
#define STRESSTENSOR_H

#include "stress_strain_tensor.h"
#include <iostream>

namespace XC {
class Material_Model;


//! @ingroup Matrix
//
class stresstensor : public stressstraintensor
  {
  public:
    friend class Material_Model;

  public:
    // just send appropriate arguments to the base constructor
    stresstensor(int rank_of_tensor=2, double initval=0.0); // default constructor
    stresstensor(const boost::python::list &l);
    stresstensor(const double *values );
    stresstensor(const std::vector<double> &values );
    stresstensor(double initvalue );
    explicit stresstensor(const Vector &);

    stresstensor(const stresstensor & x );
    stresstensor(const BJtensor & x); // copy-initializer
    stresstensor(const nDarray & x); // copy-initializer

    //~stresstensor( );
    

    stresstensor operator=(const stresstensor & rval);// stresstensor assignment
    stresstensor operator=(const BJtensor & rval);// tensor assignment to stresstensor
    stresstensor operator=(const nDarray & rval);// nDarray assignment to stresstensor

    stresstensor deep_copy(void);
    //..    stresstensor * p_deep_copy(void);

    //ini  // use "from" and initialize already allocated stress tensor from "from" values
    //ini      void Initialize( const stresstensor & from );

    //___// operator() overloading for 3D Gauss points!
    //___    stresstensor & operator()(short ir, short is, short it,
    //___                              short tr, short ts, short tt  );
    

    stresstensor deviator( ) const;
    stresstensor principal( ) const;

    double xi( )      const;
    
    BJtensor dpoverds( void  ) const;
    BJtensor dqoverds( void ) const;
    BJtensor dthetaoverds( void ) const;
    BJtensor d2poverds2( void ) const;
    BJtensor d2qoverds2( void  ) const;
    BJtensor d2thetaoverds2( void ) const;
	     	          


    //--    stresstensor yield_surface_cross(stresstensor & end_stress,
    //--                                     Material_Model & YC);

    stresstensor pqtheta2stress( double, double, double );

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
    // prints an stresstensor's contents 
    //================================================================================
    friend std::ostream &operator<<(std::ostream &, const stresstensor &);

    //  // routine used by root finder, takes an alfa and returns the
    //  // yield function value for that alfa
    //    public:
    //      double func( stresstensor & start_stress,
    //                   stresstensor & end_stress,
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
    friend double zbrentstress(stresstensor   & start_stress,
                             stresstensor   & end_stress,
                             Material_Model & YC,
                             double x1, double x2, double tol);
  
    //  friend double zbrent(double x1, double x2, double tol);
    //  
    //  
  };

std::ostream &operator<<(std::ostream &, const stresstensor &);

} // end of XC namespace

#endif

