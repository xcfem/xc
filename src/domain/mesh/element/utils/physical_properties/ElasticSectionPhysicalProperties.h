// -*-c++-*-
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
//ElasticSectionPhysicalProperties.h

#ifndef ElasticSectionPhysicalProperties_h
#define ElasticSectionPhysicalProperties_h

#include "PhysicalProperties.h"

namespace XC {

//! @ingroup PhysicalProperties
//
//! @brief Three-dimensional elastic section physical properties.
template<class ElasticSection>
class ElasticSectionPhysicalProperties: public PhysicalProperties<ElasticSection>
  {
  public:
    ElasticSectionPhysicalProperties(const size_t &nMat= 0);
    
    bool haveRho(void) const;
    Vector getRhoi(void) const;
    
    void Print(std::ostream &s, int) const;
  };

//! @brief Constructor
template<class ElasticSection>
XC::ElasticSectionPhysicalProperties<ElasticSection>::ElasticSectionPhysicalProperties(const size_t &nMat)
  : PhysicalProperties<ElasticSection>(nMat,nullptr)
  {
    const ElasticSection section;
    for(size_t i= 0;i<nMat;i++)
      this->theMaterial[i]= dynamic_cast<ElasticSection *>(section.getCopy());
  }

//! @brief check to see if have mass
template <class ElasticSection>
bool XC::ElasticSectionPhysicalProperties<ElasticSection>::haveRho(void) const
  {
    const size_t numMaterials= this->theMaterial.size();
    bool retval= false;
    for(size_t i=0; i<numMaterials; i++)
      {
        if(this->theMaterial[i]->getRho() != 0.0)
	  {
	    retval= true;
            break;
          }
      }
    return retval;
  }

//! @brief Returns densities for each position.
template <class ElasticSection>
XC::Vector XC::ElasticSectionPhysicalProperties<ElasticSection>::getRhoi(void) const
  {
    const size_t numMaterials= this->theMaterial.size();
    Vector retval(numMaterials);
    for(size_t i=0; i<numMaterials; i++)
      retval[i]= this->theMaterial[i]->getRho();
    return retval;
  }

//! @brief print out element data
template <class ElasticSection>
void XC::ElasticSectionPhysicalProperties<ElasticSection>::Print(std::ostream &s, int flag ) const
  {
    if(flag == -1)
      {
        s << this->getClassName() 
	  << "\t" << this->theMaterial.size() << "\t" << std::endl;
      }
    else if(flag < -1)
      {
        int counter= (flag + 1) * -1;
        for(size_t i= 0;i < this->theMaterial.size();i++)
          {
            const Vector &stress= this->theMaterial[i]->getStressResultant();
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
        this->theMaterial[0]->Print( s, flag );
        s << std::endl;
      }
  }
  
} // end of XC namespace
#endif
