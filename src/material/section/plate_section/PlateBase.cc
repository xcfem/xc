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
//PlateBase.cc

#include <material/section/plate_section/PlateBase.h>

//! @brief Constructor.
XC::PlateBase::PlateBase(int tag,int classTag)
  : SectionForceDeformation(tag, classTag), h(0.0) {}

//! @brief null constructor
XC::PlateBase::PlateBase(int classTag)
  : SectionForceDeformation( 0, classTag), h(0.0) { }

//! @brief full constructor
XC::PlateBase::PlateBase(int tag, int classTag, double thickness, double rho)
  : SectionForceDeformation(tag,classTag), h(thickness), rho(rho) {}

//! @brief Return material density.
double XC::PlateBase::getRho(void) const
  { return rho; }

//! @brief Assign material density.
void XC::PlateBase::setRho(const double &r)
  { rho= r; }

//! @brief Return density per unit area
double XC::PlateBase::getArealRho(void) const
  { return rho*h; }

//! @brief Assign density per unit area
void XC::PlateBase::setArealRho(const double &r)
  {
    if(h==0.0)
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; thickness is zero. Can't compute areal density."
	        << " Command ignored."
	        << std::endl;
    else
      rho= r/h;
  }

//! @brief Return values of internal forces, deformations...
//! @param cod: name of the requested value.
//! @param silent: if true don't complain about non-existen property.
XC::Matrix XC::PlateBase::getValues(const std::string &cod, bool silent) const
  {
    Matrix retval;
    if(cod == "thickness")
      {
	retval.resize(1,1);
	retval(0,0)= getH();
      }
    else
      retval= SectionForceDeformation::getValues(cod, silent);
    return retval;
  }

//! @brief Returns strain at position being passed as parameter.
double XC::PlateBase::getStrain(const double &,const double &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Send data through the communicator argument.
int XC::PlateBase::sendData(Communicator &comm)
  {
    int res= SectionForceDeformation::sendData(comm);
    res+= comm.sendDoubles(h, rho,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::PlateBase::recvData(const Communicator &comm)
  {
    int res= SectionForceDeformation::recvData(comm);
    res+= comm.receiveDoubles(h, rho,getDbTagData(),CommMetaData(4));
    return res;
  }
