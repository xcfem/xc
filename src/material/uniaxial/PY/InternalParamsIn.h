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
//InternalParamsIn.h

#ifndef INTERNALPARAMSIN_H
#define INTERNALPARAMSIN_H

#include <material/uniaxial/PY/InternalParamsA.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Parámetros internos de un material que representa una curva p-y.
class InternalParamsIn: public InternalParamsA
  {
  protected:
    double Rin; //  R at start of current plastic loading cycle
    double vin; //  v (y or z) at start of current plastic loading cycle
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    InternalParamsIn(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0,const double &Vin= 0.0,const double &rin= 0.0);
    inline const double &yin(void) const //Para PySimple1
      { return vin; }
    inline double &yin(void) //Para PySimple1
      { return vin; }
    inline double &zin(void) //Para QzSimple1
      { return vin; }
    inline const double &zin(void) const //Para QzSimple1
      { return vin; }
    inline const double &Pin(void) const //Para PySimple1
      { return Rin; }
    inline double &Pin(void) //Para PySimple1
      { return Rin; }
    inline const double &Qin(void) const //Para QzSimple1
      { return Rin; }
    inline double &Qin(void) //Para QzSimple1
      { return Rin; }
    inline const double &Tin(void) const //Para TzSimple1
      { return Rin; }
    inline double &Tin(void) //Para TzSimple1
      { return Rin; }
    void revertToStart(const double &tg);
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC


#endif
