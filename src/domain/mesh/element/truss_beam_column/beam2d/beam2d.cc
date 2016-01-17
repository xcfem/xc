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
//beam2d.cc

#include "beam2d.h"
#include "utility/actor/actor/MovableVector.h"

//! @brief Constructor por defecto.
XC::beam2d::beam2d(int tag,int class_tag)
  :ProtoBeam2d(tag,class_tag), L(0)
  {}

//! @brief Constructor.
XC::beam2d::beam2d(int tag, int class_tag, double a, double e, double i, int Nd1, int Nd2)
  :ProtoBeam2d(tag,class_tag,a,e,i,Nd1,Nd2), L(0)   
  {}

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::beam2d::sendData(CommParameters &cp)
  {
    int res= ProtoBeam2d::sendData(cp);
    res+= cp.sendDoubles(L,sn,cs,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::beam2d::recvData(const CommParameters &cp)
  {
    int res= ProtoBeam2d::recvData(cp);
    res+= cp.receiveDoubles(L,sn,cs,getDbTagData(),CommMetaData(9));
    return res;
  }
