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
// $Date: 2001/08/23 16:45:44 $
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/basics.cpp,v $
                                                                        
                                                                        
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
//#############################################################################

//
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:                                                                       #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #                                                                              #
//# DATE:              November '92                                              #
//# UPDATE HISTORY:    05 - __ avgust '93.  redefined as derived class from      #
//#                                 nDarray class                                #
//#                    january 06 '93  added Matrix2BJtensor_1, Matrix2BJtensor_2    #
//#                                   Matrix2BJtensor_3                            #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//################################################################################
//
#ifndef BASICS_CC
#define BASICS_CC


#include "basics.h"


//##############################################################################
//DEBUGheap // heap checking function
//DEBUGheap void heap(char *message)
//DEBUGheap   {
//DEBUGheap     struct heapinfo hi;
//DEBUGheap
//DEBUGheap     if (*message) ::printf("%s",message);
//DEBUGheap
//DEBUGheap     int counter = 0;
//DEBUGheap     hi.ptr = nullptr;
//DEBUGheap     ::printf( "\n  counter      Pointer        Size   Status\n" );
//DEBUGheap
//DEBUGheap     unsigned int size = 0;
//DEBUGheap     char use[10];
//DEBUGheap     while( heapwalk( &hi ) == _HEAPOK )
//DEBUGheap       {
//DEBUGheap          counter++;
//DEBUGheap          size = hi.size;
//DEBUGheap
//DEBUGheap          if(hi.in_use == 0 ) strcpy(use,"   free");
//DEBUGheap          if(hi.in_use == 1 ) strcpy(use,"USED");
//DEBUGheap          printf( "  %7d    %p     %7u    %s\n", counter,hi.ptr,size, use );
//DEBUGheap       }
//DEBUGheap   }


//##############################################################################
// find out the machine epsilon of the runing platform !
//##############################################################################
float f_macheps()
  {
    float eps = 1.0;
    int counter = 1;
    float one = 1.0;
    while ( (one + eps) > one )
      {
        eps = eps/2.0;
        counter++;
//        ::printf("eps = %.20e   counter = %7d \n",eps,counter);
      }
    eps = eps*2.0;
    counter--;

//    ::printf("\n flota macheps = %.20e at counter = %d \n",eps,counter);

    return eps;
  }

//##############################################################################
double d_macheps()
  {
    double eps = 1.0;
    int counter = 1;
    while ( ((double) 1.0 + eps) > ((double) 1.0) )
      {
        eps = eps/2.0;
        counter++;
//        ::printf("eps = %.16e   counter = %7d \n",eps,counter);
      }
    eps = eps*2.0;
    counter--;
//    ::printf("\n double macheps = %.20e  at counter = %d \n",eps,counter);

    return eps;
  }

//##############################################################################
long double ld_macheps()
  {
    long double eps = 1.0;
    int counter = 1;
    while ( ((long double) 1.0 + eps) > ((long double) 1.0) )
      {
        eps = eps/2.0;
        counter++;
//        ::printf("eps = %.16le   counter = %7d \n",eps,counter);
      }
    eps = eps*2.0;
    counter--;
//    ::printf("\n long double macheps = %.20le  at counter = %d \n",eps,counter);

    return eps;
  }


double min(double a, double b)
  {
    double ret=0;
    ret=(a<b) ? a : b;
    return ret;
  }

double max(double a, double b)
  {
    double ret=0;
    ret=(a>b) ? a : b;
    return ret;
  }

int min(int a, int b)
  {
    int ret=0;
    ret=(a<b) ? a : b;
    return ret;
  }

int max(int a, int b)
  {
    int ret=0;
    ret=(a>b) ? a : b;
    return ret;
  }

#endif

