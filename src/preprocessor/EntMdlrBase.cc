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
//EntMdlrBase.cc

#include "EntMdlrBase.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Edge.h"

#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableString.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Return a pointer to the object preprocessor.
const XC::Preprocessor *XC::EntMdlrBase::getPreprocessor(void) const
  { return dynamic_cast<const Preprocessor *>(Owner()); }

//! @brief Return a pointer to the object preprocessor.
XC::Preprocessor *XC::EntMdlrBase::getPreprocessor(void)
  { return dynamic_cast<Preprocessor *>(Owner()); }

//! @brief Constructor.
XC::EntMdlrBase::EntMdlrBase(const std::string &nmb,Preprocessor *prep)
  : NamedEntity(nmb,prep), MovableObject(0) {}

//! @brief Comparison operator.
bool XC::EntMdlrBase::operator==(const EntMdlrBase &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= NamedEntity::operator==(other);
        if(retval)
          retval= (labels==other.labels);
      }
    return retval;
  }

//! @brief += operator.
XC::EntMdlrBase &XC::EntMdlrBase::operator+=(const EntMdlrBase &other)
  {
    NamedEntity::operator+=(other);
    labels+= other.labels;
    return *this;
  }

//! @brief -= operator.
XC::EntMdlrBase &XC::EntMdlrBase::operator-=(const EntMdlrBase &other)
  {
    NamedEntity::operator-=(other);
    labels-= other.labels;
    return *this;
  }

//! @brief *= operator (intersection).
XC::EntMdlrBase &XC::EntMdlrBase::operator*=(const EntMdlrBase &other)
  {
    NamedEntity::operator*=(other);
    labels*= other.labels;
    return *this;
  }

//! @brief Return the object identifier in the model (tag).
size_t XC::EntMdlrBase::getTag(void) const
  {
    const std::string tmp= getName().substr(1);
    int retval= 0;
    try
      { retval= boost::lexical_cast<size_t>(tmp); }
    catch(boost::bad_lexical_cast &)
      {
	retval= reinterpret_cast<size_t>(this); // and hope for the best.
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR: could not extract entity tag from its name: '"
		  << getName() << "'. Weird results could follow."
		  << " Returning: " << retval << std::endl;
      }    
    return retval;
  }

//! @brief Check for preprocessor.
bool XC::EntMdlrBase::check_preprocessor(void) const
  {
    if(getPreprocessor())
      return true;
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << ": preprocessor not set for object: '"
                  << getName() << "'."
                  << std::endl;
        return false;
      }
  }

//! @brief Return a pointer to the point identified by
//! the tag being passed as parameter.
XC::Pnt *XC::EntMdlrBase::BuscaPnt(const size_t &id_point)
  {
    Pnt *retval= nullptr;
    if(check_preprocessor())
      {
        MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
        retval= mbt.getPoints().busca(id_point);
      }
    return retval;
  }

//! @brief Return a const pointer to the point identified by
//! the tag being passed as parameter.
const XC::Pnt *XC::EntMdlrBase::BuscaPnt(const size_t &id_point) const
  {
    const Pnt *retval= nullptr;
    if(check_preprocessor())
      {
        const MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
        retval= mbt.getPoints().busca(id_point);
      }
    return retval;
  }

//! @brief Return a pointer to the edge identified by
//! the tag being passed as parameter.
XC::Edge *XC::EntMdlrBase::BuscaEdge(const size_t &id_edge)
  {
    Edge *retval= nullptr;
    if(check_preprocessor())
      {
        MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
        retval= mbt.getLines().busca(id_edge);
      }
    return retval;
  }

//! @brief Return a pointer to the edge identified by
//! the tag being passed as parameter.
const XC::Edge *XC::EntMdlrBase::BuscaEdge(const size_t &id_edge) const
  {
    const Edge *retval= nullptr;
    if(check_preprocessor())
      {
        const MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
        retval= mbt.getLines().busca(id_edge);
      }
    return retval;
  }

//! @brief Return a pointer to the face identified by
//! the tag being passed as parameter.
XC::Face *XC::EntMdlrBase::BuscaFace(const size_t &id_face)
  {
    Face *retval= nullptr;
    if(check_preprocessor())
      {
        MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
        retval= mbt.getSurfaces().busca(id_face);
      }
    return retval;
  }

//! @brief Returns a pointer to the face identified by
//! the tag being passed as parameter.
const XC::Face *XC::EntMdlrBase::BuscaFace(const size_t &id_face) const
  {
    const Face *retval= nullptr;
    if(check_preprocessor())
      {
        const MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
        retval= mbt.getSurfaces().busca(id_face);
      }
    return retval;
  }

//! @brief Send labels through the communicator argument.
int XC::EntMdlrBase::sendIdsLabels(const int &posSize,const int &posDbTag, Communicator &comm)
  {
    int res= 0;
    static ID labelIds;
    const std::set<int> ids= labels.getIdsLabels();
    const size_t sz= ids.size();
    setDbTagDataPos(posSize,sz);
    if(sz>0)
      {
        labelIds.resize(sz);
        int conta= 0;
        for(std::set<int>::const_iterator i= ids.begin();i!=ids.end();i++,conta++)
          labelIds[conta]= *i;
        res+= comm.sendID(labelIds,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      comm.sendInt(0,getDbTagData(),CommMetaData(posDbTag));
    return res;
  }

//! @brief Receive labels through the communicator argument.
int XC::EntMdlrBase::recvIdsLabels(const int &posSize,const int &posDbTag,const Communicator &comm)
  {
    int res= 0;
    static ID labelIds;
    const size_t sz= getDbTagDataPos(posSize);
    if(sz>0)
      {
        const LabelDictionary &dic= labels.getDictionary();
        res= comm.receiveID(labelIds,getDbTagData(),CommMetaData(posDbTag));
    
        const size_t sz= labelIds.Size();
        for(size_t i=0;i<sz;i++)
          labels.addLabel(dic(labelIds[i]));
      }
    return res;
  }

//! @brief Send members through the communicator argument.
int XC::EntMdlrBase::sendData(Communicator &comm)
  {
    int res= comm.sendString(getName(),getDbTagData(),CommMetaData(0));
    res+= sendIdsLabels(1,2,comm);
    return res;
  }

//! @brief Receive members through the communicator argument.
int XC::EntMdlrBase::recvData(const Communicator &comm)
  {
    std::string tmp;
    int res= comm.receiveString(tmp,getDbTagData(),CommMetaData(0));
    setName(tmp);
    res+= recvIdsLabels(1,2,comm);
    return res;
  }

//! @brief Send objects through the communicator argument.
int XC::EntMdlrBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receive objects through the communicator argument.
int XC::EntMdlrBase::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Return the union of both objects.
XC::EntMdlrBase XC::operator+(const XC::EntMdlrBase &a,const XC::EntMdlrBase &b)
  {
    XC::EntMdlrBase retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the labels in a that are not in b.
XC::EntMdlrBase XC::operator-(const XC::EntMdlrBase &a,const XC::EntMdlrBase &b)
  {
    XC::EntMdlrBase retval(a);
    retval-= b;
    return retval;
  }

//! @brief Return the labels in a that are also in b.
XC::EntMdlrBase XC::operator*(const XC::EntMdlrBase &a,const XC::EntMdlrBase &b)
  {
    XC::EntMdlrBase retval(a);
    retval*= b;
    return retval;    
  }

//! @brief Print stuff.
std::ostream &XC::operator<<(std::ostream &os, const XC::EntMdlrBase &e)
  {
    os << e.getName() << " labels" << e.getLabels() << std::endl;
    return os;
  }
