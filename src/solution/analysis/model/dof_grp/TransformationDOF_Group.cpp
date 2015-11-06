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
                                                                        
// $Revision: 1.16 $
// $Date: 2005/11/29 23:31:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/TransformationDOF_Group.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 05/99
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the TransformationDOF_Group class interface.
//
// What: "@(#) TransformationDOF_Group.C, revA"

#include <solution/analysis/model/dof_grp/TransformationDOF_Group.h>
#include <cstdlib>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <domain/constraints/SP_Constraint.h>
#include <domain/constraints/SP_ConstraintIter.h>
#include <solution/analysis/handler/TransformationConstraintHandler.h>

const int MAX_NUM_DOF= 16;

// static variables initialisation
XC::UnbalAndTangentStorage XC::TransformationDOF_Group::unbalAndTangentArrayMod(MAX_NUM_DOF+1);
int XC::TransformationDOF_Group::numTransDOFs(0);     // number of objects
XC::TransformationConstraintHandler *XC::TransformationDOF_Group::theHandler= nullptr;     // number of objects

XC::TransformationDOF_Group::TransformationDOF_Group(int tag, Node *node, MP_Constraint *mp, TransformationConstraintHandler *theTHandler)  
  :DOF_Group(tag,node), theMP(mp), theMRMP(nullptr), unbalAndTangentMod(0,unbalAndTangentArrayMod),theSPs()
  {
    // determine the number of DOF 
    int numNodalDOF= node->getNumberDOF();
    const ID &retainedDOF= mp->getRetainedDOFs();
    const ID &constrainedDOF= mp->getConstrainedDOFs();    
    int numNodalDOFConstrained= constrainedDOF.Size();
    int numConstrainedNodeRetainedDOF= numNodalDOF - numNodalDOFConstrained;
    int numRetainedNodeDOF= retainedDOF.Size();

    modNumDOF= numConstrainedNodeRetainedDOF + numRetainedNodeDOF;
    unbalAndTangentMod= UnbalAndTangent(modNumDOF,unbalAndTangentArrayMod);

    // create space for the XC::SP_Constraint array
    theSPs= std::vector<SP_Constraint *>(numNodalDOF,static_cast<SP_Constraint *>(nullptr));
    for(int ii=0; ii<numNodalDOF; ii++) 
      theSPs[ii]= 0;

    /***********************
    // set the XC::SP_Constraint corresponding to the dof in modID
    Domain *theDomain=node->getDomain();
    int nodeTag= node->getTag();    
    SP_ConstraintIter &theSPIter= theDomain->getSPs();
    SP_Constraint *sp;
    while ((sp= theSPIter()) != 0) {
        if(sp->getNodeTag() == nodeTag) {
            int dof= sp->getDOF_Number();
            int loc= 0;
            for(int i=0; i<dof; i++) 
                if(constrainedDOF.getLocation(i) < 0)
                    loc++;
            theSPs[loc]= sp;
        }
    }
    *******************/
     
    // create XC::ID and transformation matrix
    modID= ID(modNumDOF);
    Trans= Matrix(numNodalDOF, modNumDOF);

    // initially set the id values to -2 for any dof still due to constrained node
    for(int i=0; i<numConstrainedNodeRetainedDOF; i++)
      modID(i)= -2;
    
    // for all the constrained dof values set to -1
    for(int j=numConstrainedNodeRetainedDOF; j<modNumDOF; j++)
      modID(j)= -1;

    // for all the dof corresponding to the retained node set initially to -1
    // we don't initially assign these equation nos. - this is done in doneID()
    for(int k=numConstrainedNodeRetainedDOF; k<modNumDOF; k++)
      modID(k)= -1;
    
    numTransDOFs++;
    theHandler= theTHandler;
  }

XC::TransformationDOF_Group::TransformationDOF_Group(int tag, Node *node, MRMP_Constraint *mrmp, TransformationConstraintHandler *theTHandler)  
  :DOF_Group(tag,node), theMRMP(mrmp),unbalAndTangentMod(0,unbalAndTangentArrayMod),theSPs()
  {
    std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
              << std::endl;
  }

