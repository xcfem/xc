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
//BaseElasticSection1d.cpp

#include <material/section/elastic_section/BaseElasticSection1d.h>
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


//! @brief Constructor.
//! @param tag: object identifier.
//! @param classTag: class identifier.
XC::BaseElasticSection1d::BaseElasticSection1d(int tag, int classTag, MaterialHandler *mat_ldr)
  : BaseElasticSection(tag, classTag,1,mat_ldr), ctes_scc() {}

//! @brief Constructor.
//! @param classTag: class identifier.
XC::BaseElasticSection1d::BaseElasticSection1d(int classTag)
  : BaseElasticSection(0, classTag, 1), ctes_scc() {}

//! @brief Constructor.
//! @param tag: object identifier.
//! @param classTag: class identifier.
XC::BaseElasticSection1d::BaseElasticSection1d(int tag, int classTag, const double &E, const double &A)
  : BaseElasticSection(tag, classTag, 1), ctes_scc(E,A) {}

void XC::BaseElasticSection1d::sectionGeometry(const std::string &cod_geom)
  {
    const MaterialHandler *ldr= getMaterialHandler();
    if(ldr)
      {
        const SectionGeometry *geom= ldr->find_ptr_section_geometry(cod_geom);
        if(geom)
          {
            if(ctes_scc.E()==0.0)
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; reference elastic modulus must not be zero."
			<< std::endl;
            ctes_scc= geom->getCrossSectionProperties1d(ctes_scc);
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; section geometry called: '"
                    << cod_geom << "' not found." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; material handler not set." << std::endl;
  }

//! brief Returns strain at position being passed as parameter.
double XC::BaseElasticSection1d::getStrain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return def(0);
  }

//! @brief Send object members through the communicator argument.
int XC::BaseElasticSection1d::sendData(Communicator &comm)
  {
    int res= BaseElasticSection::sendData(comm);
    res+= comm.sendMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::BaseElasticSection1d::recvData(const Communicator &comm)
  {
    int res= BaseElasticSection::recvData(comm);
    res+= comm.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Returns the mechanical properties of the section.
const XC::CrossSectionProperties1d &XC::BaseElasticSection1d::getCrossSectionProperties(void) const
  { return ctes_scc; }

//! @brief Returns the mechanical properties of the section.
XC::CrossSectionProperties1d &XC::BaseElasticSection1d::getCrossSectionProperties(void)
  { return ctes_scc; }

void XC::BaseElasticSection1d::setCrossSectionProperties(const CrossSectionProperties1d &cs)  
  { ctes_scc= cs; }

//! @brief Print stuff.
void XC::BaseElasticSection1d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection1d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

