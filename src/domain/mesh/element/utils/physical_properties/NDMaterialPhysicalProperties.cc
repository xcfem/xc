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
//NDMaterialPhysicalProperties.cc

#include "NDMaterialPhysicalProperties.h"
#include "utility/matrix/Matrix.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"

//! @brief Constructor.
XC::NDMaterialPhysicalProperties::NDMaterialPhysicalProperties(const size_t &nMat,const NDMaterial *ptr_mat)
  :PhysicalProperties<NDMaterial>(nMat,ptr_mat) {}

//! @brief Constructor.
XC::NDMaterialPhysicalProperties::NDMaterialPhysicalProperties(const size_t &nMat, NDMaterial &theMat,const std::string &type ) 
  : PhysicalProperties<NDMaterial>(nMat,nullptr)
  {
    if(check_material_type(type))
      theMaterial.setMaterial(&theMat,type);
  }

//! @brief Check the material type, MUST be redefined in derived classes.
bool XC::NDMaterialPhysicalProperties::check_material_type(const std::string &type) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "Error; must be overloaded." << std::endl;
    return false;
  }

bool XC::NDMaterialPhysicalProperties::check_material_elast_plana(const std::string &type)
  {
    if((type!="PlaneStrain") && (type!="PlaneStress")
      && (type!="PlaneStrain2D") && (type!="PlaneStress2D"))
       {
         std::cerr << "NDMaterialPhysicalProperties::" << __FUNCTION__
		   << "; improper material type '" << type << "'.\n";
         return false;
       }
    else
      return true;
  }

//! @brief returns committed strains at each Gauss point.
XC::Matrix XC::NDMaterialPhysicalProperties::getCommittedStrain(void) const
  {
    const size_t numberOfRows= theMaterial.size();
    std::vector<Vector> strains(numberOfRows);
    for(size_t i= 0;i<numberOfRows;i++)
      strains[i]= getCommittedStrain(i);
    const size_t numberOfColumns= strains[0].Size();
    Matrix retval(numberOfRows,numberOfColumns);
    for(size_t i= 0;i<numberOfRows;i++)
      for(size_t j= 0;j<numberOfColumns;j++)
        retval(i,j)= strains[i](j);
    return retval;
  }   

//! @brief returns committed stresses at each Gauss point.
XC::Matrix XC::NDMaterialPhysicalProperties::getCommittedStress(void) const
  {
    const size_t numberOfRows= theMaterial.size();
    std::vector<Vector> stresses(numberOfRows);
    for(size_t i= 0;i<numberOfRows;i++)
      stresses[i]= getCommittedStress(i);
    const size_t numberOfColumns= stresses[0].Size();
    Matrix retval(numberOfRows,numberOfColumns);
    for(size_t i= 0;i<numberOfRows;i++)
      for(size_t j= 0;j<numberOfColumns;j++)
        retval(i,j)= stresses[i](j);
    return retval;
  }   


//! @brief Return the tensión media in the element.
XC::Vector XC::NDMaterialPhysicalProperties::getCommittedAvgStress(void) const
  {
    const size_t numMaterials= theMaterial.size();
    Vector retval= theMaterial[0]->getCommittedStress();
    for(size_t i=1; i<numMaterials; i++)
      retval+= theMaterial[i]->getCommittedStress();
    retval/= numMaterials;
    return retval;
  }

//! @brief Return the tensión media in the element.
XC::Vector XC::NDMaterialPhysicalProperties::getCommittedAvgStrain(void) const
  {
    const size_t numMaterials= theMaterial.size();
    Vector retval= theMaterial[0]->getCommittedStrain();
    for(size_t i=1; i<numMaterials; i++)
      retval+= theMaterial[i]->getCommittedStrain();
    retval/= numMaterials;
    return retval;
  }

//! @brief Return the componente iComp de la tensión media in the element.
double XC::NDMaterialPhysicalProperties::getCommittedAvgStress(const size_t &iComp) const
  {
    const size_t numMaterials= theMaterial.size();
    double retval(0.0);
    for(size_t i=0; i<numMaterials; i++)
      retval+= theMaterial[i]->getCommittedStress()[iComp];
    retval/= numMaterials;
    return retval;
  }

