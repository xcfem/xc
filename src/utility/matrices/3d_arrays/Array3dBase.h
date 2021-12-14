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
//Array3dBase.h
//Array of point matrices.

#ifndef ARRAY3DBASE_H
#define ARRAY3DBASE_H

#include <iostream>
#include "Array3dBoxConstRef.h"
#include "ConstantILayerConstRef.h"
#include "ConstantJLayerConstRef.h"
#include "ConstantKLayerConstRef.h"
#include "IRowConstRef.h"
#include "JRowConstRef.h"
#include "KRowConstRef.h"
#include "Array3dBoxVarRef.h"
#include "ConstantILayerVarRef.h"
#include "ConstantJLayerVarRef.h"
#include "ConstantKLayerVarRef.h"
#include "IRowVarRef.h"
#include "JRowVarRef.h"
#include "KRowVarRef.h"



//! @defroup arrays3d
//! @ingroup arrays3d
//
//! @brief Base class for grids of bool in 3D (used to express if
//! something exists or not in a (layer,row,column).
template <class Array>
class Array3dBase: public std::vector<Array>
  {
  public:
    typedef typename Array::value_type value_type;
    typedef typename Array::reference reference;
    typedef typename Array::const_reference const_reference;
    typedef typename std::vector<Array>::value_type matrix_type;
    typedef typename std::vector<Array>::iterator iterator;
    typedef typename std::vector<Array>::const_iterator const_iterator;

    typedef Array3dBoxConstRef<Array3dBase<Array> > box_const_ref;
    typedef ConstantILayerConstRef<Array3dBase<Array> > constant_i_layer_const_ref;
    typedef ConstantJLayerConstRef<Array3dBase<Array> > constant_j_layer_const_ref;
    typedef ConstantKLayerConstRef<Array3dBase<Array> > constant_k_layer_const_ref;
    typedef IRowConstRef<Array3dBase<Array> > const_ref_i_row;
    typedef JRowConstRef<Array3dBase<Array> > const_ref_j_row;
    typedef KRowConstRef<Array3dBase<Array> > const_ref_k_row;

    typedef Array3dBoxVarRef<Array3dBase<Array> > box_var_ref;
    typedef ConstantILayerVarRef<Array3dBase<Array> > constant_i_layer_variable_ref;
    typedef ConstantJLayerVarRef<Array3dBase<Array> > constant_j_layer_variable_ref;
    typedef ConstantKLayerVarRef<Array3dBase<Array> > constant_k_layer_variable_ref;
    typedef IRowVarRef<Array3dBase<Array> > var_ref_i_row;
    typedef JRowVarRef<Array3dBase<Array> > var_ref_j_row;
    typedef KRowVarRef<Array3dBase<Array> > var_ref_k_row;

  protected:
    inline matrix_type &get_layer(const size_t &iLayer)
      { return (*this)[iLayer-1]; }
    inline const matrix_type &get_layer(const size_t &iLayer) const
      { return (*this)[iLayer-1]; }
  public:
    Array3dBase(const size_t &n_layers= 0);
    Array3dBase(const size_t &n_layers,const Array &);
    Array3dBase(const size_t &,const size_t &,const size_t &, const value_type &def_value= value_type());

    bool check_range(const size_t &,const size_t &,const size_t &) const;
    void resize(const size_t &);
    void resize(const size_t &,const size_t &,const size_t &,const value_type &def_value= value_type());
    size_t getNumberOfLayers(void) const;
    size_t getNumberOfRows(const size_t &layer= 1) const;
    size_t getNumberOfColumns(const size_t &layer= 1) const;
    size_t getNumberOfComponents(void) const;
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
    void clear(void);

    void dim(const size_t &,const size_t &,const size_t &, const value_type &default_value= value_type());

    const Array &operator()(const size_t &layer) const;
    Array &operator()(const size_t &layer);
    inline void assign(const size_t &i,const size_t &j,const size_t &k, const bool &value)
      { get_layer(i).assign(j,k,value); }
    inline const_reference operator()(const size_t &i,const size_t &j,const size_t &k) const
      { return ((*this)(i))(j,k); }
    inline reference operator()(const size_t &i,const size_t &j,const size_t &k)
      { return ((*this)(i))(j,k); }
    reference getAtI(const size_t &i);
    const_reference getAtI(const size_t &) const;
    reference getAtIJ(const size_t &,const size_t &);
    const_reference getAtIJ(const size_t &,const size_t &) const;
    value_type getAtIJK(const size_t &,const size_t &,const size_t &);

    box_const_ref getBoxConstRef(size_t layer=1,size_t f=1, size_t c=1) const;
    box_const_ref getBoxConstRef(const Array3dRange &rango) const;

    constant_i_layer_const_ref getConstantILayerConstRef(size_t layer=1,size_t f=1, size_t c=1) const;
    constant_i_layer_const_ref getConstantILayerConstRef(size_t layer,const RangoIndice &,const RangoIndice &) const;
    constant_j_layer_const_ref getConstantJLayerConstRef(size_t f=1,size_t layer=1, size_t c=1) const;
    constant_j_layer_const_ref getConstantJLayerConstRef(const RangoIndice &,const size_t &f,const RangoIndice &) const;
    constant_k_layer_const_ref getConstantKLayerConstRef(size_t c=1,size_t layer=1, size_t f=1) const;
    constant_k_layer_const_ref getConstantKLayerConstRef(const RangoIndice &,const RangoIndice &,const size_t &) const;

    const_ref_i_row getIRowConstRef(size_t f=1,size_t c=1) const;
    const_ref_i_row getIRowConstRef(const RangoIndice &,const size_t &f,const size_t &c) const;
    const_ref_j_row getJRowConstRef(size_t layer=1,size_t c=1) const;
    const_ref_j_row getJRowConstRef(const size_t &layer,const RangoIndice &,const size_t &c) const;
    const_ref_k_row getKRowConstRef(size_t layer=1,size_t f=1) const;
    const_ref_k_row getKRowConstRef(const size_t &layer,const size_t &f,const RangoIndice &) const;

    box_var_ref getBoxVarRef(size_t layer=1,size_t f=1, size_t c=1);
    box_var_ref getBoxVarRef(const Array3dRange &);

    constant_i_layer_variable_ref getConstantILayerVarRef(size_t layer=1,size_t f=1, size_t c=1);
    constant_i_layer_variable_ref getConstantILayerVarRef(const size_t &layer,const RangoIndice &,const RangoIndice &);
    constant_j_layer_variable_ref getConstantJLayerVarRef(size_t f=1,size_t layer=1, size_t c=1);
    constant_j_layer_variable_ref getConstantJLayerVarRef(const RangoIndice &,const size_t &f,const RangoIndice &);
    constant_k_layer_variable_ref getConstantKLayerVarRef(size_t c=1,size_t layer=1, size_t f=1);
    constant_k_layer_variable_ref getConstantKLayerVarRef(const RangoIndice &,const RangoIndice &,const size_t &c);

    var_ref_i_row getVarRefIRow(size_t f=1,size_t c=1);
    var_ref_i_row getVarRefIRow(const RangoIndice &,const size_t &f,const size_t &c);
    var_ref_j_row getVarRefJRow(size_t layer=1,size_t c=1);
    var_ref_j_row getVarRefJRow(const size_t &layer,const RangoIndice &,const size_t &c);
    var_ref_k_row getVarRefKRow(size_t layer=1,size_t f=1);
    var_ref_k_row getVarRefKRow(const size_t &layer,const size_t &f,const RangoIndice &);

    void putBox(size_t layer_offset,size_t row_offset,size_t offset_col,const Array3dBase<Array> &);

  };

