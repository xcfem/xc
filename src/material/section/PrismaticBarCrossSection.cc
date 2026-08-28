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
//PrismaticBarCrossSection.cpp

#include "PrismaticBarCrossSection.h"
#include "material/section/interaction_diagram/DeformationPlane.h"
#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <deque>
#include "material/section/elastic_section/ElasticSection1d.h"
#include "material/section/elastic_section/ElasticSection2d.h"
#include "material/section/elastic_section/ElasticShearSection2d.h"
#include "material/section/elastic_section/ElasticSection3d.h"
#include "material/section/elastic_section/ElasticShearSection3d.h"


#include "material/ResponseId.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "material/section/interaction_diagram/InteractionDiagram.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes2D.h"
#include "utility/utils/misc_utils/inertia.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::PrismaticBarCrossSection::PrismaticBarCrossSection(int tag, int classTag,MaterialHandler *mat_ldr)
  : SectionForceDeformation(tag,classTag,mat_ldr) {}

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
//! to the given deformation plane.
const XC::Vector &XC::PrismaticBarCrossSection::getGeneralizedStrainVector(const DeformationPlane &plane) const
  {
    const int order= getOrder();
    const ResponseId &code= getResponseType();
    return plane.getDeformation(order,code);
  }

//! @brief Returns the deformation plane of this section.
XC::DeformationPlane XC::PrismaticBarCrossSection::getDeformationPlane(void) const
  {
    const Vector &df= this->getSectionDeformation();
    // The deformation plane is constructed from the 
    // (epsilon, zCurvature, yCurvature) pairs which are not necessarily the
    // first three components of the section's deformation vector
    // (see ResponseID class).
    const int order= this->getOrder();
    const ResponseId &code= this->getResponseType();
    Vector tmp(3); tmp.Zero();
    for(int i= 0;i<order;i++)
      {
	const int &cdi= code(i);
	if(cdi == SECTION_RESPONSE_P)
	  tmp[0]+= df(i);
	else if(cdi == SECTION_RESPONSE_MZ)
	  tmp[1]+= df(i);
	else if(cdi == SECTION_RESPONSE_MY)
	  tmp[2]+= df(i);
      }
    return DeformationPlane(tmp);
  }

//! @brief Returns strain at position being passed as parameter.
double XC::PrismaticBarCrossSection::getStrain(const double &y,const double &z) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "not implemented."
              << Color::def << std::endl;
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
    const ResponseId &code= getResponseType();
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
  {
    const Matrix &tang= this->getSectionTangent();
    const size_t sz= tang.noRows();
    if(sz<2)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                  << "; this section has not inertia."
                  << Color::def << std::endl;
	exit(-1);
      }
    return tang(1,1);
  }
//! @brief Returns the bending stiffness of the cross-section with respect to eht y axis.
const double &XC::PrismaticBarCrossSection::EIy(void) const
  {
    static const double retval= -1.0; 
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    exit(-1);
    return retval;
  }

//! @brief Returns the product of inertia multiplied by the Young modulus.
const double &XC::PrismaticBarCrossSection::EIyz(void) const
  {
    static const double retval= -1.0; 
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    exit(-1);
    return retval;
  }

//! @brief Returns the shear stiffness along y axis.
const double &XC::PrismaticBarCrossSection::GAy(void) const
  {
    static const double retval= -1.0; 
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    exit(-1);
    return retval;
  }
  
//! @brief Returns the shear stiffness along z axis.
const double &XC::PrismaticBarCrossSection::GAz(void) const
  {
    static const double retval= -1.0; 
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    exit(-1);
    return retval;
  }

//! @brief Returns the torsional stiffness.
const double &XC::PrismaticBarCrossSection::GJ(void) const
  {
    static const double retval= -1.0; 
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    exit(-1);
    return retval;
  }

//! @brief Returns the angle that defines the principal axis of inertia.
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

double XC::PrismaticBarCrossSection::getLinearRho(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "not implemented yet."
              << Color::def << std::endl;
    return 0.0;
  }

