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
//ProtoArcLength.h

#ifndef ProtoArcLength_h
#define ProtoArcLength_h

#include "solution/analysis/integrator/StaticIntegrator.h"
#include "IntegratorVectors.h"
#include "utility/matrix/Vector.h"

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;

//! @ingroup StaticIntegrator
//
//! @brief Clase base para ??.
class ProtoArcLength: public StaticIntegrator
  {
  protected:
    double arcLength2;
    IntegratorVectors vectores;
    int signLastDeltaLambdaStep;

    virtual double getDLambdaNewStep(void) const= 0;
    virtual double getDLambdaUpdate(void) const= 0;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);

    ProtoArcLength(SoluMethod *,int classTag,double ArcLength);
  public:

    int newStep(void);
    int update(const Vector &deltaU);
    int domainChanged(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif

