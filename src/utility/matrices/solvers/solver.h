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
//solver.h

#ifndef SOLVER_H
#define SOLVER_H

#include "utility/matrices/m_sizet.h"

class Solver
  {
  private:
    m_sizet PP;
  protected:
    size_t verbosity; //0: No mostrar ningún mensaje de error o aviso.
                      //1: Mostrar sólo los importantes.
                      //2: Mostrar todos los mensajes.
    size_t n;
    int desc;

    inline const size_t &P(size_t i) const
      { return PP(i); }
    inline size_t &P(size_t i)
      { return PP(i); }

    void inic_p(void);
    int check_pp(const size_t n_rows) const;
  public:
    Solver(const size_t &vrbrr= 1): PP(0,0), verbosity(vrbrr), n(0), desc(0) {}
    void SetVerbosity(const size_t &v)
      { verbosity= v; }
  };

template<class M>
class SolverM: public Solver
  {
  protected:
    M *A;

    typedef typename M::value_type tipo_val;
    static const tipo_val CERO;

    virtual bool decomp(void)= 0;

    inline int check_matrix(void) const
      {
        if(A->getNumberOfColumns() != A->getNumberOfRows())
          {
	    if(verbosity)
              std::cerr << "Not a square matrix" << std::endl;
	    return 0;
          }
        return 1;
      }

    template <class V>
    inline int check_backsub(const V &B) const
      {
        if(!check_matrix()) return 0;
        if(B.getNumberOfRows()!=A->getNumberOfRows() || B.getNumberOfColumns()!=1)
          {
	    if(verbosity)
	      std::cerr << "Matrix " << B
			<< " has a different row number than "
			<< A << std::endl;
	    return 0;
          }
        return check_pp(A->getNumberOfRows());
      }
  public:
    SolverM(const size_t &vrbrr= 1): Solver(vrbrr), A(NULL) {}
    void putMatrix(M &m)
      {
        A= &m;
        n= A->getNumberOfColumns();
        desc= 0;
      }
    bool Decomp(void)
      {
        if(!A)
          {
	    if(verbosity)
	      std::cerr << "There is no matrix to decompose." << std::endl;
            return false;
          }
        if(!desc)
          return decomp();
        else
          return true;
      }
    virtual ~SolverM(void) {}
  };


template<class M>
const typename SolverM<M>::tipo_val SolverM<M>::CERO= neutro_suma(typename SolverM<M>::tipo_val());


#endif
