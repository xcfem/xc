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
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT' in  main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.12 $
// $Date: 2005/11/28 21:23:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/AnalysisModel.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/AnalysisModel.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for XC::AnalysisModel
// AnalysisModel is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints 
// and MFreedom_Constraints. These objects are all added to the XC::AnalysisModel by a 
// ModelBuilder.
//
// What: "@(#) AnalysisModel.C, revA"

#include <cstdlib>

#include "AnalysisModel.h"
#include "solution/analysis/ModelWrapper.h"
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltySFreedom_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltyMRMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/transformation/TransformationFE.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include "solution/analysis/model/dof_grp/LagrangeDOF_Group.h"
#include "solution/analysis/model/dof_grp/TransformationDOF_Group.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/subdomain/Subdomain.h"
#include "domain/mesh/node/NodeIter.h"
#include "solution/analysis/handler/ConstraintHandler.h"
#include "solution/analysis/handler/TransformationConstraintHandler.h"

//! @brief Constructor.
XC::AnalysisModel::AnalysisModel(ModelWrapper *owr)
  :MovableObject(AnaMODEL_TAGS_AnalysisModel), EntCmd(owr),
   numFE_Ele(0), numDOF_Grp(0), numEqn(0),
   theFEs(this,256,"FEs"), theDOFGroups(this,256,"DOFs"), theFEiter(&theFEs), theDOFGroupiter(&theDOFGroups),
   theFEconst_iter(&theFEs), theDOFGroupconst_iter(&theDOFGroups),
   myDOFGraph(*this), myGroupGraph(*this), updateGraphs(false) {}

//! @brief Constructor.
XC::AnalysisModel::AnalysisModel(int theClassTag,EntCmd *owr)
  :MovableObject(theClassTag), EntCmd(owr),
   numFE_Ele(0), numDOF_Grp(0), numEqn(0),
   theFEs(this,1024,"FEs"), theDOFGroups(this,1024,"DOFs"),theFEiter(&theFEs), theDOFGroupiter(&theDOFGroups),
   theFEconst_iter(&theFEs), theDOFGroupconst_iter(&theDOFGroups),
   myDOFGraph(*this), myGroupGraph(*this), updateGraphs(false) {}

//! @brief Constructor de copia.
XC::AnalysisModel::AnalysisModel(const AnalysisModel &otro)
  : MovableObject(otro), EntCmd(otro),
   numFE_Ele(otro.numFE_Ele), numDOF_Grp(otro.numDOF_Grp), numEqn(otro.numEqn),
   theFEs(otro.theFEs), theDOFGroups(otro.theDOFGroups),theFEiter(&theFEs), theDOFGroupiter(&theDOFGroups),
   theFEconst_iter(&theFEs), theDOFGroupconst_iter(&theDOFGroups),
   myDOFGraph(*this), myGroupGraph(*this), updateGraphs(false) {}

//! @brief Operador asignación.
XC::AnalysisModel &XC::AnalysisModel::operator=(const AnalysisModel &otro)
  { 
    MovableObject::operator=(otro);
    EntCmd::operator=(otro);
    numFE_Ele= otro.numFE_Ele;
    numDOF_Grp= otro.numDOF_Grp;
    numEqn= otro.numEqn;
    theFEs= otro.theFEs;
    theDOFGroups= otro.theDOFGroups;
    myDOFGraph= DOF_Graph(*this);
    myGroupGraph= DOF_GroupGraph(*this);
    updateGraphs= false; //Los acabamos de actualizar
    return *this;
  }

//! @brief Constructor virtual.
XC::AnalysisModel *XC::AnalysisModel::getCopy(void) const
  { return new AnalysisModel(*this); }

// ~AnalysisModel();    
XC::AnalysisModel::~AnalysisModel(void)
  { clearAll(); }    

