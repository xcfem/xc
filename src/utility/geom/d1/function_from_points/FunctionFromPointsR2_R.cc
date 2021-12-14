//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
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
//FunctionFromPointsR2_R.cc

#include "FunctionFromPointsR2_R.h"

//! @brief Return the row and column indexes of the maximum.
FunctionFromPointsR2_R::row_column FunctionFromPointsR2_R::PosMax(void) const
  {
    row_column retval;
    retval.first= 1;
    retval.second= 1;
    const size_t n_rows= getNumberOfRows();
    const size_t n_columns= getNumberOfColumns();
    double max= Valor(1,1);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        {
          const double tmp= Valor(i,j);
          if(max < tmp)
            {
              max= tmp;
              retval.first= i;
              retval.second= j;
            }
        }
    return retval;
  }
//! @brief Return the maximum value de los valores definidos.
double FunctionFromPointsR2_R::Max(void) const
  {
    const row_column fc= PosMax();
    return Valor(fc.first,fc.second);
  }
//! @brief Return the position that corresponds to the maximum.
Pos2d FunctionFromPointsR2_R::getMaxPoint(void) const
  {
    const row_column fc= PosMax();
    return Posicion(fc.first,fc.second);
  }
//! @brief Return the row and column del mínimo.
FunctionFromPointsR2_R::row_column FunctionFromPointsR2_R::PosMin(void) const
  {
    row_column retval;
    retval.first= 1;
    retval.second= 1;
    const size_t n_rows= getNumberOfRows();
    const size_t n_columns= getNumberOfColumns();
    double min= Valor(1,1);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        {
          const double tmp= Valor(i,j);
          if(min > tmp)
            {
              min= tmp;
              retval.first= i;
              retval.second= j;
            }
        }
    return retval;
  }
//! @brief Return el valor mínimo de los valores definidos.
double FunctionFromPointsR2_R::Min(void) const
  {
    const row_column fc= PosMin();
    return Valor(fc.first,fc.second);
  }
//! @brief Return the position of the minimum value from the points.
Pos2d FunctionFromPointsR2_R::getMinPoint(void) const
  {
    const row_column fc= PosMin();
    return Posicion(fc.first,fc.second);
  }
