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
//AlphaOSBase.cpp

#include <solution/analysis/integrator/transient/rayleigh/AlphaOSBase.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/FE_EleIter.h>

//! @brief Constructor.
XC::AlphaOSBase::AlphaOSBase(SoluMethod *owr,int classTag)
    : HHTBase(owr,classTag) {}

//! @brief Constructor.
XC::AlphaOSBase::AlphaOSBase(SoluMethod *owr,int classTag, double _beta, double _gamma)
  : HHTBase(owr,classTag,0,_beta,_gamma) {}

//! @brief Constructor.
XC::AlphaOSBase::AlphaOSBase(SoluMethod *owr,int classTag, double _beta, double _gamma,const RayleighDampingFactors &rF)
  : HHTBase(owr,classTag,0,_beta,_gamma,rF) {}

int XC::AlphaOSBase::newStep(double _deltaT)
  {
    updateCount = 0;

    deltaT = _deltaT;
    if(beta == 0 || gamma == 0 )
      {
        std::cerr << "XC::AlphaOSBase::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
     }
    
    if (deltaT <= 0.0)  {
        std::cerr << "XC::AlphaOSBase::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << "\n";
        return -2;	
    }
    
    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*deltaT);
    c3 = 1.0/(beta*deltaT*deltaT);
    
    if(U.get().Size() == 0)
      {
        std::cerr << "XC::AlphaOSBase::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
      }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;  
    
    // determine new_ displacements and velocities at time t+deltaT
    U.get().addVector(1.0, Ut.getDot(), deltaT);
    const double a1 = (0.5 - beta)*deltaT*deltaT;
    U.get().addVector(1.0, Ut.getDotDot(), a1);
    
    const double a2 = deltaT*(1.0 - gamma);
    U.getDot().addVector(1.0, Ut.getDotDot(), a2);
    return 0;
  }


int XC::AlphaOSBase::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()>0)
      U= Ut;
    return 0;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::AlphaOSBase::sendData(CommParameters &cp)
  {
    int res= HHTBase::sendData(cp);
    res+= cp.sendMovable(Upt,getDbTagData(),CommMetaData(9));
    res+= cp.sendInt(updateCount,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::AlphaOSBase::recvData(const CommParameters &cp)
  {
    int res= HHTBase::recvData(cp);
    res+= cp.receiveMovable(Upt,getDbTagData(),CommMetaData(9));
    res+= cp.receiveInt(updateCount,getDbTagData(),CommMetaData(10));
    return res;
  }
