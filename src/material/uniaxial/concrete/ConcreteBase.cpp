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
//ConcreteBase.cpp

#include <material/uniaxial/concrete/ConcreteBase.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/Information.h>
#include <cmath>
#include <cfloat>


//! @brief Constructor.
XC::ConcreteBase::ConcreteBase(int tag, int classTag, double FPC, double EPSC0, double EPSCU)
  :RawConcrete(tag, classTag,FPC,EPSC0,EPSCU) {}

//! @brief Constructor
XC::ConcreteBase::ConcreteBase(int tag, int classTag)
  :RawConcrete(tag, classTag) {}

//! @brief Devuelve la tensión en el material.
double XC::ConcreteBase::getStress(void) const
  { return trialState.getStress(); }

//! @breif Devuelve la deformación del material.
double XC::ConcreteBase::getStrain(void) const
  { return trialState.getStrain(); }

//! @breif Devuelve la tangente al diagrama tensión - deformación.
double XC::ConcreteBase::getTangent(void) const
  { return trialState.getTangent(); }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ConcreteBase::sendData(CommParameters &cp)
  {
    int res= RawConcrete::sendData(cp);
    res+= cp.sendMovable(convergedState,getDbTagData(),CommMetaData(2));
    res+= cp.sendMovable(trialState,getDbTagData(),CommMetaData(3));
    res+= cp.sendMovable(convergedHistory,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(trialHistory,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(fpc,epsc0,epscu,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ConcreteBase::recvData(const CommParameters &cp)
  {
    int res= RawConcrete::recvData(cp);
    res+= cp.receiveMovable(convergedState,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMovable(trialState,getDbTagData(),CommMetaData(3));
    res+= cp.receiveMovable(convergedHistory,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(trialHistory,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(fpc,epsc0,epscu,getDbTagData(),CommMetaData(6));
    return res;
  }

