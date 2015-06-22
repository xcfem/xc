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

// Written: lcpt
// Created: 03/2015
// Revision: A

#include <solution/analysis/model/fe_ele/lagrange/LagrangeMRMP_FE.h>
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::LagrangeMRMP_FE::LagrangeMRMP_FE(int tag, Domain &theDomain, MRMP_Constraint &TheMRMP,DOF_Group &theDofGrp, double Alpha)
  :MRMP_FE(tag, TheMRMP.getNumDofGroups()+1,(TheMRMP.getNumLagrangeDofs()),TheMRMP,Alpha), Lagrange_FE(theDofGrp)
  {
    const Matrix &constraintMatrix = theMRMP->getConstraint();
    const int noRows = constraintMatrix.noRows();
    const int noCols = constraintMatrix.noCols();
    const int size = noCols+2*noRows;
    tang= Matrix(size,size);
    resid= Vector(size);
    tang.Zero();
    resid.Zero();

    theConstrainedNode= theDomain.getNode(theMRMP->getNodeConstrained());
    myDOF_Groups(0)= determineConstrainedNodeDofGrpPtr()->getTag();

    int offset= determineRetainedNodesDofGrpPtr(theDomain, 1);

    myDOF_Groups(offset)= getLagrangeDOFGroup()->getTag();

    if(theMRMP->isTimeVarying() == false)
      this->determineTangent();
  }

//! @brief Method to set the corresponding index of the ID to value.
int XC::LagrangeMRMP_FE::setID(void)
  {
    int offset= 0;
    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    const int offset1= determineConstrainedDOFsIDs(0);
    if(offset1>=0) offset= offset1;

    // then determine the IDs for the retained dof's
    const int offset2= determineRetainedDOFsIDs(offset1);
    if(offset2>=0) offset= offset2;

    const ID &theGroupsID = getLagrangeDOFGroup()->getID();
    const int size3 = theGroupsID.Size();
    for(int k=0; k<size3; k++)
      myID(k+offset) = theGroupsID(k);
    offset+=size3;

    return offset;
  }

const XC::Matrix &XC::LagrangeMRMP_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMRMP->isTimeVarying() == true)
      this->determineTangent();
    return tang;
  }

const XC::Vector &XC::LagrangeMRMP_FE::getResidual(Integrator *theNewIntegrator)
  { return resid; }



const XC::Vector &XC::LagrangeMRMP_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMRMP_FE::getTangForce() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return  resid;
  }

const XC::Vector &XC::LagrangeMRMP_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMRMP_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMRMP_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMRMP_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMRMP_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMRMP_FE::getM_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

void XC::LagrangeMRMP_FE::determineTangent(void)
  {
    const Matrix &constraintMatrix = theMRMP->getConstraint();
    const int noRows= constraintMatrix.noRows();
    const int noCols= constraintMatrix.noCols();
    const int n= noRows+noCols;
 
    tang.Zero();

    for(int j=0; j<noRows; j++)
      {
        tang(n+j, j)= -alpha;
        tang(j, n+j)= -alpha;
      }

    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
        {
          const double val= constraintMatrix(i,j) * alpha;
          tang(n+i, j+noRows)= val;
          tang(noRows+j, n+i)= val;
        }
  }





