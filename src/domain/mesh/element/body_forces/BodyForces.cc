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
//BodyForces.cc

#include "BodyForces.h"

XC::BodyForces::BodyForces(const size_t &sz)
  : MovableObject(-1), forces(sz) {} //XXX Definition of classTag pending.

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::BodyForces::getDbTagData(void) const
  {
    static DbTagData retval(2); 
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::BodyForces::sendData(CommParameters &cp)
  {
    int res= cp.sendVector(forces,this->getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BodyForces::recvData(const CommParameters &cp)
  {
    int res= cp.receiveVector(forces,this->getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object.
int XC::BodyForces::sendSelf(CommParameters &cp)
  {
    inicComm(2);

    int res= this->sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "BodyForces::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object
int XC::BodyForces::recvSelf(const CommParameters &cp)
  {
    inicComm(2);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BodyForces::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }

//! @brief Sends object.
void XC::BodyForces::Print(std::ostream &os) const
  {
    os << forces;
  }  
