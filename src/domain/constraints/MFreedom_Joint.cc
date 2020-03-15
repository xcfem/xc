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

#include "MFreedom_Joint.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>

#include <cstdlib>
#include <cmath>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
 
//! @brief Constructor.
//! @param tag: tag for the multi-freedom constraint.
//! @param classTag: tag of the object class.
XC::MFreedom_Joint::MFreedom_Joint(int tag, int classTag)
 : MFreedom_Constraint(tag, classTag),
   RetainedNode(nullptr), ConstrainedNode(nullptr),
   LargeDisplacement(0), Length0(0.0)
  {}

//! @brief Constructor.
//! @param theDomain: domain where the constraint is defined.
//! @param tag: tag for the multi-freedom constraint.
//! @param classTag: tag of the object class.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param LrgDsp: true if large displacement (geometric non-linearity) must be expected: 0 for constant constraint matrix(small deformations), 1 for time varying constraint matrix(large deformations), 2 for large deformations with length correction.
XC::MFreedom_Joint::MFreedom_Joint(Domain *domain, int tag, int classTag, int nodeRetain, int nodeConstr, int LrgDsp )
  : MFreedom_Constraint(tag,nodeRetain,nodeConstr, classTag),
    RetainedNode(nullptr), ConstrainedNode(nullptr),
    LargeDisplacement( LrgDsp ), Length0(0.0)
  {
    setDomain(domain);
    if(getDomain() == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; specified domain does not exist";
        std::cerr << "Domain = 0\n";
        return;
      }
  }

//! @brief Destructor.
XC::MFreedom_Joint::~MFreedom_Joint(void)
  {
    if(RetainedNode)
      RetainedNode->disconnect(this);
    if(ConstrainedNode)
      ConstrainedNode->disconnect(this);
  }


//! @brief Returns true if constraint varies with time (geometric non-linearity).
bool XC::MFreedom_Joint::isTimeVarying(void) const
  { return (LargeDisplacement!=0);}


//! @brief Printing.
void XC::MFreedom_Joint::Print(std::ostream &s, int flag ) const
  {
    s << "MFreedom_Joint: " << this->getTag() << "\n";
    s << "\tConstrained XC::Node: " << getNodeConstrained();
    s << " Retained XC::Node: " << getNodeRetained() ;
    s << " Large Disp: " << LargeDisplacement;
    s << " constrained dof: " << constrDOF;    
    s << " retained dof: " << retainDOF;        
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


//! @brief Sets the domain of the constraint.
void XC::MFreedom_Joint::setDomain(Domain *domain)
  {
    if(domain == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; specified domain does not exist";
        std::cerr << "Domain = 0\n";
        return;
      }
    MFreedom_Constraint::setDomain(domain);
    RetainedNode= getDomain()->getNode(getNodeRetained());
    if(RetainedNode)
      RetainedNode->connect(this);
    else
      {
        std::cerr <<  getClassName() << "::" << __FUNCTION__
                  << "; retained node: "<< getNodeRetained()
		  << "does not exist in model\n";
      }
    ConstrainedNode= getDomain()->getNode(getNodeConstrained());
    if(ConstrainedNode)
      ConstrainedNode->connect(this);
    else
      {
        std::cerr <<  getClassName() << "::" << __FUNCTION__
                  << "; constrained node: "<< getNodeConstrained()
                  << "does not exist in model\n";
      }
  
  }


