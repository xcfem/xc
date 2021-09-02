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

#include <domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2dBase.h>

#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include "domain/mesh/element/utils/coordTransformation/CrdTransf2d.h"
#include "domain/mesh/node/Node.h"

//! @brief Set the coordinate transformation for the element.
void XC::ElasticBeam2dBase::set_transf(const CrdTransf *trf)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf2d *tmp= dynamic_cast<const CrdTransf2d *>(trf);
        if(tmp)
          theCoordTransf= tmp->getCopy();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to get a copy of coordinate transformation."
	              << std::endl;
            exit(-1);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to coordinate transformation is null."
		<< std::endl;
  }

//! @brief Default constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
XC::ElasticBeam2dBase::ElasticBeam2dBase(int tag, int classTag)
  : ProtoBeam2d(tag, classTag), theCoordTransf(nullptr)
  {}

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param m: element material.
//! @param trf: coordinate transformation for the element.
XC::ElasticBeam2dBase::ElasticBeam2dBase(int tag, int classTag, const Material *m,const CrdTransf *trf)
  :ProtoBeam2d(tag, classTag, m), theCoordTransf(nullptr)
  { set_transf(trf); }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param a: area of the element cross section.
//! @param e: elastic modulus of the element material.
//! @param i: modulus of inertia the element section.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam2dBase::ElasticBeam2dBase(int tag, int classTag, double a, double e, double i, int Nd1, int Nd2, CrdTransf2d &coordTransf, double r)
  : ProtoBeam2d(tag, classTag,a,e,i,Nd1,Nd2), 
    theCoordTransf(nullptr)
  {
    setRho(r);
    set_transf(&coordTransf);
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param a: area of the element cross section.
//! @param e: elastic modulus of the element material.
//! @param g: shear modulus of the element material.
//! @param i: modulus of inertia the element section.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param theTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam2dBase::ElasticBeam2dBase(int tag, int classTag, double a, double alpha, double e, double g, double i, int Nd1, int Nd2, CrdTransf2d &theTransf, double r)
  : ProtoBeam2d(tag, classTag,a,alpha,e,g,i,Nd1,Nd2), 
    theCoordTransf(nullptr)  
  {
    setRho(r);
    set_transf(&theTransf);
  }

//! @brief Copy constructor.
XC::ElasticBeam2dBase::ElasticBeam2dBase(const ElasticBeam2dBase &other)
  :ProtoBeam2d(other), theCoordTransf(nullptr)
  {
    set_transf(other.theCoordTransf);
  }

//! @brief Assignment operator.
XC::ElasticBeam2dBase &XC::ElasticBeam2dBase::operator=(const ElasticBeam2dBase &other)
  {
    ProtoBeam2d::operator=(other);
    set_transf(other.theCoordTransf);
    return *this;
  }

//! brief Destructor.
XC::ElasticBeam2dBase::~ElasticBeam2dBase(void)
  {
    if(theCoordTransf)
      {
	delete theCoordTransf;
	theCoordTransf= nullptr;
      }
  }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
XC::CrdTransf *XC::ElasticBeam2dBase::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::CrdTransf *XC::ElasticBeam2dBase::getCoordTransf(void) const
  { return theCoordTransf; }

//! @brief Set the element domain.
void XC::ElasticBeam2dBase::setDomain(Domain *theDomain)
  {
    ProtoBeam2d::setDomain(theDomain);

    const int dofNd1= theNodes[0]->getNumberDOF();
    if(dofNd1 != 3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Node 1: " << theNodes.getTagNode(0)
                  << " has incorrect number of DOF: "
	          << dofNd1 << " instead of 3.\n";
        exit(-1);
      }

    const int dofNd2= theNodes[1]->getNumberDOF();
    if(dofNd2 != 3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Node 2: " << theNodes.getTagNode(1)
                  << " has incorrect number of DOF: "
	          << dofNd2 << " instead of 3.\n";
        exit(-1);
      }
    if(theCoordTransf)
      {
        if(theCoordTransf->initialize(theNodes[0], theNodes[1]) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; error initializing coordinate transformation\n";
            exit(-1);
          }
        const double L= theCoordTransf->getInitialLength();
        if(L==0.0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; element has zero length\n";
            exit(-1);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; the element has not coordinate transformation."
		<< std::endl;
   }

//! @brief Returns the direction vector of element strong axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam2dBase::getVDirStrongAxisGlobalCoord(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; not implemented for deformed geometry." << std::endl;
    if(theCoordTransf)
      {
        const Vector eF= getVDirStrongAxisLocalCoord();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eF);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; coordinate transformation not defined."
                  << std::endl;
	static Vector retval;
        return retval;
      }
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam2dBase::getVDirWeakAxisGlobalCoord(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; not implemented for deformed geometry." << std::endl;
    if(theCoordTransf)
      {
        const Vector eD= getVDirWeakAxisLocalCoord();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eD);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; coordinate transformation not defined."
                  << std::endl;
	static Vector retval;
        return retval;
      }
  }

//! @brief Send members through the communicator argument.
int XC::ElasticBeam2dBase::sendData(Communicator &comm)
  {
    int res= ProtoBeam2d::sendData(comm);
    res+= sendCoordTransf(8,9,10,comm);
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ElasticBeam2dBase::recvData(const Communicator &comm)
  {
    int res= ProtoBeam2d::recvData(comm);
    theCoordTransf= recvCoordTransf2d(8,9,10,comm);
    return res;
  }

//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! @param code: identifier of the requested value.
//! @param silent: if true don't complaint about non-existen property.
boost::python::list XC::ElasticBeam2dBase::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    if(code=="N")
      {
	retval.append(getN1());
	retval.append(getN2());
      }
    else if((code=="M") || (code=="Mz"))
      {
	retval.append(getM1());
	retval.append(getM2());
      }
    else if((code=="V") || (code=="Vy") || (code=="Q") || (code=="Qy"))
      {
	retval.append(getV1());
	retval.append(getV2());
      }
    else
      retval= ProtoBeam2d::getValuesAtNodes(code, silent); 
    return retval;
  }