//! @brief Default constructor.
template <class Array>
Array3dBase<Array>::Array3dBase(const size_t &n_layers)
  : std::vector<Array>(n_layers)
  {}

//! @brief Constructor.
template <class Array>
Array3dBase<Array>::Array3dBase(const size_t &n_layers,const Array &m)
  : std::vector<Array>(n_layers,m)
  {}

//! @brief Constructor.
template <class Array>
Array3dBase<Array>::Array3dBase(const size_t &n_layers,const size_t &n_rows,const size_t &cols, const value_type &def_value)
  : std::vector<Array>(n_layers)
  {
    for(size_t i=0;i<n_layers;i++)
      (*this)[i]= Array(n_rows,cols,def_value);
  }

//! @brief Matrix dimensions.
template <class T>
void Array3dBase<T>::dim(const size_t &nLayers,const size_t &nRows,const size_t &numberOfColumns, const value_type &def_value)
  { this->resize(nLayers,nRows,numberOfColumns,def_value); }

//! @brief Resize the array.
template <class Array>
void Array3dBase<Array>::resize(const size_t &n_layers)
  {
    std::vector<Array>::resize(n_layers);
  }

//! @brief Resize the array.
template <class Array>
void Array3dBase<Array>::resize(const size_t &n_layers,const size_t &n_rows,const size_t &cols,const value_type &v)
  {
    std::vector<Array>::resize(n_layers);
    for(size_t i= 0;i<n_layers;i++)
      (*this)[i].resize(n_rows,cols,v);
  }

template <class Array>
bool Array3dBase<Array>::check_range(const size_t &layer,const size_t &iRow,const size_t &iCol) const
  {
    if(layer<=getNumberOfLayers())
      return Array3dBase<Array>::operator()(layer).CheckIndices(iRow,iCol);
    else
      return false;
  }


template <class Array>
size_t Array3dBase<Array>::getNumberOfLayers(void) const
  { return this->size(); }

template <class Array>
size_t Array3dBase<Array>::getNumberOfRows(const size_t &layer) const
  {
    size_t retval= 0;
    if(this->size())
      retval= get_layer(layer).getNumberOfRows();
    return retval;
  }
template <class Array>
size_t Array3dBase<Array>::getNumberOfColumns(const size_t &layer) const
  {
    size_t retval= 0;
    if(this->size())
      retval= get_layer(layer).getNumberOfColumns();
    return retval;
  }

//! @brief Return the number of components in the container.
template <class Array>
size_t Array3dBase<Array>::getNumberOfComponents(void) const
  {
    const size_t sz= this->size();
    if(sz<1)
      return 0;
    else
      {
        const matrix_type &layer= (*this)(1);
        return sz*layer.getNumberOfRows()*layer.getNumberOfColumns();
      }
  }

template <class Array>
size_t Array3dBase<Array>::GetDim(void) const
  {
    size_t retval= 0;
    if(this->size()>1) retval++;
    if(this->getNumberOfRows()>1) retval++;
    if(this->getNumberOfColumns()>1) retval++;
    return retval;
  }

//! @brief Return the number of elements of the array.
template <class Array>
size_t Array3dBase<Array>::NumPtrs(void) const
  {
    const size_t sz= this->size();
    if(sz<1)
      return 0;
    else
      {
        const Array &layer= (*this)(1); 
        return sz*layer.getNumberOfRows()*layer.getNumberOfColumns();
      }
  }

//! @brief Returns true if it's empty or the pointers are null.
template <class Array>
bool Array3dBase<Array>::Null(void) const
  {
    if(this->empty())
      return true;
    else
      return (*this)[0].Null();
  }

//! @brief Returns true if it's empty or some of the pointers are null.
template <class Array>
bool Array3dBase<Array>::HasNull(void) const
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

//! @brief Clears container.
template <class Array>
void Array3dBase<Array>::clear(void)
  {
    std::vector<Array>::clear();
  }