//! @brief Return the 1D properties of the this cross-section.
XC::CrossSectionProperties1d XC::PrismaticBarCrossSection::getCrossSectionProperties1d(const double &e, const double &iw) const
  {
    return CrossSectionProperties1d(*this, e, iw);
  }

//! @brief Return the 2D properties of the this cross-section.
XC::CrossSectionProperties2d XC::PrismaticBarCrossSection::getCrossSectionProperties2d(const double &e, const double &iw, const double &g) const
  {
    return CrossSectionProperties2d(*this, e, iw, g);
  }

//! @brief Return the 3D properties of the this cross-section.
XC::CrossSectionProperties3d XC::PrismaticBarCrossSection::getCrossSectionProperties3d(const double &e, const double &iw, const double &g) const
  {
    return CrossSectionProperties3d(*this, e, iw, g);
  }

//! @brief Return a ElasticSection1d object using the geometry from this
//! object.
//! @param name: name of the new fiber section object.
//! @param E0: reference elastic modulus.
//! @param Iw: warping constant.
XC::ElasticSection1d *XC::PrismaticBarCrossSection::getElasticSection1d(const std::string &name, const double &E0, const double &Iw) const
  {
    ElasticSection1d *retval= nullptr;
    if(this->material_handler)
      {
	retval= dynamic_cast<ElasticSection1d *>(material_handler->newMaterial("ElasticSection1d", name));
	CrossSectionProperties1d &sp= retval->getCrossSectionProperties();
	sp.setE(E0);
	sp.setIw(Iw);
	const double area= this->EA()/E0;
	sp.setA(area);
	const double rho= this->getLinearRho();
	sp.setLinearRho(rho);
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no material handler available."
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Return a ElasticSection2d object using the geometry from this
//! object.
//! @param name: name of the new fiber section object.
//! @param E0: reference elastic modulus.
//! @param Iw: warping constant.
//! @param strongAxis: if true set the inertio of the strong axis for the new
//!                    section.
XC::ElasticSection2d *XC::PrismaticBarCrossSection::getElasticSection2d(const std::string &name, const double &E0, const double &Iw, bool strongAxis) const
  {
    ElasticSection2d *retval= nullptr;
    if(this->material_handler)
      {
	retval= dynamic_cast<ElasticSection2d *>(material_handler->newMaterial("ElasticSection2d", name));
	CrossSectionProperties2d &sp= retval->getCrossSectionProperties();
	sp.setE(E0);
	sp.setIw(Iw);
	const double area= this->EA()/E0;
	sp.setA(area);
	const double iy= this->EIy()/E0;
	const double iz= this->EIz()/E0;
	if(strongAxis)
	  sp.setI(std::max(iy, iz));
	else
	  sp.setI(std::min(iy, iz));
	const double rho= this->getLinearRho();
	sp.setLinearRho(rho);
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no material handler available."
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Return a ElasticShearSection2d object using the geometry from this
//! object.
//! @param name: name of the new fiber section object.
//! @param E0: reference elastic modulus.
//! @param Iw: warping constant.
//! @param G: shear modulus of the material.
//! @param alpha: shear shape factor.
//! @param strongAxis: if true set the inertio of the strong axis for the new
//!                    section.
XC::ElasticShearSection2d *XC::PrismaticBarCrossSection::getElasticShearSection2d(const std::string &name, const double &E0, const double &Iw, const double &G, bool strongAxis) const
  {
    ElasticShearSection2d *retval= nullptr;
    if(this->material_handler)
      {
	retval= dynamic_cast<ElasticShearSection2d *>(material_handler->newMaterial("ElasticShearSection2d", name));
	CrossSectionProperties2d &sp= retval->getCrossSectionProperties();
	sp.setE(E0);
	sp.setIw(Iw);
	sp.setG(G);
	const double area= this->EA()/E0;
	sp.setA(area);
	const double iy= this->EIy()/E0;
	const double iz= this->EIz()/E0;
	const double alpha_y= this->GAy()/G/area;
	const double alpha_z= this->GAz()/G/area;
	if(strongAxis)
	  {
	    if(iy>iz) // strong axis: y.
	      {
		sp.setI(iy);
		sp.setAlpha(alpha_z);
	      }
	    else // strong axis: z.
	      {
		sp.setI(iz);
		sp.setAlpha(alpha_y);
	      }	      
	  }
	else // weak axis.
	  {
	    if(iy<iz) // weak axis: y.
	      {
		sp.setI(iy);
		sp.setAlpha(alpha_z);
	      }
	    else  // weak axis: z.
	      {
		sp.setI(iz);
		sp.setAlpha(alpha_y);
	      }
	  }
	const double rho= this->getLinearRho();
	sp.setLinearRho(rho);
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no material handler available."
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Return a ElasticSection3d object using the geometry from this
//! object.
//! @param name: name of the new fiber section object.
//! @param E0: reference elastic modulus.
//! @param Iw: warping constant.
//! @param G: shear modulus of the material.
XC::ElasticSection3d *XC::PrismaticBarCrossSection::getElasticSection3d(const std::string &name, const double &E0, const double &Iw, const double &G) const
  {
    ElasticSection3d *retval= nullptr;
    if(this->material_handler)
      {
	retval= dynamic_cast<ElasticSection3d *>(material_handler->newMaterial("ElasticSection3d", name));
	CrossSectionProperties3d &sp= retval->getCrossSectionProperties();
	sp.setE(E0);
	sp.setIw(Iw);
	sp.setG(G);
	const double area= this->EA()/E0;
	sp.setA(area);
	const double iy= this->EIy()/E0;
	sp.setIy(iy);
	const double iz= this->EIz()/E0;
	sp.setIz(iz);
	const double pyz= this->EIyz()/E0;
	sp.setIyz(pyz);
	const double j= this->GJ()/G;
	sp.setJ(j);	
	const double rho= this->getLinearRho();
	sp.setLinearRho(rho);
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no material handler available."
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Return a ElasticShearSection3d object using the geometry from this
//! object.
//! @param name: name of the new fiber section object.
//! @param E0: reference elastic modulus.
//! @param Iw: warping constant.
//! @param G: shear modulus of the material.
XC::ElasticShearSection3d *XC::PrismaticBarCrossSection::getElasticShearSection3d(const std::string &name, const double &E0, const double &Iw, const double &G) const
  {
    ElasticShearSection3d *retval= nullptr;
    if(this->material_handler)
      {
	retval= dynamic_cast<ElasticShearSection3d *>(material_handler->newMaterial("ElasticShearSection3d", name));
	CrossSectionProperties3d &sp= retval->getCrossSectionProperties();
	sp.setE(E0);
	sp.setIw(Iw);
	sp.setG(G);
	const double area= this->EA()/E0;
	sp.setA(area);
	const double iy= this->EIy()/E0;
	sp.setIy(iy);
	const double iz= this->EIz()/E0;
	sp.setIz(iz);
	const double pyz= this->EIyz()/E0;
	sp.setIyz(pyz);
	const double j= this->GJ()/G;
	sp.setJ(j);	
	const double alpha_y= this->GAy()/G/area;
	sp.setAlphaY(alpha_y);
	const double alpha_z= this->GAz()/G/area;
	sp.setAlphaZ(alpha_z);
	const double rho= this->getLinearRho();
	sp.setLinearRho(rho);
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no material handler available."
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Returns the principal axes of inertia of the cross-section.
PrincipalAxes2D XC::PrismaticBarCrossSection::getInertiaAxes(void) const
  { return PrincipalAxes2D(getCenterOfMass(),EIy(),EIz(),EIyz());  }
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
    const ResponseId &code= getResponseType();
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
    const ResponseId &code= getResponseType();
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
    const ResponseId &code= getResponseType();
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
    const ResponseId &code= getResponseType();
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

