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
//ResponseQuantities.cpp

#include "ResponseQuantities.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/actor/actor/CommMetaData.h"

XC::ResponseQuantities::ResponseQuantities(const int &size)
  :MovableObject(0), R(size), Rdot(size), Rdotdot(size) {}


void XC::ResponseQuantities::resize(const int &size)
  {  
    R.resize(size);
    Rdot.resize(size);
    Rdotdot.resize(size);
  }

void XC::ResponseQuantities::setDisp(const ID &id,const Vector &disp)
  {
    const int idSize = id.Size();
    for(int i=0;i<idSize; i++)
      {
        const int loc = id(i);
        if(loc >= 0)
          { R(loc) = disp(i); }
      }
  }

void XC::ResponseQuantities::setVel(const ID &id,const Vector &vel)
  {
    const int idSize = id.Size();
    for(int i=0;i<idSize;i++)
      {
        const int loc = id(i);
        if(loc >= 0)
         { Rdot(loc) = vel(i); }
      }
  }

void XC::ResponseQuantities::setAccel(const ID &id,const Vector &accel)
  {
    const int idSize = id.Size();
    for(int i=0;i<idSize;i++)
      {
        const int loc = id(i);
        if(loc >= 0)
          { Rdotdot(loc) = accel(i); }
      }
  }

void XC::ResponseQuantities::Zero(void)
  {  
    R.Zero();
    Rdot.Zero();
    Rdotdot.Zero();
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ResponseQuantities::sendData(CommParameters &cp)
  {
    int res= 0;//MovableObject::sendData(cp);
    res+= cp.sendVector(R,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(Rdot,getDbTagData(),CommMetaData(2));
    res+= cp.sendVector(Rdotdot,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ResponseQuantities::recvData(const CommParameters &cp)
  {
    int res= 0;//MovableObject::receiveData(cp);
    res+= cp.receiveVector(R,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(Rdot,getDbTagData(),CommMetaData(2));
    res+= cp.receiveVector(Rdotdot,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ResponseQuantities::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(27);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ResponseQuantities::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ResponseQuantities::recvSelf(const CommParameters &cp)
  {
    inicComm(27);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "ResponseQuantities::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << "ResponseQuantities::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::ResponseQuantities::Print(std::ostream &s)
  {
    s << " R: " << R << " Rdot: " << Rdot << std::endl;
    s << " Rdotdot: " << Rdotdot;	    
  }

