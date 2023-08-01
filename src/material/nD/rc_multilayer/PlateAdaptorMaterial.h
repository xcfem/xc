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
//===============================================================================

#ifndef PlateAdaptorMaterial_h
#define PlateAdaptorMaterial_h

#include "material/nD/NDMaterial.h"
#include "material/MaterialWrapper.h"

namespace XC {

//! @ingroup NDMat
//
//! @brief Plate response adaptor.
template <class MaterialWrapper>
class PlateAdaptorMaterial: public NDMaterial
  {
  public:
    typedef typename MaterialWrapper::MaterialType MaterialType;
  private:
    MaterialWrapper theMaterial;

  protected:
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public: 
    PlateAdaptorMaterial(int tag, int classTag);
    PlateAdaptorMaterial(int tag, int classTag, const MaterialType &);

    //! @brief Return a pointer to the encapsulated material.
    inline const MaterialType *getMaterial(void) const
      { return theMaterial.getMaterial(); }
    //! @brief Return a pointer to the encapsulated material.
    inline MaterialType *getMaterial(void)
      { return theMaterial.getMaterial(); }
    virtual void setMaterial(const MaterialType &);
    void setMaterial(const std::string &);
    
    //send back order of strain in vector form
    int getOrder(void) const;

    //send back order of strain in vector form
    const std::string &getType(void) const;

    //density
    double getRho(void) const;

    void Print(std::ostream &s, int flag = 0) const;
  };
//! @brief Sets the encapsulated material.
template <class MaterialWrapper>
void PlateAdaptorMaterial<MaterialWrapper>::setMaterial(const MaterialType &material)
  { theMaterial.setMaterial(material); }

//! @brief Sets the encapsulated material.
template <class MaterialWrapper>
void PlateAdaptorMaterial<MaterialWrapper>::setMaterial(const std::string &matName)
  {
    const Material *ptr_mat= getMaterialByName(matName);
    if(ptr_mat)
      {
	const MaterialType *tmp= dynamic_cast<const MaterialType *>(ptr_mat);
	if(tmp)
	  setMaterial(*tmp);
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		    << "material identified by: '" << matName
		    << "' is not an appropriate material." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		<< "material identified by: '" << matName
		<< "' not found." << std::endl;
  }

//null constructor
template <class MaterialWrapper>
PlateAdaptorMaterial<MaterialWrapper>::PlateAdaptorMaterial(int tag, int classTag)
  : NDMaterial(tag, classTag), theMaterial()
  { }


//full constructor
template <class MaterialWrapper>
PlateAdaptorMaterial<MaterialWrapper>::PlateAdaptorMaterial(int tag, int classTag, const MaterialType &mat)
  : NDMaterial(tag, classTag), theMaterial(mat)
  {}

//send back order of strain in vector form
template <class MaterialWrapper>
int PlateAdaptorMaterial<MaterialWrapper>::getOrder(void) const
  { return 5; }

template <class MaterialWrapper>
const std::string &PlateAdaptorMaterial<MaterialWrapper>::getType(void) const 
  {
    static const std::string retval("PlateFiber");
    return retval;
  }

//mass per unit volume
template <class MaterialWrapper>
double PlateAdaptorMaterial<MaterialWrapper>::getRho(void) const
  { return this->getMaterial()->getRho(); }


//! @brief print out data
template <class MaterialWrapper>
void PlateAdaptorMaterial<MaterialWrapper>::Print(std::ostream &s, int flag) const
  {
    s << getType() << " material tag: " << this->getTag() << std::endl;
    s << "using material: " << std::endl;
    this->getMaterial()->Print(s, flag);
  }

//! @brief Send material data.
template <class MaterialWrapper>
int PlateAdaptorMaterial<MaterialWrapper>::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendMovable(theMaterial,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receive material data.
template <class MaterialWrapper>
int PlateAdaptorMaterial<MaterialWrapper>::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveMovable(theMaterial,getDbTagData(),CommMetaData(1));
    return res;
  }
  
} // end of XC namespace

#endif






