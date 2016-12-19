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
//MRMFreedom_FE.cpp

#include "MRMFreedom_FE.h"
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/domain/Domain.h>

//! @brief Constructor.
XC::MRMFreedom_FE::MRMFreedom_FE(int tag, int numDOF_Group, int ndof, MRMFreedom_Constraint &TheMP,const double &Alpha)
  : MPBase_FE(tag,numDOF_Group,ndof,Alpha), theMRMP(&TheMP)
  {}


//! @brief determine the IDs in myID for those DOFs marked
//! as constrained DOFs, this is obtained from the DOF_Group
//! associated with the constrained node.
int XC::MRMFreedom_FE::determineConstrainedDOFsIDs(const int &offset)
  { return MPBase_FE::determineConstrainedDOFsIDs(*theMRMP,offset); }

//! @brief determine determine the IDs for the retained dof's
int XC::MRMFreedom_FE::determineRetainedDOFsIDs(const int &offset)
  {
    int retval= offset;
    const ID &retainedNodeTags= theMRMP->getRetainedNodeTags();
    const size_t sz= retainedNodeTags.Size();
    int conta= offset;
    const ID &RetainedDOFs = theMRMP->getRetainedDOFs();
    for(size_t i= 0;i<sz;i++)
      {
        Node *theRetainedNode= theRetainedNodes[i];

        DOF_Group *theRetainedNodesDOFs = theRetainedNode->getDOF_GroupPtr();
        if(theRetainedNodesDOFs == 0)
          {
	    std::cerr << "WARNING XC::MRMFreedom_FE::setID(void)";
	    std::cerr << " - no XC::DOF_Group with Retained XC::Node\n";
	    return -2;
          }    
    
        const ID &theRetainedNodesID = theRetainedNodesDOFs->getID();    

        int size2 = RetainedDOFs.Size();
        for(int j=0; j<size2; j++)
          {
	    const int retained = RetainedDOFs(j);
	    if(retained < 0 || retained >= theRetainedNode->getNumberDOF())
              {
	        std::cerr << "WARNING XC::MRMFreedom_FE::setID(void) - unknown DOF ";
	        std::cerr << retained << " at XC::Node\n";
	        myID(conta) = -1; // modify so nothing will be added
	        retval = -3;
	      }    	
	    else
              {
	        if(retained >= theRetainedNodesID.Size())
                  {
		    std::cerr << "WARNING XC::MRMFreedom_FE::setID(void) - ";
		    std::cerr << " Nodes DOF_Group too small\n";
		    myID(conta) = -1; // modify so nothing will be added 
		    retval = -4;
	          }
	        else
		  myID(conta) = theRetainedNodesID(retained);
	      }
            conta++;
          }
        myDOF_Groups(i+1)= theRetainedNodesDOFs->getTag();
      }
    if(retval>=0) retval+= conta-RetainedDOFs.Size();
    return retval;
  }

//! @brief get the DOF_Group attached to the retained nodes.
int XC::MRMFreedom_FE::determineRetainedNodesDofGrpPtr(Domain &theDomain, const int &offset)
  {
    const ID &retainedNodeTags= theMRMP->getRetainedNodeTags();
    const size_t sz= retainedNodeTags.Size();
    theRetainedNodes= std::vector<Node *>(sz,nullptr);   
    DOF_Group *dofGrpPtr= nullptr;
    for(size_t i= 0;i<sz;i++)
      {
        Node *theRetainedNode= theDomain.getNode(retainedNodeTags(i));
        theRetainedNodes[i]= theRetainedNode;

        if(theRetainedNode == 0)
          {
	    std::cerr << "FATAL MRMFreedom_FE::determineRetainedNodesDofGrpPtr() - Retained";
	    std::cerr << " node does not exist in Domain\n";
	    std::cerr << retainedNodeTags(i) << std::endl;
	    exit(-1);
          }	


        dofGrpPtr = theRetainedNode->getDOF_GroupPtr();
        if(dofGrpPtr!= 0)
          myDOF_Groups(i+offset) = dofGrpPtr->getTag();
        else 
          std::cerr << "WARNING MRMFreedom_FE::MRMFreedom_FE() - node no Group yet?\n"; 
      }
    return offset+sz;
  }

