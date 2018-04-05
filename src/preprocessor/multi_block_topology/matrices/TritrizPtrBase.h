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
//TritrizPtrBase.h
//Pointers in an array of matrices

#ifndef TRITRIZPTRBASE_H
#define TRITRIZPTRBASE_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/geom/pos_vec/ConstRefCajaTritriz.h"
#include "xc_utils/src/geom/pos_vec/ConstRefCapaICte.h"
#include "xc_utils/src/geom/pos_vec/ConstRefCapaJCte.h"
#include "xc_utils/src/geom/pos_vec/ConstRefCapaKCte.h"
#include "xc_utils/src/geom/pos_vec/ConstRefFilaI.h"
#include "xc_utils/src/geom/pos_vec/ConstRefFilaJ.h"
#include "xc_utils/src/geom/pos_vec/ConstRefFilaK.h"
#include "xc_utils/src/geom/pos_vec/VarRefCajaTritriz.h"
#include "xc_utils/src/geom/pos_vec/VarRefCapaICte.h"
#include "xc_utils/src/geom/pos_vec/VarRefCapaJCte.h"
#include "xc_utils/src/geom/pos_vec/VarRefCapaKCte.h"
#include "xc_utils/src/geom/pos_vec/VarRefFilaI.h"
#include "xc_utils/src/geom/pos_vec/VarRefFilaJ.h"
#include "xc_utils/src/geom/pos_vec/VarRefFilaK.h"

namespace XC{

//! @ingroup MultiBlockTopologyMR
//! 
//! @brief "Tritriz" de pointers to objects.
template <class MatrizPtr>
class TritrizPtrBase: public std::vector<MatrizPtr>, public EntCmd
  {
  public:
    typedef typename MatrizPtr::value_type value_type;
    typedef typename MatrizPtr::reference reference;
    typedef typename MatrizPtr::const_reference const_reference;
    typedef typename std::vector<MatrizPtr>::iterator iterator;
    typedef typename std::vector<MatrizPtr>::const_iterator const_iterator;

    typedef ConstRefCajaTritriz<TritrizPtrBase<MatrizPtr> > const_ref_caja;
    typedef ConstRefCapaICte<TritrizPtrBase<MatrizPtr> > const_ref_capa_i_cte;
    typedef ConstRefCapaJCte<TritrizPtrBase<MatrizPtr> > const_ref_capa_j_cte;
    typedef ConstRefCapaKCte<TritrizPtrBase<MatrizPtr> > const_ref_capa_k_cte;
    typedef ConstRefFilaI<TritrizPtrBase<MatrizPtr> > const_ref_fila_i;
    typedef ConstRefFilaJ<TritrizPtrBase<MatrizPtr> > const_ref_fila_j;
    typedef ConstRefFilaK<TritrizPtrBase<MatrizPtr> > const_ref_fila_k;

    typedef VarRefCajaTritriz<TritrizPtrBase<MatrizPtr> > var_ref_caja;
    typedef VarRefCapaICte<TritrizPtrBase<MatrizPtr> > var_ref_capa_i_cte;
    typedef VarRefCapaJCte<TritrizPtrBase<MatrizPtr> > var_ref_capa_j_cte;
    typedef VarRefCapaKCte<TritrizPtrBase<MatrizPtr> > var_ref_capa_k_cte;
    typedef VarRefFilaI<TritrizPtrBase<MatrizPtr> > var_ref_fila_i;
    typedef VarRefFilaJ<TritrizPtrBase<MatrizPtr> > var_ref_fila_j;
    typedef VarRefFilaK<TritrizPtrBase<MatrizPtr> > var_ref_fila_k;
  protected:
    void set_owner_matrices(void);

  public:
    TritrizPtrBase(const size_t &capas= 0);
    TritrizPtrBase(const size_t &capas,const MatrizPtr &);
    TritrizPtrBase(const size_t &,const size_t &,const size_t &);

    bool check_range(const size_t &,const size_t &,const size_t &) const;
    void resize(const size_t &);
    void resize(const size_t &,const size_t &,const size_t &,const value_type &v= value_type());
    size_t GetCapas(void) const;
    size_t getNumFilas(void) const;
    size_t getNumCols(void) const;
    size_t GetDim(void) const;
    bool EsFilaI(void) const;
    bool EsFilaJ(void) const;
    bool EsFilaK(void) const;
    bool EsCapaICte(void) const;
    bool EsCapaJCte(void) const;
    bool EsCapaKCte(void) const;
    size_t NumPtrs(void) const;
    bool Null(void) const;
    bool HasNull(void) const;
    void clearAll(void);

