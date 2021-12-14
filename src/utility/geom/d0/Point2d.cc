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
//Point2d.cc

#include "Point2d.h"
#include <plotter.h>

#include "utility/geom/trf/Trf2d.h"
void Point2d::Print(std::ostream &os) const
  { os << org; }
void Point2d::Plot(Plotter &plotter) const
  {
    org.Plot(plotter);
  }

//! @brief Aplica al objeto la transformación que se
//! pasa como parámetro.
void Point2d::Transform(const Trf2d &trf2d)
  {
    const Pos2d pA= trf2d.Transform(org);
    (*this)= Point2d(pA);
  }

