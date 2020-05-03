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
                                                                        
                                                                        
#include <material/section/elastic_section/ElasticSection1d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "material/section/ResponseId.h"

XC::Vector XC::ElasticSection1d::s(1);

//! @brief Constructor.
//!
//! @param tag: identifier for the object.
//! @param mat_ldr: manager of the material objects.
XC::ElasticSection1d::ElasticSection1d(int tag, MaterialHandler *mat_ldr)
  : BaseElasticSection1d(tag,SEC_TAG_Elastic1d, mat_ldr) {}

//! To construct an ElasticSection1D with an integer identifier {\em
//! tag}, an elastic modulus of \f$E\f$, a second moment of area \f$I\f$,
//! a section area of \f$A\f$.
//!
//! @param tag: identifier for the object.
//! @param E: elastic modulus
//! @param A: area.
XC::ElasticSection1d::ElasticSection1d(int tag, double E, double A)
  : BaseElasticSection1d(tag,SEC_TAG_Elastic1d,E,A)
  {}

XC::ElasticSection1d::ElasticSection1d(int tag, double EA)
  : BaseElasticSection1d(tag, SEC_TAG_Elastic1d,EA,0.0)
  {}

double XC::ElasticSection1d::getStress(void) const
  {
    const Vector &e= getSectionDeformation();
    return ctes_scc.E()*e(0);
  }

//! @brief Returns the cross-section stress resultant.
//!
//! Returns the section stress resultants, \f$ssec\f$, the product of the 
//! section stiffness matrix, \f$ksec\f$, and the section deformation 
//! vector, \f$esec\f$,
//! \f[
//! \begin{equation}
//! ssec = ksec esec = \left[
//!    \begin{array}{c}
//!        P
//!    \end{array} 
//!  \right]
//! \end{equation}
//! \f]
//! where \f$P\f$ is the axial force, \f$M_z\f$ is the bending moment about the
//! local z-axis, and \f$V_y\f$ is the shear force along the local y-axis.
const XC::Vector &XC::ElasticSection1d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0)= ctes_scc.EA()*e(0);
    return s;
  }

//! @brief Returns the tangent stiffness matrix.
//!
//! \f[
//! \begin{equation}
//! fsec = \left[
//!    \begin{array}{c}
//!        EA 
//!    \end{array} 
//!  \right]
//! \end{equation}
//! \f]
const XC::Matrix &XC::ElasticSection1d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent1x1(); }


//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::ElasticSection1d::getInitialTangent(void) const
  { return getSectionTangent(); }

//! @brief Returns the flexibility matrix.
//!
//! \f[
//! \begin{equation}
//! fsec = \left[
//!    \begin{array}{c}
//!        \frac{1}{EA}
//!    \end{array} 
//!  \right]
//! \end{equation}
//! \f]
const XC::Matrix &XC::ElasticSection1d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility1x1(); }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::ElasticSection1d::getInitialFlexibility(void) const
  { return getSectionFlexibility(); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::ElasticSection1d::getCopy(void) const
  { return new ElasticSection1d(*this); }

//! @brief Section stiffness contribution response identifiers.
//!
//! Returns the section ID code that indicates the ordering of
//! section response quantities. For this section, axial response is the
//! first quantity, bending about the local z-axis is the second, and
//! shear along the local y-axis is third.
//! \f[
//! code := \left[
//!   \begin{array}{c}
//!       2
//!   \end{array} 
//! \right]
//! \f]
const XC::ResponseId &XC::ElasticSection1d::getType(void) const
  { return RespElasticSection1d; }

//! @brief Return 3.
int XC::ElasticSection1d::getOrder(void) const
  { return 1; }

//! @brief Sends object through the communicator argument.
int XC::ElasticSection1d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ElasticSection1d::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

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
void XC::ElasticSection1d::Print(std::ostream &s, int flag) const
  {
    s << "ElasticSection1d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

