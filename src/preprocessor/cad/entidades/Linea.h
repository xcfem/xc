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
//Linea.h

#ifndef LINEA_H
#define LINEA_H

#include "LineaBase.h"

class Segmento3d;

namespace XC {

//! \ingroup CadEnt
//!
//! @brief Segmento de recta entre dos puntos.
class Linea: public LineaBase
  {
  protected:

    MatrizPos3d get_posiciones(void) const;
    Edge *split_at(Pnt *,const double &,const double &);
  public:
    Linea(Preprocessor *m,const size_t &ndiv= 4);
    Linea(const std::string &nombre= "",Preprocessor *m= NULL,const size_t &ndiv= 4);
    virtual SetEstruct *getCopy(void) const;

    double getLongitud(void) const;
    Edge *splitAtPoint(Pnt *p);
    Edge *splitAtLambda(const double &);
    Edge *splitAtCooNatural(const double &);

    double getLambda(const Pos3d &) const;
    
    //! @brief Returns the número de vértices.
    inline virtual size_t NumVertices(void) const
      { return 2; }
    virtual BND3d Bnd(void) const;
    double DistanciaA2(const Pos3d &pt) const;

    const Vector &getVector(void) const;
    Segmento3d getSegmento(void) const;
    virtual const Vector &getTang(const double &) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;


  };

} //end of XC namespace
#endif
