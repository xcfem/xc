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
//RayleighBase.h

#ifndef RayleighBase_h
#define RayleighBase_h

#include "DampingFactorsIntegrator.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @defgroup RayleighIntegrator Integración del sistema de ecuaciones dinámico mediante el método de Rayleigh.
//
//! @ingroup RayleighIntegrator
//
//! @brief ??.
class RayleighBase : public DampingFactorsIntegrator
  {
  protected:
    double deltaT;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    RayleighBase(SoluMethod *,int classTag);
    RayleighBase(SoluMethod *,int classTag,const RayleighDampingFactors &);
  };
} // end of XC namespace

#endif