//! @brief Returns true if only index I varies.
template <class Array>
bool Array3dBase<Array>::isIRow(void) const
  {
    if((this->getNumberOfRows()>1) || (this->getNumberOfColumns()>1)) return false;
    return true;
  }

//! @brief Returns true if only index J varies.
template <class Array>
bool Array3dBase<Array>::isJRow(void) const
  {
    if((this->getNumberOfLayers()>1) || (this->getNumberOfColumns()>1)) return false;
    return true;
  }

//! @brief Returns true if only index K varies.
template <class Array>
bool Array3dBase<Array>::isKRow(void) const
  {
    if((this->getNumberOfLayers()>1) || (this->getNumberOfRows()>1)) return false;
    return true;
  }

//! @brief Returns true if the indexes that vary are los J y K.
template <class Array>
bool Array3dBase<Array>::isIConstantLayer(void) const
  { return (this->getNumberOfLayers()==1); }

//! @brief Returns true if the indexes that vary are los I y K.
template <class Array>
bool Array3dBase<Array>::isJConstantLayer(void) const
  { return (this->getNumberOfRows()==1); }

//! @brief Returns true if the indexes that vary are los I y J.
template <class Array>
bool Array3dBase<Array>::isKConstantLayer(void) const
  { return (this->getNumberOfColumns()==1); }

//! @brief Return the i-th layer.
template <class Array>
const Array &Array3dBase<Array>::operator()(const size_t &i) const
  { return get_layer(i); }

//! @brief Return the i-th layer.
template <class Array>
Array &Array3dBase<Array>::operator()(const size_t &i)
  { return get_layer(i); }


template <class Array>
typename Array3dBase<Array>::reference Array3dBase<Array>::getAtI(const size_t &i)
  {  return const_cast<reference>(static_cast<const Array3dBase<Array> &>(*this).getAtI(i)); }

template <class Array>
typename Array3dBase<Array>::const_reference Array3dBase<Array>::getAtI(const size_t &i) const
  {
    if(isIRow())
      return operator()(i,1,1);
    else if(isJRow())
      return operator()(1,i,1);
    else if(isKRow())
      return operator()(1,1,i);
    else
      {
	std::cerr << "Array3dBase::getAtI; the set is not one-dimensional." << std::endl;
        return operator()(1,1,1);
      }
  }

template <class Array>
typename Array3dBase<Array>::value_type Array3dBase<Array>::getAtIJK(const size_t &i,const size_t &j,const size_t &k)
  {
    typename Array3dBase<Array>::value_type retval= value_type();
    if(this->check_range(i,j,k))
      retval= (*this)(i,j,k);
    return retval;
  }

template <class Array>
typename Array3dBase<Array>::reference Array3dBase<Array>::getAtIJ(const size_t &i, const size_t &j)
  {  return const_cast<reference>(static_cast<const Array3dBase<Array> &>(*this).getAtIJ(i)); }

template <class Array>
typename Array3dBase<Array>::const_reference Array3dBase<Array>::getAtIJ(const size_t &i, const size_t &j) const
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
	std::cerr << "Array3dBase::" << __FUNCTION__
	          << "; not bidimensional." << std::endl;
        return nullptr;
      } 
  }

template <class Array>
typename Array3dBase<Array>::box_const_ref Array3dBase<Array>::getBoxConstRef(size_t layer,size_t f, size_t c) const
  { return box_const_ref(*this,layer,f,c); }
template <class Array>
typename Array3dBase<Array>::box_const_ref Array3dBase<Array>::getBoxConstRef(const Array3dRange &rango) const
  { return box_const_ref(*this,rango); }

template <class Array>
typename Array3dBase<Array>::constant_i_layer_const_ref Array3dBase<Array>::getConstantILayerConstRef(size_t layer,size_t f, size_t c) const
  { return constant_i_layer_const_ref(*this,layer,f,c); }
