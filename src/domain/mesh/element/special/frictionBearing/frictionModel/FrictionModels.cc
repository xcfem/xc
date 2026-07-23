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
//FrictionModels.cc


#include "FrictionModels.h"
#include "material/ResponseId.h"
#include "utility/actor/actor/MovableSTDVector.h"

void XC::FrictionModels::alloc(const std::vector<FrictionModel *> &fModels)
  {
    this->clearAll();
    const size_t nMat= fModels.size();
    this->theFrictionModels.resize(nMat);
    for(size_t i= 0;i<nMat;i++)
      {
        if(fModels[i])
          {
	    FrictionModel *tmp= fModels[i]->getCopy();
	    if(tmp)
	      { this->theFrictionModels[i]= tmp; }
          }
      }
  }

void XC::FrictionModels::clearAll()
  {
    clearFrictionModels();
    theFrictionModels.clear();
  }

//! @brief Constructor.
XC::FrictionModels::FrictionModels(const size_t &nMat,const FrictionModel *fModel)
  : MovableObject(0), theFrictionModels(nMat, nullptr)
  {
    setFrictionModel(fModel);
  }

//! @brief Constructor.
XC::FrictionModels::FrictionModels(const size_t sz, const FrictionModel &fModel)
  : MovableObject(0), theFrictionModels(sz, nullptr)
  {
    setFrictionModel(&fModel);
  }

//! @brief Constructor.
XC::FrictionModels::FrictionModels(const FrictionModel &fModel)
  : MovableObject(0), theFrictionModels(1, nullptr)
  {
    setFrictionModel(0, &fModel);
  }

XC::FrictionModels::FrictionModels(const FrictionModels &other)
  : MovableObject(other), theFrictionModels()
  {
    this->alloc(other.theFrictionModels);
  }
XC::FrictionModels &XC::FrictionModels::operator=(const FrictionModels &other)
  {
    MovableObject::operator=(other);
    this->alloc(other.theFrictionModels);
    return *this;
  }
XC::FrictionModels::~FrictionModels()
  { this->clearAll(); }

int XC::FrictionModels::commitState(void)
  {
    int retval= 0;
    for(friction_model_vector::iterator i= theFrictionModels.begin();i!=theFrictionModels.end();i++)
      {
        if(*i)
	  {
	    retval+= (*i)->commitState();
	  }
      }
    return retval;
  }


int XC::FrictionModels::revertToLastCommit(void)
  {
    int retval= 0;
    for(friction_model_vector::iterator i= theFrictionModels.begin();i!=theFrictionModels.end();i++)
      {
        if(*i)
	  {
	    retval+= (*i)->revertToLastCommit();
	  }
      }
    return retval;
  }



int XC::FrictionModels::revertToStart(void)
  {
    int retval= 0;
    for(friction_model_vector::iterator i= theFrictionModels.begin();i!=theFrictionModels.end();i++)
      {
        if(*i)
	  {
	    retval+= (*i)->revertToStart();
	  }
      }
    return retval;    
  }

void XC::FrictionModels::clearFrictionModels(void)
  {
    for(friction_model_vector::iterator i= theFrictionModels.begin();i!=theFrictionModels.end();i++)
      {
        if(*i)
	  {
	    delete (*i);
            (*i)= nullptr;
	  }
      }
  }

void XC::FrictionModels::setFrictionModel(const FrictionModel *fModel)
  {
    clearFrictionModels();
    if(fModel)
      {
        for(friction_model_vector::iterator i= theFrictionModels.begin();i!=theFrictionModels.end();i++)
          {
	    FrictionModel *tmp= fModel->getCopy();
	    if(tmp)
	      { (*i)= tmp; }
          }
      }
  }


void XC::FrictionModels::setFrictionModel(size_t i,const FrictionModel *fModel)
  {
    if((*this)[i])
      delete (*this)[i];
    FrictionModel *tmp= fModel->getCopy();
    this->theFrictionModels[i]= tmp;
  }



void XC::FrictionModels::setFrictionModel(size_t i,const FrictionModel &fModel)
  { this->setFrictionModel(i, &fModel); }

void XC::FrictionModels::copyFrictionModelsFrom(const FrictionModels &other)
  { this->alloc(other.theFrictionModels); }

void XC::FrictionModels::setPyList(const boost::python::list &l)
  {
    const size_t sz= boost::python::len(l);
    std::vector<FrictionModel *> tmp(sz, nullptr);
    for(size_t i= 0; i<sz; i++) 
      {
	tmp[i]= boost::python::extract<FrictionModel *>(l[i]);
      }
    this->alloc(tmp);
  }

boost::python::list XC::FrictionModels::getPyList() const
  {
    boost::python::list retval;
    const size_t sz= this->size();
    for(size_t i=0; i<sz; i++)
      {
	const FrictionModel *ptr= this->theFrictionModels[i];
	boost::python::object pyObj(boost::ref(*ptr));
        retval.append(pyObj);
      }
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.

XC::DbTagData &XC::FrictionModels::getDbTagData(void) const
  {
    static DbTagData retval(2); 
    return retval;
  }

//! @brief Send members through the communicator argument.

int XC::FrictionModels::sendData(Communicator &comm)
  {
    int res= sendSTDVector(theFrictionModels, comm, getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives members through the communicator argument.

int XC::FrictionModels::recvData(const Communicator &comm)
  {
    int res= receiveSTDVector(theFrictionModels, comm,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewFrictionModel);
    return res;
  }

//! @brief Sends object.

int XC::FrictionModels::sendSelf(Communicator &comm)
  {
    inicComm(2);

    int res= this->sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send ID data."
	        << std::endl;
    return res;
  }

//! @brief Receives object

int XC::FrictionModels::recvSelf(const Communicator &comm)
  {
    inicComm(2);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data"
	        << std::endl;
    else
      res+= this->recvData(comm);
    return res;
  }

//! @brief Obtain information from the analysis results.

int XC::FrictionModels::getResponse(int responseID, Information &eleInformation)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet"
	        << std::endl;
    return -1;
  }
  
//! @brief FrictionModel response.

XC::Response *XC::FrictionModels::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet.\n";
    return nullptr;
  }

//! @brief Print stuff.
void XC::FrictionModels::Print(std::ostream &s, int flag) const
  {
    for(friction_model_vector::const_iterator i= theFrictionModels.begin();i!=theFrictionModels.end();i++)
      {
        if(*i)
	  {
	    s << "  FrictionModel: " << (*i)->getTag() << std::endl;
	  }
      }
  }  
 
