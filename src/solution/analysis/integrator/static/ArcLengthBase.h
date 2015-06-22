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
//ArcLengthBase.h

#ifndef ArcLengthBase_h
#define ArcLengthBase_h

#include "solution/analysis/integrator/static/ProtoArcLength.h"

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;
class Vector;

//! @ingroup ProtoArcLength
//
//! @brief Clase base para ??.
class ArcLengthBase: public ProtoArcLength
  {
  protected:
    double alpha2;

    double getDLambdaNewStep(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);

    ArcLengthBase(SoluMethod *,int classTag,double ArcLength, double alpha = 1.0);
  public:

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif

