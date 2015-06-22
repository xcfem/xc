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
//InternalParamsA.h

#ifndef INTERNALPARAMSA_H
#define INTERNALPARAMSA_H

#include "utility/actor/actor/MovableObject.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Parámetros internos de un material que representa una curva p-y.
  class InternalParamsA: public MovableObject
  {
  protected:
    double v; //!< Desplazamiento.
    double R; //!< Fuerza.
    double Tang; //!< Combined tangent

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    InternalParamsA(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0);
    inline const double &y(void) const //Para PySimple1
      { return v; }
    inline double &y(void) //Para PySimple1
      { return v; }
    inline double &z(void) //Para QzSimple1
      { return v; }
    inline const double &z(void) const //Para QzSimple1
      { return v; }
    inline const double &P(void) const //Para PySimple1
      { return R; }
    inline double &P(void) //Para PySimple1
      { return R; }
    inline const double &Q(void) const //Para QzSimple1
      { return R; }
    inline double &Q(void) //Para QzSimple1
      { return R; }
    inline const double &T(void) const //Para TzSimple1
      { return R; }
    inline double &T(void) //Para TzSimple1
      { return R; }
    inline const double &tang(void) const
      { return Tang; }
    inline double &tang(void)
      { return Tang; }

    void getField(const double &V);
    const double &updateField(const double &Tp);
    inline const double unbalance(const double &Tp) const
      { return (Tp - R); }
    inline const double residual(const double &Tp) const
      { return unbalance(Tp)/Tang; }

    void revertToStart(const double &tg);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC


#endif
