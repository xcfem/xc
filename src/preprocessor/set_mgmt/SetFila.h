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
//SetFila.h

#ifndef SETFILA_H
#define SETFILA_H

#include "SetEstruct.h"
#include "xc_utils/src/geom/pos_vec/RangoTritriz.h"

namespace XC {

class SFreedom_Constraint;

//!  \ingroup Set
//! 
//!  \brief Conjunto de objetos.
//! 
//!  Un objeto SetFila contiene un conjunto de 0 o más:
//!  - Nodos.
//!  - Elementos finitos.
//!  que corresponden a una fila_[ijk] de un objeto EntMdlr.
template <class FILATTZNOD,class FILATTZELEM>
class SetFila: public SetEstruct
  {
    FILATTZNOD fila_nod; //!< Referencia a nodos de una fila.
    FILATTZELEM fila_elem; //!< Referencia a elemento de una fila.
  public:
    typedef typename FILATTZNOD::reference reference_nod;
    typedef typename FILATTZNOD::const_reference const_reference_nod; 

    typedef typename FILATTZELEM::reference reference_elem;
    typedef typename FILATTZELEM::const_reference const_reference_elem; 
  public:
    SetFila(const FILATTZNOD &fn,const FILATTZELEM &fe,const std::string &nmb="",Preprocessor *preprocessor= NULL);
    SetFila(const SetFila &otro);
    SetFila &operator=(const SetFila &otro);
    virtual SetEstruct *getCopy(void) const;
    reference_nod Nodo(const size_t &i)
      { return fila_nod(i); }
    const reference_nod Nodo(const size_t &i) const
      { return fila_nod(i); }
    reference_elem Elemento(const size_t &i)
      { return fila_elem(i); }
    const reference_elem Elemento(const size_t &i) const
      { return fila_elem(i); }

    RangoTritriz RangoNodos(void) const
      { return RangoTritriz(fila_nod); }
    RangoTritriz RangoElementos(void) const
      { return RangoTritriz(fila_elem); }

    virtual size_t GetNumCapasNodos(void) const
      { return fila_nod.GetCapas(); }
    virtual size_t GetNumFilasNodos(void) const
      { return fila_nod.getNumFilas(); }
    virtual size_t GetNumColsNodos(void) const
      { return fila_nod.getNumCols(); }
    virtual size_t GetNumCapasElementos(void) const
      { return fila_elem.GetCapas(); }
    virtual size_t GetNumFilasElementos(void) const
      { return fila_elem.getNumFilas(); }
    virtual size_t GetNumColsElementos(void) const
      { return fila_elem.getNumCols(); }

    virtual Node *GetNodo(const size_t &i=1,const size_t &j=1,const size_t &k=1)
      { return fila_nod(i,j,k); }
    virtual const Node *GetNodo(const size_t &i=1,const size_t &j=1,const size_t &k=1) const
      { return fila_nod(i,j,k); }
    virtual Element *GetElemento(const size_t &i=1,const size_t &j=1,const size_t &k=1)
      { return fila_elem(i,j,k); }
    virtual const Element *GetElemento(const size_t &i=1,const size_t &j=1,const size_t &k=1) const
      { return fila_elem(i,j,k); }

    //void fix(int &tag_fix,const SFreedom_Constraint &);

  };

template <class FILATTZNOD,class FILATTZELEM>
SetFila<FILATTZNOD,FILATTZELEM>::SetFila(const FILATTZNOD &fn,const FILATTZELEM &fe,const std::string &nmb,Preprocessor *preprocessor)
  : SetEstruct(nmb,preprocessor), fila_nod(fn), fila_elem(fe) {}

template <class FILATTZNOD,class FILATTZELEM>
SetFila<FILATTZNOD,FILATTZELEM>::SetFila(const SetFila &otro)
  : SetEstruct(otro), fila_nod(otro.fila_nod), fila_elem(otro.fila_elem) {}

  //! @brief Operador asignación.
template <class FILATTZNOD,class FILATTZELEM>
SetFila<FILATTZNOD,FILATTZELEM> &XC::SetFila<FILATTZNOD,FILATTZELEM>::operator=(const SetFila &otro)
  {
    SetEstruct::operator=(otro);
    fila_nod= otro.fila_nod;
    fila_elem= otro.fila_elem;
    return *this;
  }

//! @brief Constructor virtual.
template <class FILATTZNOD,class FILATTZELEM>
SetEstruct *XC::SetFila<FILATTZNOD,FILATTZELEM>::getCopy(void) const
  { return new SetFila<FILATTZNOD,FILATTZELEM>(*this); }


/* //! @brief Impone desplazamiento nulo en los nodos de este conjunto. */
/* template <class FILATTZNOD,class FILATTZELEM> */
/* void XC::SetFila<FILATTZNOD,FILATTZELEM>::fix(const SFreedom_Constraint &spc) */
/*   { fix(fila_nod,spc); } */


} //end of XC namespace
#endif
