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
//MP_FE.cpp

#include "MP_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MP_Constraint.h>


//! @brief Constructor.
XC::MP_FE::MP_FE(int tag, int numDOF_Group, int ndof, MP_Constraint &TheMP,const double &Alpha)
  : MPBase_FE(tag,numDOF_Group,ndof,Alpha), theMP(&TheMP), theRetainedNode(nullptr)
  {}


//! @brief determine the IDs in myID for those DOFs marked
//! as constrained DOFs, this is obtained from the DOF_Group
//! associated with the constrained node.
int XC::MP_FE::determineConstrainedDOFsIDs(const int &offset)
  { return MPBase_FE::determineConstrainedDOFsIDs(*theMP,offset); }

//! @brief determine determine the IDs for the retained dof's
int XC::MP_FE::determineRetainedDOFsIDs(const int &offset)
  {
    int retval= offset;
    DOF_Group *theRetainedNodesDOFs = theRetainedNode->getDOF_GroupPtr();
    if(theRetainedNodesDOFs == 0)
     {
	std::cerr << "WARNING XC::MP_FE::setID(void)";
	std::cerr << " - no DOF_Group with Retained XC::Node\n";
	return -2;
      }    
    
    const ID &RetainedDOFs = theMP->getRetainedDOFs();
    const ID &theRetainedNodesID = theRetainedNodesDOFs->getID();    

    const int size2 = RetainedDOFs.Size();
    retval+= size2;
    for(int j=0; j<size2; j++)
      {
	int retained = RetainedDOFs(j);
	if(retained < 0 || retained >= theRetainedNode->getNumberDOF())
          {
	    std::cerr << "WARNING XC::PenaltyMP_FE::setID(void) - unknown DOF ";
	    std::cerr << retained << " at XC::Node\n";
	    myID(j+offset) = -1; // modify so nothing will be added
	    retval= -3;
	  }    	
	else
          {
	    if(retained >= theRetainedNodesID.Size())
              {
		std::cerr << "WARNING XC::PenaltyMP_FE::setID(void) - ";
		std::cerr << " Nodes XC::DOF_Group too small\n";
		myID(j+offset) = -1; // modify so nothing will be added 
		retval= -4;
	      }
	    else
	      myID(j+offset) = theRetainedNodesID(retained);
	  }
      }

     myDOF_Groups(1) = theRetainedNodesDOFs->getTag();
    return retval;
  }

//! @brief get the DOF_Group attached to the retained node.
XC::DOF_Group *XC::MP_FE::determineRetainedNodeDofGrpPtr(void)
  {
    DOF_Group *retval= nullptr;
    if(theRetainedNode == nullptr)
      {
	std::cerr << "FATAL MPBase_FE::determineRetainedNodeDofGrpPtr() - Retained";
	std::cerr << " node does not exist in domain.\n";
	exit(-1);
      }
    else // set up the dof groups tags
      {
        DOF_Group *dofGrpPtr= theRetainedNode->getDOF_GroupPtr();
        if(dofGrpPtr != nullptr) 
          retval= dofGrpPtr;	        
        else
         std::cerr << "WARNING MPBase_FE::determineRetainedNodeDofGrpPtr() - node no Group yet?\n";
      }
    return retval;
  }

