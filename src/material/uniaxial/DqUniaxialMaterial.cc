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
//DqUniaxialMaterial.cc

#include "DqUniaxialMaterial.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableID.h"

//! @brief Copy the list being passed as parameter.
void XC::DqUniaxialMaterial::copy_list(const DqUniaxialMaterial &other,SectionForceDeformation *s)
  {
    free_mem();
    const size_t numMats= other.size();
    if(numMats>0)
      {
        resize(numMats);
        for(register size_t i= 0;i<numMats;i++)
          {
            if(!other[i])
              {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; null uniaxial material pointer.\n";
                return;
              }
            if(s)
              (*this)[i] = other[i]->getCopy(s);
            else
              (*this)[i]= other[i]->getCopy();
            if(!(*this)[i])
              {
	        std::cerr << "DqUniaxialMaterial::copy_list; failed to copy uniaxial material\n";
	        exit(-1);
              }
          }
      }
    else
      erase(begin(),end());
  }

void XC::DqUniaxialMaterial::free_mem(void)
  {
    for(iterator i= begin();i!=end();i++)
      if(*i)
        {
          delete *i;
          (*i)= nullptr;
        }
  }

void XC::DqUniaxialMaterial::resize(const size_t &n)
  {
    free_mem();
    lst_ptr::resize(n,nullptr);
  }

//! @brief Constructor.
XC::DqUniaxialMaterial::DqUniaxialMaterial(CommandEntity *owner,const size_t &sz)
  : CommandEntity(owner), MovableObject(0), lst_ptr(sz) 
  {
    for(size_t i=0;i<sz;i++)
      lst_ptr::operator[](i)= nullptr;
  }

//! @brief Constructor.
XC::DqUniaxialMaterial::DqUniaxialMaterial(CommandEntity *owner,const UniaxialMaterial &um)
  : CommandEntity(owner), MovableObject(0), lst_ptr()
  { push_back(&um); }

//! @brief Constructor.
XC::DqUniaxialMaterial::DqUniaxialMaterial(CommandEntity *owner,const UniaxialMaterial &um,const size_t &sz)
  : CommandEntity(owner), MovableObject(0), lst_ptr(sz)
  {
    for(size_t i=0;i<sz;i++)
      lst_ptr::operator[](i)= um.getCopy();
  }

//! @brief Constructor.
XC::DqUniaxialMaterial::DqUniaxialMaterial(CommandEntity *owner,const UniaxialMaterial *um,const size_t &sz)
  : CommandEntity(owner), MovableObject(0), lst_ptr(sz)
  {
    if(um)
      {
        for(size_t i=0;i<sz;i++)
          lst_ptr::operator[](i)= um->getCopy();
      }
  }


//! @brief Copy constructor.
XC::DqUniaxialMaterial::DqUniaxialMaterial(const DqUniaxialMaterial &other)
  : CommandEntity(other), MovableObject(other), lst_ptr()
  { copy_list(other); }

//! @brief Copy constructor.
XC::DqUniaxialMaterial::DqUniaxialMaterial(const DqUniaxialMaterial &other,SectionForceDeformation *s)
  : CommandEntity(other), MovableObject(0), lst_ptr()
  { copy_list(other,s); }

//! @brief Assignment operator.
XC::DqUniaxialMaterial &XC::DqUniaxialMaterial::operator=(const DqUniaxialMaterial &other)
  {
    CommandEntity::operator=(other);
    MovableObject::operator=(other);
    copy_list(other);
    return *this;
  }

//! @brief Destructor.
XC::DqUniaxialMaterial::~DqUniaxialMaterial(void)
  { clearAll(); }

//! @brief Clears the container.
void XC::DqUniaxialMaterial::clear(void)
  {
    free_mem();
    lst_ptr::clear();
  }

//! @brief Clears the container and deletes object properties.
void XC::DqUniaxialMaterial::clearAll(void)
  {
    clear();
    CommandEntity::clearPyProps();
  }

//! @brief Commit materials state (normally when convergence is achieved).
int XC::DqUniaxialMaterial::commitState(void)
  {
    int err = 0;
    for(register iterator i= begin();i!=end(); i++)
      {
        int tmp= (*i)->commitState();
        if(tmp!=0)
          {
	    std::cerr << "WARNING XC::DqUniaxialMaterial::commitState() ";
	    std::cerr << "MaterialModel failed to commitState():" ;
	    (*i)->Print(std::cerr);
	  }
        err+= tmp;
      }
    return err;
  }

//! @brief Returns to the last commited state.
int XC::DqUniaxialMaterial::revertToLastCommit(void)
  {
    int err = 0;
    for(register iterator i= begin();i!=end(); i++)
      {
        int tmp= (*i)->revertToLastCommit();
        if(tmp!=0)
          {
	    std::cerr << "WARNING XC::DqUniaxialMaterial::revertToLastCommit() ";
	    std::cerr << "MaterialModel failed to revertToLastCommit():" ;
	    (*i)->Print(std::cerr);
	  }
        err+= tmp;
      }
    return err;
  }

//! @brief Returns to the initial state.
int XC::DqUniaxialMaterial::revertToStart(void)
  {
    int err = 0;
    for(register iterator i= begin();i!=end(); i++)
      {
        int tmp= (*i)->revertToStart();
        if(tmp!=0)
          {
	    std::cerr << "WARNING XC::DqUniaxialMaterial::revertToStart() ";
	    std::cerr << "MaterialModel failed to revertToStart():" ;
	    (*i)->Print(std::cerr);
	  }
        err+= tmp;
      }
    return err;
  }

