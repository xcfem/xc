//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
    //  //..// polinomial root solver friend functions definitions
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
} // fin namespace XC

#endif