template <class Array>
typename Array3dBase<Array>::constant_i_layer_const_ref Array3dBase<Array>::getConstantILayerConstRef(size_t layer,const RangoIndice &row_range,const RangoIndice &column_range) const
  { return constant_i_layer_const_ref(*this,layer,row_range,column_range); }
template <class Array>
typename Array3dBase<Array>::constant_j_layer_const_ref Array3dBase<Array>::getConstantJLayerConstRef(size_t f,size_t layer, size_t c) const
  { return constant_j_layer_const_ref(*this,layer,f,c); }
template <class Array>
typename Array3dBase<Array>::constant_j_layer_const_ref Array3dBase<Array>::getConstantJLayerConstRef(const RangoIndice &layer_range,const size_t &f,const RangoIndice &column_range) const
  { return constant_j_layer_const_ref(*this,layer_range,f,column_range); }
template <class Array>
typename Array3dBase<Array>::constant_k_layer_const_ref Array3dBase<Array>::getConstantKLayerConstRef(size_t c,size_t layer, size_t f) const
  { return constant_k_layer_const_ref(*this,c,layer,f); }
template <class Array>
typename Array3dBase<Array>::constant_k_layer_const_ref Array3dBase<Array>::getConstantKLayerConstRef(const RangoIndice &layer_range, const RangoIndice &row_range, const size_t &c) const
  { return constant_k_layer_const_ref(*this,layer_range,row_range,c); }

template <class Array>
typename Array3dBase<Array>::const_ref_i_row Array3dBase<Array>::getIRowConstRef(size_t f,size_t c) const
  { return const_ref_i_row(*this,f,c); }
template <class Array>
typename Array3dBase<Array>::const_ref_i_row Array3dBase<Array>::getIRowConstRef(const RangoIndice &layer_range,const size_t &f,const size_t &c) const
  { return const_ref_i_row(*this,layer_range,f,c); }
template <class Array>
typename Array3dBase<Array>::const_ref_j_row Array3dBase<Array>::getJRowConstRef(size_t layer,size_t c) const
  { return const_ref_j_row(*this,layer,c); }
template <class Array>
typename Array3dBase<Array>::const_ref_j_row Array3dBase<Array>::getJRowConstRef(const size_t &layer,const RangoIndice &row_range,const size_t &c) const
  { return const_ref_j_row(*this,layer,row_range,c); }
template <class Array>
typename Array3dBase<Array>::const_ref_k_row Array3dBase<Array>::getKRowConstRef(size_t layer,size_t f) const
  { return const_ref_k_row(*this,layer,f); }
template <class Array>
typename Array3dBase<Array>::const_ref_k_row Array3dBase<Array>::getKRowConstRef(const size_t &layer,const size_t &f,const RangoIndice &column_range) const
  { return const_ref_k_row(*this,layer,f,column_range); }

template <class Array>
typename Array3dBase<Array>::box_var_ref Array3dBase<Array>::getBoxVarRef(size_t layer,size_t f, size_t c)
  { return box_var_ref(*this,layer,f,c); }
template <class Array>
typename Array3dBase<Array>::box_var_ref Array3dBase<Array>::getBoxVarRef(const Array3dRange &rango)
  { return box_var_ref(*this,rango); }

template <class Array>
typename Array3dBase<Array>::constant_i_layer_variable_ref Array3dBase<Array>::getConstantILayerVarRef(size_t layer,size_t f, size_t c)
  { return constant_i_layer_variable_ref(*this,layer,f,c); }
template <class Array>
typename Array3dBase<Array>::constant_i_layer_variable_ref Array3dBase<Array>::getConstantILayerVarRef(const size_t &layer,const RangoIndice &row_range,const RangoIndice &rcols)
  { return constant_i_layer_variable_ref(*this,layer,row_range,rcols); }
template <class Array>
typename Array3dBase<Array>::constant_j_layer_variable_ref Array3dBase<Array>::getConstantJLayerVarRef(size_t f,size_t layer, size_t c)
  { return constant_j_layer_variable_ref(*this,layer,f,c); }
