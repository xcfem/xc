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
//MPBase_FE.cpp

#include "MPBase_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MFreedom_ConstraintBase.h>

//! @brief Constructor.
XC::MPBase_FE::MPBase_FE(int tag, int numDOF_Group, int ndof,const double &Alpha)
  : MPSPBaseFE(tag,numDOF_Group,ndof,Alpha), theConstrainedNode(nullptr)
  {}

//! @brief determine the IDs in myID for those DOFs marked
//! as constrained DOFs, this is obtained from the DOF_Group
//! associated with the constrained node.
int XC::MPBase_FE::determineConstrainedDOFsIDs(const MFreedom_ConstraintBase &theMP,const int &offset)
  {
    int retval= offset;
    DOF_Group *theConstrainedNodesDOFs = theConstrainedNode->getDOF_GroupPtr();
    if(theConstrainedNodesDOFs == 0)
      {
	std::cerr << "WARNING MPBase_FE::setID(void)";
	std::cerr << " - no DOF_Group with Constrained XC::Node\n";
	return -2;
      }
    else
      myDOF_Groups(0) = theConstrainedNodesDOFs->getTag();

    const ID &constrainedDOFs = theMP.getConstrainedDOFs();
    const ID &theConstrainedNodesID = theConstrainedNodesDOFs->getID();    
    
    const int size1 = constrainedDOFs.Size();
    retval+= size1;
    for(int i=0; i<size1; i++)
      {
	int constrained = constrainedDOFs(i);
	if(constrained < 0 || constrained >= theConstrainedNode->getNumberDOF())
          {	    
	    std::cerr << "WARNING MPBase_FE::setID(void) - unknown DOF ";
	    std::cerr << constrained << " at Node\n";
	    myID(i+offset) = -1; // modify so nothing will be added to equations
	    retval = -3;
	  }    	
	else
          {
	    if(constrained >= theConstrainedNodesID.Size())
              {
		std::cerr << "WARNING XC::MPBase_FE::setID(void) - ";
		std::cerr << " Nodes XC::DOF_Group too small\n";
		myID(i+offset) = -1; // modify so nothing will be added to equations
		retval = -4;
	      }
	    else
	      myID(i+offset) = theConstrainedNodesID(constrained);
	  }
      }
    return retval;
  }


//! @brief get the DOF_Group attached to the constrained node.
XC::DOF_Group *XC::MPBase_FE::determineConstrainedNodeDofGrpPtr(void)
  {
    DOF_Group *retval= nullptr;
    if(theConstrainedNode == nullptr)
      {
	std::cerr << "FATAL MPBase_FE::determineConstrainedNodeDofGrpPtr() - Constrained";
	std::cerr << " node does not exist in domain.\n";
	exit(-1);
      }
    else // set up the dof groups tags
      {
        DOF_Group *dofGrpPtr= theConstrainedNode->getDOF_GroupPtr();
       if(dofGrpPtr != nullptr) 
         retval= dofGrpPtr;	        
       else
        std::cerr << "WARNING MPBase_FE::determineConstrainedNodeDofGrpPtr() - node no Group yet?\n";
      }
    return retval;
  }


