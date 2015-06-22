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
//MovableMatrices.cc

#include "MovableMatrices.h"
#include "MovableID.h"
#include "MovableMatrix.h"
#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "CommMetaData.h"

//! @brief Constructor.
XC::MovableMatrices::MovableMatrices(std::vector<Matrix> &v)
  : MovableObject(0), vectores(v) {}

//! @brief Envia el vector por el canal que se pasa como parámetro.
int XC::MovableMatrices::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    const int sz= vectores.size();
    setDbTagDataPos(0,sz);

    int res= cp.sendMatrices(vectores,getDbTagData(),CommMetaData(1));

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MovableMatrices::sendSelf() - failed to send ID.\n";
    return res;
  }

//! @brief Recibe el vector a través del canal que se pasa como parámetro.
int XC::MovableMatrices::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag = getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "MovableMatrices::recvSelf() - failed to receive ID data\n";
    else
      res= cp.receiveMatrices(vectores,getDbTagData(),CommMetaData(1));
    if(res<0)
      {
        std::cerr << "MovableMatrices::recvSelf - failed to receive data\n";
        return res;
      }
    return res;
  }

