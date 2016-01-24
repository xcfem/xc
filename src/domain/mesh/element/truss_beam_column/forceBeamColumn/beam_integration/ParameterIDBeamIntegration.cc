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

#include "ParameterIDBeamIntegration.h"

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


XC::ParameterIDBeamIntegration::ParameterIDBeamIntegration(int classTag, const Vector &pt, const Vector &wt)
  : UserDefinedBeamIntegrationBase(classTag,pt,wt), parameterID(0)
  {}

XC::ParameterIDBeamIntegration::ParameterIDBeamIntegration(int classTag, const Vector &pt)
  : UserDefinedBeamIntegrationBase(classTag,pt), parameterID(0)
  {}

XC::ParameterIDBeamIntegration::ParameterIDBeamIntegration(int classTag)
  : UserDefinedBeamIntegrationBase(classTag), parameterID(0)
  {}


int XC::ParameterIDBeamIntegration::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    return -1;
  }

int XC::ParameterIDBeamIntegration::updateParameter(int parameterID, Information &info)
  {
    // Does nothing for now -- MHS
    return 0;
  }

int XC::ParameterIDBeamIntegration::activateParameter(int paramID)
  {
    parameterID = paramID;
    // For Terje to do
    return 0;
  }

//! @brief Send object members through the channel defined in cp.
int XC::ParameterIDBeamIntegration::sendData(CommParameters &cp)
  {
    int res= UserDefinedBeamIntegrationBase::sendData(cp);
    res+= cp.sendInt(parameterID,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object members through the channel defined in cp.
int XC::ParameterIDBeamIntegration::recvData(const CommParameters &cp)
  {
    int res= UserDefinedBeamIntegrationBase::recvData(cp);
    res+= cp.receiveInt(parameterID,getDbTagData(),CommMetaData(5));
    return res;
  }

void XC::ParameterIDBeamIntegration::Print(std::ostream &s, int flag)
  {
    s << "ParameterID" << std::endl;
    UserDefinedBeamIntegrationBase::Print(s);
    s << " Points: " << pts;
    s << " Weights: " << wts;
    double sum = 0.0;
    int N = wts.Size();
    for (int i = 0; i < N; i++)
      sum += fabs(wts(i));
    s << " Condition Number: " << sum << std::endl;
  }
