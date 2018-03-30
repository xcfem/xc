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
//BaseElasticSection3d.cpp

#include <material/section/elastic_section/BaseElasticSection3d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>



XC::BaseElasticSection3d::BaseElasticSection3d(int tag, int classTag, const size_t &dim, MaterialHandler *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc() {}

//! Construct an elastic section for three-dimensional elements with an
//! integer identifier \p tag, and the mass properties \p ctes.
XC::BaseElasticSection3d::BaseElasticSection3d(int tag,int classTag,const size_t &dim,const CrossSectionProperties3d &ctes,MaterialHandler *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc(ctes) {}

//! @brief Set the mass properties of the section from the section geometry
//! identified by the argument.
void XC::BaseElasticSection3d::sectionGeometry(const std::string &cod_geom)
  {
    const MaterialHandler *ldr= getMaterialHandler();
    if(ldr)
      {
        const GeomSection *geom= ldr->find_ptr_geom_section(cod_geom);
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

//! \brief Returns the mass properties of the section.
const XC::CrossSectionProperties3d &XC::BaseElasticSection3d::getCrossSectionProperties(void) const
  { return ctes_scc; }

//! @brief Returns strain at position being passed as parameter.
double XC::BaseElasticSection3d::getStrain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1) + z*def(2));
  }

//! @brief Setst the mass properties of the section.
void XC::BaseElasticSection3d::setCrossSectionProperties(const CrossSectionProperties3d &cs)  
  { ctes_scc= cs; }


//! @brief Send object members through the channel being passed as parameter.
int XC::BaseElasticSection3d::sendData(CommParameters &cp)
  {
    int res= BaseElasticSection::sendData(cp);
    res+= cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::BaseElasticSection3d::recvData(const CommParameters &cp)
  {
    int res= BaseElasticSection::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

void XC::BaseElasticSection3d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection3d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

