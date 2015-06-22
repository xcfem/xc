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
// $Date: 2001/08/23 16:45:47 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJvector.cpp,v $                                                                
                                                                        
                                                                        
//############################################################################
//#                                                                          #
//#             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\              #
//#            |                                          |____|             #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                 B A S E                  |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |              C L A S S E S               |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |          C + +     S O U R C E           |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                  #
//#        |                                         |    |                  #
//#         \_________________________________________\__/                   #
//#                                                                          #
//#                                                                          #
//############################################################################
//
//   "C makes it easy to shoot yourself in the foot, C++ makes it harder,
//   but when you do, it blows away your whole leg" -- Bjarne Stroustrup
//
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             Vector class                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic ( with help from ARKoenig in JOOP )         #
//# PROGRAMMER(S):     Boris Jeremic ( with help from ARKoenig in JOOP )         #
//#                                                                              #
//#                                                                              #
//# DATE:              Nov. 14. 1992.                                            #
//# UPDATE HISTORY:    05 - __ avgust '93.  derived class from XC::BJmatrix class      #
//#                                         which is derived from                #
//#                                         nDarray class                        #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

// All of this inheritance idioms are after
// Jim Coplien : "Advanced C++ programing styles and idioms".

#ifndef VECTOR_CC
#define VECTOR_CC

#include "BJvector.h"

//##############################################################################
XC::BJvector::BJvector(int order_n, double initvalue):
  BJmatrix( 2, order_n, 1, initvalue)  {  }  // default constructor
// rank 2 ^ just to be consistent with rank of XC::BJmatrix
//##############################################################################
XC::BJvector::BJvector(int order_n, double *initval):
  BJmatrix(2, order_n, 1, initval)  {  }
//rank 2 ^ just to be consistent with rank of XC::BJmatrix

//##############################################################################
XC::BJvector::BJvector( const XC::nDarray & x):
  BJmatrix( x )   {  } // copy-initializer



//.... // IT IS NOT INHERITED so must be defined in all derived classes
//.... // See ARM page 277.
//.... //##############################################################################
//.... XC::BJvector::~BJvector()
//.... {
//....   if (reference_count(-1) == 0)  // if reference count  goes to 0
//....     {
//.... // DEallocate memory of the actual XC::nDarray
//.... //    delete [pc_nDarray_rep->pc_nDarray_rep->total_numb] pc_nDarray_rep->pd_nDdata;
//.... // nema potrebe za brojem clanova koji se brisu## see ELLIS & STROUSTRUP $18.3
//.... //                                                and note on the p.65($5.3.4)
//.... //  and the page 276 ($12.4)
//....     delete [] data();
//....     delete [] dim();
//....     delete pc_nDarray_rep;
//....   }
//.... }




//#############################################################################
XC::BJvector &XC::BJvector::operator=(const XC::BJvector & rval)
  {
    rval.pc_nDarray_rep->n++; // we're adding another reference.
//    rval.reference_count(+1);  // tell the rval it has another reference
//   /*  It is important to increment the reference_counter in the new
//       BJtensor before decrementing the reference_counter in the
//       old BJtensor_rep to ensure proper operation when assigning a
//       BJtensor_rep to itself ( after ARKoenig JOOP May/June '90 )  */
// clean up current value;
    if( reference_count(-1) == 0)  // if nobody else is referencing us.
      {
        delete [] data();
        delete [] dim();
        delete pc_nDarray_rep;
      }
 // connect to new value
    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval nDarray_rep
    return *this;
  }

//..//#############################################################################
//..BJvector& XC::BJvector::operator=( const XC::BJmatrix & rval)
//..{
//..    rval.pc_nDarray_rep->n++; // we're adding another reference.
//..//    rval.reference_count(+1);  // tell the rval it has another reference
//..//   /*  It is important to increment the reference_counter in the new
//..//       BJtensor before decrementing the reference_counter in the
//..//       old BJtensor_rep to ensure proper operation when assigning a
//..//       BJtensor_rep to itself ( after ARKoenig JOOP May/June '90 )  */
//..
//.. // clean up current value;
//..    if( reference_count(-1) == 0)  // if nobody else is referencing us.
//..      {
//..        delete [] data();
//..        delete [] dim();
//..        delete pc_nDarray_rep;
//..      }
//..
//..// set back rank to 1 for XC::BJvector instead of 2 as in XC::BJmatrix case
//..      rval.pc_nDarray_rep->nDarray_rank = 1;
//..//    rval.rank(1);
//..// connect to new value
//..    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval nDarray_rep
//..    return *this;
//..}
//..
//..//#############################################################################
//..BJvector& XC::BJvector::operator=( const XC::nDarray & rval)
//..{
//..    rval.pc_nDarray_rep->n++; // we're adding another reference.
//..//    rval.reference_count(+1);  // tell the rval it has another reference
//..//   /*  It is important to increment the reference_counter in the new
//..//       BJtensor before decrementing the reference_counter in the
//..//       old BJtensor_rep to ensure proper operation when assigning a
//..//       BJtensor_rep to itself ( after ARKoenig JOOP May/June '90 )  */
//..
//.. // clean up current value;
//..    if( reference_count(-1) == 0)  // if nobody else is referencing us.
//..      {
//..        delete [] data();
//..        delete [] dim();
//..        delete pc_nDarray_rep;
//..      }
//..
//.. // connect to new value
//..    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval nDarray_rep
//..    return *this;
//..}
//..

//#######  //#############################################################################
//#######  // I had to overload the operator* from XC::BJmatrix class
//#######  // because in the case of BJvectors s*sT and so on
//#######  BJmatrix XC::BJvector::operator*( BJvector & arg)
//#######    {
//#######  //    if( cols() != arg.rows())
//#######  //      error("# rows of second mat must equal "
//#######  //               "# cols of first for multiply#");
//#######      BJmatrix result(rows(),arg.cols());
//#######      for( int row=0 ; row<rows() ; row++ )
//#######        for( int col=0 ; col<arg.cols() ; col++ )
//#######          {
//#######            double sum = 0;
//#######            for( int i=0 ; i<cols() ; i++ )
//#######              sum += mval(row,i)*arg.mval(i,col);
//#######            result.mval(row,col) = sum;
//#######          }
//#######      return result; // Returning a local variable?
//#######      // copy-initializer happens before the destructor,
//#######      // so reference count is 2 when destructor is called,
//#######      // thus destructor doesn't free the memory.
//#######    }
//#######  
//....//#############################################################################
//....BJvector XC::BJvector::operator*( double arg)
//....  {
//....    BJvector result(rows());
//....    for ( int i=0 ; i<rows() ; i++ )
//....      result.val(i) = cval(i) * arg;
//....    return result;
//....  }



//##############################################################################
//
// CODE BOUND checking routine ( slower but safer )
//
double XC::BJvector::cval(int subscript, ... )  const
  { return (this->XC::BJmatrix::cval(subscript,1)); }

double &XC::BJvector::val(int subscript, ... )
  { return (this->XC::BJmatrix::val(subscript,1)); }

const double &XC::BJvector::val(int subscript, ... ) const
  { return (this->XC::BJmatrix::val(subscript,1)); }

#endif
