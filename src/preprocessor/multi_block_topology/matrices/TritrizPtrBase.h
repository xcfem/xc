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
#include "xc_utils/src/geom/pos_vec/ConstantILayerConstRef.h"
#include "xc_utils/src/geom/pos_vec/ConstantJLayerConstRef.h"
#include "xc_utils/src/geom/pos_vec/ConstantKLayerConstRef.h"
#include "xc_utils/src/geom/pos_vec/IRowConstRef.h"
#include "xc_utils/src/geom/pos_vec/JRowConstRef.h"
#include "xc_utils/src/geom/pos_vec/KRowConstRef.h"
#include "xc_utils/src/geom/pos_vec/VarRefCajaTritriz.h"
#include "xc_utils/src/geom/pos_vec/ConstantILayerVarRef.h"
#include "xc_utils/src/geom/pos_vec/ConstantJLayerVarRef.h"
#include "xc_utils/src/geom/pos_vec/ConstantKLayerVarRef.h"
#include "xc_utils/src/geom/pos_vec/IRowVarRef.h"
#include "xc_utils/src/geom/pos_vec/JRowVarRef.h"
#include "xc_utils/src/geom/pos_vec/KRowVarRef.h"

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
    typedef ConstantILayerConstRef<TritrizPtrBase<MatrizPtr> > constant_i_layer_const_ref;
    typedef ConstantJLayerConstRef<TritrizPtrBase<MatrizPtr> > constant_j_layer_const_ref;
    typedef ConstantKLayerConstRef<TritrizPtrBase<MatrizPtr> > constant_k_layer_const_ref;
    typedef IRowConstRef<TritrizPtrBase<MatrizPtr> > const_ref_i_row;
    typedef JRowConstRef<TritrizPtrBase<MatrizPtr> > const_ref_j_row;
    typedef KRowConstRef<TritrizPtrBase<MatrizPtr> > const_ref_k_row;

    typedef VarRefCajaTritriz<TritrizPtrBase<MatrizPtr> > var_ref_caja;
    typedef ConstantILayerVarRef<TritrizPtrBase<MatrizPtr> > constant_i_layer_variable_ref;
    typedef ConstantJLayerVarRef<TritrizPtrBase<MatrizPtr> > constant_j_layer_variable_ref;
    typedef ConstantKLayerVarRef<TritrizPtrBase<MatrizPtr> > constant_k_layer_variable_ref;
    typedef IRowVarRef<TritrizPtrBase<MatrizPtr> > var_ref_i_row;
    typedef JRowVarRef<TritrizPtrBase<MatrizPtr> > var_ref_j_row;
    typedef KRowVarRef<TritrizPtrBase<MatrizPtr> > var_ref_k_row;
  protected:
    void set_owner_matrices(void);

  public:
    TritrizPtrBase(const size_t &n_layers= 0);
    TritrizPtrBase(const size_t &n_layers,const MatrizPtr &);
    TritrizPtrBase(const size_t &,const size_t &,const size_t &);

    bool check_range(const size_t &,const size_t &,const size_t &) const;
    void resize(const size_t &);
    void resize(const size_t &,const size_t &,const size_t &,const value_type &v= value_type());
    size_t getNumberOfLayers(void) const;
    size_t getNumberOfRows(void) const;
    size_t getNumberOfColumns(void) const;
    size_t GetDim(void) const;
    bool isIRow(void) const;
    bool isJRow(void) const;
    bool isKRow(void) const;
    bool isIConstantLayer(void) const;
    bool isJConstantLayer(void) const;
    bool isKConstantLayer(void) const;
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

    constant_i_layer_const_ref getConstantILayerConstRef(size_t capa=1,size_t f=1, size_t c=1) const;
    constant_i_layer_const_ref getConstantILayerConstRef(size_t capa,const RangoIndice &,const RangoIndice &) const;
    constant_j_layer_const_ref getConstantJLayerConstRef(size_t f=1,size_t capa=1, size_t c=1) const;
    constant_j_layer_const_ref getConstantJLayerConstRef(const RangoIndice &,const size_t &f,const RangoIndice &) const;
    constant_k_layer_const_ref getConstantKLayerConstRef(size_t c=1,size_t capa=1, size_t f=1) const;
    constant_k_layer_const_ref getConstantKLayerConstRef(const RangoIndice &,const RangoIndice &,const size_t &) const;

    const_ref_i_row getIRowConstRef(size_t f=1,size_t c=1) const;
    const_ref_i_row getIRowConstRef(const RangoIndice &,const size_t &f,const size_t &c) const;
    const_ref_j_row getJRowConstRef(size_t capa=1,size_t c=1) const;
    const_ref_j_row getJRowConstRef(const size_t &capa,const RangoIndice &,const size_t &c) const;
    const_ref_k_row getKRowConstRef(size_t capa=1,size_t f=1) const;
    const_ref_k_row getKRowConstRef(const size_t &capa,const size_t &f,const RangoIndice &) const;

    var_ref_caja GetVarRefCaja(size_t capa=1,size_t f=1, size_t c=1);
    var_ref_caja GetVarRefCaja(const RangoTritriz &);

    constant_i_layer_variable_ref getConstantILayerVarRef(size_t capa=1,size_t f=1, size_t c=1);
    constant_i_layer_variable_ref getConstantILayerVarRef(const size_t &capa,const RangoIndice &,const RangoIndice &);
    constant_j_layer_variable_ref getConstantJLayerVarRef(size_t f=1,size_t capa=1, size_t c=1);
    constant_j_layer_variable_ref getConstantJLayerVarRef(const RangoIndice &,const size_t &f,const RangoIndice &);
    constant_k_layer_variable_ref getConstantKLayerVarRef(size_t c=1,size_t capa=1, size_t f=1);
    constant_k_layer_variable_ref getConstantKLayerVarRef(const RangoIndice &,const RangoIndice &,const size_t &c);

    var_ref_i_row getVarRefIRow(size_t f=1,size_t c=1);
    var_ref_i_row getVarRefIRow(const RangoIndice &,const size_t &f,const size_t &c);
    var_ref_j_row getVarRefJRow(size_t capa=1,size_t c=1);
    var_ref_j_row getVarRefJRow(const size_t &capa,const RangoIndice &,const size_t &c);
    var_ref_k_row getVarRefKRow(size_t capa=1,size_t f=1);
    var_ref_k_row getVarRefKRow(const size_t &capa,const size_t &f,const RangoIndice &);

    void PutCaja(size_t offset_capa,size_t row_offset,size_t offset_col,const TritrizPtrBase<MatrizPtr> &otra);

    std::vector<int> getLayerObjectsTags(const size_t &);
    std::vector<int> getRowObjectsTags(const size_t &,const size_t &);
    std::vector<int> getRowsObjectsTags(const size_t &);
    std::vector<int> getColumnObjectsTags(const size_t &,const size_t &);
    std::vector<int> getColumnsObjectsTags(const size_t &);
    std::vector<int> getLayerInteriorObjectsTags(const size_t &);
    std::vector<int> getRowInteriorObjectsTags(const size_t &,const size_t &);
    std::vector<int> getRowsInteriorObjectsTags(const size_t &);
    std::vector<int> getColumnInteriorObjectsTags(const size_t &,const size_t &);
    std::vector<int> getColumnsInteriorObjectsTags(const size_t &);
    std::vector<int> getTagsInteriorObjs(void);
  };

