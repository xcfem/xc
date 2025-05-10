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
//  under the terms of the GNU General Public License published by 
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
//DomainVector.cc

#include "DampingVector.h"
#include "Damping.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/utils/misc_utils/colormod.h"
#include "utility/actor/actor/MovableSTDVector.h"

//! @brief Default constructor.
XC::DampingVector::DampingVector(const size_t &nMat,const Damping *matModel)
  : std::vector<Damping *>(nMat,nullptr), MovableObject(DAMPING_VECTOR_TAG)
  {
    if(matModel)
      {
        for(iterator i= damping_vector::begin();i!=damping_vector::end();i++)
          {
            (*i)= dynamic_cast<Damping *>(matModel->getCopy());
            if(!(*i))
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate damping model pointer"
			<< Color::def << std::endl;
          }
      }
  }
//! @brief Copy dampings from another vector.
void XC::DampingVector::alloc(const std::vector<Damping *> &mats)
  {
    clearAll();
    const size_t nDamp= mats.size();
    this->resize(nDamp);
    for(size_t i= 0;i<nDamp;i++)
      {
        if(mats[i])
          {
            (*this)[i]= dynamic_cast<Damping *>(mats[i]->getCopy());
            if(!(*this)[i])
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate damping model pointer"
			<< Color::def << std::endl;
          }
      }
  }


//! @brief Copy constructor.
XC::DampingVector::DampingVector(const DampingVector &other)
  : std::vector<Damping *>(other.size(),nullptr), MovableObject(DAMPING_VECTOR_TAG)
  { alloc(other); }

//! @brief Assignment operator.
XC::DampingVector &XC::DampingVector::operator=(const DampingVector &other)
  { 
    alloc(other);
    return *this;
  }

void XC::DampingVector::setDamping(const Damping *new_damp)
  {
    clearDampings();
    if(new_damp)
      {
        for(iterator i= damping_vector::begin();i!=damping_vector::end();i++)
          {
            (*i)= new_damp->getCopy();
            if(!(*i))
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate damping model pointer"
			<< Color::def << std::endl;
          }
      }
  }

void XC::DampingVector::setDamping(size_t i,Damping *new_damp)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= new_damp;
  }

void XC::DampingVector::setDamping(size_t i, const Damping &new_damp)
  { setDamping(i, new_damp.getCopy()); }

//! @brief copy the user defined properties of the given object on each of
//! the dampings.  
void XC::DampingVector::copyPropsFrom(const EntityWithProperties *other_damp)
  {
    if(other_damp)
      {
	const EntityWithProperties &tmp= *other_damp;
        for(iterator i= damping_vector::begin();i!=damping_vector::end();i++)
          {
            (*i)->copyPropsFrom(tmp);
          }
      }
  }

void XC::DampingVector::clearDampings(void)
  {
    for(iterator i= damping_vector::begin();i!=damping_vector::end();i++)
      {
        if(*i)
	  {
	    delete (*i);
            (*i)= nullptr;
	  }
      }
  }

//! @brief Returns true if no damping assigned.
bool XC::DampingVector::empty(void) const
  {
    if(damping_vector::empty())
      return true;
    else
      return ((*this)[0]==nullptr);
  }

void XC::DampingVector::clearAll(void)
  {
    clearDampings();
    std::vector<Damping *>::clear();
  }

void XC::DampingVector::setDomain(Domain *theDomain, int nComp)
  {
    for(iterator i=damping_vector::begin();i!=damping_vector::end();i++)
      if((*i) && (*i)->setDomain(theDomain, nComp))
	{
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; error initializing damping."
		    << std::endl;
	  exit(-1);
	}
    
  }

//! @brief Commits dampings state (normally after convergence).
int XC::DampingVector::commitState(void)
  {
    int retVal= 0;

    for(iterator i=damping_vector::begin();i!=damping_vector::end();i++)
      retVal+= (*i)->commitState();
    return retVal;
  }

//! @brief Returns dampings to its last committed state.
int XC::DampingVector::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=damping_vector::begin();i!=damping_vector::end();i++)
      retVal+= (*i)->revertToLastCommit() ;
    return retVal;
  }


//! @brief Return dampings to its initial state.
int XC::DampingVector::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=damping_vector::begin();i!=damping_vector::end();i++)
      retVal+= (*i)->revertToStart();
    return retVal;
  }


//! @brief Return the identifiers of the materials.
std::set<int> XC::DampingVector::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= damping_vector::begin();i!=damping_vector::end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Return the identifiers of the materials in a python list.
boost::python::list XC::DampingVector::getTagsPy(void) const
  {
    boost::python::list retval;
    std::set<int> tmp= this->getTags();
    for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::DampingVector::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::DampingVector::sendData(Communicator &comm)
  {
    int res= sendSTDVector(*this, comm, getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::DampingVector::recvData(const Communicator &comm)
  {
    int res= receiveSTDVector(*this, comm,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewDamping);
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::DampingVector::sendSelf(Communicator &comm)
  {
    inicComm(2);
    int res= sendData(comm);
    const int dataTag=getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to send ID"
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::DampingVector::recvSelf(const Communicator &comm)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to receive ID"
	        << Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }
