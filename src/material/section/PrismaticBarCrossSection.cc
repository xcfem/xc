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
//PrismaticBarCrossSection.cpp

#include "PrismaticBarCrossSection.h"
#include "material/section/interaction_diagram/DeformationPlane.h"
#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <deque>


#include "material/section/ResponseId.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "material/section/interaction_diagram/InteractionDiagram.h"
#include "xc_utils/src/geom/d1/Line3d.h"
#include "xc_utils/src/geom/d1/Line2d.h"
#include "xc_utils/src/geom/d2/HalfPlane2d.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "xc_utils/src/geom/sis_ref/PrincipalAxesOfInertia2D.h"
#include "xc_utils/src/utils/misc_utils/inertia.h"

//! @brief Constructor.
XC::PrismaticBarCrossSection::PrismaticBarCrossSection(int tag, int classTag,MaterialHandler *mat_ldr)
  : SectionForceDeformation(tag,classTag,mat_ldr) {}

//! @brief Copy constructor.
XC::PrismaticBarCrossSection::PrismaticBarCrossSection(const PrismaticBarCrossSection &other)
  : SectionForceDeformation(other) {}

//! @brief Assignment operator.
XC::PrismaticBarCrossSection &XC::PrismaticBarCrossSection::operator=(const PrismaticBarCrossSection &other)
  {
    SectionForceDeformation::operator=(other);
    return *this;
  }

//! @brief Sets the deformation plane of the section.
int XC::PrismaticBarCrossSection::setTrialDeformationPlane(const DeformationPlane &plane)
  { return setTrialSectionDeformation(getGeneralizedStrainVector(plane)); }

//! @brief Sets the plane that defines initial strains of the section.
int XC::PrismaticBarCrossSection::setInitialDeformationPlane(const DeformationPlane &plane)
  { return setInitialSectionDeformation(getGeneralizedStrainVector(plane)); }

//! @brief Returns initial strain plane.
XC::DeformationPlane XC::PrismaticBarCrossSection::getInitialDeformationPlane(void) const
  { return DeformationPlane(getInitialSectionDeformation()); }


//! @brief Return the generalized strains vector that corresponds
//! to the deformation plane being passed as parameter.
const XC::Vector &XC::PrismaticBarCrossSection::getGeneralizedStrainVector(const DeformationPlane &plane) const
  {
    const int order= getOrder();
    const ResponseId &code= getType();
    return plane.getDeformation(order,code);
  }

//! @brief Returns the generalized strains vector of the cross-section.
XC::DeformationPlane XC::PrismaticBarCrossSection::getDeformationPlane(void) const
  {
    const Vector &df= getSectionDeformation();
    return  DeformationPlane(df);
  }

//! @brief Returns strain at position being passed as parameter.
double XC::PrismaticBarCrossSection::getStrain(const double &y,const double &z) const
  {
    std::cerr << "getStrain not implemented for class: "
              << getClassName() << std::endl;
    return 0.0;
  }

//! @brief Returns the y coordinate of the center of gravity of the cross-section.
double XC::PrismaticBarCrossSection::getCenterOfMassY(void) const
  { return 0.0; }

//! @brief Returns the z coordinate of the center of gravity of the cross-section.
double XC::PrismaticBarCrossSection::getCenterOfMassZ(void) const
  { return 0.0; }

//! @brief Returns the position of the cross-section centroid.
Pos2d XC::PrismaticBarCrossSection::getCenterOfMass(void) const
  { return Pos2d(getCenterOfMassY(),getCenterOfMassZ()); }

//! @brief Returns true if the section is subjected to an axial force.
bool XC::PrismaticBarCrossSection::hayAxil(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_P))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_P))>=tol;
    return retval;
  }

//! @brief Returns internal axial force.
double XC::PrismaticBarCrossSection::getN(void) const
  { return getStressResultant(SECTION_RESPONSE_P); }

//! @brief Returns internal bending moment around y axis.
double XC::PrismaticBarCrossSection::getMy(void) const
  { return getStressResultant(SECTION_RESPONSE_MY); }

//! @brief Returns internal bending moment around z axis.
double XC::PrismaticBarCrossSection::getMz(void) const
  { return getStressResultant(SECTION_RESPONSE_MZ); }

//! @brief Returns the section axial stiffness.
const double &XC::PrismaticBarCrossSection::EA(void) const
  { return getSectionTangent()(0,0); }
//! @brief Returns the bending stiffness of the cross-section en torno the z axis.
const double &XC::PrismaticBarCrossSection::EIz(void) const
  { return getSectionTangent()(1,1); }
//! @brief Returns the bending stiffness of the cross-section en torno the y axis.
const double &XC::PrismaticBarCrossSection::EIy(void) const
  { return getSectionTangent()(2,2); }
//! @brief Returns the product of inertia multiplicado por el
//! Young's modulus.
const double &XC::PrismaticBarCrossSection::EIyz(void) const
  { return getSectionTangent()(1,2); }

//! @brief Returns the angle that defines un principal axis of inertia.
double XC::PrismaticBarCrossSection::getTheta(void) const
  { return theta_inertia(EIy(),EIz(),EIyz()); }

//! @brief Returns the bending stiffness around
//! the major principal axis of inertia.
double XC::PrismaticBarCrossSection::getEI1(void) const
  { return I1_inertia(EIy(),EIz(),EIyz()); }

