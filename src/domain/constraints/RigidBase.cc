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
//RigidBase.cpp


#include "RigidBase.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"

void XC::RigidBase::setDomain(Domain *theDomain)
  {
    if(theDomain == nullptr)
      {
        std::cerr << "WARNING RigidBase::SetDomain: Specified domain does not exist";
        std::cerr << "Domain = 0\n";
        return;
      }
    MP_Constraint::setDomain(theDomain);

    if(theDomain)
      {
        // get a pointer to the retained and constrained nodes - make sure they exist
        nodeR = theDomain->getNode(getNodeRetained());
        if(!nodeR)
          {
            std::cerr << "XC::RigidBase::setup - retained node " <<  getNodeRetained() <<  " not in domain\n";
          }
        else
          nodeR->connect(this);
        nodeC = theDomain->getNode(getNodeConstrained());
        if(!nodeC)
          {
            std::cerr << "XC::RigidBase::setup - constrained node " <<  getNodeConstrained() <<  " not in domain\n";
          }
        else
          nodeC->connect(this);
      }
  }

XC::RigidBase::RigidBase(int mPtag)
  : MP_Constraint(mPtag), nodeR(nullptr), nodeC(nullptr) {}

XC::RigidBase::RigidBase(int mPtag,const int &masterNode, const int &slaveNode, int classTag)
  : MP_Constraint(mPtag,masterNode,slaveNode,classTag), nodeR(nullptr), nodeC(nullptr) {}

XC::RigidBase::~RigidBase(void)
  {
    if(nodeR)
      nodeR->disconnect(this);
    if(nodeC)
      nodeC->disconnect(this);
  }
