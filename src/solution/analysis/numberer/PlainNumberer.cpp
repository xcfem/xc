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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/PlainNumberer.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/numberer/PlainNumberer.C
// 
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for XC::PlainNumberer.
// PlainNumberer is a subclass of XC::DOF_Numberer. The XC::PlainNumberer assigns
// equation numbers to the DOFs on a first come first serve basis; that is 
// it gets the XC::DOF_GrpIter and assigns the equation numbers to the DOFs
// as it iterates through the iter.
//
// What: "@(#) PlainNumberer.C, revA"

#include <solution/analysis/numberer/PlainNumberer.h>
#include <solution/analysis/model/AnalysisModel.h>

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <utility/matrix/ID.h>

#include <domain/domain/Domain.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MP_ConstraintIter.h>
#include <domain/constraints/MRMP_ConstraintIter.h>


XC::PlainNumberer::PlainNumberer(ModelWrapper *owr) 
  :DOF_Numberer(owr,NUMBERER_TAG_PlainNumberer) {}

//! @brief Constructor virtual.
XC::DOF_Numberer *XC::PlainNumberer::getCopy(void) const
  { return new PlainNumberer(*this);  }


//! @brief Method to number the unnumbered DOFs in the DOF Groups. It does so
//!        on a first come fist serve basis, first assigning all DOFs with a -2
//!        a number, then all DOFs with a -3 a number.
int XC::PlainNumberer::numberDOF(int lastDOF)
  {
    int eqnNumber= 0; // start equation number= 0

    // get a pointer to the model & check its not null
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    Domain *theDomain= nullptr;
    if(theModel != 0) theDomain= theModel->getDomainPtr();

    if(theModel == 0 || theDomain == 0)
      {
        std::cerr << "WARNING PlainNumberer::numberDOF(int) -";
        std::cerr << " - no AnalysisModel.\n";
        return -1;
      }
    
    if(lastDOF != -1)
      {
        std::cerr << "WARNING PlainNumberer::numberDOF(int lastDOF):";
        std::cerr << " does not use the lastDOF as requested\n";
      }
    
    // iterate throgh  the DOFs first time setting -2 values
    DOF_GrpIter &theDOFGroups= theModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    while((dofGroupPtr= theDOFGroups()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -2) 
            dofGroupPtr->setID(i,eqnNumber++);
      }


    // iterate throgh  the DOFs second time setting -3 values
    DOF_GrpIter &moreDOFs= theModel->getDOFGroups();
    
    while((dofGroupPtr= moreDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -3)
            dofGroupPtr->setID(i,eqnNumber++);
      }

    // iterate through the DOFs one last time setting any -4 values
    DOF_GrpIter &tDOFs= theModel->getDOFGroups();
    while((dofGroupPtr= tDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        bool have4s= false;
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -4)
            {
              have4s= true;
              break;
            }

        if(have4s)
          {
            const int nodeID= dofGroupPtr->getNodeTag();
            // loop through the MP_Constraints to see if any of the
            // DOFs are constrained, note constraint matrix must be diagonal
            // with 1's on the diagonal
            MP_ConstraintIter &theMPs= theDomain->getCondsContorno().getMPs();
            MP_Constraint *mpPtr= nullptr;
            while((mpPtr= theMPs()) != 0 )
              {
                // note keep looping over all in case multiple constraints
                // are used to constrain a node -- can't assume intelli user
                if(mpPtr->getNodeConstrained() == nodeID)
                  {
                    int nodeRetained= mpPtr->getNodeRetained();
                    Node *nodeRetainedPtr= theDomain->getNode(nodeRetained);
                    DOF_Group *retainedDOF= nodeRetainedPtr->getDOF_GroupPtr();
                    const ID &retainedDOFIDs= retainedDOF->getID();
                    const ID &constrainedDOFs= mpPtr->getConstrainedDOFs();
                    const ID &retainedDOFs= mpPtr->getRetainedDOFs();
                    for(int i=0; i<constrainedDOFs.Size(); i++)
                      {
                        const int dofC= constrainedDOFs(i);
                        const int dofR= retainedDOFs(i);
                        const int dofID= retainedDOFIDs(dofR);
                        dofGroupPtr->setID(dofC, dofID);
                      }
                  }
              }
            MRMP_ConstraintIter &theMRMPs= theDomain->getCondsContorno().getMRMPs();
            MRMP_Constraint *mrmpPtr;
            while((mrmpPtr= theMRMPs()) != 0 )
              {
                std::cerr << "PlainNumberer::numberDOF(int) write code loop through the MRMP_Constraints." << std::endl;
              }
          }        
      }
    eqnNumber--;
    const int numEqn= eqnNumber - START_EQN_NUMBER +1;
        
    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= theModel->getFEs();
    FE_Element *elePtr= nullptr;
    while((elePtr= theEle()) != 0)
      elePtr->setID();

    // set the numOfEquation in the Model
    theModel->setNumEqn(numEqn);

    return numEqn;
  }


