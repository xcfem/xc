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
//HingeBeamIntegration3d.cc

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration3d.h>
#include "domain/mesh/element/fvectors/FVector.h"
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/component/Parameter.h"

#include <utility/matrix/Vector.h>
#include <domain/mesh/element/Information.h>
#include "utility/matrix/ID.h"

XC::HingeBeamIntegration3d::HingeBeamIntegration3d(int classTag,double e,double a,double iz,double iy,double g,double j,double lpi,double lpj)
  : PlasticLengthsBeamIntegration(classTag,lpi,lpj),ctes_scc(e,a,iz,iy,g,j) {}

XC::HingeBeamIntegration3d::HingeBeamIntegration3d(int classTag, const ConstantesSecc3d &cts,const double &lpi,const double &lpj)
  : PlasticLengthsBeamIntegration(classTag,lpi,lpj), ctes_scc(cts) {}

void XC::HingeBeamIntegration3d::addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0)
  {
    if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
      beamMecLoad->addElasticDeformations(L,ctes_scc,lpI,lpJ,loadFactor,v0);
    return;
  }

int XC::HingeBeamIntegration3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    std::cerr << "Se pasa un puntero nulo." << std::endl;
    int tmp= ctes_scc.setParameter(argv,param, nullptr);
    if(tmp > 0)
      return tmp;
    else if(argv[0] == "lpI")
      return param.addObject(4, this);
    else if(argv[0] == "lpJ")
      return param.addObject(5, this);
    else 
      return -1;
  }

int XC::HingeBeamIntegration3d::updateParameter(int parameterID, Information &info)
  {
    if(parameterID<=6)
      return ctes_scc.updateParameter(parameterID,info);
    else
     switch (parameterID)
        {
        case 7:
          lpI = info.theDouble;
          return 0;
        case 8:
          lpJ = info.theDouble;
          return 0;
        default:
          return -1;
        }
  }

int XC::HingeBeamIntegration3d::activateParameter(int parameterID)
  {
    // For Terje to do
    return 0;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::HingeBeamIntegration3d::sendData(CommParameters &cp)
  {
    int res= PlasticLengthsBeamIntegration::sendData(cp);
    res+=cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::HingeBeamIntegration3d::recvData(const CommParameters &cp)
  {
    int res= PlasticLengthsBeamIntegration::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::HingeBeamIntegration3d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::HingeBeamIntegration3d::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Imprime el objeto.
void XC::HingeBeamIntegration3d::Print(std::ostream &s, int flag)
  {
    s << "HingeMidpoint3d" << std::endl;
    ctes_scc.Print(s);
    s << " lpI = " << lpI;
    s << " lpJ = " << lpJ << std::endl;
    return;
  }
