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
//modifier_quad_triangle.cc

#include "modifier_quad_triangle.h"

float correction_factor(unsigned int ne, unsigned int nq)
  {
    if(ne == 2 && nq == 1)
      return -0.20505f;
    if(ne == 3 && nq == 1)
      return 0.80597f;
    if(ne == 3 && nq == 2)
      return 0.61539f;
    if(ne == 4 && nq == 1)
      return 0.34792f;
    if(ne == 4 && nq == 2)
      return 0.21380f;
    if(ne == 4 && nq == 3)
      return 0.10550f;
    return 0.0f;
  }
