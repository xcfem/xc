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
//HingeBeamIntegration2d.cc

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration2d.h>
#include "domain/mesh/element/fvectors/FVector.h"
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/repres/CrossSectionProperties2d.h"

XC::HingeBeamIntegration2d::HingeBeamIntegration2d(int classTag,double e,double a,double i,double lpi,double lpj)
  : PlasticLengthsBeamIntegration(classTag,lpi,lpj), E(e), A(a), I(i) {}

XC::HingeBeamIntegration2d::HingeBeamIntegration2d(int classTag)
  : PlasticLengthsBeamIntegration(classTag), E(0.0), A(0.0), I(0.0) {}


void XC::HingeBeamIntegration2d::addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0)
  {
    if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
      beamMecLoad->addElasticDeformations(L,CrossSectionProperties2d(E,A,I),lpI,lpJ,loadFactor,v0);
    else
      std::cerr << "XC::HingeBeamIntegration2d::addElasticDeformations -- load type unknown." << std::endl;
    return;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::HingeBeamIntegration2d::sendData(CommParameters &cp)
  {
    int res= PlasticLengthsBeamIntegration::sendData(cp);
    res+= cp.sendDoubles(E,A,I,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::HingeBeamIntegration2d::recvData(const CommParameters &cp)
  {
    int res= PlasticLengthsBeamIntegration::recvData(cp);
    res+= cp.receiveDoubles(E,A,I,getDbTagData(),CommMetaData(2));
    return res;
  }

void XC::HingeBeamIntegration2d::Print(std::ostream &s, int flag)
  {
    PlasticLengthsBeamIntegration::Print(s,flag);
    s << "Hinge2d" << std::endl;
    s << " E = " << E;
    s << " A = " << A;
    s << " I = " << I << std::endl;
    return;
  }
