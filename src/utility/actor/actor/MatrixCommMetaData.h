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
                                                                        
                                                                        
#ifndef MatrixCommMetaData_h
#define MatrixCommMetaData_h

#include "ArrayCommMetaData.h"

namespace XC {

//! @brief Datos acerca de la posición, tamaño,... del
//! objeto a transmitir.
class MatrixCommMetaData: public ArrayCommMetaData
  {
  private:
    std::size_t posNc; //!< Número de columnas de la matriz.
  public:
    MatrixCommMetaData(const int &pos,const int &flag,const size_t &nf,const size_t &nc);

    //! @brief Return the posición del número de filas.
    inline const size_t &getPosNumFilas(void) const
      { return posSz; }
    //! @brief Return the posición del número de columnas.
    inline const size_t &getPosNumCols(void) const
      { return posNc; }
    //! @brief Returns the número de filas.
    inline const size_t &getNumFilas(void) const
      { return posSz; }
    //! @brief Returns the número de columnas.
    inline const size_t &getNumCols(void) const
      { return posNc; }
  };

} // end of XC namespace

#endif
