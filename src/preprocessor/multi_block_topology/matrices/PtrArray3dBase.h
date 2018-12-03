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
//PtrArray3dBase.h
//Pointers in an array of matrices

#ifndef PTRARRAY3DBASE_H
#define PTRARRAY3DBASE_H

#include "xc_utils/src/matrices/3d_arrays/Array3dBase.h"
#include "xc_utils/src/kernel/CommandEntity.h"

namespace XC{

//! @ingroup MultiBlockTopologyMR
//! 
//! @brief Three-dimensional array of object pointers.
template <class PtrArray>
class PtrArray3dBase: public Array3dBase<PtrArray>, public CommandEntity
  {
  protected:
    void set_owner_matrices(void);

  public:
    typedef typename Array3dBase<PtrArray>::value_type value_type;
    typedef typename Array3dBase<PtrArray>::iterator iterator;
    PtrArray3dBase(const size_t &n_layers= 0);
    PtrArray3dBase(const size_t &n_layers,const PtrArray &);
    PtrArray3dBase(const size_t &,const size_t &,const size_t &);

    void resize(const size_t &);
    void resize(const size_t &,const size_t &,const size_t &,const value_type &v=  value_type());
    size_t NumPtrs(void) const;
    void clearAll(void);

    void dim(const size_t &,const size_t &,const size_t &);

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
template <class PtrArray>
PtrArray3dBase<PtrArray>::PtrArray3dBase(const size_t &n_layers)
  : Array3dBase<PtrArray>(n_layers), CommandEntity() 
  { set_owner_matrices(); }

//! @brief Constructor.
template <class PtrArray>
PtrArray3dBase<PtrArray>::PtrArray3dBase(const size_t &n_layers,const PtrArray &m)
  : Array3dBase<PtrArray>(n_layers,m), CommandEntity()
  { set_owner_matrices(); }

//! @brief Constructor.
template <class PtrArray>
PtrArray3dBase<PtrArray>::PtrArray3dBase(const size_t &n_layers,const size_t &n_rows,const size_t &n_cols)
  : Array3dBase<PtrArray>(n_layers,n_rows,n_cols,nullptr), CommandEntity()
  {
    set_owner_matrices();
  }

//! @brief Sets the owner for the matrices.
template <class PtrArray>
void PtrArray3dBase<PtrArray>::set_owner_matrices(void)
  {
    for(iterator i=this->begin();i!=this->end();i++)
      i->set_owner(this);
  }

//! @brief Matrix dimensions.
template <class PtrArray>
void XC::PtrArray3dBase<PtrArray>::dim(const size_t &nLayers,const size_t &nRows,const size_t &numberOfColumns)
  { Array3dBase<PtrArray>::dim(nLayers,nRows,numberOfColumns,nullptr); }

//! @brief Resize the array.
template <class PtrArray>
void PtrArray3dBase<PtrArray>::resize(const size_t &n_layers)
  {
    Array3dBase<PtrArray>::resize(n_layers);
    set_owner_matrices();
  }
//! @brief Resize the array.
template <class PtrArray>
void PtrArray3dBase<PtrArray>::resize(const size_t &n_layers,const size_t &n_rows,const size_t &n_cols,const value_type &v)
  {
    Array3dBase<PtrArray>::resize(n_layers,n_rows,n_cols);
    set_owner_matrices();
  }

//! @brief Return the number of elements of the array.
template <class PtrArray>
size_t PtrArray3dBase<PtrArray>::NumPtrs(void) const
  { return Array3dBase<PtrArray>::getNumberOfComponents(); }

//! @brief Clears all.
template <class PtrArray>
void PtrArray3dBase<PtrArray>::clearAll(void)
  {
    Array3dBase<PtrArray>::clear();
    CommandEntity::clearPyProps();
  }


//! @brief Return the identifiers of the objects in the layer.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getLayerObjectsTags(const size_t &c)
  { return (*this)(c).getTagsObjs(); }

//! @brief Return the identifiers of the objects in the row.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getRowObjectsTags(const size_t &c,const size_t &f)
  {
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval;
    for(size_t k= 1;k<=numCols;k++)
      retval[k-1]= (*this)(c,f,k)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the f-th rows.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getRowsObjectsTags(const size_t &f)
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
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getColumnObjectsTags(const size_t &layer,const size_t &c)
  {
    const size_t n_rows= this->getNumberOfRows();
    std::vector<int> retval(n_rows);
    for(size_t j= 1;j<=n_rows;j++)
      retval[j-1]= (*this)(layer,j,c)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th columns.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getColumnsObjectsTags(const size_t &c)
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
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getLayerInteriorObjectsTags(const size_t &c)
  { return (*this)(c).getObjsInteriores(); }

//! @brief Return the identifiers of the objects in the row interior.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getRowInteriorObjectsTags(const size_t &c,const size_t &f)
  {
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval(numCols);    
    for(size_t k= 1;k<=numCols;k++)
      retval[k-1]= (*this)(c,f,k)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th rows interior.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getRowsInteriorObjectsTags(const size_t &f)
  {
    const size_t num_layers= this->getNumberOfLayers();
    const size_t numCols= this->getNumberOfColumns();
    m_int retval(num_layers-2,numCols-2);
    for(size_t i= 2;i<num_layers;i++)
      for(size_t k= 2;k<numCols;k++)
        retval(i-1,k-1)= (*this)(i,f,k)->getTag();
    return retval.getVector();
  }

//! @brief Return the identifiers of the objects in the c-th column interior.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getColumnInteriorObjectsTags(const size_t &layer,const size_t &c)
  {
    const size_t n_rows= this->getNumberOfRows();
    std::vector<int> retval(n_rows-2);
    for(size_t j= 2;j<n_rows;j++)
      retval[j-2]= (*this)(layer,j,c)->getTag();
    return retval;
  }

//! @brief Return the identifiers of the objects in the c-th columns interior.
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getColumnsInteriorObjectsTags(const size_t &c)
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
template <class PtrArray>
std::vector<int> PtrArray3dBase<PtrArray>::getTagsInteriorObjs(void)
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
