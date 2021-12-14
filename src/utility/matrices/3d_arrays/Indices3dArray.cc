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
//Indices3dArray.cc

#include "Indices3dArray.h"

Indices3dArray::Indices3dArray(const size_t &capas,const size_t &n_rows,const size_t &n_columns)
  : std::vector<IndicesMatrix>(capas,IndicesMatrix(n_rows,n_columns)) {}

//! @brief Return the number of elements of the array.
size_t Indices3dArray::NumIndices(void) const
  {
    const size_t sz= this->size();
    if(sz<1)
      return 0;
    else
      {
        const IndicesMatrix &capa= (*this)(1); 
        return sz*capa.getNumberOfRows()*capa.getNumberOfColumns();
      }
  }

//! @brief Return the number of rows of the array.
size_t Indices3dArray::getNumberOfRows(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumberOfRows();
    return retval;
  }

//! @brief Return the number of columns of the array.
size_t Indices3dArray::getNumberOfColumns(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumberOfColumns();
    return retval;
  }

//! @brief Inserta el objeto en el stream de salida.
inline std::ostream &operator<<(std::ostream &os,const Indices3dArray &t)
  {
    const size_t ncapas= t.getNumberOfLayers();
    for(size_t i=1;i<=ncapas;i++)
      os << t(i);
    return os;
  }

//! @brief Agrega a todos los elementos el valor
//! que is being passed as parameter.
void Indices3dArray::Offset(const VIndices &vi)
  {
    const size_t nCapas= getNumberOfLayers();
    for(size_t i= 1;i<= nCapas;i++)
      get_capa(i).Offset(vi);
  }
