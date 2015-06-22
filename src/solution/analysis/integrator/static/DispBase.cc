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
//DisplacementControl.cpp

#include <solution/analysis/integrator/static/DispBase.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/matrix/ID.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::DispBase::DispBase(SoluMethod *owr,int classTag,const int &numIncr) 
  :BaseControl(owr,classTag,numIncr){}

//! @brief Lee un objeto XC::DispBase desde archivo
bool XC::DispBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DispBase) Procesando comando: " << cmd << std::endl;
    return BaseControl::procesa_comando(status);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DispBase::sendData(CommParameters &cp)
  {
    int res= BaseControl::sendData(cp);
    res+= cp.sendMovable(vectores,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DispBase::recvData(const CommParameters &cp)
  {
    int res= BaseControl::recvData(cp);
    res+= cp.receiveMovable(vectores,getDbTagData(),CommMetaData(1));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::DispBase::GetProp(const std::string &cod) const
  {
    if(cod=="getDeltaLambdaStep")
      return any_const_ptr(vectores.getDeltaLambdaStep());
    else if(cod== "getCurrentLambda")
      return any_const_ptr(vectores.getCurrentLambda());
    else
      return BaseControl::GetProp(cod);
  }
