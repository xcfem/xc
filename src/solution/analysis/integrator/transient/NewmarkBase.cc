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
//NewmarkBase.cpp

#include <solution/analysis/integrator/transient/NewmarkBase.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::NewmarkBase::NewmarkBase(SoluMethod *owr,int classTag)
 : DampingFactorsIntegrator(owr,classTag),gamma(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
XC::NewmarkBase::NewmarkBase(SoluMethod *owr,int classTag, double _gamma)
 : DampingFactorsIntegrator(owr,INTEGRATOR_TAGS_Newmark), gamma(_gamma), c2(0.0), c3(0.0) {}

//! @brief Constructor.
XC::NewmarkBase::NewmarkBase(SoluMethod *owr,int classTag,double _gamma,const RayleighDampingFactors &rF)
  : DampingFactorsIntegrator(owr,classTag,rF),  gamma(_gamma),  c2(0.0), c3(0.0) {}

//! @brief Lee un objeto XC::NewmarkBase desde archivo
bool XC::NewmarkBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NewmarkBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "gamma")
      {
        gamma= interpretaDouble(status.GetString());
        return true;
      }
    else
      return DampingFactorsIntegrator::procesa_comando(status);
  }

//! @brief Populate U, Udot and Udotdot by iterating through the DOF_Groups and 
//! getting the last committed displacement, velocity and accel
void XC::NewmarkBase::PopulateUs(AnalysisModel *model)
  {
    DOF_GrpIter &theDOFGroups = model->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        const ID &id = dofGroupPtr->getID();
        
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        const Vector &accel = dofGroupPtr->getCommittedAccel();
        U.setAccel(id,accel);
        // NOTE WE CAN't DO TOGETHER BECAUSE DOF_GROUPS USING SINGLE VECTOR
//         for (int i=0; i < id.Size(); i++) {
//           int loc = id(i);
//           if (loc >= 0) {
//  	    (*U)(loc) = disp(i);		
//   	    (U.getDot())(loc) = vel(i);
//  	    (U.getDotDot())(loc) = accel(i);
//           }
//         }

      }
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NewmarkBase::sendData(CommParameters &cp)
  {
    int res= DampingFactorsIntegrator::sendData(cp);
    res+= cp.sendDoubles(gamma,c2,c3,getDbTagData(),CommMetaData(3));
    res+= cp.sendMovable(U,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::NewmarkBase::recvData(const CommParameters &cp)
  {
    int res= DampingFactorsIntegrator::recvData(cp);
    res+= cp.receiveDoubles(gamma,c2,c3,getDbTagData(),CommMetaData(3));
    res+= cp.receiveMovable(U,getDbTagData(),CommMetaData(4));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::NewmarkBase::GetProp(const std::string &cod) const
  {
    if(cod=="getGamma")
      return any_const_ptr(gamma);
    else if(cod=="getC2")
      return any_const_ptr(c2);
    else if(cod=="getC3")
      return any_const_ptr(c3);
    else
      return DampingFactorsIntegrator::GetProp(cod);    
  }
