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
//PseudoTimeTracker.cc

#include "PseudoTimeTracker.h"
#include <cassert>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

XC::PseudoTimeTracker::PseudoTimeTracker(void)
  : MovableObject(0), currentTime(0.0), committedTime(0.0), dT(0.0), eigenvalueTimeSet(0.0) {}

const double &XC::PseudoTimeTracker::getCurrentTime(void) const
  { return currentTime; }

const double &XC::PseudoTimeTracker::getCommittedTime(void) const
  { return committedTime; }

const double &XC::PseudoTimeTracker::getDt(void) const
  { return dT; }

const double &XC::PseudoTimeTracker::getEigenValueTimeSet(void) const
  { return eigenvalueTimeSet; }

void XC::PseudoTimeTracker::Inic(const double &newTime)
  {
    currentTime= newTime;
    committedTime= newTime;
    dT= 0.0;
  }

void XC::PseudoTimeTracker::setCurrentTime(const double &newTime)
  {
    currentTime= newTime;
    dT= currentTime - committedTime;
  }

void XC::PseudoTimeTracker::setCommittedTime(const double &newTime)
  {
    committedTime= newTime;
    dT= currentTime - committedTime;
    assert(dT>=0.0);
  }

void XC::PseudoTimeTracker::setEigenvalueTime(void)
  { eigenvalueTimeSet= currentTime; }

void XC::PseudoTimeTracker::Zero(void)
  { Inic(0.0); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::PseudoTimeTracker::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::PseudoTimeTracker::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(currentTime,committedTime,dT,eigenvalueTimeSet,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::PseudoTimeTracker::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(currentTime,committedTime,dT,eigenvalueTimeSet,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::PseudoTimeTracker::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "PseudoTimeTracker::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::PseudoTimeTracker::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "PseudoTimeTracker::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << "PseudoTimeTracker::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Imprime el objeto.
void XC::PseudoTimeTracker::Print(std::ostream &s, int flag) const
  {

    s << "Pseudo time Information\n";
    s << "\tCurrent Time: " << currentTime;
    s << "\ntCommitted Time: " << committedTime;
  }

std::ostream &XC::operator<<(std::ostream &s,const PseudoTimeTracker &tt)
  {
    tt.Print(s);
    return s;
  }