//! @brief Zeroes initial strains.
int XC::DqUniaxialMaterial::zeroInitialStrain(void)
  {
    int err= 0;
    for(register iterator i= begin();i!=end(); i++)
      err+= (*i)->setInitialStrain(0.0);
    return err;
  }

//! @brief Sets initial strain.
int XC::DqUniaxialMaterial::setInitialStrain(const Vector &def,const size_t &offset)
  {
    int err= 0;
    size_t j= offset;
    assert(static_cast<size_t>(def.Size()) >= (size()+offset));
    for(register iterator i= begin();i!=end(); i++,j++)
      err+= (*i)->setInitialStrain(def(j));
    return err;
  }

//! @brief Sets trial strain.
int XC::DqUniaxialMaterial::setTrialStrain(const Vector &def,const size_t &offset)
  {
    int err= 0;
    size_t j= offset;
    assert(static_cast<size_t>(def.Size()) >= (size()+offset));
    for(register iterator i= begin();i!=end(); i++,j++)
      err += (*i)->setTrialStrain(def(j));
    return err;
  }

//! @brief Asigna la trial strain (se emplea en ParallelMaterial).
int XC::DqUniaxialMaterial::setTrialStrain(const double &strain,const double &strainRate)
  {
    int err= 0;
    for(register iterator i= begin();i!=end(); i++)
      err += (*i)->setTrialStrain(strain,strainRate);
    return err;
  }


//! @brief Returns the initial strains.
void XC::DqUniaxialMaterial::getInitialStrain(Vector &def,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(def.Size()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      def(j)= (*i)->getInitialStrain();
  }

//! @brief Returns strains vector.
void XC::DqUniaxialMaterial::getStrain(Vector &def,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(def.Size()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      def(j)= (*i)->getStrain();
  }

//! @brief Return the tangent stiffness matrix.
void XC::DqUniaxialMaterial::getTangent(Matrix &k,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(k.noRows()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      k(j,j)= (*i)->getTangent();
  }

//! @brief Return the initial tangent stiffness matrix.
void XC::DqUniaxialMaterial::getInitialTangent(Matrix &k,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(k.noRows()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      k(j,j)= (*i)->getInitialTangent();
  }

//! @brief Return the flexibility matrix.
void XC::DqUniaxialMaterial::getFlexibility(Matrix &f,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(f.noRows()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      f(j,j)= (*i)->getFlexibility();
  }

//! @brief Return the initial flexibility matrix.
void XC::DqUniaxialMaterial::getInitialFlexibility(Matrix &f,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(f.noRows()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      f(j,j)= (*i)->getInitialFlexibility();
  }

//! @brief Returns the tensiones.
void XC::DqUniaxialMaterial::getStress(Vector &s,const size_t &offset) const
  {
    size_t j= offset;
    assert(static_cast<size_t>(s.Size()) >= (size()+offset));
    for(register const_iterator i= begin();i!=end(); i++,j++)
      s(j)= (*i)->getStress();
  }

void XC::DqUniaxialMaterial::push_back(const UniaxialMaterial *t,SectionForceDeformation *s)
  {
    if(!t)
      std::cerr << "DqUniaxialMaterial::push_back; pointer to material is null." << std::endl;
    else
      {
        UniaxialMaterial *tmp= nullptr;
        if(s)
          tmp= t->getCopy(s);
        else
          tmp= t->getCopy();
        if(tmp)
          lst_ptr::push_back(tmp);
        else
          std::cerr << "DqUniaxialMaterial::push_back; can't create an UniaxialMaterial" << std::endl;
      }
  }

void XC::DqUniaxialMaterial::push_front(const UniaxialMaterial *t,SectionForceDeformation *s)
  {
    UniaxialMaterial *tmp= nullptr;
    if(!t)
      std::cerr << "DqUniaxialMaterial::push_back; pointer to material is null." << std::endl;
    else
      {
        if(s)
          tmp= t->getCopy(s);
        else
          tmp= t->getCopy();
        if(tmp)
          lst_ptr::push_front(tmp);
        else
          std::cerr << "DqUniaxialMaterial::push_front; can't create UniaxialMaterial" << std::endl;
      }
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DqUniaxialMaterial::sendData(CommParameters &cp)
  {
    const size_t sz= size();
    setDbTagDataPos(0,sz);
    DbTagData cpMat(sz*2);
    inicComm(sz*2);
    int res= 0;
    for(size_t i= 0;i<sz;i++)
      res+= cp.sendBrokedPtr((*this)[i],cpMat,BrokedPtrCommMetaData(i,0,i+sz));
    res+= cpMat.send(getDbTagData(),cp,CommMetaData(1));
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DqUniaxialMaterial::recvData(const CommParameters &cp)
  {
    const size_t sz= getDbTagDataPos(0);
    DbTagData cpMat(sz*2);
    inicComm(sz*2);
    int res= cpMat.receive(getDbTagData(),cp,CommMetaData(1));

    for(size_t i= 0;i<sz;i++)
      {
        // Receive the material
        (*this)[i]= cp.getBrokedMaterial((*this)[i],cpMat,BrokedPtrCommMetaData(i,0,i+sz));
        if(!(*this)[i])
          std::cerr << "MaterialVector::recvSelf() - material "
                    << i << "failed to recv itself\n";
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DqUniaxialMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING DqUniaxialMaterial::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DqUniaxialMaterial::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING DqUniaxialMaterial::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Print stuff.
void XC::DqUniaxialMaterial::Print(std::ostream &s, int flag) const
  {
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      s << "\t\tUniaxial XC::Material, tag: " << (*this)[i]->getTag() << std::endl;
  }
