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
//SteelBase.cc


#include <material/uniaxial/steel/SteelBase.h>

#include "utility/actor/actor/MovableVector.h"

//! @brief Constructor.
XC::SteelBase::SteelBase(int tag,int classTag,const double &Fy,const double &e0,const double &B,const double &A1,const double &A2,const double &A3,const double &A4)
  : UniaxialMaterial(tag,classTag), fy(Fy),E0(e0),b(B),a1(A1),a2(A2),a3(A3),a4(A4) {}

XC::SteelBase::SteelBase(int tag,int classTag)
  :UniaxialMaterial(tag,classTag), fy(0.0),E0(0.0),b(0.0),a1(0.0), a2(0.0), a3(0.0), a4(0.0) {}

//! @brief Assigns intial Young's modulus.
void XC::SteelBase::setInitialTangent(const double &d)
  {
    E0= d;
    setup_parameters(); //Inicializa las variables históricas.
  }

//! @brief Returns intial Young's modulus.
double XC::SteelBase::getInitialTangent(void) const
  { return E0; }

//! @brief Assigns yield stress.
void XC::SteelBase::setFy(const double &d)
  {
    fy= d;
    setup_parameters(); //Inicializa las variables históricas.
  }

//! @brief Returns yield stress.
double XC::SteelBase::getFy(void) const
  { return fy; }


//! @brief Send members del objeto through the channel being passed as parameter.
int XC::SteelBase::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(fy,E0,b,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(a1,a2,a3,a4,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::SteelBase::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(fy,E0,b,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(a1,a2,a3,a4,getDbTagData(),CommMetaData(3));
    return res;
  }
