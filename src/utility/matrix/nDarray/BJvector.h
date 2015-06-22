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
                                                                        
// $Revision: 1.1 $                                                              
// $Date: 2001/08/23 16:45:51 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJvector.h,v $                                                                
                                                                        
//#############################################################################
//                                                                            #
//                                                                            #
//             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\                #
//            |                                          |____|               #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |        B A S E   C L A S S E S           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |          C + +     H E A D E R           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                    #
//         \_________________________________________\__/                     #
//                                                                            #
//                                                                            #
//#############################################################################
//#############################################################################
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             BJvector                                                    #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              November '92                                              #
//# UPDATE HISTORY:    05 - __ avgust '93.  redefined as derived class from      #
//#                                 nDarray class                                #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/


#ifndef VECTOR_HH
#define VECTOR_HH

#include "utility/matrix/nDarray/BJmatrix.h"

// All of this inheritance idioms are after
// Jim Coplien : "Advanced C++ programing styles and idioms".
// I tried to understand idioms and I think I succeded.

namespace XC {
class BJvector: virtual public BJmatrix
  {
    public:
      BJvector(int order_n = 1, double initvalue = 0.0);  // default constructor

      BJvector(int order_n, double *initval);

      explicit BJvector(const nDarray &x); // copy-initializer

//....       ~BJvector( );

      BJvector& operator=( const BJvector & x ); // BJvector assignment
//..      BJvector& operator=( const BJmatrix & x ); // BJvector assignment
//..      BJvector& operator=( const nDarray & x ); // BJvector assignment

//#######        BJmatrix operator*( BJvector &); // BJvector multiplication

//....     BJvector operator*( double arg); // scalar multiplication 
// this  ellipsis at the end are just to prevent the compiler
// from issuing a warning on hiding function from base class nDarray . . . 
     double &val(int subscript, ... );
     const double &val(int subscript, ... ) const;
     double cval(int subscript, ... ) const; // const
// THE ROW COUNTER STARTS FROM 1 ( NOT FROM 0 )
  };
} // fin namespace XC
#endif
