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
//Constraint.cc

#include "domain/constraints/Constraint.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"

#include "utility/matrix/ID.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Default constructor; receives the tag of the class as a paramenter.
//! @param classTag: identifier for the object class (see classTags.h).
XC::Constraint::Constraint(int classTag)
  :ContinuaReprComponent(0,classTag), constrNodeTag(0) {}


//! @brief Constructor.
//! @param classTag: identifier for the object class (see classTags.h).
//! @param tag: identifier for the constraint.
//! @param node: identifier for the node that will be constrained.
XC::Constraint::Constraint(int tag, int node, int classTag)
  :ContinuaReprComponent(tag, classTag), constrNodeTag(node) {}

//! @brief Destructor.
XC::Constraint::~Constraint(void)
  {
    Domain *tmp= getDomain();
    if(tmp)
      {
        Node *n= tmp->getNode(constrNodeTag);
        if(n) n->disconnect(this);
      }
  }

//! @brief Sets the domain for the constraint.
//! @param model: domain in wich the constraint is created.
void XC::Constraint::setDomain(Domain *model)
  {
    Domain *tmp= getDomain();
    if(tmp!=model)
      {
        if(tmp)
          {
            Node *n= tmp->getNode(constrNodeTag);
            if(n) n->disconnect(this);
          }
        DomainComponent::setDomain(model);
        if(model)
          {
            Node *n= model->getNode(constrNodeTag);
            if(n) n->connect(this);
          }
      }
  }

//! @brief Sets the identifier of the node that will be constrained.
//! @param nt: node tag.
void XC::Constraint::setNodeTag(const int &nt)
  { constrNodeTag= nt; }


//! @brief Returns the identifier of the constrained node.
int XC::Constraint::getNodeTag(void) const
  {
    // return id of constrained node
    return constrNodeTag;
  }

//! @brief Returns a pointer to the constrained node.
const XC::Node *XC::Constraint::getNode(void) const
  {
    const Node *retval= nullptr;
    Domain *tmp= getDomain();
    if(tmp)
      retval= tmp->getNode(constrNodeTag);
    return retval;
  }

//! @brief Returns an index for the node (used for VTK arrays).
int XC::Constraint::getNodeIdx(void) const
  {
    int retval= -1;
    const Node *n= getNode();
    if(n)
      retval= n->getIdx();
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
//! @param cp: definition of the communication parameters.
int XC::Constraint::sendData(CommParameters &cp)
  {
    int res= ContinuaReprComponent::sendData(cp);
    res+= cp.sendInt(constrNodeTag,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
//! @param cp: definition of the communication parameters.
int XC::Constraint::recvData(const CommParameters &cp)
  {
    int res= ContinuaReprComponent::recvData(cp);
    res+= cp.receiveInt(constrNodeTag,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Prints some information about the constraint.
//! @param s: output stream.
//! @param flag: indicates the amount of information to be printed.
void XC::Constraint::Print(std::ostream &s, int flag) 
  {
    s << "Constraint: " << this->getTag();
    s << "\t XC::Node: " << constrNodeTag << std::endl;
  }

//! @brief returns the VTK cell type.
int XC::Constraint::getVtkCellType(void) const
  {
    std::cerr << "Constraint::getVtkCellType; no implementada." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief returns the "MED fichier" cell type.
int XC::Constraint::getMEDCellType(void) const
  { return MED_POINT1; }






