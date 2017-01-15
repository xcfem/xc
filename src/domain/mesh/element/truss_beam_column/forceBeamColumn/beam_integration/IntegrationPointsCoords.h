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
//IntegrationPointsCoords.h

#ifndef IntegrationPointsCoords_h
#define IntegrationPointsCoords_h

#include "utility/matrix/Matrix.h"
#include <vector>

class ExprAlgebra;
class MapValores;

namespace XC {

class BeamIntegration;
class CrdTransf;

//! \ingroup BeamInteg
//
//! @brief Almacena las coordenadas de los puntos de control.
class IntegrationPointsCoords
  {
  protected:
    Matrix rst; //!< Coordenadas naturales (varían entre -1 y 1).
    Matrix rnsntn; //!< Coordenadas normalizadas (varían entre 0 y 1).
    Matrix xyz; //!< Coordenadas locales.
    Matrix XYZ; //!< Coordenadas globales.

    MapValores getMapValores(const size_t &i,const std::vector<std::string> &) const;
  public:
    IntegrationPointsCoords(const BeamIntegration &,int nIP,const CrdTransf &trf);

    inline const double &r(const size_t &i) const
      { return rst(i,1); }
    inline const double &s(const size_t &i) const
      { return rst(i,2); }
    inline const double &t(const size_t &i) const
      { return rst(i,3); }

    inline const double &rn(const size_t &i) const
      { return rnsntn(i,1); }
    inline const double &sn(const size_t &i) const
      { return rnsntn(i,2); }
    inline const double &tn(const size_t &i) const
      { return rnsntn(i,3); }

    inline const double &x(const size_t &i) const
      { return xyz(i,1); }
    inline const double &y(const size_t &i) const
      { return xyz(i,2); }
    inline const double &z(const size_t &i) const
      { return xyz(i,3); }

    inline const double &X(const size_t &i) const
      { return XYZ(i,1); }
    inline const double &Y(const size_t &i) const
      { return XYZ(i,2); }
    inline const double &Z(const size_t &i) const
      { return XYZ(i,3); }

    const XC::Vector &eval(const ExprAlgebra &expr) const;
  };
} // end of XC namespace

#endif