void XC::TransformationDOF_Group::setID(int dof, int value)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    if(theMP == 0)
      this->DOF_Group::setID(dof,value);
    else
      modID(dof)= value;
  }
        

XC::TransformationDOF_Group::TransformationDOF_Group(int tag, Node *node, TransformationConstraintHandler *theTHandler)
  :DOF_Group(tag,node), theMP(nullptr), theMRMP(nullptr), modNumDOF(node->getNumberDOF()),
   unbalAndTangentMod(node->getNumberDOF(),unbalAndTangentArrayMod),theSPs() 
  {
    // create space for the SP_Constraint array
    theSPs= std::vector<SP_Constraint *>(modNumDOF,static_cast<SP_Constraint *>(nullptr));
    for(int i=0; i<modNumDOF; i++) 
      theSPs[i]= 0;

    // set the SP_Constraint corresponding to the dof in myID
    Domain *theDomain=node->getDomain();
    int nodeTag= node->getTag();
    SP_ConstraintIter &theSPIter= theDomain->getConstraints().getSPs();
    SP_Constraint *sp;
    while ((sp= theSPIter()) != 0) {
        if(sp->getNodeTag() == nodeTag) {
            int dof= sp->getDOF_Number();
            theSPs[dof]= sp;
        }
    }

    numTransDOFs++;
    theHandler= theTHandler;
}


// ~TransformationDOF_Group();    
//        destructor.

XC::TransformationDOF_Group::~TransformationDOF_Group()
  {
    numTransDOFs--;
  }    


const XC::ID &XC::TransformationDOF_Group::getID(void) const
  {
    if(!modID.Nulo())
      return modID;
    else
      return this->DOF_Group::getID();
  }

int XC::TransformationDOF_Group::getNumDOF(void) const
  { return modNumDOF; }


int XC::TransformationDOF_Group::getNumFreeDOF(void) const
{
  if(!modID.Nulo())
    {
      int numFreeDOF= modNumDOF;
      for(int i=0; i<modNumDOF; i++)
        if(modID(i) == -1)
          numFreeDOF--;
        return numFreeDOF;        
    }
  else
    return this->DOF_Group::getNumFreeDOF();
  }

int XC::TransformationDOF_Group::getNumConstrainedDOF(void) const
  {   
    if(!modID.Nulo())
      {    
        int numConstr= 0;
        for(int i=0; i<modNumDOF; i++)
          if(modID(i) < 0)
            numConstr++;
        return numConstr;
      }
    else
      return this->DOF_Group::getNumConstrainedDOF();        
  }    


const XC::Matrix &XC::TransformationDOF_Group::getTangent(Integrator *theIntegrator) 
  {
    const Matrix &unmodTangent= this->DOF_Group::getTangent(theIntegrator);
    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangentMod.getTangent()= (*T) ^ unmodTangent * (*T);
        unbalAndTangentMod.getTangent().addMatrixTripleProduct(0.0, *T, unmodTangent, 1.0);
        return unbalAndTangentMod.getTangent();
        
      }
    else 
      return unmodTangent;
  }

const XC::Vector &XC::TransformationDOF_Group::getUnbalance(Integrator *theIntegrator)
  {
    const Vector &unmodUnbalance= this->DOF_Group::getUnbalance(theIntegrator);

    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, unmodUnbalance, 1.0);
        return unbalAndTangentMod.getResidual();    
    } else
        return unmodUnbalance;
}


