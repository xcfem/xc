//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//num_integration.cc

#include "num_integration.h"
#include "ExprAlgebra.h"

//! @brief Integra la expresión mediante la regla del trapecio.
ExprAlgebra integ_trapecio(const ExprAlgebra &f,const std::string &palabra,const unsigned int &n,const ExprAlgebra &x0,const ExprAlgebra &x1)
  {
    const ExprAlgebra h= (x1-x0)/(n-1);
    // Evaluate endpoints
    ExprAlgebra value= 0.5*( f(palabra,x0) + f(palabra,x1) );
     // Now the midpoints
    for(size_t k=2; k<n; k++)
      { value+=f(palabra,x0 + h*(k-1)); }
    value*=h;
    return value;
  }

//! @brief Integra la expresión mediante la regla de simpson.
ExprAlgebra integ_simpson(const ExprAlgebra &f,const std::string &palabra,const unsigned int &n,const ExprAlgebra &a,const ExprAlgebra &b)
  {
   // Etape 1
   const ExprAlgebra h= (b - a)/n;
  
   // Etape 2
   const ExprAlgebra Iapp0= f(palabra,a) + f(palabra,b);
   ExprAlgebra Iapp1(0.0);
   ExprAlgebra Iapp2(0.0);
  
   // Etape 3
   unsigned int nn = n-1;
   ExprAlgebra x;
   for(size_t i=1;i<=nn;i++)
     {
       // Etape 4
       x= a + i*h;
       // Etape 5
       if((i%2) == 0)
         Iapp2= Iapp2 + f(palabra,x);
       else
         Iapp1= Iapp1 + f(palabra,x);
     }
  
   // Etape 6
   const ExprAlgebra Iapp= (Iapp0 + 2.0 * Iapp2 + 4.0 * Iapp1) * h/3.0;
  
   // Etape 7
   return Iapp;
 }

//! @brief Integra la expresión mediante la regla del trapecio.
ExprAlgebra integ_trapecio(const ExprAlgebra &f,const Intervalo1D &i)
  { return integ_trapecio(f,i.NmbVar().c_str(),i.N(),i.LimInf(),i.LimSup()); }
//! @brief Integra la expresión mediante la regla de simpson.
ExprAlgebra integ_simpson(const ExprAlgebra &f,const Intervalo1D &i)
  { return integ_simpson(f,i.NmbVar().c_str(),i.N(),i.LimInf(),i.LimSup()); }

//! @brief Devuelve las integrales de la expresión en cada uno de los intervalos [i,i+1] que se pasan como parámetro.
std::vector<ExprAlgebra> integ_simpson(const ExprAlgebra &f,const std::string &nmb_var,const std::vector<ExprAlgebra> &intervalos,const int &n)
  {
    const size_t sz= intervalos.size()-1;
    std::vector<ExprAlgebra> retval(sz,ExprAlgebra(0.0));
    for(size_t i= 0;i<sz;i++)
      retval[i]= f.IntegSimpson(Intervalo1D(nmb_var,intervalos[i],intervalos[i+1],n));
    return retval;
  }