template <class Array>
typename Array3dBase<Array>::constant_j_layer_variable_ref Array3dBase<Array>::getConstantJLayerVarRef(const RangoIndice &layer_range,const size_t &f,const RangoIndice &rcols)
  { return constant_j_layer_variable_ref(*this,layer_range,f,rcols); }
template <class Array>
typename Array3dBase<Array>::constant_k_layer_variable_ref Array3dBase<Array>::getConstantKLayerVarRef(size_t c,size_t layer, size_t f)
  { return constant_k_layer_variable_ref(*this,c,layer,f); }
template <class Array>
typename Array3dBase<Array>::constant_k_layer_variable_ref Array3dBase<Array>::getConstantKLayerVarRef(const RangoIndice &layer_range, const RangoIndice &row_range,const size_t &c)
  { return constant_k_layer_variable_ref(*this,layer_range,row_range,c); }

template <class Array>
typename Array3dBase<Array>::var_ref_i_row Array3dBase<Array>::getVarRefIRow(size_t f,size_t c)
  { return var_ref_i_row(*this,f,c); }
template <class Array>
typename Array3dBase<Array>::var_ref_i_row Array3dBase<Array>::getVarRefIRow(const RangoIndice &layer_range,const size_t &f,const size_t &c)
  { return var_ref_i_row(*this,layer_range,f,c); }
template <class Array>
typename Array3dBase<Array>::var_ref_j_row Array3dBase<Array>::getVarRefJRow(size_t layer,size_t c)
  { return var_ref_j_row(*this,layer,c); }
template <class Array>
typename Array3dBase<Array>::var_ref_j_row Array3dBase<Array>::getVarRefJRow(const size_t &layer,const RangoIndice &row_range,const size_t &c)
  { return var_ref_j_row(*this,layer,row_range,c); }
template <class Array>
typename Array3dBase<Array>::var_ref_k_row Array3dBase<Array>::getVarRefKRow(size_t layer,size_t f)
  { return var_ref_k_row(*this,layer,f); }
template <class Array>
typename Array3dBase<Array>::var_ref_k_row Array3dBase<Array>::getVarRefKRow(const size_t &layer,const size_t &f,const RangoIndice &rcols)
  { return var_ref_k_row(*this,layer,f,rcols); }

//! @brief Put in the array the array argument.
template <class Array>
void Array3dBase<Array>::putBox(size_t layer_offset,size_t row_offset,size_t offset_col,const Array3dBase<Array> &other)
  {
    const size_t numberOfLayers= other.getNumberOfLayers();
    const size_t numberOfRows= other.getNumberOfRows();
    const size_t numberOfColumns= other.getNumberOfColumns();
    if((numberOfLayers+layer_offset)>getNumberOfLayers())
      {
        std::cerr << "Array3dBase::" << __FUNCTION__
	          << "; layer index out of range." << std::endl;
        return;
      }
    if((numberOfRows+row_offset)>this->getNumberOfRows())
      {
        std::cerr << "Array3dBase::" << __FUNCTION__
		  << "; row index out of range." << std::endl;
        return;
      }
    if((numberOfColumns+offset_col)>this->getNumberOfColumns())
      {
        std::cerr << "Array3dBase::" << __FUNCTION__
		  << "; column index out of range." << std::endl;
        return;
      }
    for(size_t i=1;i<=numberOfLayers;i++)
      for(size_t j=1;j<=numberOfRows;j++)
        for(size_t k=1;k<=numberOfColumns;k++)
          (*this)(layer_offset+i,row_offset+j,offset_col+k)= other(i,j,k);
  }

//! @brief Print stuff.
template <class Array>
inline std::ostream &operator<<(std::ostream &os,const Array3dBase<Array> &t)
  {
    const size_t n_layers= t.getNumberOfLayers();
    for(size_t i=1;i<=n_layers;i++)
      os << t(i);
    return os;
  }

#endif