//! @brief Returns the bending stiffness en
//! torno al principal axis of inertia menor.
double XC::PrismaticBarCrossSection::getEI2(void) const
  { return I2_inertia(EIy(),EIz(),EIyz()); }

//! @brief Returns the principal axes of inertia of the cross-section.
PrincipalAxesOfInertia2D XC::PrismaticBarCrossSection::getInertiaAxes(void) const
  { return PrincipalAxesOfInertia2D(getCenterOfMass(),EIy(),EIz(),EIyz());  }
//! @brief Returns the vector of the principal axis I.
Vector2d XC::PrismaticBarCrossSection::getAxis1VDir(void) const
  { return getInertiaAxes().getAxis1VDir(); }
//! @brief Returns the vector of the principal axis I.
Vector2d XC::PrismaticBarCrossSection::getVDirStrongAxis(void) const
  { return getAxis1VDir(); }
//! @brief Returns the vector of the principal axis II.
Vector2d XC::PrismaticBarCrossSection::getAxis2VDir(void) const
  { return getInertiaAxes().getAxis2VDir(); }
//! @brief Returns the vector of the principal axis II.
Vector2d XC::PrismaticBarCrossSection::getVDirWeakAxis(void) const
  { return getAxis2VDir(); }

//! @brief Returns true if the section is subjected to a bending moment.
bool XC::PrismaticBarCrossSection::isSubjectedToBending(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_MY))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_MY))>=tol;
    else if(code.hasResponse(SECTION_RESPONSE_MZ))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_MZ))>=tol;
    return retval;
  }

//! @brief Returns true if the section is subjected to a shearing force.
bool XC::PrismaticBarCrossSection::isSubjectedToShear(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_VY))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_VY))>=tol;
    else if(code.hasResponse(SECTION_RESPONSE_VZ))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_VZ))>=tol;
    return retval;
  }

//! @brief Returns true if the section is subjected to a torsional force.
bool XC::PrismaticBarCrossSection::hayTorsor(const double &tol) const
  {
    bool retval= false;
    const ResponseId &code= getType();
    if(code.hasResponse(SECTION_RESPONSE_T))
      retval= std::abs(getStressResultant(SECTION_RESPONSE_T))>=tol;
    return retval;
  }

//! @brief Returns the neutral axis.
Line2d XC::PrismaticBarCrossSection::getNeutralAxis(void) const
  { return getDeformationPlane().getNeutralAxis(); }

//! @brief Returns the axis that is aligned with the
//! cross-section internal forces.
Line2d XC::PrismaticBarCrossSection::getInternalForcesAxis(void) const
  {
    Line2d retval(getCenterOfMass(),Vector2d(1,0));
    const ResponseId &code= getType();
    if(isSubjectedToBending()) //Direction of the bending moment.
      {
        if(code.hasResponse(SECTION_RESPONSE_MY) && code.hasResponse(SECTION_RESPONSE_MZ))
          retval= Line2d(getCenterOfMass(),Vector2d(getStressResultant(SECTION_RESPONSE_MY),getStressResultant(SECTION_RESPONSE_MZ)));
        else if(code.hasResponse(SECTION_RESPONSE_MY))
          retval= Line2d(getCenterOfMass(),Vector2d(1,0));
        else if(code.hasResponse(SECTION_RESPONSE_MZ))
          retval= Line2d(getCenterOfMass(),Vector2d(0,1));
      }
    else if(isSubjectedToShear()) //Direction normal to the shear force.
      {
        if(code.hasResponse(SECTION_RESPONSE_VY) && code.hasResponse(SECTION_RESPONSE_VZ))
          retval= Line2d(getCenterOfMass(),Vector2d(-getStressResultant(SECTION_RESPONSE_VZ),getStressResultant(SECTION_RESPONSE_VY)));
        else if(code.hasResponse(SECTION_RESPONSE_VY))
          retval= Line2d(getCenterOfMass(),Vector2d(0,1));
        else if(code.hasResponse(SECTION_RESPONSE_VZ))
          retval= Line2d(getCenterOfMass(),Vector2d(1,0));
      }
    return retval;
  }

//! @brief Returns (if possible) a point in the tensioned region.
Pos2d XC::PrismaticBarCrossSection::getPointOnTensionedHalfPlane(void) const
  { return getDeformationPlane().getPointOnTensionedHalfPlane(); }

//! @brief Returns (if possible) a point in the compressed region.
Pos2d XC::PrismaticBarCrossSection::getPointOnCompressedHalfPlane(void) const
  { return getDeformationPlane().getPointOnCompressedHalfPlane(); }

//! @brief Returns the tensioned half-plane defined by the edge
//! being passed as parameter.
HalfPlane2d XC::PrismaticBarCrossSection::getTensionedHalfPlane(const Line2d &r) const
  { return getDeformationPlane().getTensionedHalfPlane(r); }

//! @brief Returns the tensioned half-plane.
HalfPlane2d XC::PrismaticBarCrossSection::getTensionedHalfPlane(void) const
  { return getDeformationPlane().getTensionedHalfPlane(); }

//! @brief Returns the compressed half-plane defined by the edge
//! being passed as parameter.
HalfPlane2d XC::PrismaticBarCrossSection::getCompressedHalfPlane(const Line2d &r) const
  { return getDeformationPlane().getCompressedHalfPlane(r); }

//! @brief Returns the compressed half-plane.
HalfPlane2d XC::PrismaticBarCrossSection::getCompressedHalfPlane(void) const
  { return getDeformationPlane().getCompressedHalfPlane(); }

