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
//HHTRayleighBase.cpp

#include <solution/analysis/integrator/transient/rayleigh/HHTRayleighBase.h>

//! @brief Constructor.
XC::HHTRayleighBase::HHTRayleighBase(SoluMethod *owr,int classTag)
  : RayleighBase(owr,classTag),
    alpha(1.0), gamma(0.0), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
XC::HHTRayleighBase::HHTRayleighBase(SoluMethod *owr,int classTag,double _alpha)
    : RayleighBase(owr,classTag),
    alpha(_alpha),gamma(1.5-_alpha), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
XC::HHTRayleighBase::HHTRayleighBase(SoluMethod *owr,int classTag,double _alpha,const RayleighDampingFactors &rF)
    : RayleighBase(owr,classTag,rF),
    alpha(_alpha), gamma(1.5-_alpha), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
XC::HHTRayleighBase::HHTRayleighBase(SoluMethod *owr,int classTag,double _alpha, double _gamma)
    : RayleighBase(owr,classTag),
    alpha(_alpha), gamma(_gamma), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
XC::HHTRayleighBase::HHTRayleighBase(SoluMethod *owr,int classTag,double _alpha, double _gamma,const RayleighDampingFactors &rF)
    : RayleighBase(owr,classTag,rF), alpha(_alpha), gamma(_gamma), c2(0.0), c3(0.0), 
    Ut(0), U(0), Ualpha(0) {}


//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::HHTRayleighBase::sendData(CommParameters &cp)
  {
    int res= RayleighBase::sendData(cp);
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(U,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(Ualpha,getDbTagData(),CommMetaData(6));
    res+= cp.sendDoubles(alpha,gamma,c2,c3,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::HHTRayleighBase::recvData(const CommParameters &cp)
  {
    int res= RayleighBase::recvData(cp);
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(U,getDbTagData(),CommMetaData(5));
    res+= cp.receiveMovable(Ualpha,getDbTagData(),CommMetaData(6));
    res+= cp.receiveDoubles(alpha,gamma,c2,c3,getDbTagData(),CommMetaData(7));
    return res;
  }

