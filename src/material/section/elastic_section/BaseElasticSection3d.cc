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
//BaseElasticSection3d.cpp

#include <material/section/elastic_section/BaseElasticSection3d.h>
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>



//! @brief Constructor.
//! @param tag: object identifier.
//! @param classTag: class identifier.
XC::BaseElasticSection3d::BaseElasticSection3d(int tag, int classTag, const size_t &dim, MaterialHandler *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc() {}

//! @brief Constructor.
//! 
//! Construct an elastic section for three-dimensional elements with an
//! integer identifier \p tag, and the mass properties \p ctes.
//! @param tag: object identifier.
//! @param classTag: class identifier.
XC::BaseElasticSection3d::BaseElasticSection3d(int tag,int classTag,const size_t &dim,const CrossSectionProperties3d &ctes,MaterialHandler *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc(ctes) {}

//! @brief Return true if both objects are equal.
bool XC::BaseElasticSection3d::isEqual(const BaseElasticSection3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
	retval= BaseElasticSection::isEqual(other);
	if(retval)
	  retval= (ctes_scc==other.ctes_scc);
      }
    return retval;
  }

//! @brief Set the mass properties of the section from the section geometry
//! identified by the argument.
void XC::BaseElasticSection3d::sectionGeometry(const std::string &cod_geom)
  {
    const MaterialHandler *ldr= getMaterialHandler();
    if(ldr)
      {
        const SectionGeometry *geom= ldr->find_ptr_section_geometry(cod_geom);
        if(geom)
          {
            if(ctes_scc.E()==0.0)
	      std::cerr << getClassName()<< "::" << __FUNCTION__
		        << "; reference elastic modulus must not be zero."
			<< std::endl;
            ctes_scc= geom->getCrossSectionProperties3d(ctes_scc);
          }
        else
          std::cerr << getClassName()<< "::" << __FUNCTION__
		    << "; section geometry called: '"
                    << cod_geom << "' not found." << std::endl;
      }
    else
      std::cerr << getClassName()<< "::" << __FUNCTION__
		<< "; material handler not defined." << std::endl;
  }

//! @brief Returns the mass properties of the section.
const XC::CrossSectionProperties3d &XC::BaseElasticSection3d::getCrossSectionProperties(void) const
  { return ctes_scc; }

//! @brief Returns the mass properties of the section.
XC::CrossSectionProperties3d &XC::BaseElasticSection3d::getCrossSectionProperties(void)
  { return ctes_scc; }

//! @brief Returns strain at position being passed as parameter.
double XC::BaseElasticSection3d::getStrain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1) + z*def(2));
  }

//! @brief Returns the bending stiffness of the cross-section around the z axis.
const double &XC::BaseElasticSection3d::EIz(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(1,1);
  }

//! @brief Returns the bending stiffness of the cross-section with respect to eht y axis.
const double &XC::BaseElasticSection3d::EIy(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(2, 2);
  }

//! @brief Returns the product of inertia multiplied by the Young modulus.
const double &XC::BaseElasticSection3d::EIyz(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(1,2);
  }

//! @brief Returns the torsional stiffness.
const double &XC::BaseElasticSection3d::GJ(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(3,3);
  }

//! @brief Setst the mass properties of the section.
void XC::BaseElasticSection3d::setCrossSectionProperties(const CrossSectionProperties3d &cs)  
  { ctes_scc= cs; }


//! @brief Send object members through the communicator argument.
int XC::BaseElasticSection3d::sendData(Communicator &comm)
  {
    int res= BaseElasticSection::sendData(comm);
    res+= comm.sendMovable(ctes_scc,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::BaseElasticSection3d::recvData(const Communicator &comm)
  {
    int res= BaseElasticSection::recvData(comm);
    res+= comm.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::BaseElasticSection3d::getPyDict(void) const
  {
    boost::python::dict retval= BaseElasticSection::getPyDict();
    retval["section_constants"]= this->ctes_scc.getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::BaseElasticSection3d::setPyDict(const boost::python::dict &d)
  {
    BaseElasticSection::setPyDict(d);
    const boost::python::dict &section_constants= boost::python::extract<boost::python::dict>(d["section_constants"]);
    this->ctes_scc.setPyDict(section_constants);
  }

void XC::BaseElasticSection3d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection3d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

