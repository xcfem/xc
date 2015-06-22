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
} // fin namespace XC

#endif

