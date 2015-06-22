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
//IntegrationPointsCoords.h

#ifndef IntegrationPointsCoords_h
#define IntegrationPointsCoords_h

#include "utility/matrix/Matrix.h"
#include <vector>

class ExprAlgebra;

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
} // fin namespace XC

#endif
