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
//BaseElasticSection.cpp

#include <material/section/elastic_section/BaseElasticSection.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


//! @brief Constructor.
//!
//! @param tag: identifier for the object.
//! @param classTag: identifier for the object class.
//! @param dim: dimension (2 or 3).
//! @param mat_ldr: manager of the material objects.
XC::BaseElasticSection::BaseElasticSection(int tag, int classTag, const size_t &dim, MaterialLoader *mat_ldr)
  : PrismaticBarCrossSection(tag, classTag,mat_ldr), eTrial(dim), eInic(dim), eCommit(dim) {}

//! @brief Commits the section state.
int XC::BaseElasticSection::commitState(void)
  {
    eCommit = eTrial;
    return 0;
  }

//! @brief Revert to the last commited state. 
int XC::BaseElasticSection::revertToLastCommit(void)
  {
    eTrial= eCommit;
    return 0;
  }

//! @brief Revert to the initial state. 
int XC::BaseElasticSection::revertToStart(void)
  {
    eCommit.Zero();
    return 0;
  }

//! @brief Set the initial (generalized) deformation of the section.
int XC::BaseElasticSection::setInitialSectionDeformation(const Vector &def)
  {
    eInic= def;
    return 0;
  }

//! @brief Set the trial (generalized) deformation of the section.
//! Sets the value of the section deformation vector, \f$esec\f$ to be {\em
//! def}. The section deformation vector, \f$esec\f$, is defined by
//!
//! \f[
//! \begin{equation}
//! esec := \left[
//!    \begin{array}{c}
//!        \epsilon_a
//!        \kappa_z
//!        \gamma_y
//!    \end{array} 
//!  \right]
//! \end{equation}
//! \f]
//!
//! where \f$\epsilon_a\f$ is the axial strain, \f$\kappa_z\f$ is
//! the curvature about the local z-axis, and \f$\gamma_y\f$ is the
//! shear strain along the local y-axis.
int XC::BaseElasticSection::setTrialSectionDeformation(const Vector &def)
  {
    eTrial= def;
    return 0;
  }

//! @brief Returns the current value of the (generalized) deformation.
const XC::Vector &XC::BaseElasticSection::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= eTrial-eInic;
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::BaseElasticSection::sendData(CommParameters &cp)
  {
    int res= PrismaticBarCrossSection::sendData(cp);
    res+= cp.sendVector(eTrial,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(eInic,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(eCommit,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::BaseElasticSection::recvData(const CommParameters &cp)
  {
    int res= PrismaticBarCrossSection::recvData(cp);
    res+= cp.receiveVector(eTrial,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(eInic,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(eCommit,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Printing.
void XC::BaseElasticSection::Print(std::ostream &s, int flag) const
  {
    s << getClassName() << ", tag: "
      << this->getTag() << std::endl;
  }