//! @brief Return the componente iComp de la average strain in the element.
double XC::NDMaterialPhysicalProperties::getCommittedAvgStrain(const size_t &iComp) const
  {
    const size_t numMaterials= theMaterial.size();
    double retval(0.0);
    for(size_t i=0; i<numMaterials; i++)
      retval+= theMaterial[i]->getCommittedStrain()[iComp];
    retval/= numMaterials;
    return retval;
  }

//! @brief Returns the component of the average strain vector which
//! has the code being passed as parameter.
//!
//! @param cod: component code.
double XC::NDMaterialPhysicalProperties::getMeanGeneralizedStrainByName(const std::string &cod) const
  {
    const size_t iComp= getComponentIndexFromCode(cod);
    return getCommittedAvgStrain(iComp);
  }

//! @brief Returns the component of the average strain vector which
//! has the code being passed as parameter.
//!
//! @param cod: component code.
double XC::NDMaterialPhysicalProperties::getMeanGeneralizedStressByName(const std::string &cod) const
  {
    const size_t iComp= getComponentIndexFromCode(cod);
    return getCommittedAvgStress(iComp);
  }

size_t  XC::NDMaterialPhysicalProperties::getComponentIndexFromCode(const std::string &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return 0;
  }

// check to see if have mass
bool XC::NDMaterialPhysicalProperties::haveRho(void) const
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

//! @brief Returns the average of the densities for each material.
double XC::NDMaterialPhysicalProperties::getRho(void) const
  {
    const size_t numMaterials= theMaterial.size();
    double retval= 0.0;
    for(size_t i=0; i<numMaterials; i++)
      retval+= theMaterial[i]->getRho();
    retval/= numMaterials;
    return retval;
  }

//! @brief Set the density for all materials.
void XC::NDMaterialPhysicalProperties::setRho(const double &r)
  {
    const size_t numMaterials= theMaterial.size();
    for(size_t i=0; i<numMaterials; i++)
      theMaterial[i]->setRho(r);
  }

//! @brief Returns densities for each position.
XC::Vector XC::NDMaterialPhysicalProperties::getRhoi(const double &rhoDefault) const
  {
    const size_t numMaterials= theMaterial.size();
    Vector retval(numMaterials);
    for(size_t i=0; i<numMaterials; i++)
      {
        retval[i]= theMaterial[i]->getRho();
        if(retval[i] == 0.0)
          { retval[i] = rhoDefault; }
      }
    return retval;
  }

//! @brief Obtain information from an analysis.
int XC::NDMaterialPhysicalProperties::getResponse(int responseID, Information &eleInfo)
  {
    int retval= -1;
    if(responseID == 3)
      {
        // Loop over the integration points
        int cnt = 0;
	const size_t numGaussPoints= this->size();
	if(numGaussPoints>0)
	  {
	    const size_t numComponents= (*this)[0]->getStress().Size();
	    static Vector tmp;
	    tmp.resize(numGaussPoints*numComponents);
	    for(size_t i= 0;i<numGaussPoints; i++)
	      {
		// Get material stress response
		const Vector &sigma = (*this)[i]->getStress();
		for(size_t j= 0;j<numComponents;j++,cnt++)
		  tmp(cnt)= sigma(j);
	      }
            retval= eleInfo.setVector(tmp);
	  }
      }
    else if(responseID == 4)
      {
        // Loop over the integration points
        int cnt = 0;
	const size_t numGaussPoints= this->size();
	if(numGaussPoints>0)
	  {
	    const size_t numComponents= (*this)[0]->getStrain().Size();
	    Vector tmp;
	    tmp.resize(numGaussPoints*numComponents);
	    for(size_t i= 0;i<numGaussPoints; i++)
	      {
		// Get material stress response
		const Vector &sigma = (*this)[i]->getStrain();
		for(size_t j= 0;j<numComponents;j++,cnt++)
		  tmp(cnt)= sigma(j);
	      }
            retval= eleInfo.setVector(tmp);
	  }
      }
    return retval;
  }

//! @brief Material response.
XC::Response *XC::NDMaterialPhysicalProperties::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    Response *retval= nullptr;
    if(argv[0] == "material" || argv[0] == "integrPoint")
      {
        size_t pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= this->size())
	  {
	    const size_t offset= 2;
            std::vector<std::string> argvOffset(argv);
            argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
            retval= this->setResponse(argvOffset,eleInfo);
	  }
        else
          retval= nullptr;
      }
    else // otherwise response quantity is unknown for the quad class
      retval= nullptr;
    return retval;
  }