const XC::Vector &XC::TransformationDOF_Group::getCommittedDisp(void)
  {
    const Vector &responseC= myNode->getDisp();

    if(theMRMP!= nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;
    
    if(theMP == 0)
        return responseC;
    else
      {
        int retainedNode= theMP->getNodeRetained();
        Domain *theDomain= myNode->getDomain();
        Node *retainedNodePtr= theDomain->getNode(retainedNode);
        const Vector &responseR= retainedNodePtr->getDisp();
        const ID &retainedDOF= theMP->getRetainedDOFs();
        const ID &constrainedDOF= theMP->getConstrainedDOFs();            
        int numCNodeDOF= myNode->getNumberDOF();
        int numRetainedNodeDOF= retainedDOF.Size();

        int loc= 0;
        for(int i=0; i<numCNodeDOF; i++) {
            if(constrainedDOF.getLocation(i) < 0) {
                (unbalAndTangentMod.getResidual())(loc)= responseC(i);
                loc++;
            } 
        }
        for(int j=0; j<numRetainedNodeDOF; j++) {
            int dof= retainedDOF(j);
            (unbalAndTangentMod.getResidual())(loc)= responseR(dof);
            loc++;
        }

        return unbalAndTangentMod.getResidual();
    }
}

const XC::Vector &XC::TransformationDOF_Group::getCommittedVel(void)
{
    const Vector &responseC= myNode->getVel();
    
    if(theMP == 0)
        return responseC;
    else
      {
        int retainedNode= theMP->getNodeRetained();
        Domain *theDomain= myNode->getDomain();
        Node *retainedNodePtr= theDomain->getNode(retainedNode);
        const Vector &responseR= retainedNodePtr->getVel();
        const ID &retainedDOF= theMP->getRetainedDOFs();
        const ID &constrainedDOF= theMP->getConstrainedDOFs();            
        int numCNodeDOF= myNode->getNumberDOF();
        int numRetainedNodeDOF= retainedDOF.Size();

        int loc= 0;
        for(int i=0; i<numCNodeDOF; i++)
          {
            if(constrainedDOF.getLocation(i) < 0)
              {
                (unbalAndTangentMod.getResidual())(loc)= responseC(i);
                loc++;
              } 
          }
        for(int j=0; j<numRetainedNodeDOF; j++)
          {
            int dof= retainedDOF(j);
            (unbalAndTangentMod.getResidual())(loc)= responseR(dof);
            loc++;
          }
        return unbalAndTangentMod.getResidual();
      }
  }

const XC::Vector &XC::TransformationDOF_Group::getCommittedAccel(void)
  {
    const Vector &responseC= myNode->getAccel();
    
    if(theMP == 0)
        return responseC;
    else
      {
        int retainedNode= theMP->getNodeRetained();
        Domain *theDomain= myNode->getDomain();
        Node *retainedNodePtr= theDomain->getNode(retainedNode);
        const Vector &responseR= retainedNodePtr->getAccel();
        const ID &retainedDOF= theMP->getRetainedDOFs();
        const ID &constrainedDOF= theMP->getConstrainedDOFs();            
        int numCNodeDOF= myNode->getNumberDOF();
        int numRetainedNodeDOF= retainedDOF.Size();

        int loc= 0;
        for(int i=0; i<numCNodeDOF; i++)
          {
            if(constrainedDOF.getLocation(i) < 0)
              {
                (unbalAndTangentMod.getResidual())(loc)= responseC(i);
                loc++;
              } 
          }
        for(int j=0; j<numRetainedNodeDOF; j++)
          {
            int dof= retainedDOF(j);
            (unbalAndTangentMod.getResidual())(loc)= responseR(dof);
            loc++;
          }
        return unbalAndTangentMod.getResidual();
      }
  }

// void setNodeDisp(const XC::Vector &u);
//        Method to set the corresponding nodes displacements to the
//        values in u, components identified by myID;

void XC::TransformationDOF_Group::setNodeDisp(const Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::setNodeDisp(u);
        return;
      }

    const ID &theID= this->getID();
    for(int i=0; i<modNumDOF; i++)
      {
        const int loc= theID(i);
        if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i)= u(loc);
        else
            (unbalAndTangentMod.getResidual())(i)= 0.0;        
      }    
    Matrix *T= this->getT();
    if(T != 0)
      {

        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        myNode->setTrialDisp(unbalAndTangent.getResidual());
      }
    else
      myNode->setTrialDisp(unbalAndTangentMod.getResidual());
  }

