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
//ModalAnalysis.h


#ifndef ModalAnalysis_h
#define ModalAnalysis_h

#include "EigenAnalysis.h"
#include "xc_utils/src/geom/d1/func_por_puntos/FuncPorPuntosR_R.h"

namespace XC {
class Matrix;

//! @ingroup TipoAnalisis
//
//! @brief Análisis de eigenvalues y vectores propios.
class ModalAnalysis : public EigenAnalysis
  {
  protected:
    FuncPorPuntosR_R espectro;

    friend class ProcSolu;
    ModalAnalysis(SoluMethod *metodo);
  public:
    inline const FuncPorPuntosR_R &getSpectrum(void) const
      { return espectro; }
    inline void setSpectrum(const FuncPorPuntosR_R &s)
      { espectro= s; }
    //Aceleraciones.
    double getAcceleration(const double &) const;
    Vector getAccelerations(const Vector &) const;
    Vector getModalAccelerations(void) const;
    Matrix getCQCModalCrossCorrelationCoefficients(const Vector &zetas) const;

    //Fuerza estática equivalente.
    Vector getEquivalentStaticLoad(int mode) const;
  };

} // end of XC namespace

#endif