//! @brief Default constructor.
template <class MatrizPtr>
TritrizPtrBase<MatrizPtr>::TritrizPtrBase(const size_t &n_layers)
  : std::vector<MatrizPtr>(n_layers), EntCmd() 
  { set_owner_matrices(); }

//! @brief Constructor.
template <class MatrizPtr>
TritrizPtrBase<MatrizPtr>::TritrizPtrBase(const size_t &n_layers,const MatrizPtr &m)
  : std::vector<MatrizPtr>(n_layers,m), EntCmd()
  { set_owner_matrices(); }

//! @brief Constructor.
template <class MatrizPtr>
TritrizPtrBase<MatrizPtr>::TritrizPtrBase(const size_t &n_layers,const size_t &n_rows,const size_t &cols)
  : std::vector<MatrizPtr>(n_layers), EntCmd()
  {
    for(size_t i=0;i<n_layers;i++)
      (*this)[i]= MatrizPtr(n_rows,cols);
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
void XC::TritrizPtrBase<T>::dim(const size_t &nLayers,const size_t &nRows,const size_t &numberOfColumns)
  { this->resize(nLayers,nRows,numberOfColumns,nullptr); }

//! @brief Cambia el size of tritriz.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::resize(const size_t &n_layers)
  {
    std::vector<MatrizPtr>::resize(n_layers);
    set_owner_matrices();
  }

//! @brief Cambia el size of tritriz.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::resize(const size_t &n_layers,const size_t &n_rows,const size_t &cols,const value_type &v)
  {
    std::vector<MatrizPtr>::resize(n_layers);
    for(size_t i= 0;i<n_layers;i++)
      (*this)[i].resize(n_rows,cols,v);
    set_owner_matrices();
  }

template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::check_range(const size_t &capa,const size_t &iRow,const size_t &iCol) const
  {
    if(capa<=getNumberOfLayers())
      return TritrizPtrBase<MatrizPtr>::operator()(capa).CheckIndices(iRow,iCol);
    else
      return false;
  }


template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::getNumberOfLayers(void) const
  { return this->size(); }

template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::getNumberOfRows(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumberOfRows();
    return retval;
  }
template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::getNumberOfColumns(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumberOfColumns();
    return retval;
  }
template <class MatrizPtr>
size_t TritrizPtrBase<MatrizPtr>::GetDim(void) const
  {
    size_t retval= 0;
    if(this->size()>1) retval++;
    if(this->getNumberOfRows()>1) retval++;
    if(this->getNumberOfColumns()>1) retval++;
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
        return sz*capa.getNumberOfRows()*capa.getNumberOfColumns();
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
        const size_t numberOfLayers= this->getNumberOfLayers();
        for(size_t i=0;i<numberOfLayers;i++)
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
bool TritrizPtrBase<MatrizPtr>::isIRow(void) const
  {
    if((this->getNumberOfRows()>1) || (this->getNumberOfColumns()>1)) return false;
    return true;
  }

//! @brief Returns true if only index J varies.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::isJRow(void) const
  {
    if((this->getNumberOfLayers()>1) || (this->getNumberOfColumns()>1)) return false;
    return true;
  }

//! @brief Returns true if only index K varies.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::isKRow(void) const
  {
    if((this->getNumberOfLayers()>1) || (this->getNumberOfRows()>1)) return false;
    return true;
  }

//! @brief Returns true if the indexes that vary are los J y K.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::isIConstantLayer(void) const
  { return (this->getNumberOfLayers()==1); }

//! @brief Returns true if the indexes that vary are los I y K.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::isJConstantLayer(void) const
  { return (this->getNumberOfRows()==1); }

//! @brief Returns true if the indexes that vary are los I y J.
template <class MatrizPtr>
bool TritrizPtrBase<MatrizPtr>::isKConstantLayer(void) const
  { return (this->getNumberOfColumns()==1); }

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
    if(isIRow())
      return operator()(i,1,1);
    else if(isJRow())
      return operator()(1,i,1);
    else if(isKRow())
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
    if(isIConstantLayer())
      return operator()(1,i,j);
    else if(isJConstantLayer()) 
      return operator()(i,1,j);
    else if(isKConstantLayer()) 
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
typename TritrizPtrBase<MatrizPtr>::constant_i_layer_const_ref TritrizPtrBase<MatrizPtr>::getConstantILayerConstRef(size_t capa,size_t f, size_t c) const
  { return constant_i_layer_const_ref(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_i_layer_const_ref TritrizPtrBase<MatrizPtr>::getConstantILayerConstRef(size_t capa,const RangoIndice &row_range,const RangoIndice &column_range) const
  { return constant_i_layer_const_ref(*this,capa,row_range,column_range); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_j_layer_const_ref TritrizPtrBase<MatrizPtr>::getConstantJLayerConstRef(size_t f,size_t capa, size_t c) const
  { return constant_j_layer_const_ref(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_j_layer_const_ref TritrizPtrBase<MatrizPtr>::getConstantJLayerConstRef(const RangoIndice &layer_range,const size_t &f,const RangoIndice &column_range) const
  { return constant_j_layer_const_ref(*this,layer_range,f,column_range); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_k_layer_const_ref TritrizPtrBase<MatrizPtr>::getConstantKLayerConstRef(size_t c,size_t capa, size_t f) const
  { return constant_k_layer_const_ref(*this,c,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_k_layer_const_ref TritrizPtrBase<MatrizPtr>::getConstantKLayerConstRef(const RangoIndice &layer_range, const RangoIndice &row_range, const size_t &c) const
  { return constant_k_layer_const_ref(*this,layer_range,row_range,c); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_i_row TritrizPtrBase<MatrizPtr>::getIRowConstRef(size_t f,size_t c) const
  { return const_ref_i_row(*this,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_i_row TritrizPtrBase<MatrizPtr>::getIRowConstRef(const RangoIndice &layer_range,const size_t &f,const size_t &c) const
  { return const_ref_i_row(*this,layer_range,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_j_row TritrizPtrBase<MatrizPtr>::getJRowConstRef(size_t capa,size_t c) const
  { return const_ref_j_row(*this,capa,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_j_row TritrizPtrBase<MatrizPtr>::getJRowConstRef(const size_t &capa,const RangoIndice &row_range,const size_t &c) const
  { return const_ref_j_row(*this,capa,row_range,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_k_row TritrizPtrBase<MatrizPtr>::getKRowConstRef(size_t capa,size_t f) const
  { return const_ref_k_row(*this,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::const_ref_k_row TritrizPtrBase<MatrizPtr>::getKRowConstRef(const size_t &capa,const size_t &f,const RangoIndice &column_range) const
  { return const_ref_k_row(*this,capa,f,column_range); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_caja TritrizPtrBase<MatrizPtr>::GetVarRefCaja(size_t capa,size_t f, size_t c)
  { return var_ref_caja(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_caja TritrizPtrBase<MatrizPtr>::GetVarRefCaja(const RangoTritriz &rango)
  { return var_ref_caja(*this,rango); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_i_layer_variable_ref TritrizPtrBase<MatrizPtr>::getConstantILayerVarRef(size_t capa,size_t f, size_t c)
  { return constant_i_layer_variable_ref(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_i_layer_variable_ref TritrizPtrBase<MatrizPtr>::getConstantILayerVarRef(const size_t &capa,const RangoIndice &row_range,const RangoIndice &rcols)
  { return constant_i_layer_variable_ref(*this,capa,row_range,rcols); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_j_layer_variable_ref TritrizPtrBase<MatrizPtr>::getConstantJLayerVarRef(size_t f,size_t capa, size_t c)
  { return constant_j_layer_variable_ref(*this,capa,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_j_layer_variable_ref TritrizPtrBase<MatrizPtr>::getConstantJLayerVarRef(const RangoIndice &layer_range,const size_t &f,const RangoIndice &rcols)
  { return constant_j_layer_variable_ref(*this,layer_range,f,rcols); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_k_layer_variable_ref TritrizPtrBase<MatrizPtr>::getConstantKLayerVarRef(size_t c,size_t capa, size_t f)
  { return constant_k_layer_variable_ref(*this,c,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::constant_k_layer_variable_ref TritrizPtrBase<MatrizPtr>::getConstantKLayerVarRef(const RangoIndice &layer_range, const RangoIndice &row_range,const size_t &c)
  { return constant_k_layer_variable_ref(*this,layer_range,row_range,c); }

template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_i_row TritrizPtrBase<MatrizPtr>::getVarRefIRow(size_t f,size_t c)
  { return var_ref_i_row(*this,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_i_row TritrizPtrBase<MatrizPtr>::getVarRefIRow(const RangoIndice &layer_range,const size_t &f,const size_t &c)
  { return var_ref_i_row(*this,layer_range,f,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_j_row TritrizPtrBase<MatrizPtr>::getVarRefJRow(size_t capa,size_t c)
  { return var_ref_j_row(*this,capa,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_j_row TritrizPtrBase<MatrizPtr>::getVarRefJRow(const size_t &capa,const RangoIndice &row_range,const size_t &c)
  { return var_ref_j_row(*this,capa,row_range,c); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_k_row TritrizPtrBase<MatrizPtr>::getVarRefKRow(size_t capa,size_t f)
  { return var_ref_k_row(*this,capa,f); }
template <class MatrizPtr>
typename TritrizPtrBase<MatrizPtr>::var_ref_k_row TritrizPtrBase<MatrizPtr>::getVarRefKRow(const size_t &capa,const size_t &f,const RangoIndice &rcols)
  { return var_ref_k_row(*this,capa,f,rcols); }

//! @brief Coloca en la tritriz la being passed as parameter.
template <class MatrizPtr>
void TritrizPtrBase<MatrizPtr>::PutCaja(size_t offset_capa,size_t row_offset,size_t offset_col,const TritrizPtrBase<MatrizPtr> &otra)
  {
    const size_t numberOfLayers= otra.getNumberOfLayers();
    const size_t numberOfRows= otra.getNumberOfRows();
    const size_t numberOfColumns= otra.getNumberOfColumns();
    if((numberOfLayers+offset_capa)>getNumberOfLayers())
      {
        std::cerr << "TritrizPtrNod::PutCaja; layer index out of range." << std::endl;
        return;
      }
    if((numberOfRows+row_offset)>this->getNumberOfRows())
      {
        std::cerr << "TritrizPtrNod::PutCaja; row index out of range." << std::endl;
        return;
      }
    if((numberOfColumns+offset_col)>this->getNumberOfColumns())
      {
        std::cerr << "TritrizPtrNod::PutCaja; column index out of range." << std::endl;
        return;
      }
    for(size_t i=1;i<=numberOfLayers;i++)
      for(size_t j=1;j<=numberOfRows;j++)
        for(size_t k=1;k<=numberOfColumns;k++)
          (*this)(offset_capa+i,row_offset+j,offset_col+k)= otra(i,j,k);
  }


//! @brief Return the identifiers of the objects in the layer.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getLayerObjectsTags(const size_t &c)
  { return (*this)(c).getTagsObjs(); }

//! @brief Return the identifiers of the objects in the row.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getRowObjectsTags(const size_t &c,const size_t &f)
  {
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval;
    for(size_t k= 1;k<=numCols;k++)
      retval[k-1]= (*this)(c,f,k)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the f-th rows.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getRowsObjectsTags(const size_t &f)
  {
    const size_t num_layers= this->getNumberOfLayers();
    const size_t numCols= this->getNumberOfColumns();
    m_int retval(num_layers,numCols);
    for(size_t i= 1;i<=num_layers;i++)
      for(size_t k= 1;k<=numCols;k++)
        retval(i,k)= (*this)(i,f,k)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the column.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getColumnObjectsTags(const size_t &capa,const size_t &c)
  {
    const size_t n_rows= this->getNumberOfRows();
    std::vector<int> retval(n_rows);
    for(size_t j= 1;j<=n_rows;j++)
      retval[j-1]= (*this)(capa,j,c)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th columns.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getColumnsObjectsTags(const size_t &c)
  {
    const size_t num_layers= this->getNumberOfLayers();
    const size_t n_rows= this->getNumberOfRows();
    m_int retval(num_layers,n_rows);
    for(size_t i= 1;i<=num_layers;i++)
      for(size_t j= 1;j<=n_rows;j++)
        retval(i,j)= (*this)(i,j,c)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the layer interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getLayerInteriorObjectsTags(const size_t &c)
  { return (*this)(c).getObjsInteriores(); }

//! @brief Return the identifiers of the objects in the row interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getRowInteriorObjectsTags(const size_t &c,const size_t &f)
  {
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval(numCols);    
    for(size_t k= 1;k<=numCols;k++)
      retval[k-1]= (*this)(c,f,k)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th rows interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getRowsInteriorObjectsTags(const size_t &f)
  {
    const size_t num_layers= this->getNumberOfLayers();
    const size_t numCols= getNumberOfColumns();
    m_int retval(num_layers-2,numCols-2);
    for(size_t i= 2;i<num_layers;i++)
      for(size_t k= 2;k<numCols;k++)
        retval(i-1,k-1)= (*this)(i,f,k)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the c-th column interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getColumnInteriorObjectsTags(const size_t &capa,const size_t &c)
  {
    const size_t n_rows= this->getNumberOfRows();
    std::vector<int> retval(n_rows-2);
    for(size_t j= 2;j<n_rows;j++)
      retval[j-2]= (*this)(capa,j,c)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th columns interior.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getColumnsInteriorObjectsTags(const size_t &c)
  {
    const size_t num_layers= this->getNumberOfLayers();
    const size_t n_rows= this->getNumberOfRows();
    m_int retval(num_layers-2,n_rows-2);
    for(size_t i= 2;i<num_layers;i++)
      for(size_t j= 2;j<n_rows;j++)
        retval(i-1,j-1)= (*this)(i,j,c)->getTag();
    return retval.getVector();
  }

//! @brief Return the tabs of the inerior elements.
template <class MatrizPtr>
std::vector<int> TritrizPtrBase<MatrizPtr>::getTagsInteriorObjs(void)
  {
    std::cerr << "deprecated; use python" << std::endl;
    std::vector<int> retval;
    const size_t num_layers= this->getNumberOfLayers();
    if(num_layers==1)
      retval= (*this)(1).getTagsInteriorObjs();
    else
      {
        const size_t n_rows= this->getNumberOfRows();
        if(n_rows==1)
          retval= getRowsInteriorObjectsTags(1);
        else
          {
            const size_t numCols= this->getNumberOfColumns();
            if(numCols==1)
              retval= getColumnsInteriorObjectsTags(1);
            else
              for(size_t i= 2;i<num_layers;i++)
                {
		  std::vector<int> tmp= getLayerInteriorObjectsTags(i);
                  retval.insert(retval.end(),tmp.begin(),tmp.end());
                }
          }
      }
    return retval;
  }

} //end of XC namespace.

#endif
