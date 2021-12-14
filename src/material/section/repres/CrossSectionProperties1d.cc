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
//CrossSectionProperties1d.cc

#include <material/section/repres/CrossSectionProperties1d.h>
#include "utility/utils/misc_utils/inertia.h"
#include "domain/mesh/element/utils/Information.h"
#include "material/section/ResponseId.h"

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "material/section/SectionForceDeformation.h"

XC::Matrix XC::CrossSectionProperties1d::ks(1,1);

bool XC::CrossSectionProperties1d::check_values(void)
  {
    bool retval= true;
    if(e <= 0.0)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; Input E <= 0.0 ... setting E to 1.0\n";
        e= 1.0;
        retval= false;
      }
    if(a <= 0.0)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; Input A <= 0.0 ... setting A to 1.0\n";
        a= 1.0;
        retval= false;
      }
    return retval;
  }

//! @brief Constructor.
XC::CrossSectionProperties1d::CrossSectionProperties1d(void)
  :CommandEntity(), MovableObject(0), e(0), a(0), rho(0) {}

//! @brief Constructor.
XC::CrossSectionProperties1d::CrossSectionProperties1d(double EA_in)
  : CommandEntity(), MovableObject(0), e(1.0), a(EA_in), rho(0)
  { check_values(); }

//! @brief Constructor.
XC::CrossSectionProperties1d::CrossSectionProperties1d(const SectionForceDeformation &section)
  : CommandEntity(), MovableObject(0), e(1.0), a(0.0), rho(0)
  {
    const Matrix &sectTangent= section.getInitialTangent();
    const ResponseId &sectCode= section.getType();
    for(int i=0; i<sectCode.Size(); i++)
      {
	int code = sectCode(i);
	switch(code)
	  {
	  case SECTION_RESPONSE_P:
	    a = sectTangent(i,i);
	    break;
	  default:
	    break;
	  }
      }
  }

//! @brief Constructor (1D cross sections).
XC::CrossSectionProperties1d::CrossSectionProperties1d(double E_in, double A_in, double r)
  : CommandEntity(), MovableObject(0), e(E_in), a(A_in), rho(r)
  { check_values(); }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties1d::getSectionTangent1x1(void) const
  {
    ks(0,0) = EA(); //Axial stiffness.
    return ks;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties1d::getInitialTangent1x1(void) const
  { return getSectionTangent1x1(); }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties1d::getSectionFlexibility1x1(void) const
  {
    ks(0,0) = 1.0/(EA());
    return ks;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties1d::getInitialFlexibility1x1(void) const
  { return getSectionFlexibility1x1(); }

//! @brief Set the linear density of the element.
void XC::CrossSectionProperties1d::setLinearRho(const double &lr)
  {
    if(a!=0.0)
      rho= lr/a;
    else if(lr!=0.0)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING null section area. No changes made.\n";      
  }

int XC::CrossSectionProperties1d::setParameter(const std::vector<std::string> &argv,Parameter &param,SectionForceDeformation *scc)
  {
    if(argv.size() < 1)
      return -1;

    if(argv[0] == "E")
      {
        param.setValue(E());
        return param.addObject(1,scc);
      }
    if(argv[0] == "A")
      {
        param.setValue(A());
        return param.addObject(2,scc);
      }
    if(argv[0] == "rho")
      {
        param.setValue(getRho());
        return param.addObject(3,scc);
      }
    return -1;
  }

int XC::CrossSectionProperties1d::updateParameter(int paramID, Information &info)
  {
    if(paramID == 1)
      e= info.theDouble;
    if(paramID == 2)
      a= info.theDouble;
    return 0;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::CrossSectionProperties1d::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::CrossSectionProperties1d::sendData(Communicator &comm)
  {
    return comm.sendDoubles(e,a,rho,getDbTagData(),CommMetaData(0)); }

//! @brief Receives members through the communicator argument.
int XC::CrossSectionProperties1d::recvData(const Communicator &comm)
  {
    int retval= comm.receiveDoubles(e,a,rho,getDbTagData(),CommMetaData(0));
    return retval;
  }

//! @brief Sends object through the communicator argument.
int XC::CrossSectionProperties1d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(1);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::CrossSectionProperties1d::recvSelf(const Communicator &comm)
  {
    inicComm(1);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }


//! @brief Print stuff.
void XC::CrossSectionProperties1d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {}
    else
      {
        s << "CrossSectionProperties1d, E: " << E() << std::endl;
        s << "\t A: " << A();
      }
  }

//! @brief Insertion on an output stream.
std::ostream &XC::operator<<(std::ostream &os,const XC::CrossSectionProperties1d &c)
  {
    c.Print(os);
    return os;
  }
