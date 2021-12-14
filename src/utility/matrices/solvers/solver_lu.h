// -*-c++-*-
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
//solver_lu.h

#ifndef SOLVER_LU_H
#define SOLVER_LU_H

#include "utility/matrices/solvers/solver.h"
#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/matsimZ.h"
#include "utility/matrices/matdispZ.h"

template<class M,class V>
class SolverLU: public SolverM<M>
  {
  protected:
    typedef SolverM<M> solver_m;
    typedef typename solver_m::tipo_val tipo_val;

    long v;

    virtual tipo_val find_pivot(const size_t &k,size_t &maxi) const
      {
        //For each row of the k colum starting by row k
        tipo_val c;
        tipo_val c1;
        size_t i= 0;
        for(i= k, maxi= k, c=solver_m::CERO;i<=this->n;i++)
          {
            c1= (*this->A)(this->P(i),k);
            c1*= sgn(c1);
            if (c1 > c) { c = c1; maxi = i; }
          }
        return c;
      }
    void rest_of_row(const size_t &i,const size_t &k,const tipo_val &d)
      {
        const size_t pk= this->P(k);
        for(size_t j=k+1;j<=this->n;j++) //Next columns of the row.
          (*this->A)(i,j)-= d * (*this->A)(pk,j);
      }
    void aplica_pivote(const size_t &k)
      {
        tipo_val d= (*this->A)(this->P(k),k);
        for(size_t i= k+1;i<=this->n;i++)
          {
            //Divide el elemento de the column 
	    //del pivote por el pivote.
            const size_t pi= this->P(i);
            const tipo_val &dd= ((*this->A)(pi,k)/= d);
            if(dd!=solver_m::CERO) rest_of_row(pi,k,dd);
          }
      }

    //! @brie Return true if it can decompose the matrix.
    virtual bool decomp(void)
      {
        size_t maxi; //pivot's row.
        tipo_val c; //pivot's value.
        size_t p= 1;
        this->inic_p();

        for(size_t k= 1;k<= this->n; k++) //For each column.
          {
            c= find_pivot(k,maxi);
            if (k != maxi)
              {
                p++;
		std::swap(this->P(k),this->P(maxi));
	      }
            if ((*this->A)(this->P(k),k) == solver_m::CERO) //If the pivot of this column is zero.
              {
                v = -1;
                if(this->verbosity)
		  std::cerr << "Singular matrix" << std::endl;
                return false;
              }
            aplica_pivote(k);
          }
        v=p;
        this->desc= 1;
        return true;
      }
    void triang_inf(V &B) const
    //Iterates through the lower triangle
      {
        for(size_t k=1; k<=this->n; k++) 
          {
            const size_t pk= this->P(k);
            const tipo_val &Bpk= B(pk);
            for(size_t i=k+1; i<=this->n; i++)
              {
                const size_t pi= this->P(i);
	        B(pi)-= ( (*this->A)(pi,k) * Bpk );
              }
          }
      }
    V atras(const V &B) const
      //Sustitución hacia atrás.
      {
        size_t j;
        tipo_val sum;
        V X(B);
        X(this->n)= B(this->P(this->n)) / (*this->A)(this->P(this->n),this->n);
        for(size_t k=this->n; k>1;)
          {
            k--;
            sum=solver_m::CERO;
            const size_t pk= this->P(k);
            for(j=k+1; j<=this->n; j++) sum+= (*this->A)(pk,j) * X(j);
            X(k)= (B(pk) - sum) /(*this->A)(pk,k);
          }
        return X;
      }
    virtual V backsub(V &B) const
      {
        if(!check_backsub(B)) exit(1);
        triang_inf(B);
        return atras(B);
      }
    inline void check_decomp(void) const
      { if(!this->desc) std::cerr << "¡Ojo! Se ha llamado a Backsub sin llamar antes a Decomp." << std::endl; }
  public:
    SolverLU(const size_t &verbosity= 0): solver_m(verbosity) {}
    V BackSub(V &B) const
      {
        check_decomp();
        return backsub(B);
      }
    V BackSub(const V &B) const
      {
        check_decomp();
        V C(B);
        return backsub(C);
      }
  };

template<class M,class V>
class SolverConstLU: public SolverLU<M,V>
  {
  protected:
    M a;
  public:
    SolverConstLU(const size_t &verbosity): SolverLU<M,V>(verbosity), a(0,0){}
    void putMatrix(const M &m)
      {
        a= m;
        SolverLU<M,V>::putMatrix(a);
      }
  };


template <class treal,class V>
class SolverSimLU: public SolverConstLU<ZMatrix<treal>,V>
  {
  public:
    SolverSimLU(const size_t &verbosity)
      : SolverConstLU<ZMatrix<treal>,V>(verbosity)
      {}
    void putMatrix(const matsimZ<treal> &m)
      { SolverConstLU<ZMatrix<treal>,V>::putMatrix(m.GetCompleta()); }
  };

template <class treal,class V>
class SolverDispLU: public SolverLU<matdispZ<treal>,V>
  {
  public:
    typedef SolverLU<matdispZ<treal>,V> solver_lu;
    typedef matdispZ<treal> msp_treal;
    typedef typename msp_treal::value_type tipo_val;
    typedef typename msp_treal::const_c_iterator const_c_iterator;
    typedef typename msp_treal::const_f_iterator const_f_iterator;
    typedef typename msp_treal::c_iterator c_iterator;
    typedef typename msp_treal::f_iterator f_iterator;
    typedef typename msp_treal::sp_vector sp_vector;

  private:
    tipo_val find_pivot( const c_iterator &ic,
                           const_f_iterator &maxi) const
      {
        //For each row of the column starting with row ic->first
        tipo_val c1,c= solver_lu::CERO;
        const_f_iterator fi= this->A->find_row(ic,this->P(ic->first));
        for(maxi= fi,c= solver_lu::CERO;fi!=this->A->rows_end(ic);fi++)
          {
            c1= fabs(fi->second);
            if(c1 > c) { c = c1; maxi = fi; }
          }
        return c;
      }
    void aplica_pivote(const c_iterator &ic)
      {
        const size_t k= ic->first;
        tipo_val d= (*this->A)(this->P(k),k);
        std::cout << "***** aplica pivote col: " << k << ' ' << d << std::endl;
	sp_vector &col= this->A->getColumn(ic);
        for(size_t i= k+1;i<=this->n;i++)
          {
            //Divide el elemento de the column 
	    //del pivote por el pivote.
            const double &dd= (col[this->P(i)]/=d);
            if(dd!=solver_lu::CERO) rest_of_row(this->P(i),k,dd);
          }
      }
    //! Return true if it can decompose the matrix.
    bool decomp(void)
      {
        std::cout << "seems to fail" << std::endl;
        const_f_iterator maxi; //pivot row.
        tipo_val c; //Valor del pivote.
        size_t p= 1;
        this->inic_p();

	c_iterator ic;
        for(ic= this->A->rows_begin();ic!= this->A->rows_end(); ic++) //For each column.
          {
            size_t k= ic->first;
            c= find_pivot(ic,maxi);
            if (k != maxi->first)
              {
                p++;
		std::swap(this->P(k),this->P(maxi->first));
	      }
            if ((*this->A)(this->P(k),k) == solver_lu::CERO) //If the pivot of this column is zero.
              {
                this->v = -1;
                if(this->verbosity)
		  std::cerr << "Singular matrix" << std::endl;
                return false;
              }
            //aplica_pivote(ic->first);
            aplica_pivote(ic);
          }
        this->v=p;
        this->desc= 1;
        return true;
      }
  };

#endif
