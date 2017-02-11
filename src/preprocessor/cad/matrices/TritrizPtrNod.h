//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//TritrizPtrNod.h
//Vector de matrices de puntos

#ifndef TRITRIZPTRNOD_H
#define TRITRIZPTRNOD_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_basic/src/funciones/algebra/ExprAlgebra.h"
#include "MatrizPtrNod.h"
#include "TritrizPtrBase.h"
#include "utility/matrix/Vector.h"


class ExprAlgebra;
class Intervalo1D;
class Lista;
class RangoIndice;
class RangoTritriz;

namespace XC{

//! \ingroup CadMR
//! 
//! @brief "Tritriz" de pointers to elementos.
class TritrizPtrNod: public TritrizPtrBase<MatrizPtrNod>
  {
  protected:

  public:

    TritrizPtrNod(const size_t capas= 0);
    TritrizPtrNod(const size_t ,const size_t ,const size_t );

    Node *buscaNodo(const int &tag);
    const Node *buscaNodo(const int &tag) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;
    ID getNodeIndices(const Node *) const;

    template <class F>
    std::deque<double> IntegSimpsonFila(const F &,const std::string &,const size_t &,const ExprAlgebra &,const size_t &) const;
    Vector IntegSimpsonFilaI(const size_t &,const size_t &,const ExprAlgebra &,const size_t &n= 10) const;
    Vector IntegSimpsonFilaJ(const size_t &,const size_t &,const ExprAlgebra &,const size_t &n= 10) const;
    Vector IntegSimpsonFilaK(const size_t &,const size_t &,const ExprAlgebra &,const size_t &n= 10) const;

    void fix(const SFreedom_Constraint &) const;

    std::vector<int> getTags(void) const;


    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os,const TritrizPtrNod &);

template <class F>
std::deque<double> TritrizPtrNod::IntegSimpsonFila(const F &f,const std::string &nmb_coo,const size_t &num_coo,const ExprAlgebra &e,const size_t &n) const
  {
    const size_t sz= f.Size();
    std::deque<double> retval;
    if(sz<2) return retval;
    const Vector *p0= &(f(1)->getCrds());
    if(!p0) return retval;
    double s0= (*p0)(num_coo);
    for(size_t i=2;i<=sz;i++)
      {
        const Vector *p1= &(f(i)->getCrds());
        if(!p1) return retval;
        double s1= ((*p0)(num_coo)+(*p1)(num_coo))/2.0;
        Intervalo1D iv(nmb_coo,ExprAlgebra(s0),ExprAlgebra(s1),n);
        retval.push_back(e.IntegSimpson(iv).ToNum());
        s0= s1;
        p0= p1;
      }
    const Vector *p1= &(f(sz)->getCrds());
    if(!p1) return retval;
    double s1= (*p1)(num_coo);
    Intervalo1D iv(nmb_coo,ExprAlgebra(s0),ExprAlgebra(s1),n);
    retval.push_back(e.IntegSimpson(iv).ToNum());
    return retval;
  }

inline void fix(const TritrizPtrNod &ttz,const SFreedom_Constraint &spc)
  { ttz.fix(spc); }
void fix(const TritrizPtrNod::var_ref_caja &ref_caja,const SFreedom_Constraint &spc);

std::vector<int> getIdNodosQuad4N(const TritrizPtrNod::const_ref_capa_i_cte &nodos,const size_t &j,const size_t &k);
std::vector<int> getIdNodosQuad9N(const TritrizPtrNod::const_ref_capa_i_cte &nodos,const size_t &j,const size_t &k);

} //end of XC namespace.

#endif