void XC::TransformationDOF_Group::setNodeVel(const XC::Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::setNodeVel(u);
        return;
      }
    
   const ID &theID= this->getID();
   for(int i=0; i<modNumDOF; i++)
     {
       int loc= theID(i);
       if(loc >= 0)
         (unbalAndTangentMod.getResidual())(i)= u(loc);
       else         // NO SP STUFF .. WHAT TO DO
         (unbalAndTangentMod.getResidual())(i)= 0.0;            

      }    
    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        myNode->setTrialVel(unbalAndTangent.getResidual());
      }
    else
      myNode->setTrialVel(unbalAndTangentMod.getResidual());
  }


void XC::TransformationDOF_Group::setNodeAccel(const Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::setNodeAccel(u);
        return;
      }
    
   const ID &theID= this->getID();
   for(int i=0; i<modNumDOF; i++)
     {
        int loc= theID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= u(loc);
        else         // NO SP STUFF .. WHAT TO DO
          (unbalAndTangentMod.getResidual())(i)= 0.0;            
      }    
    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        myNode->setTrialAccel(unbalAndTangent.getResidual());
      }
    else
      myNode->setTrialAccel(unbalAndTangentMod.getResidual());
  }


// void setNodeIncrDisp(const XC::Vector &u);
//        Method to set the corresponding nodes displacements to the
//        values in u, components identified by myID;

void XC::TransformationDOF_Group::incrNodeDisp(const XC::Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::incrNodeDisp(u);
        return;
      }

   const ID &theID= this->getID();

   for(int i=0; i<modNumDOF; i++)
     {
       int loc= theID(i);
       if(loc >= 0)
         (unbalAndTangentMod.getResidual())(i)= u(loc);
       else         // DO THE SP STUFF
         (unbalAndTangentMod.getResidual())(i)= 0.0;            
     }    

   Matrix *T= this->getT();
   if(T != 0)
     {
       // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
       unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
       myNode->incrTrialDisp(unbalAndTangent.getResidual());
     }
   else 
     myNode->incrTrialDisp(unbalAndTangentMod.getResidual());
  }
        

void XC::TransformationDOF_Group::incrNodeVel(const Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::incrNodeVel(u);
        return;
      }
    
    const ID &theID= this->getID();
    for(int i=0; i<modNumDOF; i++)
      {
        int loc= theID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= u(loc);
        else         // DO THE SP STUFF
          (unbalAndTangentMod.getResidual())(i)= 0.0;            
      }    
    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        myNode->incrTrialVel(unbalAndTangent.getResidual());
      }
    else
      myNode->incrTrialVel(unbalAndTangentMod.getResidual());
  }


void XC::TransformationDOF_Group::incrNodeAccel(const XC::Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::incrNodeAccel(u);
        return;
      }
    
    const ID &theID= this->getID();
    for(int i=0; i<modNumDOF; i++)
      {
        int loc= theID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= u(loc);
        else         // DO THE SP STUFF
          (unbalAndTangentMod.getResidual())(i)= 0.0;            
      }    
    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        myNode->incrTrialAccel(unbalAndTangent.getResidual());
      }
    else
      myNode->incrTrialAccel(unbalAndTangentMod.getResidual());
  }


void XC::TransformationDOF_Group::setEigenvector(int mode, const Vector &u)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        this->DOF_Group::setEigenvector(mode, u);
        return;
      }
        
   const ID &theID= this->getID();
   for(int i=0; i<modNumDOF; i++)
     {
        int loc= theID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= u(loc);
        // DO THE SP STUFF
      }    
   Matrix *T= this->getT();
   if(T != 0)
     {
       // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
       unbalAndTangent.getResidual().addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
       myNode->setEigenvector(mode, unbalAndTangent.getResidual());
     }
   else
     myNode->setEigenvector(mode, unbalAndTangentMod.getResidual());
  }


XC::Matrix *XC::TransformationDOF_Group::getT(void)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    if(!theMP)
      return nullptr;

    if(theMP->isTimeVarying() == false)
      {
        return &Trans;
      }
    
    const int numNodalDOF= myNode->getNumberDOF();
    const ID &retainedDOF= theMP->getRetainedDOFs();
    const ID &constrainedDOF= theMP->getConstrainedDOFs();    
    int numNodalDOFConstrained= constrainedDOF.Size();
    int numRetainedDOF= numNodalDOF - numNodalDOFConstrained;
    int numRetainedNodeDOF= retainedDOF.Size();

    Trans.Zero();
    const Matrix &Ccr= theMP->getConstraint();
    int col= 0;
    for(int i=0; i<numNodalDOF; i++)
      {
        const int loc= constrainedDOF.getLocation(i);
        if(loc < 0)
          {
            Trans(i,col)= 1.0;
            col++;
          }
        else
          {
            for(int j=0; j<numRetainedNodeDOF; j++)
              Trans(i,j+numRetainedDOF)= Ccr(loc,j);
          }
      }
    return &Trans;    
  }


int XC::TransformationDOF_Group::doneID(void)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    if(theMP == 0)
        return 0;
    
    // get number of DOF & verify valid
    int numNodalDOF= myNode->getNumberDOF();
    const XC::ID &retainedDOF= theMP->getRetainedDOFs();
    const XC::ID &constrainedDOF= theMP->getConstrainedDOFs();    
    int numNodalDOFConstrained= constrainedDOF.Size();
    int numRetainedDOF= numNodalDOF - numNodalDOFConstrained;
    int numRetainedNodeDOF= retainedDOF.Size();

    int retainedNode= theMP->getNodeRetained();
    Domain *theDomain= myNode->getDomain();
    Node *retainedNodePtr= theDomain->getNode(retainedNode);
    DOF_Group *retainedGroup= retainedNodePtr->getDOF_GroupPtr();
    const ID &otherID= retainedGroup->getID();
    
    // set the XC::ID for those dof corresponding to dof at another node
    for(int i=0; i<numRetainedNodeDOF; i++)
      {
        int dof= retainedDOF(i);
        int id= otherID(dof);
        modID(i+numRetainedDOF)= id;
      }
    
    // if constraint is not time-varying determine the transformation matrix
    if(theMP->isTimeVarying() == false)
      {
        Trans.Zero();
        const Matrix &Ccr= theMP->getConstraint();
        int col= 0;
        for(int i=0; i<numNodalDOF; i++)
          {
            const int loc= constrainedDOF.getLocation(i);
            if(loc < 0)
              {
                Trans(i,col)= 1.0;
                col++;
              }
            else
              {
                for(int j=0; j<numRetainedNodeDOF; j++)
                  Trans(i,j+numRetainedDOF)= Ccr(loc,j);
              }
          }
      }
        
    return 0;
  }

int XC::TransformationDOF_Group::addSP_Constraint(SP_Constraint &theSP)
  {
    // add the XC::SP_Constraint
    int dof= theSP.getDOF_Number();
    theSPs[dof]= &theSP;

    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // set a -1 in the correct XC::ID location
    if(theMP == 0)
        this->setID(dof,-1);
    else
      {
        const ID &constrainedDOF= theMP->getConstrainedDOFs();
        int loc= 0;
        for(int i=0; i<dof; i++) 
            if(constrainedDOF.getLocation(i) < 0)
                loc++;
        this->setID(loc,-1);
      } 
    return 0;
  }

int XC::TransformationDOF_Group::enforceSPs(void)
{
  int numDof= myNode->getNumberDOF();
  for(int i=0; i<numDof; i++)
    if(theSPs[i] != 0)
      {
        const double value= theSPs[i]->getValue();
        myNode->setTrialDispComponent(value, i);
      }
  
  return 0;
}

void XC::TransformationDOF_Group::addM_Force(const XC::Vector &Udotdot, double fact)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0 || modID.Nulo())
      {
        this->DOF_Group::addM_Force(Udotdot, fact);
        return;
      }
    
   for(int i=0; i<modNumDOF; i++)
     {
        int loc= modID(i);
        if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i)= Udotdot(loc);
        else         // DO THE SP STUFF
            (unbalAndTangentMod.getResidual())(i)= 0.0;            
      }    

    Vector unmod(Trans.noRows());
    //unmod= Trans * (unbalAndTangentMod.getResidual());
    unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
    this->addLocalM_Force(unmod, fact);
  }


const XC::Vector &XC::TransformationDOF_Group::getM_Force(const XC::Vector &Udotdot, double fact)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0 || modID.Nulo())
      { return this->DOF_Group::getM_Force(Udotdot, fact); }

    this->DOF_Group::zeroTangent();    
    this->DOF_Group::addMtoTang();    
    const Matrix &unmodTangent= this->DOF_Group::getTangent(0);

    
    Vector data(modNumDOF);
    for(int i=0; i<modNumDOF; i++)
      {
        int loc= modID(i);
        if(loc >= 0)
          data(i)= Udotdot(loc);
        else         // DO THE SP STUFF
         data(i)= 0.0;            
      }    

  Matrix *T= this->getT();
  if(T != 0)
    {
      // unbalAndTangentMod.getTangent()= (*T) ^ unmodTangent * (*T);
      unbalAndTangentMod.getTangent().addMatrixTripleProduct(0.0, *T, unmodTangent, 1.0);
      unbalAndTangentMod.getResidual().addMatrixVector(0.0, unbalAndTangentMod.getTangent(), data, 1.0);
      return unbalAndTangentMod.getResidual();
    }
  else
    {
      unbalAndTangentMod.getResidual().addMatrixVector(0.0, unmodTangent, data, 1.0);
      return unbalAndTangentMod.getResidual();
    }
  }

const XC::Vector &XC::TransformationDOF_Group::getC_Force(const XC::Vector &Udotdot, double fact)
  {
    std::cerr << "XC::TransformationDOF_Group::getC_Force() - not yet implemented\n";
    return unbalAndTangentMod.getResidual();
  }

const XC::Vector &XC::TransformationDOF_Group::getTangForce(const XC::Vector &Udotdot, double fact)
  {
    std::cerr << "XC::TransformationDOF_Group::getTangForce() - not yet implemented\n";
    return unbalAndTangentMod.getResidual();
  }



// AddingSensitivity:BEGIN ////////////////////////////////////////
const XC::Vector &XC::TransformationDOF_Group::getDispSensitivity(int gradNumber)
  {
    const XC::Vector &result= this->DOF_Group::getDispSensitivity(gradNumber);

    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, result, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return result;
  }

const XC::Vector &XC::TransformationDOF_Group::getVelSensitivity(int gradNumber)
  {
    const XC::Vector &result= this->DOF_Group::getVelSensitivity(gradNumber);

    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, result, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return result;
  }

const XC::Vector &XC::TransformationDOF_Group::getAccSensitivity(int gradNumber)
  {
    const XC::Vector &result= this->DOF_Group::getAccSensitivity(gradNumber);

    Matrix *T= this->getT();
    if(T != 0)
      {
        // unbalAndTangentMod.getResidual()= (*T) ^ unmodUnbalance;
        unbalAndTangentMod.getResidual().addMatrixTransposeVector(0.0, *T, result, 1.0);
        return unbalAndTangentMod.getResidual();    
      }
    else
      return result;
  }
        
        

