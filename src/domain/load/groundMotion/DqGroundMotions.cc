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
//DqGroundMotions.cc

#include "DqGroundMotions.h"
#include "GroundMotion.h"
#include "boost/any.hpp"



//! @brief Constructor.
XC::DqGroundMotions::DqGroundMotions(const size_t &num)
  : EntCmd(), MovableObject(0), gMotions(num,static_cast<GroundMotion *>(nullptr)) {}

//! @brief Constructor de copia.
XC::DqGroundMotions::DqGroundMotions(const DqGroundMotions &otro)
  : EntCmd(otro), MovableObject(0), gMotions(otro.gMotions) {}

//! @brief Operador asignación.
XC::DqGroundMotions &XC::DqGroundMotions::operator=(const DqGroundMotions &otro)
  {
    EntCmd::operator=(otro);
    gMotions= otro.gMotions;
    return *this;
  }

void XC::DqGroundMotions::addMotion(GroundMotion &gm)
  {
    const_iterator i= std::find(begin(),end(),&gm);
    if(i == end())
      gMotions.push_back(&gm);
  }

void XC::DqGroundMotions::clear(void)
  {
    iterator i= gMotions.begin();
    for(;i!= gMotions.end();i++)
      if(*i)
        delete (*i);
    gMotions.erase(gMotions.begin(),gMotions.end());
  }

//! @brief Destructor:
XC::DqGroundMotions::~DqGroundMotions(void)
  { clear(); }

//! @brief Sends object through the channel being passed as parameter.
int XC::DqGroundMotions::sendData(CommParameters &cp)
  {
    const size_t sz= getNumGroundMotions();
    setDbTagDataPos(0,sz);
    DbTagData cpGM(sz*2);
    int res= 0;
    for(size_t i= 0;i<sz;i++)
      res+= cp.sendBrokedPtr((*this)[i],cpGM,BrokedPtrCommMetaData(i,0,i+sz));
    res+= cpGM.send(getDbTagData(),cp,CommMetaData(1));
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DqGroundMotions::recvData(const CommParameters &cp)
  {
    const size_t sz= getDbTagDataPos(0);
    DbTagData cpGM(sz*2);
    int res= cpGM.receive(getDbTagData(),cp,CommMetaData(1));

    for(size_t i= 0;i<sz;i++)
      {
        // Receive the ground motion
        (*this)[i]= cp.getBrokedGroundMotion((*this)[i],cpGM,BrokedPtrCommMetaData(i,0,i+sz));
        if(!(*this)[i])
          std::cerr << "DqGroundMotions::recvSelf() - ground motion "
                    << i << "failed to recv itself\n";
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DqGroundMotions::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING DqGroundMotions::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DqGroundMotions::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING DqGroundMotions::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::DqGroundMotions::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of GroundMotions: " << getNumGroundMotions() << std::endl;
  }