//! @brief Método para agregar un objeto FE_Element al modelo.
bool XC::AnalysisModel::addFE_Element(FE_Element *theElement)
  {
    // check we don't add a null pointer or this is a subclass
    // trying to use this method when it should'nt
    if(theElement == 0)
      return false;

    // check if an XC::Element with a similar tag already exists in the Domain
    int tag= theElement->getTag();
    TaggedObject *other= theFEs.getComponentPtr(tag);
    if(other)
      {
        std::cerr << "AnalysisModel::addFE_Element - element with tag " << tag << " already exists in model\n"; 
        return false;
      }

    // add the element to the container object for the elements
    bool result= theFEs.addComponent(theElement);
    if(result == true)
      {
        theElement->setAnalysisModel(*this);
        numFE_Ele++;
        updateGraphs= true;
        return true;  // o.k.
      }
    else
      return false;
    return result;
  }

//! @brief Método para crear un objeto FE_Element y agregarlo al modelo.
XC::FE_Element *XC::AnalysisModel::createFE_Element(const int &tag, Element *elePtr)
  {
    FE_Element *retval= nullptr;
    // only create an FE_Element for a subdomain element if it does not
    // do independent analysis .. then subdomain part of this analysis so create
    // an FE_element & set subdomain to point to it.
    if(elePtr->isSubdomain() == true)
      {
        Subdomain *theSub= dynamic_cast<Subdomain *>(elePtr);
        if(theSub->doesIndependentAnalysis() == false)
          {
            retval=new FE_Element(tag, elePtr);
            if(retval) theSub->setFE_ElementPtr(retval);
          }
      }
    else
      retval= new FE_Element(tag, elePtr);

    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating FE_Element: " << tag << std::endl;
    return retval;
  }

