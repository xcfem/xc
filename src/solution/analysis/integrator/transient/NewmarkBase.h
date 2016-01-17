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
//NewmarkBase.h

#ifndef NewmarkBase_h
#define NewmarkBase_h

#include <solution/analysis/integrator/transient/DampingFactorsIntegrator.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {
class Vector;
class ID;

//! @ingroup TransientIntegrator
//
//! @defgroup NewmarkIntegrator Integración del sistema de ecuaciones dinámico mediante el método de Newmark.
//
//! @ingroup NewmarkIntegrator
//
//! @brief The two parameter time-stepping method developed by NewmarkBase
class NewmarkBase: public DampingFactorsIntegrator
  {
  protected:
    double gamma;
    
    double c2, c3; // some constants we need to keep
    ResponseQuantities U; // response quantities at time t+deltaT = predicted + corrected

    void PopulateUs(XC::AnalysisModel *model);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    NewmarkBase(SoluMethod *,int classTag);
    NewmarkBase(SoluMethod *,int classTag,double gamma);
    NewmarkBase(SoluMethod *,int classTag,double gamma,const RayleighDampingFactors &rF);
  };
} // end of XC namespace

#endif