    void dim(const size_t &,const size_t &,const size_t &);

    const MatrizPtr &operator()(const size_t &capa) const;
    MatrizPtr &operator()(const size_t &capa);
    inline const_reference operator()(const size_t &i,const size_t &j,const size_t &k) const
      { return ((*this)(i))(j,k); }
    inline reference operator()(const size_t &i,const size_t &j,const size_t &k)
      { return ((*this)(i))(j,k); }
    reference getAtI(const size_t &i);
    const_reference getAtI(const size_t &) const;
    reference getAtIJ(const size_t &,const size_t &);
    const_reference getAtIJ(const size_t &,const size_t &) const;
    value_type getAtIJK(const size_t &,const size_t &,const size_t &);

    const_ref_caja GetConstRefCaja(size_t capa=1,size_t f=1, size_t c=1) const;
    const_ref_caja GetConstRefCaja(const RangoTritriz &rango) const;

    const_ref_capa_i_cte GetConstRefCapaICte(size_t capa=1,size_t f=1, size_t c=1) const;
    const_ref_capa_i_cte GetConstRefCapaICte(size_t capa,const RangoIndice &,const RangoIndice &) const;
    const_ref_capa_j_cte GetConstRefCapaJCte(size_t f=1,size_t capa=1, size_t c=1) const;
    const_ref_capa_j_cte GetConstRefCapaJCte(const RangoIndice &,const size_t &f,const RangoIndice &) const;
    const_ref_capa_k_cte GetConstRefCapaKCte(size_t c=1,size_t capa=1, size_t f=1) const;
    const_ref_capa_k_cte GetConstRefCapaKCte(const RangoIndice &,const RangoIndice &,const size_t &) const;

    const_ref_fila_i GetConstRefFilaI(size_t f=1,size_t c=1) const;
    const_ref_fila_i GetConstRefFilaI(const RangoIndice &,const size_t &f,const size_t &c) const;
    const_ref_fila_j GetConstRefFilaJ(size_t capa=1,size_t c=1) const;
    const_ref_fila_j GetConstRefFilaJ(const size_t &capa,const RangoIndice &,const size_t &c) const;
    const_ref_fila_k GetConstRefFilaK(size_t capa=1,size_t f=1) const;
    const_ref_fila_k GetConstRefFilaK(const size_t &capa,const size_t &f,const RangoIndice &) const;

    var_ref_caja GetVarRefCaja(size_t capa=1,size_t f=1, size_t c=1);
    var_ref_caja GetVarRefCaja(const RangoTritriz &);

    var_ref_capa_i_cte GetVarRefCapaICte(size_t capa=1,size_t f=1, size_t c=1);
    var_ref_capa_i_cte GetVarRefCapaICte(const size_t &capa,const RangoIndice &,const RangoIndice &);
    var_ref_capa_j_cte GetVarRefCapaJCte(size_t f=1,size_t capa=1, size_t c=1);
    var_ref_capa_j_cte GetVarRefCapaJCte(const RangoIndice &,const size_t &f,const RangoIndice &);
    var_ref_capa_k_cte GetVarRefCapaKCte(size_t c=1,size_t capa=1, size_t f=1);
    var_ref_capa_k_cte GetVarRefCapaKCte(const RangoIndice &,const RangoIndice &,const size_t &c);

    var_ref_fila_i GetVarRefFilaI(size_t f=1,size_t c=1);
    var_ref_fila_i GetVarRefFilaI(const RangoIndice &,const size_t &f,const size_t &c);
    var_ref_fila_j GetVarRefFilaJ(size_t capa=1,size_t c=1);
    var_ref_fila_j GetVarRefFilaJ(const size_t &capa,const RangoIndice &,const size_t &c);
    var_ref_fila_k GetVarRefFilaK(size_t capa=1,size_t f=1);
    var_ref_fila_k GetVarRefFilaK(const size_t &capa,const size_t &f,const RangoIndice &);

    void PutCaja(size_t offset_capa,size_t offset_fila,size_t offset_col,const TritrizPtrBase<MatrizPtr> &otra);

    std::vector<int> getTagsObjsCapa(const size_t &);
    std::vector<int> getTagsObjsFila(const size_t &,const size_t &);
    std::vector<int> getTagsObjsFilas(const size_t &);
    std::vector<int> getTagsObjsColumna(const size_t &,const size_t &);
    std::vector<int> getTagsObjsColumnas(const size_t &);
    std::vector<int> getTagsObjsInterioresCapa(const size_t &);
    std::vector<int> getTagsObjsInterioresFila(const size_t &,const size_t &);
    std::vector<int> getTagsObjsInterioresFilas(const size_t &);
    std::vector<int> getTagsObjsInterioresColumna(const size_t &,const size_t &);
    std::vector<int> getTagsObjsInterioresColumnas(const size_t &);
    std::vector<int> getTagsInteriorObjs(void);
  };

//! @brief Default constructor.
template <class MatrizPtr>
TritrizPtrBase<MatrizPtr>::TritrizPtrBase(const size_t &capas)
  : std::vector<MatrizPtr>(capas), EntCmd() 
  { set_owner_matrices(); }

//! @brief Constructor.
template <class MatrizPtr>
TritrizPtrBase<MatrizPtr>::TritrizPtrBase(const size_t &capas,const MatrizPtr &m)
  : std::vector<MatrizPtr>(capas,m), EntCmd()
  { set_owner_matrices(); }

//! @brief Constructor.
template <class MatrizPtr>
TritrizPtrBase<MatrizPtr>::TritrizPtrBase(const size_t &capas,const size_t &filas,const size_t &cols)
  : std::vector<MatrizPtr>(capas), EntCmd()
  {
    for(size_t i=0;i<capas;i++)
      (*this)[i]= MatrizPtr(filas,cols);
    set_owner_matrices();
  }

//! @brief Sets the owner for the matrices.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::set_owner_matrices(void)
  {
    for(iterator i=this->begin();i!=this->end();i++)
      i->set_owner(this);
  }

//! @brief Matrix dimensions.
template <class T>
void XC::TritrizPtrBase<T>::dim(const size_t &nLayers,const size_t &nRows,const size_t &nCols)
  { this->resize(nLayers,nRows,nCols,nullptr); }

//! @brief Cambia el size of tritriz.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::resize(const size_t &capas)
  {
    std::vector<MatrizPtr>::resize(capas);
    set_owner_matrices();
  }

//! @brief Cambia el size of tritriz.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::resize(const size_t &capas,const size_t &filas,const size_t &cols,const value_type &v)
  {
    std::vector<MatrizPtr>::resize(capas);
    for(size_t i= 0;i<capas;i++)
      (*this)[i].resize(filas,cols,v);
    set_owner_matrices();
  }

template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::check_range(const size_t &capa,const size_t &fila,const size_t &col) const
  {
    if(capa<=GetCapas())
      return TritrizPtrBase<MatrizPtr>::operator()(capa).CheckIndices(fila,col);
    else
      return false;
  }


template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::GetCapas(void) const
  { return this->size(); }

template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::getNumFilas(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumFilas();
    return retval;
  }
template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::getNumCols(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumCols();
    return retval;
  }
template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::GetDim(void) const
  {
    size_t retval= 0;
    if(this->size()>1) retval++;
    if(this->getNumFilas()>1) retval++;
    if(this->getNumCols()>1) retval++;
    return retval;
  }

//! @brief Return the number of elements de la tritriz.
template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::NumPtrs(void) const
  {
    const size_t sz= this->size();
    if(sz<1)
      return 0;
    else
      {
        const MatrizPtr &capa= (*this)(1); 
        return sz*capa.getNumFilas()*capa.getNumCols();
      }
  }

//! @brief Returns true if it's empty or the pointers are null.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::Null(void) const
  {
    if(this->empty())
      return true;
    else
      return (*this)[0].Null();
  }

//! @brief Returns true if it's empty or some of the pointers are null.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::HasNull(void) const
  {
    bool retval= false;
    if(this->empty())
      retval= true;
    else
      {
        const size_t ncapas= this->GetCapas();
        for(size_t i=0;i<ncapas;i++)
          if((*this)[i].Null())
            {
              retval= true;
              break;
            }
      }
    return retval;
  }

//! @brief Clears all.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::clearAll(void)
  {
    std::vector<MatrizPtr>::clear();
    EntCmd::clearPyProps();
  }


//! @brief Returns true if only index I varies.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::EsFilaI(void) const
  {
    if((this->getNumFilas()>1) || (this->getNumCols()>1)) return false;
    return true;
  }

//! @brief Returns true if only index J varies.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::EsFilaJ(void) const
  {
    if((this->GetCapas()>1) || (this->getNumCols()>1)) return false;
    return true;
  }

//! @brief Returns true if only index K varies.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::EsFilaK(void) const
  {
    if((this->GetCapas()>1) || (this->getNumFilas()>1)) return false;
    return true;
  }

//! @brief Returns true if the indexes that vary are los J y K.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::EsCapaICte(void) const
  { return (this->GetCapas()==1); }

//! @brief Returns true if the indexes that vary are los I y K.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::EsCapaJCte(void) const
  { return (this->getNumFilas()==1); }

//! @brief Returns true if the indexes that vary are los I y J.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::EsCapaKCte(void) const
  { return (this->getNumCols()==1); }

//! @brief Return the capa which index is being passed as parameter.
template <class MatrizPtr>
const MatrizPtr &TritrizPtrBase<MatrizPtr>::operator()(const size_t &capa) const
  { return (*this)[capa-1]; }

//! @brief Return the capa which index is being passed as parameter.
template <class MatrizPtr>
MatrizPtr &TritrizPtrBase<MatrizPtr>::operator()(const size_t &capa)
  { return (*this)[capa-1]; }


template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::reference TritrizPtrBase<MatrizPtr>::getAtI(const size_t &i)
  {  return const_cast<reference>(static_cast<const TritrizPtrBase<MatrizPtr> &>(*this).getAtI(i)); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_reference TritrizPtrBase<MatrizPtr>::getAtI(const size_t &i) const
  {
    if(EsFilaI())
      return operator()(i,1,1);
    else if(EsFilaJ())
      return operator()(1,i,1);
    else if(EsFilaK())
      return operator()(1,1,i);
    else
      {
	std::cerr << "TritrizPtrBase::getAtI; the set is not one-dimensional." << std::endl;
        return operator()(1,1,1);
      }
  }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::value_type TritrizPtrBase<MatrizPtr>::getAtIJK(const size_t &i,const size_t &j,const size_t &k)
  {
    typename TritrizPtrBase<MatrizPtr>::value_type retval= nullptr;
    if(this->check_range(i,j,k))
      retval= (*this)(i,j,k);
    return retval;
  }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::reference TritrizPtrBase<MatrizPtr>::getAtIJ(const size_t &i, const size_t &j)
  {  return const_cast<reference>(static_cast<const TritrizPtrBase<MatrizPtr> &>(*this).getAtIJ(i)); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_reference TritrizPtrBase<MatrizPtr>::getAtIJ(const size_t &i, const size_t &j) const
  {
    //XXX We still need to get the layer
    // (It will not necessarily be the first one).
    if(EsCapaICte())
      return operator()(1,i,j);
    else if(EsCapaJCte()) 
      return operator()(i,1,j);
    else if(EsCapaKCte()) 
      return operator()(i,j,1);
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; the node set is not bidimensional." << std::endl;
        return nullptr;
      } 
  }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_caja TritrizPtrBase<MatrizPtr>::GetConstRefCaja(size_t capa,size_t f, size_t c) const
  { return const_ref_caja(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_caja TritrizPtrBase<MatrizPtr>::GetConstRefCaja(const RangoTritriz &rango) const
  { return const_ref_caja(*this,rango); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_capa_i_cte TritrizPtrBase<MatrizPtr>::GetConstRefCapaICte(size_t capa,size_t f, size_t c) const
  { return const_ref_capa_i_cte(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_capa_i_cte TritrizPtrBase<MatrizPtr>::GetConstRefCapaICte(size_t capa,const RangoIndice &rango_filas,const RangoIndice &rango_cols) const
  { return const_ref_capa_i_cte(*this,capa,rango_filas,rango_cols); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_capa_j_cte TritrizPtrBase<MatrizPtr>::GetConstRefCapaJCte(size_t f,size_t capa, size_t c) const
  { return const_ref_capa_j_cte(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_capa_j_cte TritrizPtrBase<MatrizPtr>::GetConstRefCapaJCte(const RangoIndice &rango_capas,const size_t &f,const RangoIndice &rango_cols) const
  { return const_ref_capa_j_cte(*this,rango_capas,f,rango_cols); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_capa_k_cte TritrizPtrBase<MatrizPtr>::GetConstRefCapaKCte(size_t c,size_t capa, size_t f) const
  { return const_ref_capa_k_cte(*this,c,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_capa_k_cte TritrizPtrBase<MatrizPtr>::GetConstRefCapaKCte(const RangoIndice &rango_capas, const RangoIndice &rango_filas, const size_t &c) const
  { return const_ref_capa_k_cte(*this,rango_capas,rango_filas,c); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_fila_i TritrizPtrBase<MatrizPtr>::GetConstRefFilaI(size_t f,size_t c) const
  { return const_ref_fila_i(*this,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_fila_i TritrizPtrBase<MatrizPtr>::GetConstRefFilaI(const RangoIndice &rango_capas,const size_t &f,const size_t &c) const
  { return const_ref_fila_i(*this,rango_capas,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_fila_j TritrizPtrBase<MatrizPtr>::GetConstRefFilaJ(size_t capa,size_t c) const
  { return const_ref_fila_j(*this,capa,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_fila_j TritrizPtrBase<MatrizPtr>::GetConstRefFilaJ(const size_t &capa,const RangoIndice &rango_filas,const size_t &c) const
  { return const_ref_fila_j(*this,capa,rango_filas,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_fila_k TritrizPtrBase<MatrizPtr>::GetConstRefFilaK(size_t capa,size_t f) const
  { return const_ref_fila_k(*this,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_fila_k TritrizPtrBase<MatrizPtr>::GetConstRefFilaK(const size_t &capa,const size_t &f,const RangoIndice &rango_cols) const
  { return const_ref_fila_k(*this,capa,f,rango_cols); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_caja TritrizPtrBase<MatrizPtr>::GetVarRefCaja(size_t capa,size_t f, size_t c)
  { return var_ref_caja(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_caja TritrizPtrBase<MatrizPtr>::GetVarRefCaja(const RangoTritriz &rango)
  { return var_ref_caja(*this,rango); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_capa_i_cte TritrizPtrBase<MatrizPtr>::GetVarRefCapaICte(size_t capa,size_t f, size_t c)
  { return var_ref_capa_i_cte(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_capa_i_cte TritrizPtrBase<MatrizPtr>::GetVarRefCapaICte(const size_t &capa,const RangoIndice &rfilas,const RangoIndice &rcols)
  { return var_ref_capa_i_cte(*this,capa,rfilas,rcols); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_capa_j_cte TritrizPtrBase<MatrizPtr>::GetVarRefCapaJCte(size_t f,size_t capa, size_t c)
  { return var_ref_capa_j_cte(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_capa_j_cte TritrizPtrBase<MatrizPtr>::GetVarRefCapaJCte(const RangoIndice &rcapas,const size_t &f,const RangoIndice &rcols)
  { return var_ref_capa_j_cte(*this,rcapas,f,rcols); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_capa_k_cte TritrizPtrBase<MatrizPtr>::GetVarRefCapaKCte(size_t c,size_t capa, size_t f)
  { return var_ref_capa_k_cte(*this,c,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_capa_k_cte TritrizPtrBase<MatrizPtr>::GetVarRefCapaKCte(const RangoIndice &rcapas, const RangoIndice &rfilas,const size_t &c)
  { return var_ref_capa_k_cte(*this,rcapas,rfilas,c); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_fila_i TritrizPtrBase<MatrizPtr>::GetVarRefFilaI(size_t f,size_t c)
  { return var_ref_fila_i(*this,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_fila_i TritrizPtrBase<MatrizPtr>::GetVarRefFilaI(const RangoIndice &rcapas,const size_t &f,const size_t &c)
  { return var_ref_fila_i(*this,rcapas,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_fila_j TritrizPtrBase<MatrizPtr>::GetVarRefFilaJ(size_t capa,size_t c)
  { return var_ref_fila_j(*this,capa,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_fila_j TritrizPtrBase<MatrizPtr>::GetVarRefFilaJ(const size_t &capa,const RangoIndice &rfilas,const size_t &c)
  { return var_ref_fila_j(*this,capa,rfilas,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_fila_k TritrizPtrBase<MatrizPtr>::GetVarRefFilaK(size_t capa,size_t f)
  { return var_ref_fila_k(*this,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_fila_k TritrizPtrBase<MatrizPtr>::GetVarRefFilaK(const size_t &capa,const size_t &f,const RangoIndice &rcols)
  { return var_ref_fila_k(*this,capa,f,rcols); }

//! @brief Coloca en la tritriz la being passed as parameter.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::PutCaja(size_t offset_capa,size_t offset_fila,size_t offset_col,const TritrizPtrBase<MatrizPtr> &otra)
  {
    const size_t ncapas= otra.GetCapas();
    const size_t nfilas= otra.getNumFilas();
    const size_t ncols= otra.getNumCols();
    if((ncapas+offset_capa)>GetCapas())
      {
        std::cerr << "TritrizPtrNod::PutCaja; layer index out of range." << std::endl;
        return;
      }
    if((nfilas+offset_fila)>this->getNumFilas())
      {
        std::cerr << "TritrizPtrNod::PutCaja; row index out of range." << std::endl;
        return;
      }
    if((ncols+offset_col)>this->getNumCols())
      {
        std::cerr << "TritrizPtrNod::PutCaja; column index out of range." << std::endl;
        return;
      }
    for(size_t i=1;i<=ncapas;i++)
      for(size_t j=1;j<=nfilas;j++)
        for(size_t k=1;k<=ncols;k++)
          (*this)(offset_capa+i,offset_fila+j,offset_col+k)= otra(i,j,k);
  }


//! @brief Return the identifiers of the objects in the layer.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsCapa(const size_t &c)
  { return (*this)(c).getTagsObjs(); }

//! @brief Return the identifiers of the objects in the row.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsFila(const size_t &c,const size_t &f)
  {
    const size_t numCols= this->getNumCols();
    std::vector<int> retval;
    for(size_t k= 1;k<=numCols;k++)
      retval[k-1]= (*this)(c,f,k)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the f-th rows.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsFilas(const size_t &f)
  {
    const size_t numCapas= this->GetCapas();
    const size_t numCols= this->getNumCols();
    m_int retval(numCapas,numCols);
    for(size_t i= 1;i<=numCapas;i++)
      for(size_t k= 1;k<=numCols;k++)
        retval(i,k)= (*this)(i,f,k)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the column.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsColumna(const size_t &capa,const size_t &c)
  {
    const size_t numFilas= this->getNumFilas();
    std::vector<int> retval(numFilas);
    for(size_t j= 1;j<=numFilas;j++)
      retval[j-1]= (*this)(capa,j,c)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th columns.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsColumnas(const size_t &c)
  {
    const size_t numCapas= this->GetCapas();
    const size_t numFilas= this->getNumFilas();
    m_int retval(numCapas,numFilas);
    for(size_t i= 1;i<=numCapas;i++)
      for(size_t j= 1;j<=numFilas;j++)
        retval(i,j)= (*this)(i,j,c)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the layer interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsInterioresCapa(const size_t &c)
  { return (*this)(c).getObjsInteriores(); }

//! @brief Return the identifiers of the objects in the row interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsInterioresFila(const size_t &c,const size_t &f)
  {
    const size_t numCols= this->getNumCols();
    std::vector<int> retval(numCols);    
    for(size_t k= 1;k<=numCols;k++)
      retval[k-1]= (*this)(c,f,k)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th rows interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsInterioresFilas(const size_t &f)
  {
    const size_t numCapas= this->GetCapas();
    const size_t numCols= getNumCols();
    m_int retval(numCapas-2,numCols-2);
    for(size_t i= 2;i<numCapas;i++)
      for(size_t k= 2;k<numCols;k++)
        retval(i-1,k-1)= (*this)(i,f,k)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the c-th column interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsInterioresColumna(const size_t &capa,const size_t &c)
  {
    const size_t numFilas= this->getNumFilas();
    std::vector<int> retval(numFilas-2);
    for(size_t j= 2;j<numFilas;j++)
      retval[j-2]= (*this)(capa,j,c)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th columns interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsObjsInterioresColumnas(const size_t &c)
  {
    const size_t numCapas= this->GetCapas();
    const size_t numFilas= this->getNumFilas();
    m_int retval(numCapas-2,numFilas-2);
    for(size_t i= 2;i<numCapas;i++)
      for(size_t j= 2;j<numFilas;j++)
        retval(i-1,j-1)= (*this)(i,j,c)->getTag();
    return retval.getVector();
  }

//! @brief Return the tabs of the inerior elements.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsInteriorObjs(void)
  {
    std::cerr << "deprecated; use python" << std::endl;
    std::vector<int> retval;
    const size_t numCapas= this->GetCapas();
    if(numCapas==1)
      retval= (*this)(1).getTagsInteriorObjs();
    else
      {
        const size_t numFilas= this->getNumFilas();
        if(numFilas==1)
          retval= getTagsObjsInterioresFilas(1);
        else
          {
            const size_t numCols= this->getNumCols();
            if(numCols==1)
              retval= getTagsObjsInterioresColumnas(1);
            else
              for(size_t i= 2;i<numCapas;i++)
                {
		  std::vector<int> tmp= getTagsObjsInterioresCapa(i);
                  retval.insert(retval.end(),tmp.begin(),tmp.end());
                }
          }
      }
    return retval;
  }

} //end of XC namespace.

#endif
