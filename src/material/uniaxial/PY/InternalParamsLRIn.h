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
//InternalParamsLRIn.h

#ifndef INTERNALPARAMSLRIn_H
#define INTERNALPARAMSLRIn_H

#include <material/uniaxial/PY/InternalParamsLR.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Parámetros internos de un material que representa una curva p-y.
class InternalParamsLRIn: public InternalParamsLR
  {
  protected:
    double RLeft; // p at start of current plastic loading cycle - left side
    double RRight; //                                            - right side
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    InternalParamsLRIn(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0,const double &yl= 0.0,const double &yr= 0.0,const double &pl= 0.0,const double &pr= 0.0);
    inline const double &yleftIn(void) const //Para PySimple1
      { return vLeft; }
    inline double &yleftIn(void) //Para PySimple1
      { return vLeft; }
    inline const double &yrightIn(void) const //Para PySimple1
      { return vRight; }
    inline double &yrightIn(void) //Para PySimple1
      { return vRight; }
    inline const double &zleftIn(void) const //Para QzSimple1
      { return vLeft; }
    inline double &zleftIn(void) //Para QzSimple1
      { return vLeft; }
    inline const double &zrightIn(void) const //Para QzSimple1
      { return vRight; }
    inline double &zrightIn(void) //Para QzSimple1
      { return vRight; }
    inline const double &PleftIn(void) const //Para PySimple1
      { return RLeft; }
    inline double &PleftIn(void) //Para PySimple1
      { return RLeft; }
    inline const double &PrightIn(void) const //Para PySimple1
      { return RRight; }
    inline double &PrightIn(void) //Para PySimple1
      { return RRight; }
    inline const double &QleftIn(void) const //Para QzSimple1
      { return RLeft; }
    inline double &QleftIn(void) //Para QzSimple1
      { return RLeft; }
    inline const double &QrightIn(void) const //Para QzSimple1
      { return RRight; }
    inline double &QrightIn(void) //Para QzSimple1
      { return RRight; }
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif
