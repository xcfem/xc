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
//CorotTrussBase.cc

#include "CorotTrussBase.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MovableVector.h"

// constructor:
//  responsible for allocating the necessary space needed by each object
//  and storing the tags of the XC::CorotTrussBase end nodes.
XC::CorotTrussBase::CorotTrussBase(int tag, int classTag, int dim,int Nd1, int Nd2)
  :ProtoTruss(tag,classTag,Nd1,Nd2,0,dim), Lo(0.0), Ln(0.0), R(3,3)
  {}
//! @brief Constructor.
XC::CorotTrussBase::CorotTrussBase(int tag, int classTag,int dimension)
  :ProtoTruss(tag,classTag,0,0,0,dimension), Lo(0.0), Ln(0.0), R(3,3)
  {}

//! @brief Constructor de copia.
XC::CorotTrussBase::CorotTrussBase(const CorotTrussBase &otro)
  : ProtoTruss(otro), Lo(otro.Lo), Ln(otro.Ln), R(otro.R)
  {}

//! @brief Operador de asignación.
XC::CorotTrussBase &XC::CorotTrussBase::operator=(const CorotTrussBase &otro)
  {
    ProtoTruss::operator=(otro);
    Lo= otro.Lo;
    Ln= otro.Ln;
    R= otro.R;
    return *this;
  }


//! @brief Devuelve la longitud inicial del elemento.
const double &XC::CorotTrussBase::getLo(void) const
  { return Lo; }

//! @brief Devuelve la longitud actual del elemento.
const double &XC::CorotTrussBase::getLn(void) const
  { return Ln; }

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::CorotTrussBase::sendData(CommParameters &cp)
  {
    int res= ProtoTruss::sendData(cp);
    res+= cp.sendDoubles(Lo,Ln,d21[0],d21[1],d21[2],getDbTagData(),CommMetaData(16));
    res+= cp.sendMatrix(R,getDbTagData(),CommMetaData(17)); 
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::CorotTrussBase::recvData(const CommParameters &cp)
  {
    int res= ProtoTruss::recvData(cp);
    res+= cp.receiveDoubles(Lo,Ln,d21[0],d21[1],d21[2],getDbTagData(),CommMetaData(16));
    res+= cp.receiveMatrix(R,getDbTagData(),CommMetaData(17)); 
    return res;
  }
