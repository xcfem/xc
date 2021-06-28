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
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.7 $
// $Date: 2003/12/12 18:41:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection3d.cpp,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/fiber_section/ElasticSection3d.cpp
//
// Written: MHS
// Date: May 2000
//
//
// Purpose:  This file contains the function definitions
// for the XC::ElasticSection3d class.

#include "material/section/elastic_section/ElasticSection3d.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


#include "material/section/ResponseId.h"

XC::Vector XC::ElasticSection3d::s(4);

//! @brief Constructor.
//!
//! Construct an elastic section for three-dimensional elements with an
//! integer identifier \p tag, and the mass properties \p ctes.
XC::ElasticSection3d::ElasticSection3d(int tag, MaterialHandler *mat_ldr,const CrossSectionProperties3d &ctes)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,4,ctes,mat_ldr) {}

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(void)
  : BaseElasticSection3d(0, SEC_TAG_Elastic3d,4) {}

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(int tag, double E_in, double A_in, double Iz_in, double Iy_in, double G_in, double J_in)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,4,CrossSectionProperties3d(E_in,A_in,Iz_in,Iy_in,G_in,J_in)) {}

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(int tag, double EA_in, double EIz_in, double EIy_in, double GJ_in)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,4,CrossSectionProperties3d(1,EA_in,EIz_in,EIy_in,1,GJ_in)) {}


/**
 @brief Returns the stress resultant.

 Returns the section stress resultants, \f$ssec\f$, the product of the 
 section stiffness matrix, \f$ksec\f$, and the section deformation 
 vector, \f$esec\f$,
 The component of the generalized stress vector are:
 \f[
  ssec = ksec esec = \left[
   \begin{array}{c}
       P\\
       M_z\\
       M_y\\
       V_y\\
       V_z\\
       T
   \end{array} 
 \right]
 \f]
 where \f$P\f$ is the axial force, \f$M_z\f$ is the bending moment about the
 local z-axis, \f$M_y\f$ is the bending moment about the local y-axis, 
 \f$V_y\f$ is the shear force along the local y-axis, \f$V_z\f$ is the
 shear force along the local z-axis, and \f$T\f$ is the torque.
*/
const XC::Vector &XC::ElasticSection3d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0) = ctes_scc.EA()*e(0); //Esfuerzo axil.
    s(1) = ctes_scc.EIz()*e(1); //Bending moment about z axis.
    s(2) = ctes_scc.EIy()*e(2); //Bending moment about y axis.
    s(3) = ctes_scc.GJ()*e(3); //Torque.
    return s;
  }

/**
 @brief Returns the tangent stiffness matrix.

 Returns the section stiffness matrix, \f$ksec\f$, where
 \f[
 ksec = \left[
    \begin{array}{cccccc}
       EA &  0 &  0 & 0 & 0 & 0\\
        0 & EI_z & 0 & 0 & 0 & 0\\
        0 & 0 & EI_y & 0 & 0 & 0\\
        0 & 0 & 0 & \alpha GA & 0 & 0\\
        0 & 0 & 0 & 0 & \alpha GA & 0\\
        0 & 0 & 0 & 0 & 0 & GJ
   \end{array} 
 \right]
 \f]
*/
const XC::Matrix &XC::ElasticSection3d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent4x4(); }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::ElasticSection3d::getInitialTangent(void) const
  { return ctes_scc.getInitialTangent4x4(); }

/**
  @brief Returns the flexibility matrix.
  Overrides the base class implementation and returns the section
  flexibility matrix, \f$fsec\f$, where
  \f[
  fsec = \left[
    \begin{array}{cccccc}
        \frac{1}{EA} & 0 & 0 & 0 & 0 & 0 \\
        0 & \frac{1}{EI_z} & 0 & 0 & 0 & 0 \\
        0 & 0 & \frac{1}{EI_y} & 0 & 0 & 0 \\
        0 & 0 & 0 & \frac{1}{\alpha GA} & 0 & 0 \\
        0 & 0 & 0 & 0 & \frac{1}{\alpha GA} & 0 \\
        0 & 0 & 0 & 0 & 0 & \frac{1}{GJ}
    \end{array} 
  \right]
  \f]
*/
const XC::Matrix &XC::ElasticSection3d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility4x4(); }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::ElasticSection3d::getInitialFlexibility(void) const
  { return ctes_scc.getInitialFlexibility4x4(); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::ElasticSection3d::getCopy(void) const
  { return new ElasticSection3d(*this); }

/**
 @brief Response identifiers for section stiffness contribution.

 Returns the section ID code that indicates the ordering of
 section response quantities. For this section, axial response is the
 first quantity, bending about the local z-axis is the second, bending about
 the local y-axis is the third, shear along the local y-axis is the fourth,
 shear along the local z-axis is the fifth, and torsion is the sixth.
 \f[
 code := \left[
   \begin{array}{c}
       2\\
       1\\
       4\\
       3\\
       5\\
       6
   \end{array} 
 \right]
 \f]
*/
const XC::ResponseId &XC::ElasticSection3d::getType(void) const
  { return RespElasticSection3d; }

//! @brief Return 4.
int XC::ElasticSection3d::getOrder(void) const
  { return 4; }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticSection3d::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Sends object through the communicator argument.
int XC::ElasticSection3d::sendSelf(Communicator &comm)
  {
    const DbTagData &dbTagData= getDbTagData();
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(dbTagData.Size());
    int res= sendData(comm);

    res+= comm.sendIdData(dbTagData,dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ElasticSection3d::recvSelf(const Communicator &comm)
  {
    DbTagData &dbTagData= getDbTagData();
    inicComm(dbTagData.Size());
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(dbTagData,dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::ElasticSection3d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {}
    else
      {
        s << "ElasticSection3d, tag: " << this->getTag() << std::endl;
        ctes_scc.Print(s);
      }
  }
