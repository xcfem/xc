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
//ElasticSection3dPhysicalProperties.cpp

#include "ElasticSection3dPhysicalProperties.h"
#include <domain/domain/Domain.h>
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"


#include "domain/mesh/element/utils/gauss_models/GaussModel.h"


//! @brief Constructor
XC::ElasticSection3dPhysicalProperties::ElasticSection3dPhysicalProperties(const size_t &nMat)
  : PhysicalProperties<ElasticSection3d>(nMat,nullptr)
  {
    const ElasticSection3d section;
    for(size_t i= 0;i<nMat;i++)
      theMaterial[i]= dynamic_cast<ElasticSection3d *>(section.getCopy());
  }

//! @brief Assigns section properties for integration point i
void XC::ElasticSection3dPhysicalProperties::set(const size_t &i, const CrossSectionProperties3d &sp)
   { theMaterial[i]->setCrossSectionProperties(sp); }


//! @brief print out element data
void XC::ElasticSection3dPhysicalProperties::Print(std::ostream &s, int flag ) const
  {
    if(flag == -1)
      {
        s << getClassName() 
	  << "\t" << theMaterial.size() << "\t" << std::endl;
      }
    else if(flag < -1)
      {
        int counter= (flag + 1) * -1;
        for(size_t i= 0;i < theMaterial.size();i++)
          {
            const Vector &stress= theMaterial[i]->getStressResultant();
            s << "STRESS\t" << counter << "\t" << i << "\tTOP";
            for(int j=0; j<6; j++)
              s << "\t" << stress(j);
            s << std::endl;
          }
      }
    else
      {
        s << std::endl;
        s << "Material Information : \n ";
        theMaterial[0]->Print( s, flag );
        s << std::endl;
      }
  }

//! @brief Returns the components of the strain vector which has the code being passed as parameter.
//! @param cod component code.
XC::Vector XC::ElasticSection3dPhysicalProperties::getGeneralizedStressAtGaussPointsByName(const std::string &cod) const
  {
    const size_t nMat= theMaterial.size();
    Vector retval(nMat);
    for(size_t i= 0; i<nMat; i++)
      {
	const ElasticSection3d *section= dynamic_cast<const ElasticSection3d *>(theMaterial[i]);
	const Vector R= section->getStressResultant();
	if(cod == "N") // Internal axial force.
	  retval[i]= R[0];
	else if(cod == "Mz") // Bending around the axis 1.
	  retval[i]= R[1];
	else if(cod == "My") // Bending around the axis 2.
	  retval[i]= R[2];
	else if(cod == "T")
	  retval[i]= R[3]; // Torsion.
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "stress code: '" << cod << " unknown." << std::endl;
      }
    return retval;
  }

//! @brief Returns the components of the stress vector which has the code being passed as parameter.
//!
//! @param cod component code.
XC::Vector XC::ElasticSection3dPhysicalProperties::getGeneralizedStrainAtGaussPointsByName(const std::string &cod) const
  {
    const size_t nMat= theMaterial.size();
    Vector retval(nMat);
    for(size_t i= 0; i<nMat; i++)
      {
	const ElasticSection3d *section= dynamic_cast<const ElasticSection3d *>(theMaterial[i]);
	const Vector e= section->getSectionDeformation();
	if(cod == "N") // axial strain.
	  retval[i]= e[0];
	else if(cod == "Mz") // Bending around the axis 1.
	  retval[i]= e[1];
	else if(cod == "My") // Bending around the axis 2.
	  retval[i]= e[2];
	else if(cod == "T")
	  retval[i]= e[3]; // Torsion.
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "stress code: '" << cod << " unknown." << std::endl;
      }
    return retval;
  }

//! @brief check to see if have mass
bool XC::ElasticSection3dPhysicalProperties::haveRho(void) const
  {
    const size_t numMaterials= theMaterial.size();
    bool retval= false;
    for(size_t i=0; i<numMaterials; i++)
      {
        if(theMaterial[i]->getRho() != 0.0)
	  {
	    retval= true;
            break;
          }
      }
    return retval;
  }

//! @brief Returns densities for each position.
XC::Vector XC::ElasticSection3dPhysicalProperties::getRhoi(void) const
  {
    const size_t numMaterials= theMaterial.size();
    Vector retval(numMaterials);
    for(size_t i=0; i<numMaterials; i++)
      retval[i]= theMaterial[i]->getRho();
    return retval;
  }

