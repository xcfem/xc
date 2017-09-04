//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  software es libre: usted puede redistribuirlo y/o modificarlo
//  bajo los términos de la Licencia Pública General GNU publicada
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

// Written: lcpt
// Created: 03/2015
// Revision: A

#include <solution/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.h>
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
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::LagrangeMRMFreedom_FE::LagrangeMRMFreedom_FE(int tag, Domain &theDomain, MRMFreedom_Constraint &TheMRMP,DOF_Group &theDofGrp, double Alpha)
  :MRMFreedom_FE(tag, TheMRMP.getNumDofGroups()+1,(TheMRMP.getNumLagrangeDofs()),TheMRMP,Alpha), Lagrange_FE(theDofGrp)
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
int XC::LagrangeMRMFreedom_FE::setID(void)
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

const XC::Matrix &XC::LagrangeMRMFreedom_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMRMP->isTimeVarying() == true)
      this->determineTangent();
    return tang;
  }

const XC::Vector &XC::LagrangeMRMFreedom_FE::getResidual(Integrator *theNewIntegrator)
  { return resid; }



const XC::Vector &XC::LagrangeMRMFreedom_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return  resid;
  }

const XC::Vector &XC::LagrangeMRMFreedom_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMRMFreedom_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMRMFreedom_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << nombre_clase() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

void XC::LagrangeMRMFreedom_FE::determineTangent(void)
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