int XC::PlainNumberer::sendSelf(CommParameters &cp)
  { return 0; }

int XC::PlainNumberer::recvSelf(const CommParameters &cp)
  { return 0; }


int XC::PlainNumberer::numberDOF(ID &lastDOFs)
  {
    int eqnNumber= 0; // start equation number= 0
    
    // get a pointer to the model & check its not null
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    Domain *theDomain =0;
    if(theModel != 0) theDomain= theModel->getDomainPtr();

    if(theModel == 0 || theDomain == 0)
      {
        std::cerr << "WARNING PlainNumberer::numberDOF(int) -";
        std::cerr << " - no AnalysisModel.\n";
        return -1;
      }
    
    std::cerr << "WARNING PlainNumberer::numberDOF(ID):";
    std::cerr << " does not use the lastDOFs as requested\n";
    
    // iterate throgh  the DOFs first time setting -2 values
    DOF_GrpIter &theDOFGroups= theModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr= theDOFGroups()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -2) dofGroupPtr->setID(i,eqnNumber++);
      }

    // iterate throgh  the DOFs first time setting -3 values
    DOF_GrpIter &moreDOFs= theModel->getDOFGroups();
    
    while((dofGroupPtr= moreDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -3) dofGroupPtr->setID(i,eqnNumber++);
      }
    // iterate through the DOFs one last time setting any -4 values
    // iterate through the DOFs one last time setting any -4 values
    DOF_GrpIter &tDOFs= theModel->getDOFGroups();
    while((dofGroupPtr= tDOFs()) != 0)
      {
        const ID &theID= dofGroupPtr->getID();
        bool have4s= false;
        for(int i=0; i<theID.Size(); i++)
          if(theID(i) == -4)
            {
              have4s= true;
              break;
            }

        if(have4s)
          {
            int nodeID= dofGroupPtr->getNodeTag();
            // loop through the MP_Constraints to see if any of the
            // DOFs are constrained, note constraint matrix must be diagonal
            // with 1's on the diagonal
            MP_ConstraintIter &theMPs= theDomain->getCondsContorno().getMPs();
            MP_Constraint *mpPtr= nullptr;
            while((mpPtr= theMPs()) != 0 )
              {
                // note keep looping over all in case multiple constraints
                // are used to constrain a node -- can't assume intelli user
                if(mpPtr->getNodeConstrained() == nodeID)
                  {
                    int nodeRetained= mpPtr->getNodeRetained();
                    Node *nodeRetainedPtr= theDomain->getNode(nodeRetained);
                    DOF_Group *retainedDOF= nodeRetainedPtr->getDOF_GroupPtr();
                    const ID &retainedDOFIDs= retainedDOF->getID();
                    const ID &constrainedDOFs= mpPtr->getConstrainedDOFs();
                    const ID &retainedDOFs= mpPtr->getRetainedDOFs();
                    for(int i=0; i<constrainedDOFs.Size(); i++)
                      {
                        int dofC= constrainedDOFs(i);
                        int dofR= retainedDOFs(i);
                        int dofID= retainedDOFIDs(dofR);
                        dofGroupPtr->setID(dofC, dofID);
                      }
                  }
              }
            MRMP_ConstraintIter &theMRMPs= theDomain->getCondsContorno().getMRMPs();
            MRMP_Constraint *mrmpPtr;
            while((mrmpPtr= theMRMPs()) != 0 )
              {
                 std::cerr << "PlainNumberer::numberDOF(ID) write code loop through the MRMP_Constraints." << std::endl;
              }

          }        
      }
    eqnNumber--;
    int numEqn= eqnNumber - START_EQN_NUMBER +1;
        
    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= theModel->getFEs();
    FE_Element *elePtr;
    while((elePtr= theEle()) != 0)
      elePtr->setID();

    // set the numOfEquation in the Model
    theModel->setNumEqn(numEqn);

    return numEqn;
  }
