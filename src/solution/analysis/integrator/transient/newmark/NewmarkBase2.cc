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
//NewmarkBase2.cpp

#include <solution/analysis/integrator/transient/newmark/NewmarkBase2.h>
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::NewmarkBase2::NewmarkBase2(SoluMethod *owr,int classTag)
  :NewmarkBase(owr,classTag), beta(0.0), c1(0.0) {}

//! @brief Constructor.
XC::NewmarkBase2::NewmarkBase2(SoluMethod *owr,int classTag, double theGamma, double theBeta)
  :NewmarkBase(owr,classTag,theGamma), beta(theBeta), c1(0.0) {}

//! @brief Constructor.
XC::NewmarkBase2::NewmarkBase2(SoluMethod *owr,int classTag,double theGamma, double theBeta,const RayleighDampingFactors &rF)
  :NewmarkBase(owr,classTag,theGamma,rF), beta(theBeta), c1(0.0) {}

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NewmarkBase2::sendData(CommParameters &cp)
  {
    int res= NewmarkBase::sendData(cp);
    res+= cp.sendDoubles(beta,c1,getDbTagData(),CommMetaData(13));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NewmarkBase2::recvData(const CommParameters &cp)
  {
    int res= NewmarkBase::recvData(cp);
    res+= cp.receiveDoubles(beta,c1,getDbTagData(),CommMetaData(13));
    return res;
  }

