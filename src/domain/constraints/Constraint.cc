//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
#include "vtkCellType.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor; receives the tag of the class as a paramenter.
//! @param classTag: identifier for the object class (see classTags.h).
XC::Constraint::Constraint(int classTag)
  :ContinuaReprComponent(0,classTag), constrNodeTag(0), initialized(false) {}


//! @brief Constructor.
//! @param classTag: identifier for the object class (see classTags.h).
//! @param tag: identifier for the constraint.
//! @param node: identifier for the node that will be constrained.
XC::Constraint::Constraint(int tag, int node, int classTag)
  :ContinuaReprComponent(tag, classTag), constrNodeTag(node), initialized(false) {}

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
//! @param model: domain in which the constraint is created.
void XC::Constraint::setDomain(Domain *model)
  {
    Domain *tmp= this->getDomain();
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
            if(n)
	      n->connect(this);
	    else
	      {
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; constrained node: "
			  << constrNodeTag
		          << " does not exist in Domain."
			  << Color::def << std::endl;
                exit(-1);

	      }
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
      if(retval == 0)
	{
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; Constrained node does not exist in Domain\n"
		    << constrNodeTag
		    << Color::def << std::endl;
	  exit(-1);
	}
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

//! @brief Returns true ifafecta to the node cuyo tag being passed as parameter.
bool XC::Constraint::affectsNode(int nodeTag) const
  { return (nodeTag== constrNodeTag); }

//! @brief Returns true if the constraint affects the node and DOF arguments.
bool XC::Constraint::affectsNodeAndDOF(int nodeTag, int theDOF) const
  {
    bool retval= false;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << " not implemented."
	      << Color::def << std::endl;
    return retval;
  }

//! @brief Send members through the communicator argument.
//! @param cp: definition of the communication parameters.
int XC::Constraint::sendData(Communicator &comm)
  {
    int res= ContinuaReprComponent::sendData(comm);
    res+= comm.sendInt(constrNodeTag,getDbTagData(),CommMetaData(2));
    res+= comm.sendBool(initialized,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members through the communicator argument.
//! @param cp: definition of the communication parameters.
int XC::Constraint::recvData(const Communicator &comm)
  {
    int res= ContinuaReprComponent::recvData(comm);
    res+= comm.receiveInt(constrNodeTag,getDbTagData(),CommMetaData(2));
    res+= comm.receiveBool(initialized,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Prints some information about the constraint.
//! @param s: output stream.
//! @param flag: indicates the amount of information to be printed.
void XC::Constraint::Print(std::ostream &s, int flag) const
  {
    s << "Constraint: " << this->getTag();
    s << "\t XC::Node: " << constrNodeTag << std::endl;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::Constraint::getPyDict(void) const
  {
    boost::python::dict retval= ContinuaReprComponent::getPyDict();
    retval["constrNodeTag"]= constrNodeTag;
    retval["initialized"]= initialized;
    return retval;
  }
//! @brief Set the values of the object members from a Python dictionary.
void XC::Constraint::setPyDict(const boost::python::dict &d)
  {
    ContinuaReprComponent::setPyDict(d);
    this->constrNodeTag= boost::python::extract<int>(d["constrNodeTag"]);
    this->initialized= boost::python::extract<bool>(d["initialized"]);
  }

//! @brief returns the VTK cell type.
int XC::Constraint::getVtkCellType(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented."
	      << Color::def << std::endl;
    return VTK_EMPTY_CELL;
  }






