//----------------------------------------------------------------------------
//  biblioteca bibXCLcmd; bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Indices3dArray.h
//Vector de matrices de puntos

#ifndef TRITRIZINDICES_H
#define TRITRIZINDICES_H

#include "utility/matrices/IndicesMatrix.h"

//! @ingroup arrays3d
//
//! @brief Clase base para las «tritrices» de posiciones.
class Indices3dArray: public std::vector<IndicesMatrix >
  {
  public:
    typedef IndicesMatrix m_indices;
    typedef IndicesMatrix::reference reference;
    typedef IndicesMatrix::const_reference const_reference;

  protected:
    inline IndicesMatrix &get_capa(const size_t &capa)
      { return (*this)[capa-1]; }
    inline const IndicesMatrix &get_capa(const size_t &capa) const
      { return (*this)[capa-1]; }
  public:
    Indices3dArray(const size_t &capas= 1,const size_t &n_rows= 1,const size_t &n_cols= 1);
    inline size_t getNumberOfLayers(void) const
      { return this->size(); }
    size_t getNumberOfRows(void) const;
    size_t getNumberOfColumns(void) const;
    size_t NumIndices(void) const;

    inline const IndicesMatrix &operator()(const size_t &capa) const
      { return get_capa(capa); }
    inline IndicesMatrix &operator()(const size_t &capa)
      { return get_capa(capa); }
    inline const VIndices &operator()(const size_t &i,const size_t &j,const size_t &k) const
      { return get_capa(i)(j,k); }
    inline VIndices &operator()(const size_t &i,const size_t &j,const size_t &k)
      { return get_capa(i)(j,k); }

    void Offset(const VIndices &);
  };



#endif
