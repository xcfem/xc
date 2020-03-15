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
//SectionFDPhysicalProperties.cpp

#include "SectionFDPhysicalProperties.h"
#include <material/section/SectionForceDeformation.h>
#include <domain/domain/Domain.h>


#include "domain/mesh/element/utils/gauss_models/GaussModel.h"


//! @brief Constructor
XC::SectionFDPhysicalProperties::SectionFDPhysicalProperties(const size_t &nMat,const SectionForceDeformation *ptr_mat)
  : PhysicalProperties<SectionForceDeformation>(nMat,ptr_mat)
  { }


//! @brief print out element data
void XC::SectionFDPhysicalProperties::Print(std::ostream &s, int flag ) const
  {
    if(flag == -1)
      {
        s << "SectionFDPhysicalProperties\t" << theMaterial.size() << "\t";
        s << std::endl;
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

//! Returns mean value of internal force.
double XC::SectionFDPhysicalProperties::getMeanInternalForce(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio per unit length, parallel to the axis 1.
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio per unit length, parallel to the axis 2.
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio per unit length, around the axis 1.
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio per unit length, around the axis 2.
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_q23);
    else
      std::cerr << "Unknown internal force: '"
                << cod << "'\n";
    return retval;
  }

//! Returns mean value of internal force.
double XC::SectionFDPhysicalProperties::getMeanInternalDeformation(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio per unit length, parallel to the axis 1.
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio per unit length, parallel to the axis 2.
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio per unit length, around the axis 1.
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio per unit length, around the axis 2.
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_q23);
    else
      std::cerr << "Unknown internal deformation: '"
                << cod << "'\n";
    return retval;
  }

//! @brief Returns the component of the average strain vector which has the code being passed as parameter.
//! @param cod component code.
double XC::SectionFDPhysicalProperties::getMeanGeneralizedStrainByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1")
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2")
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n2);
    else if(cod == "m1") //Bending around the axis 1.
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Bending around the axis 2.
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m2);
    else if(cod == "q13")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_q23);
    else if(cod == "m12")
      retval= theMaterial.getMeanGeneralizedStrain(PLATE_RESPONSE_m12);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n12);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief Returns the components of the strain vector which has the code being passed as parameter.
//! @param cod component code.
XC::Vector XC::SectionFDPhysicalProperties::getGeneralizedStrainAtGaussPointsByName(const std::string &cod) const
  {
    const size_t nMat= theMaterial.size();
    Vector retval(nMat);
    if(cod == "n1")
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2")
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(MEMBRANE_RESPONSE_n2);
    else if(cod == "m1") //Bending around the axis 1.
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Bending around the axis 2.
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(PLATE_RESPONSE_m2);
    else if(cod == "q13")
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(PLATE_RESPONSE_q23);
    else if(cod == "m12")
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(PLATE_RESPONSE_m12);
    else if(cod == "n12")
      retval= theMaterial.getGeneralizedStrainAtGaussPoints(MEMBRANE_RESPONSE_n12);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief Returns the component of the average generalized stress vector which corresponds to the code being passed as parameter.
//! @param cod component code (n1,n2,n12,m1,m2,m12,q13,q23)
double XC::SectionFDPhysicalProperties::getMeanGeneralizedStressByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio per unit length, parallel to the axis 1.
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio per unit length, parallel to the axis 2.
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStress(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio per unit length, around the axis 1.
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio per unit length, around the axis 2.
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getMeanGeneralizedStress(PLATE_RESPONSE_q23);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }
 
//! @brief Returns the components of the stress vector which has the code being passed as parameter.
//! @param cod component code.
XC::Vector XC::SectionFDPhysicalProperties::getGeneralizedStressAtGaussPointsByName(const std::string &cod) const
  {
    const size_t nMat= theMaterial.size();
    Vector retval(nMat);
    if(cod == "n1")
      retval= theMaterial.getGeneralizedStressAtGaussPoints(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2")
      retval= theMaterial.getGeneralizedStressAtGaussPoints(MEMBRANE_RESPONSE_n2);
    else if(cod == "m1") //Bending around the axis 1.
      retval= theMaterial.getGeneralizedStressAtGaussPoints(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Bending around the axis 2.
      retval= theMaterial.getGeneralizedStressAtGaussPoints(PLATE_RESPONSE_m2);
    else if(cod == "q13")
      retval= theMaterial.getGeneralizedStressAtGaussPoints(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= theMaterial.getGeneralizedStressAtGaussPoints(PLATE_RESPONSE_q23);
    else if(cod == "m12")
      retval= theMaterial.getGeneralizedStressAtGaussPoints(PLATE_RESPONSE_m12);
    else if(cod == "n12")
      retval= theMaterial.getGeneralizedStressAtGaussPoints(MEMBRANE_RESPONSE_n12);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief check to see if have mass
bool XC::SectionFDPhysicalProperties::haveRho(void) const
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
XC::Vector XC::SectionFDPhysicalProperties::getRhoi(void) const
  {
    const size_t numMaterials= theMaterial.size();
    Vector retval(numMaterials);
    for(size_t i=0; i<numMaterials; i++)
      retval[i]= theMaterial[i]->getRho();
    return retval;
  }

