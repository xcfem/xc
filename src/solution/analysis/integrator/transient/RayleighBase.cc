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
//RayleighBase.cpp

#include <solution/analysis/integrator/transient/RayleighBase.h>

//! @brief Constructor.
XC::RayleighBase::RayleighBase(SoluMethod *owr,int classTag)
    : DampingFactorsIntegrator(owr,classTag), deltaT(0.0) {}

//! @brief Constructor.
XC::RayleighBase::RayleighBase(SoluMethod *owr,int classTag,const RayleighDampingFactors &rF)
  : DampingFactorsIntegrator(owr,classTag,rF),deltaT(0.0) {}



//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::RayleighBase::sendData(CommParameters &cp)
  {
    int res= DampingFactorsIntegrator::sendData(cp);
    res+= cp.sendDouble(deltaT,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::RayleighBase::recvData(const CommParameters &cp)
  {
    int res= DampingFactorsIntegrator::recvData(cp);
    res+= cp.receiveDouble(deltaT,getDbTagData(),CommMetaData(3));
    return res;
  }
