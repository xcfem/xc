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
// $Date: 2008/08/26 16:49:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticShearSection3d.cpp,v $

#include "material/section/elastic_section/ElasticShearSection3d.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "utility/matrix/Matrix.h"
#include <cstdlib>
#include "material/ResponseId.h"
#include "utility/utils/misc_utils/colormod.h"

XC::Vector XC::ElasticShearSection3d::s(6);
XC::Matrix XC::ElasticShearSection3d::ks(6,6);

//! @brief Default constructor.
XC::ElasticShearSection3d::ElasticShearSection3d(void)
  :BaseElasticSection3d(0, SEC_TAG_Elastic3d,6), parameterID(0) {}

//! @brief Constructor.
XC::ElasticShearSection3d::ElasticShearSection3d(int tag, MaterialHandler *mat_ldr)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,6,mat_ldr), parameterID(0) {}

//! @brief Return true if both objects are equal.
bool XC::ElasticShearSection3d::isEqual(const ElasticShearSection3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
	retval= BaseElasticSection3d::isEqual(other);
	if(retval)
	  retval= (parameterID==other.parameterID);
      }
    return retval;
  }

const XC::Vector &XC::ElasticShearSection3d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0) = ctes_scc.EA()*e(0);
    s(1) = ctes_scc.EIz()*e(1);
    s(3) = ctes_scc.EIy()*e(3);
    s(5) = ctes_scc.GJ()*e(5);

    const double GAY= ctes_scc.GAAlphaY(); //shear.
    s(2)= GAY*e(2);
    const double GAZ= ctes_scc.GAAlphaZ(); //shear.
    s(4)= GAZ*e(4);
    return s;
  }

const XC::Matrix &XC::ElasticShearSection3d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent6x6(); }

const XC::Matrix &XC::ElasticShearSection3d::getInitialTangent(void) const
  { return ctes_scc.getInitialTangent6x6(); }

const XC::Matrix &XC::ElasticShearSection3d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility6x6(); }

const XC::Matrix &XC::ElasticShearSection3d::getInitialFlexibility(void) const
  { return ctes_scc.getInitialFlexibility6x6(); }

//! @brief Returns the bending stiffness of the cross-section with respect to eht y axis.
const double &XC::ElasticShearSection3d::EIy(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(3, 3);
  }

//! @brief Returns the product of inertia multiplied by the Young modulus.
const double &XC::ElasticShearSection3d::EIyz(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(1,3);
  }

//! @brief Returns the shear stiffness along y axis.
const double &XC::ElasticShearSection3d::GAy(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(2,2);
  }

//! @brief Returns the shear stiffness along y axis.
const double &XC::ElasticShearSection3d::GAz(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(4,4);
  }

//! @brief Returns the torsional stiffness.
const double &XC::ElasticShearSection3d::GJ(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(5,5);
  }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::ElasticShearSection3d::getCopy(void) const
  { return new ElasticShearSection3d(*this); }

const XC::ResponseId &XC::ElasticShearSection3d::getResponseType(void) const
  { return RespElasticShSection3d; }

int XC::ElasticShearSection3d::getOrder(void) const
  { return 6; }

//! @brief Send object members through the communicator argument.
int XC::ElasticShearSection3d::sendData(Communicator &comm)
  {
    int res= BaseElasticSection3d::sendData(comm);
    setDbTagDataPos(9,parameterID);
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ElasticShearSection3d::recvData(const Communicator &comm)
  {
    int res= BaseElasticSection3d::recvData(comm);
    parameterID= getDbTagDataPos(9);
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::ElasticShearSection3d::getPyDict(void) const
  {
    boost::python::dict retval= BaseElasticSection3d::getPyDict();
    retval["parameterID"]= parameterID;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::ElasticShearSection3d::setPyDict(const boost::python::dict &d)
  {
    BaseElasticSection3d::setPyDict(d);
    this->parameterID= boost::python::extract<int>(d["parameterID"]);
  }

int XC::ElasticShearSection3d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

int XC::ElasticShearSection3d::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids."
	        << Color::def <<  std::endl;
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
		    << Color::def <<  std::endl;
      }
    return res;
  }
 
void XC::ElasticShearSection3d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
     {}
    else
     {
       s << "ElasticShearSection3d, tag: " << this->getTag() << std::endl;
       ctes_scc.Print(s);
     }
  }

int XC::ElasticShearSection3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv.size() < 1)
      return -1;
    else
      return ctes_scc.setParameter(argv,param,this);
  }

int XC::ElasticShearSection3d::updateParameter(int paramID, Information &info)
  { return ctes_scc.updateParameter(paramID,info); }

int XC::ElasticShearSection3d::activateParameter(int paramID)
  {
    parameterID = paramID;
    return 0;
  }

const XC::Vector &XC::ElasticShearSection3d::getStressResultantSensitivity(int gradIndex, bool conditional)
  {
    s.Zero();

//     if(parameterID == 1) { // E
//     s(0) = A*e(0);
//     s(1) = Iz*e(1);
//     s(3) = Iy*e(3);
//   }
//   if (parameterID == 2) { // A
//     s(0) = E*e(0);
//     double Galpha = G*alpha;
//     s(2) = Galpha*e(2);
//     s(4) = Galpha*e(4);
//   }
//   if (parameterID == 3) // Iz
//     s(1) = E*e(1);
//   if (parameterID == 4) // Iy
//     s(3) = E*e(3);
//   if (parameterID == 5) { // G
//     double Aalpha = A*alpha;
//     s(2) = Aalpha*e(2);
//     s(4) = Aalpha*e(4);
//     s(5) = J*e(5);
//   }
//   if (parameterID == 6) // J
//     s(5) = G*e(5);
//   if (parameterID == 7) { // alpha
//     double GA = G*A;
//     s(2) = GA*e(2);
//     s(4) = GA*e(4);
//   }

  return s;
}

const XC::Matrix &XC::ElasticShearSection3d::getInitialTangentSensitivity(int gradIndex)
  {
    ks.Zero();
    return ks;
  }
