//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
//! @brief Análisis de autovalores y vectores propios.
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