//! @brief Método para crear un objeto LagrangeSFreedom_FE y agregarlo al modelo.
XC::LagrangeSFreedom_FE *XC::AnalysisModel::createLagrangeSFreedom_FE(const int &tag, SFreedom_Constraint &theSP, DOF_Group &theDofGrp,const double &alpha)
  {
    LagrangeSFreedom_FE *retval=new LagrangeSFreedom_FE(tag,*getDomainPtr(),theSP,theDofGrp,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating LagrangeFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Método para crear un objeto LagrangeMFreedom_FE y agregarlo al modelo.
XC::LagrangeMFreedom_FE *XC::AnalysisModel::createLagrangeMFreedom_FE(const int &tag,MFreedom_Constraint &theMP,DOF_Group &theDofGrp,const double &alpha)
  {
    LagrangeMFreedom_FE *retval=new LagrangeMFreedom_FE(tag,*getDomainPtr(),theMP,theDofGrp,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating LagrangeFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Método para crear un objeto LagrangeMRMFreedom_FE y agregarlo al modelo.
XC::LagrangeMRMFreedom_FE *XC::AnalysisModel::createLagrangeMRMFreedom_FE(const int &tag,MRMFreedom_Constraint &theMRMP,DOF_Group &theDofGrp,const double &alpha)
  {
    LagrangeMRMFreedom_FE *retval=new LagrangeMRMFreedom_FE(tag,*getDomainPtr(),theMRMP,theDofGrp,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating LagrangeFE_Element: " << tag << std::endl;
    return retval;
  }

//! @brief Método para crear un objeto PenaltySFreedom_FE y agregarlo al modelo.
XC::PenaltySFreedom_FE *XC::AnalysisModel::createPenaltySFreedom_FE(const int &tag, SFreedom_Constraint &theSP, const double &alpha)
  {
    PenaltySFreedom_FE *retval=new PenaltySFreedom_FE(tag,*getDomainPtr(),theSP,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating PenaltyFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Método para crear un objeto PenaltyMFreedom_FE y agregarlo al modelo.
XC::PenaltyMFreedom_FE *XC::AnalysisModel::createPenaltyMFreedom_FE(const int &tag, MFreedom_Constraint &theMP, const double &alpha)
  {
    PenaltyMFreedom_FE *retval=new PenaltyMFreedom_FE(tag,*getDomainPtr(),theMP,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating PenaltyFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Método para crear un objeto PenaltyMRMFreedom_FE y agregarlo al modelo.
XC::PenaltyMRMFreedom_FE *XC::AnalysisModel::createPenaltyMRMFreedom_FE(const int &tag, MRMFreedom_Constraint &theMRMP, const double &alpha)
  {
    PenaltyMRMFreedom_FE *retval=new PenaltyMRMFreedom_FE(tag,*getDomainPtr(),theMRMP,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating PenaltyFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Método para crear un objeto TransformationFE_Element y agregarlo al modelo.
XC::FE_Element *XC::AnalysisModel::createTransformationFE(const int &tag, Element *elePtr, const std::set<int> &transformedEle, std::set<FE_Element *> &theFEset)
  {
    FE_Element *retval= nullptr;
    const bool transformado= transformedEle.find(tag) != transformedEle.end();
    // only create an FE_Element for a subdomain element if it does not
    // do independent analysis .. then subdomain part of this analysis so create
    // an FE_element & set subdomain to point to it.
    if(elePtr->isSubdomain() == true)
      {
        Subdomain *theSub= dynamic_cast<Subdomain *>(elePtr);
        if(theSub->doesIndependentAnalysis() == false)
          {
            if(!transformado)
              { retval=new FE_Element(tag, elePtr); }
            else
              {
                retval=new TransformationFE(tag, elePtr);
                theFEset.insert(retval);
              }
            if(retval) theSub->setFE_ElementPtr(retval);
          }
      }
    else
      if(!transformado)
        { retval=new FE_Element(tag, elePtr); }
      else
        {
          retval=new TransformationFE(tag, elePtr);
          theFEset.insert(retval); 
        }

    if(retval)
      addFE_Element(retval);
    else
      std::cerr << "WARNING AnalysisModel::createFE_Element() "
                << "- ran out of memory"
                << " creating FE_Element: " << tag << std::endl;
    return retval;
  }

// @brief Method to add an DOF to the model.
bool XC::AnalysisModel::addDOF_Group(DOF_Group *theGroup)
  {

    // check we don't add a null pointer or this is a subclass trying
    // to use a method it should'nt be using
    if(theGroup == 0)
      return false;
  
    // check if an Node with a similar tag already exists in the Domain
    int tag= theGroup->getTag();
    TaggedObject *other= theDOFGroups.getComponentPtr(tag);
    if(other)
      {
        std::cerr << "XC::AnalysisModel::addDOF_Group - group with tag " << tag << " already exists in model\n"; 
        return false;
      }

    // add the element to the container object for the elements
    bool result= theDOFGroups.addComponent(theGroup);
    if(result == true)
      {
        numDOF_Grp++;
        updateGraphs= true;
        return true;  // o.k.
      }
    else
      return false;
  }

// @brief Method to create an add a DOF_Group to the model.
XC::DOF_Group *XC::AnalysisModel::createDOF_Group(const int &tag, Node *myNode)
  {
    DOF_Group *dofPtr=new DOF_Group(tag,myNode);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;

    return dofPtr;
  }

//! @brief Añade al modelo la representación mediante multiplicadores
//! de Lagrange de una condición de contorno monopunto.
XC::LagrangeDOF_Group *XC::AnalysisModel::createLagrangeDOF_Group(const int &tag, SFreedom_Constraint *spPtr)
  {
    assert(spPtr);
    LagrangeDOF_Group *dofPtr=new LagrangeDOF_Group(tag,*spPtr);
    if(dofPtr)
      {
        //spPtr->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createLagrangeDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

//! @brief Añade al modelo la representación mediante multiplicadores
//! de Lagrange de una condición de contorno multipunto.
XC::LagrangeDOF_Group *XC::AnalysisModel::createLagrangeDOF_Group(const int &tag, MFreedom_Constraint *mpPtr)
  {
    assert(mpPtr);
    LagrangeDOF_Group *dofPtr=new LagrangeDOF_Group(tag,*mpPtr);
    if(dofPtr)
      {
        //mpPtr->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createLagrangeDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }


//! @brief Añade al modelo los grupos de grados de libertad para la
//! representación mediante multiplicadores
//! de Lagrange de una condición de contorno multiple retained nodes.
XC::LagrangeDOF_Group *XC::AnalysisModel::createLagrangeDOF_Group(const int &tag, MRMFreedom_Constraint *mrmpPtr)
  {
    assert(mrmpPtr);
    LagrangeDOF_Group *dofPtr=new LagrangeDOF_Group(tag,*mrmpPtr);
    if(dofPtr)
      {
        //mpPtr->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createLagrangeDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

XC::TransformationDOF_Group *XC::AnalysisModel::createTransformationDOF_Group(const int &tag, Node *myNode, MFreedom_Constraint *mp, TransformationConstraintHandler *handler)
  {
    assert(mp);
    assert(handler);
    TransformationDOF_Group *dofPtr=new TransformationDOF_Group(tag,myNode,mp,handler);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createTransformationDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

XC::TransformationDOF_Group *XC::AnalysisModel::createTransformationDOF_Group(const int &tag, Node *myNode, MRMFreedom_Constraint *mrmp, TransformationConstraintHandler *handler)
  {
    assert(mrmp);
    assert(handler);
    TransformationDOF_Group *dofPtr=new TransformationDOF_Group(tag,myNode,mrmp,handler);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createTransformationDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

XC::TransformationDOF_Group *XC::AnalysisModel::createTransformationDOF_Group(const int &tag, Node *myNode, TransformationConstraintHandler *handler)
  {
    assert(handler);
    TransformationDOF_Group *dofPtr=new TransformationDOF_Group(tag,myNode,handler);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << "WARNING AnalysisModel::createTransformationDOF_Group() "
                << "- ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

void XC::AnalysisModel::clearAll(void) 
  {
    //XXX LA LÍNEA SIGUIENTE FALLA CUANDO EL MODELO NO CONTIENE ELEMENTOS.
    theFEs.clearAll();
    theDOFGroups.clearAll();

    numFE_Ele=0;
    numDOF_Grp= 0;
    numEqn= 0;    
    updateGraphs= true;
  }



//! @brief Returns the umber of XC::DOF_Group objects added
int XC::AnalysisModel::getNumDOF_Groups(void) const
  { return numDOF_Grp; }


//! @brief Devuelve un puntero al grupo de grados de libertad
//! cuyo tag se pasa como parámetro.
XC::DOF_Group *XC::AnalysisModel::getDOF_GroupPtr(int tag)
  {
    DOF_Group *result= nullptr;
    TaggedObject *other= theDOFGroups.getComponentPtr(tag);
    if(other)
      result= dynamic_cast<DOF_Group *>(other);
    updateGraphs= true;
    return result;
  }

//! @brief Devuelve un puntero al grupo de grados de libertad
//! cuyo tag se pasa como parámetro.
const XC::DOF_Group *XC::AnalysisModel::getDOF_GroupPtr(int tag) const
  {
    const DOF_Group *result= nullptr;
    const TaggedObject *other= theDOFGroups.getComponentPtr(tag);
    if(other)
      result= dynamic_cast<const DOF_Group *>(other);
    return result;
  }

XC::FE_EleIter &XC::AnalysisModel::getFEs()
  {
    theFEiter.reset();
    updateGraphs= true;
    return theFEiter;
  }

XC::FE_EleConstIter &XC::AnalysisModel::getConstFEs() const
  {
    theFEconst_iter.reset();
    return theFEconst_iter;
  }

XC::DOF_GrpIter &XC::AnalysisModel::getDOFGroups()
  {
    theDOFGroupiter.reset();
    updateGraphs= true;
    return theDOFGroupiter;
  }

XC::DOF_GrpConstIter &XC::AnalysisModel::getConstDOFs() const
  {
    theDOFGroupconst_iter.reset();
    return theDOFGroupconst_iter;
  }

void XC::AnalysisModel::setNumEqn(int theNumEqn)
  { numEqn= theNumEqn; }

int XC::AnalysisModel::getNumEqn(void) const
  { return numEqn; }


XC::Graph &XC::AnalysisModel::getDOFGraph(void)
  {
    if(updateGraphs)
      {
        myDOFGraph= DOF_Graph(*this);
        updateGraphs= false;
      }
    return myDOFGraph;
  }

XC::Graph &XC::AnalysisModel::getDOFGroupGraph(void)
  {
    if(updateGraphs)
      {
        myGroupGraph= DOF_GroupGraph(*this);
        updateGraphs= false;
      }
    return myGroupGraph;
  }

const XC::Graph &XC::AnalysisModel::getDOFGraph(void) const
  {
    if(updateGraphs)
      {
        myDOFGraph= DOF_Graph(*this);    
        updateGraphs= false;
      }
    return myDOFGraph;
  }

const XC::Graph &XC::AnalysisModel::getDOFGroupGraph(void) const
  {
    if(updateGraphs)
      {
        myGroupGraph= DOF_GroupGraph(*this);
        updateGraphs= false;
      }
    return myGroupGraph;
  }

void XC::AnalysisModel::setResponse(const Vector &disp, const Vector &vel, const Vector &accel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while((dofPtr= theDOFGrps()) != 0)
      {
        dofPtr->setNodeDisp(disp);
        dofPtr->setNodeVel(vel);
        dofPtr->setNodeAccel(accel);        
      }
  }
        
void XC::AnalysisModel::setDisp(const Vector &disp)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->setNodeDisp(disp);
  }        
        
void XC::AnalysisModel::setVel(const Vector &vel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->setNodeVel(vel);
  }        
        

void XC::AnalysisModel::setAccel(const Vector &accel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->setNodeAccel(accel);        
  }

void XC::AnalysisModel::incrDisp(const Vector &disp)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->incrNodeDisp(disp);
  }
        
void XC::AnalysisModel::incrVel(const Vector &vel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->incrNodeVel(vel);
  }        
        
void XC::AnalysisModel::incrAccel(const Vector &accel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;  
    while((dofPtr= theDOFGrps()) != 0) 
        dofPtr->incrNodeAccel(accel);        
  }        


void XC::AnalysisModel::setNumEigenvectors(int numEigenvectors)
  {
    Node *theNode= nullptr;
    NodeIter &theNodes= getDomainPtr()->getNodes();
    while((theNode= theNodes()) != 0)
        theNode->setNumEigenvectors(numEigenvectors);
  }

//! @brief Comunica al dominio el valor de los autovalores obtenidos.
void XC::AnalysisModel::setEigenvalues(const Vector &eigenvalues)
  { getDomainPtr()->setEigenvalues(eigenvalues); }        

//! @brief Comunica al dominio el valor de los factores
//! de participación modal obtenidos.
void XC::AnalysisModel::setModalParticipationFactors(const Vector &values)
  { getDomainPtr()->setModalParticipationFactors(values); }        

void XC::AnalysisModel::setEigenvector(int mode, const Vector &eigenvalue)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr=nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
      dofPtr->setEigenvector(mode, eigenvalue);        
  }      

void XC::AnalysisModel::applyLoadDomain(double pseudoTime)
  {
    Domain *dom= getDomainPtr();
    if(dom)
      {
        dom->applyLoad(pseudoTime);
        getHandlerPtr()->applyLoad();
      }
    else
      std::cerr << "WARNING: AnalysisModel::applyLoadDomain. No domain linked.\n";
  }


int XC::AnalysisModel::updateDomain(void)
  {
    // check to see there is a XC::Domain linked to the Model
    int res= 0;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << "WARNING: XC::AnalysisModel::updateDomain. No XC::Domain linked.\n";
    else
      {
        // invoke the method
        res= dom->update();
        if(res==0)
          res+= getHandlerPtr()->update();
      }
    return res;
  }


int XC::AnalysisModel::updateDomain(double newTime, double dT)
  {
    // check to see there is a domain linked to the Model
    int res= 0;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << "WARNING: XC::AnalysisModel::updateDomain. No XC::Domain linked.\n";
    else
      {
        // invoke the method
        dom->applyLoad(newTime);
        res= getHandlerPtr()->applyLoad();
        if(res==0)
          res= dom->update();
        if(res==0)
          res= getHandlerPtr()->update();
      }
    return res;
  }


int XC::AnalysisModel::newStepDomain(double dT)
  {
    // check to see there is a domain linked to the Model
    int retval= -1;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << "WARNING: AnalysisModel::newStep. No domain linked."
                << std::endl;
    else
      retval= dom->newStep(dT); // invoke the domain method
    return retval;
  }


//! @brief Consuma el estado del dominio.
int XC::AnalysisModel::commitDomain(void)
  {
    // check to see there is a domain linked to the Model
    int retval= -1;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << "WARNING: AnalysisModel::commitDomain. No domain linked.\n";
    else
      {
        retval= dom->commit();
        if(retval<0)
          {
            std::cerr << "WARNING: AnalysisModel::commitDomain - Domain::commit() failed\n";
            retval= -2;
          }
      }
    return retval;
  }

//! @brief Devuelve el estado del dominio al último consumado.
int XC::AnalysisModel::revertDomainToLastCommit(void)
  {
    // check to see there is a XC::Domain linked to the Model
    int retval= -1;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << "WARNING: XC::AnalysisModel::revertDomainToLastCommit. No domain linked.\n";
    else if(dom->revertToLastCommit()<0)
      {
        std::cerr << "WARNING: AnalysisModel::revertDomainToLastCommit.";
        std::cerr << " Domain::revertToLastCommit() failed.\n";
        retval= -2;
      }        
    return retval;
  }

double XC::AnalysisModel::getCurrentDomainTime(void)
  {
    // check to see there is a Domain linked to the Model
    double retval= 0.0;
    Domain *dom= getDomainPtr();
    if(!dom)
      {
        std::cerr << "WARNING: AnalysisModel::getCurrentDomainTime.";
        std::cerr << " No Domain linked.\n";
      }
    else
      retval= dom->getTimeTracker().getCurrentTime();
    return retval;
  }


void XC::AnalysisModel::setCurrentDomainTime(double newTime)
  {
    Domain *dom= getDomainPtr();
    if(!dom)
      {
        std::cerr << "WARNING: AnalysisModel::setCurrentDomainTime.";
        std::cerr << " No Domain linked.\n";
      }
    else
      dom->setCurrentTime(newTime);
  }



void XC::AnalysisModel::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  {
    Domain *dom= getDomainPtr();
    if(!dom)
      {
        std::cerr << "WARNING: AnalysisModel::setRayleighDampingFactors.";
        std::cerr << " No Domain linked.\n";
      }
    else
      dom->setRayleighDampingFactors(rF); // invoke the method
  }


XC::ModelWrapper *XC::AnalysisModel::getModelWrapper(void)
  { return dynamic_cast<ModelWrapper *>(Owner()); }


const XC::ModelWrapper *XC::AnalysisModel::getModelWrapper(void) const
  { return dynamic_cast<const ModelWrapper *>(Owner()); }


const XC::Domain *XC::AnalysisModel::getDomainPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

XC::Domain *XC::AnalysisModel::getDomainPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

const XC::ConstraintHandler *XC::AnalysisModel::getHandlerPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getConstraintHandlerPtr();
  }

XC::ConstraintHandler *XC::AnalysisModel::getHandlerPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getConstraintHandlerPtr();
  }


int XC::AnalysisModel::sendSelf(CommParameters &cp)
  { return 0; }


int XC::AnalysisModel::recvSelf(const CommParameters &cp) 
  { return 0; }

