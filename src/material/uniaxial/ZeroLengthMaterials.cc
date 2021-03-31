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
//ZeroLengthMaterials.cpp

#include "ZeroLengthMaterials.h"
#include "domain/mesh/element/zeroLength/ZeroLength.h"
#include "material/uniaxial/UniaxialMaterial.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "domain/mesh/node/Node.h"


//!  Constructor:
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial &theMat, int direction)
  :DqUniaxialMaterial(owner,theMat,1), directions()
  {
    directions.push_back(direction);
    checkDirection();
  }

//!  Constructor:
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial *theMat, int direction)
  :DqUniaxialMaterial(owner,theMat,1), directions()
  {
    directions.push_back(direction);
    checkDirection();
  }

//! @brief  Construct container with multiple unidirectional materials
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner,const DqUniaxialMaterial &theMat,const ID &direction)
  :DqUniaxialMaterial(theMat), directions(theMat.size())
  {
    set_owner(owner);
    // initialize uniaxial materials and directions and check for valid values
    for(size_t i= 0;i<theMat.size();i++)
      directions[i]= direction(i);
    checkDirection();
  }

//   Constructor:
//   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//   to be invoked upon
XC::ZeroLengthMaterials::ZeroLengthMaterials(ZeroLength *owner)
  :DqUniaxialMaterial(owner), directions() {}

//! @brief Free memory.
void XC::ZeroLengthMaterials::clear(void)
  {
    DqUniaxialMaterial::clear();
    directions.clear();
  }

//! @brief Return the material handler.
XC::MaterialHandler *XC::ZeroLengthMaterials::get_material_handler(void)
  {
    MaterialHandler *retval= nullptr;
    CommandEntity *owr= Owner();
    if(owr)
      {
        ZeroLength *elem= dynamic_cast<ZeroLength *>(owr);
        if(elem)
          {
            Preprocessor *preprocessor= elem->getPreprocessor();
            if(preprocessor)
              retval= &preprocessor->getMaterialHandler();
            else
              std::cerr << getClassName() << __FUNCTION__
		        << "; null pointer to preprocessor." << std::endl;
          }
        else
          std::cerr << getClassName() << __FUNCTION__
		    << "; owner is not an element." << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; pointer to owner not found." << std::endl;
    return retval;
  }

void XC::ZeroLengthMaterials::push_back(const int &dir,const UniaxialMaterial *t)
  {
    if(!t)
      std::cerr << getClassName() << __FUNCTION__
		<< "; material pointer is null." << std::endl;
    else
      {
        UniaxialMaterial *tmp= nullptr;
        tmp= t->getCopy();
        if(tmp)
          {
            DqUniaxialMaterial::push_back(tmp);
            directions.push_back(dir);
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; can't create an UniaxialMaterial object." << std::endl;
      }
  }

void XC::ZeroLengthMaterials::push_front(const int &dir,const UniaxialMaterial *t)
  {
    if(!t)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; pointer to material is null." << std::endl;
    else
      {
        UniaxialMaterial *tmp= nullptr;
        tmp= t->getCopy();
        if(tmp)
          {
            DqUniaxialMaterial::push_front(tmp);
            directions.push_front(dir);
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; can't create an UniaxialMaterial object."
		    << std::endl;
      }
  }

//! @brief Return the size of the generalized stress vector.
size_t XC::ZeroLengthMaterials::getGeneralizedStressSize(void) const
  { return (*this)[0]->getGeneralizedStress().Size(); }

//! @brief Return the size of the generalized strain vector.
size_t XC::ZeroLengthMaterials::getGeneralizedStrainSize(void) const
  { return (*this)[0]->getGeneralizedStrain().Size(); }

//! @brief Returns generalized stress values on each direction.
XC::Matrix XC::ZeroLengthMaterials::getGeneralizedStresses(void) const
  {
    const size_t ncol= getGeneralizedStressSize();
    const size_t nMat= this->size();
    Matrix retval(nMat,ncol);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        retval.putRow(i,s);
      }
    return retval;
  }

//! @brief Returns generalized strain values on each direction.
XC::Matrix XC::ZeroLengthMaterials::getGeneralizedStrains(void) const
  {
    const size_t ncol= getGeneralizedStrainSize();
    const size_t nMat= this->size();
    Matrix retval(nMat,ncol);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(size_t j= 0;j<ncol;j++)
          retval(i,j)= s(j);
      }
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ZeroLengthMaterials::getDbTagData(void) const
  {
    static DbTagData retval(3);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::ZeroLengthMaterials::sendData(Communicator &comm)
  {
    int res= DqUniaxialMaterial::sendData(comm);
    // Send directions.
    const size_t sz= directions.size();
    ID dir(sz);
    for(size_t i= 0;i<sz;i++)
      dir[i]= directions[i];
    // Compute next position.
    res+= comm.sendID(dir,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ZeroLengthMaterials::recvData(const Communicator &comm)
  {
    int res= DqUniaxialMaterial::recvData(comm);
    // Receive directions.
    ID dir;
    res+= comm.receiveID(dir,getDbTagData(),CommMetaData(2));
    const size_t sz= dir.Size();
    for(size_t i= 0;i<sz;i++)
      directions.push_back(dir[i]);
    return res;
  }

//! @brief Send the object through the communicator.
//! 
//! Sends the object through the communicator. Returns 0 if successful, a
//! warning message is printed, \p tag and \f$E\f$ are set to \f$0.0\f$, and a
//! negative number is returned if the Channel object fails to receive
//! the object.
int XC::ZeroLengthMaterials::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receive the object through the communicator.
//! 
//! Receives the object through the communicator. Returns 0 if successful, a
//! warning message is printed, \p tag and \f$E\f$ are set to \f$0.0\f$, and a
//! negative number is returned if the Channel object fails to receive
//! the object.
int XC::ZeroLengthMaterials::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        res= recvData(comm);
        if(res < 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; - failed to receive data\n";
      }
    return res;
  }

//! @brief Pring stuff
void XC::ZeroLengthMaterials::Print(std::ostream &s, int flag) const
  {
    for(size_t j = 0; j < size(); j++)
      {
        s << "\tMaterial1d, tag: " << (*this)[j]->getTag()
          << ", dir: " << directions[j] << std::endl;
        s << *((*this)[j]);
      }
  }

// Private methods


// Check that direction is in the range of 0 to 5
void XC::ZeroLengthMaterials::checkDirection(void)
  {
    for(size_t i=0; i<directions.size(); i++)
      if(directions[i] < 0 || directions[i] > 5 )
        {
          std::cerr << getClassName() << __FUNCTION__
		    << "; WARNING - incorrect direction "
		    << directions[i] << " is set to 0\n";
          directions[i]= 0;
        }
  }
