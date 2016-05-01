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
//MovableBJTensor.cc

#include "MovableBJTensor.h"

#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "CommParameters.h"
#include "CommMetaData.h"

//! @brief Constructor.
XC::MovableBJTensor::MovableBJTensor(const int &rank,const double &initval)
  :BJtensor(rank,initval),MovableObject(TENSOR_TAG_BJtensor) {}

//! @brief Constructor.
XC::MovableBJTensor::MovableBJTensor(const BJtensor &v)
  :BJtensor(v),MovableObject(TENSOR_TAG_BJtensor) {}


//! @brief Asigna el vector.
void XC::MovableBJTensor::setBJTensor(const BJtensor &v)
  { BJtensor::operator=(v); }

//! @brief Envia el vector por el canal que se pasa como parámetro.
int XC::MovableBJTensor::sendSelf(CommParameters &cp)
  {
    std::cerr << "MovableBJTensor::sendSelf() - no implementada.\n";
    return 0;
  }

//! @brief Recibe el vector a través del canal que se pasa como parámetro.
int XC::MovableBJTensor::recvSelf(const CommParameters &cp)
  {
    std::cerr << "MovableBJTensor::recvSelf() - no implementada.\n";
    return 0;
  }

