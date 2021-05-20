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
//solver_gauss.h

#ifndef SOLVER_GAUSS_H
#define SOLVER_GAUSS_H

#include "utility/matrices/solvers/solver.h"
#include <cmath>
#include <algorithm>

template<class M,class V>
class SolverGauss: public SolverM<M>
  {
    typedef typename SolverM<M>::tipo_val tipo_val;
    
    inline size_t find_pivot(const size_t &k)
      {
        size_t l= 0;
        tipo_val smax= fabs((*A)(P(k),k));
        for(size_t i=k+1; i<=n; i++)
          {
            size_t ip= P(i);
            if(fabs((*A)(ip,k)) > smax)
              {
                l= i;
                smax = fabs((*A)(ip,k));
              }
          }
        return l;
      }

    virtual bool decomp(void)
      {
        inic_p();
        for(size_t k=1, l=0; k<=n; k++)
          {
            l= find_pivot(k);

            if(l) std::swap(P(k),P(l));
            size_t pi= P(k);
            tipo_val r1= 1.0/(*A)(pi,k);
            for(size_t i=k+1; i<=n; i++)
              {
                size_t ip= P(i);
                tipo_val r= (*A)(ip,k)*r1;
                for(size_t j=k+1;j<=n;j++) (*A)(ip,j)-= r*(*A)(pi,j);
                (*A)(ip,k) = -r;
              }
          }
        desc= 1;
        return true;
      }
    inline void triang_inf(V &B) const
      {
        for(size_t k=1; k<=n; k++)
          {
            size_t ip= P(k);
            for(size_t i=k+1; i<=n; i++)
              {
                size_t pi= P(i);
                B(pi)+= (*A)(pi,k)*B(ip);
              }
          }
      }
    inline V atras(const V &B) const
      {
        V X(B);
        tipo_val c;
        X(n) = B(P(n))/(*A)(P(n),n);
        for(size_t i=n-1; i>=1; i--)
          {
            size_t pi=P(i);
            size_t j;
            for(j=i+1, c=B(pi); j<=n; j++) c-= (*A)(pi,j)*X(j);
            X(i)= c/(*A)(pi,i);
          }
        return X;
      }
    virtual V backsub(V &B) const
      {
        if(!check_backsub(B)) exit(1);
        triang_inf(B);
        return atras(B);
      }
  public:
    SolverGauss(void): SolverM<M>() {}
    V BackSub(V &B) const
      {
        if(!desc)
          std::cerr << "¡Ojo! Se ha llamado a Backsub sin llamar antes a Decomp."
               << std::endl;
        return backsub(B);
      }
    V BackSub(const V &B) const
      {
        if(!desc)
          std::cerr << "¡Ojo! Se ha llamado a Backsub sin llamar antes a Decomp."
               << std::endl;
        V C(B);
        return backsub(C);
      }
  };

#endif
