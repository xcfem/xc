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

int XC::ZeroLengthMaterials::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3); 
    const size_t sz= size();

    int res= sendData(cp);
    ID direction(sz);
    for(size_t i= 0;i<sz;i++)
      direction[i]= directions[i];
    res+= cp.sendID(direction,getDbTagData(),CommMetaData(2));

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send.\n";
    return res;
  }

int XC::ZeroLengthMaterials::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive.\n";
    else
      {
        res+= recvData(cp);
        ID direction;
        res+= cp.receiveID(direction,getDbTagData(),CommMetaData(2));
        const size_t sz= direction.Size();
        directions.resize(sz);
        for(size_t i= 0;i<sz;i++)
          directions[i]= direction(i);
      }
    return res;
  }


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
