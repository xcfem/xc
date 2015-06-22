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
//CentralDifferenceBase.cc

#include <solution/analysis/integrator/transient/CentralDifferenceBase.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

//! @brief Constructor.
XC::CentralDifferenceBase::CentralDifferenceBase(SoluMethod *owr,int classTag)
:TransientIntegrator(owr,classTag), updateCount(0), deltaT(0) {}

int XC::CentralDifferenceBase::newStep(double _deltaT)
  {
    updateCount = 0;
    deltaT = _deltaT;

    if(deltaT <= 0.0)
      {
        std::cerr << "XC::CentralDifferenceBase::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }

    //AnalysisModel *theModel = this->getAnalysisModelPtr();
    double time = getCurrentModelTime();
    applyLoadModel(time);
    return 0;
  }

int XC::CentralDifferenceBase::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addMtoTang();
    return 0;
  }    

int XC::CentralDifferenceBase::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    theDof->addMtoTang();
    return(0);
  }


