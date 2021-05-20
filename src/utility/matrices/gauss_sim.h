// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//gauss_sim.h

#ifndef GAUSS_SIM_H
#define GAUSS_SIM_H

#include "matem.h"
#include "matsimZ.h"


template <class treal>
void GaussJordan(matsimZ<treal> &a,ZMatrix<treal> &x,ZMatrix<treal> &b)
//GaussJordan para matrices simEtricas.
//b: Vector de terminos independientes.
//x: Vector soluciOn.
//ipx: Vector que indica que la incognita i-Esima debe despejarse en la
//     ecuacion ipx(i)
//piv: Vector de variables lOgicas que indica que es el i-Esimo elemento
//     de la diagonal principal.
  {
    bool Singular= 0;
    const zero= 1.0E-8;
    matsimZ<treal>::size_type n_rows= a.getNumberOfRows();
    //Dimensionamos las matrices ipx y piv.
    ZMatrix<size_t> ipx(n_rows,1);
    size_t n1= n_rows + 1;
    ZMatrix<short int> piv(n1,1,0); //inicializamos a cero.
    //Dimensionamos el vector soluciOn.
    x= ZMatrix<treal>(n_rows,1,0.0);
    ZMatrix<treal>::size_type j,k;
    for(j=1;j<=n_rows;j++)
      {
        //Busqueda de pivote en la diagonal principal
        for(k=1;k<=n_rows;k++) piv(k)= 0;
        for(k=1;k<=j;k++)
          {
            k1= ipx(k);
            piv(k1)= 1;
          }
        amax= zero;
        l=0;
        for(k=1;k<=n_rows;k++)
          {
            if(piv(k)) continue;
            r= std::abs(a(k,k));
            if (r<=amax) continue;
            amax= r;
            l= k;
          }
        Singular= (l==0)
        if(Singular) return;
        piv(l)= 1;
        ipx(j)= l;
        r=a(l,l);
        //Transformation of pivot's row
        for(k=1;k<=n;k++)
          {
            if(piv(k)) continue;
            a(l,k)/= r;
          }
        b(l)/= r; //Right hand side
        //Transformation of the rest of the matrix.
        for(i=1;i<=n;i++)
          {
            if (piv(i)) continue;
            q= a(i,l) * r;
            if(std::abs(q)<zero) continue;
            for(k=1;k<=n_rows;k++)
              if(!(piv(k))) a(i,k)-= q * a(l,k);
            b(i)-= q * b(l);
            piv(i)= 1;
          }  
      }
    //Solution of the triangular system.
    for(i=1;i<=n_rows;i++)
      {
        j= n1-i;
        l= ipx(j); //Unknown variable to isolate.
        x(l)= b(l);
        if(i==1) continue;
        j1= j+1;
        for(k=j1;k<=n_rows;k++)
          {
            m= ipx(k);
            x(l)-= x(m)*a(l,m);
          } 
      }
  }

//b: Vector de tErminos independientes.
template <class treal>
ZMatrix<treal> GaussJordan(matsimZ<treal> &a,ZMatrix<treal> &b)
  {
    ZMatrix<treal>::size_type n_rows= a.getNumberOfRows();  
    ZMatrix<treal> x(n_rows,1,0.0);
    GaussJordan(a,x,b);
    return x;
  }

template <class treal>  
ZMatrix<treal> operator /(matsimZ<treal> b, ZMatrix<treal> a)
//Se le pasan copias de los valores de b y a.
  {
    ZMatrix<treal> cociente(b);
    if (b.getNumberOfRows() != a.getNumberOfRows())
      {
        std::cerr << "Matrices de dimensiones incompatibles en operador /"
		  << std::endl;
        abort();      
      }
    cociente= GaussJordan(a,b);
    return cociente;
  }

template <class treal>
ZMatrix<treal> operator /(const matsimZ<treal> &m,const treal &d)
  {
    return m*(1.0/d);
  }
  
#endif