int XC::TransformationDOF_Group::saveSensitivity(Vector *u,Vector *udot,Vector *udotdot, int gradNum,int numGrads)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0)
      {
        return this->DOF_Group::saveSensitivity(u, udot, udotdot, gradNum, numGrads);
      }
  
    // Get sensitivities for my dof out of vectors
    Vector myV(modNumDOF);
        
    const ID &theID= this->getID();
    for(int i=0; i<modNumDOF; i++)
      {
        int loc= theID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= (*u)(loc);
        // DO THE SP STUFF
      }    
    Matrix *T= this->getT();
    if(T)
      {
        // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
        myV.addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
      }
    else
      myV= unbalAndTangentMod.getResidual();


  // Vel and Acc sensitivities only if they are being delivered
  if((udot != 0) && (udotdot != 0))
    {
      Vector myVdot(modNumDOF);
      Vector myVdotdot(modNumDOF);
      for(int i=0; i<modNumDOF; i++)
        {
          int loc= theID(i);
          if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i)= (*udot)(loc);
          // DO THE SP STUFF
        }    

      if(T)
        {
          // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
          myVdot.addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        }
      else
       myVdot= unbalAndTangentMod.getResidual();

      for(int j=0; j<modNumDOF; j++)
        {
          int loc= theID(j);
          if(loc >= 0)
            (unbalAndTangentMod.getResidual())(j)= (*udotdot)(loc);
          // DO THE SP STUFF
        }
      if(T)
        {
          // unbalAndTangent.getResidual()= (*T) * (unbalAndTangentMod.getResidual());
          myVdotdot.addMatrixVector(0.0, *T, unbalAndTangentMod.getResidual(), 1.0);
        }
      else
        myVdotdot= unbalAndTangentMod.getResidual();
      myNode->saveSensitivity(&myV, &myVdot, &myVdotdot, gradNum, numGrads);
    }
  else
    myNode->saveSensitivity(&myV,nullptr,nullptr,gradNum,numGrads);
  return 0;
}

void XC::TransformationDOF_Group::addM_ForceSensitivity(const XC::Vector &Udotdot, double fact)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0 || modID.Nulo())
      {
        this->DOF_Group::addM_ForceSensitivity(Udotdot, fact);
        return;
      }
    
    for(int i=0; i<modNumDOF; i++)
      {
        const int loc= modID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= Udotdot(loc);
        else         // DO THE SP STUFF
          (unbalAndTangentMod.getResidual())(i)= 0.0;            
      }    

    Vector unmod(Trans.noRows());
    //unmod= Trans * (unbalAndTangentMod.getResidual());
    unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
    this->DOF_Group::addM_ForceSensitivity(unmod, fact);
  }

void XC::TransformationDOF_Group::addD_Force(const XC::Vector &Udot, double fact)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0 || modID.Nulo())
      {
        this->DOF_Group::addD_Force(Udot, fact);
        return;
      }
    
    for(int i=0; i<modNumDOF; i++)
      {
        const int loc= modID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= Udot(loc);
        else         // DO THE SP STUFF
          (unbalAndTangentMod.getResidual())(i)= 0.0;            
      }    

    Vector unmod(Trans.noRows());
    //unmod= Trans * (unbalAndTangentMod.getResidual());
    unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
    this->DOF_Group::addD_Force(unmod, fact);
  }

void XC::TransformationDOF_Group::addD_ForceSensitivity(const XC::Vector &Udot, double fact)
  {
    if(theMRMP != nullptr)
      std::cerr << "TransformationDOF_Group is not implemented for multi retained nodes constraints."
                << std::endl;

    // call base class method and return if no MP_Constraint
    if(theMP == 0 || modID.Nulo())
      {
        this->DOF_Group::addD_ForceSensitivity(Udot, fact);
        return;
      }
    
    for(int i=0; i<modNumDOF; i++)
      {
        const int loc= modID(i);
        if(loc >= 0)
          (unbalAndTangentMod.getResidual())(i)= Udot(loc);
        else         // DO THE SP STUFF
          (unbalAndTangentMod.getResidual())(i)= 0.0;    
      }    

    Vector unmod(Trans.noRows());
    //unmod= Trans * (unbalAndTangentMod.getResidual());
    unmod.addMatrixVector(0.0, Trans, unbalAndTangentMod.getResidual(), 1.0);
    this->DOF_Group::addD_ForceSensitivity(unmod, fact);
  }

// AddingSensitivity:END //////////////////////////////////////////
