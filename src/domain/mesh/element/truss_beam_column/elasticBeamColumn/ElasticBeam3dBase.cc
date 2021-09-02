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

#include <domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3dBase.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>
#include "domain/mesh/node/Node.h"

void XC::ElasticBeam3dBase::set_transf(const CrdTransf *trf)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf3d *tmp= dynamic_cast<const CrdTransf3d *>(trf);
        if(tmp)
          theCoordTransf = tmp->getCopy();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; failed to get copy of coordinate transformation.\n";
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
XC::ElasticBeam3dBase::ElasticBeam3dBase(int tag, int classTag)
  :ProtoBeam3d(tag,classTag), theCoordTransf(nullptr)
  {}

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param m: element material.
//! @param trf: coordinate transformation for the element.
XC::ElasticBeam3dBase::ElasticBeam3dBase(int tag, int classTag,const Material *m,const CrdTransf *trf)
  : ProtoBeam3d(tag,classTag,m), theCoordTransf(nullptr)
  { set_transf(trf); }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param a: area of the element cross section.
//! @param e: elastic modulus of the element material.
//! @param g: shear modulus of the element material.
//! @param jx: torsional section modulus of the element section.
//! @param iy: modulus of inertia the element section around the y axis.
//! @param iz: modulus of inertia the element section around the z axis.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam3dBase::ElasticBeam3dBase(int tag, int classTag,
					 double a, double e, double g,
                                         double jx, double iy, double iz,
					 int Nd1, int Nd2, CrdTransf3d &coordTransf,
					 double r)
  :ProtoBeam3d(tag,classTag,a,e,g,jx,iy,iz,Nd1,Nd2), theCoordTransf(nullptr)
  {
    setRho(r);
    set_transf(&coordTransf);
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param a: area of the element cross section.
//! @param alpha_y: ratio between section area and section shear area on y axis.
//! @param alpha_z: ratio between section area and section shear area on z axis.
//! @param e: elastic modulus of the element material.
//! @param g: shear modulus of the element material.
//! @param jx: torsional section modulus of the element section.
//! @param iy: modulus of inertia the element section around the y axis.
//! @param iz: modulus of inertia the element section around the z axis.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam3dBase::ElasticBeam3dBase(int tag, int classTag,
					 double a, double alpha_y, double alpha_z, double e, double g,
                                         double jx, double iy, double iz,
					 int Nd1, int Nd2, CrdTransf3d &coordTransf,
					 double r)
  :ProtoBeam3d(tag,classTag,a, alpha_y, alpha_z,e,g,jx,iy,iz,Nd1,Nd2),
   theCoordTransf(nullptr)
  {
    setRho(r);
    set_transf(&coordTransf);
  }

//! @brief Constructor.
//! @param tag: element identifier.
//! @param classTag: element class identifier.
//! @param Nd1: tag of the element I node.
//! @param Nd2: tag of the element J node.
//! @param section: SectionForceDeformation object to copy mechanical properties from.
//! @param coordTransf: coordinate transformation for the element.
//! @param r: density of the element material.
XC::ElasticBeam3dBase::ElasticBeam3dBase(int tag, int classTag, int Nd1, int Nd2, SectionForceDeformation *section, CrdTransf3d &coordTransf, double r)
  :ProtoBeam3d(tag,classTag,Nd1,Nd2), theCoordTransf(nullptr)
  {
    if(section)
      {
        //sectionTag= section->getTag();
	setSectionProperties(CrossSectionProperties3d(*section));
        setRho(r);
      }

    CrossSectionProperties3d &sprop= getSectionProperties();
    if(sprop.J()==0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no torsion in section -- setting GJ = 1.0e10\n";
        sprop.J() = 1.0e10;
      }

    set_transf(&coordTransf);
    
  }

//! @brief Copy constructor.
XC::ElasticBeam3dBase::ElasticBeam3dBase(const XC::ElasticBeam3dBase &other)
  :ProtoBeam3d(other), theCoordTransf(nullptr)
  {
    set_transf(other.theCoordTransf);
  }

//! @brief Assignment operator.
XC::ElasticBeam3dBase &XC::ElasticBeam3dBase::operator=(const XC::ElasticBeam3dBase &other)
  {
    ProtoBeam3d::operator=(other);
    set_transf(other.theCoordTransf);
    return *this;
  }

//! @brief Constructor.
XC::ElasticBeam3dBase::~ElasticBeam3dBase(void)
  { if(theCoordTransf) delete theCoordTransf;  }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
XC::CrdTransf *XC::ElasticBeam3dBase::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::CrdTransf *XC::ElasticBeam3dBase::getCoordTransf(void) const
  { return theCoordTransf; }

void XC::ElasticBeam3dBase::setDomain(Domain *theDomain)
  {
    ProtoBeam3d::setDomain(theDomain);

    const int dofNd1 = theNodes[0]->getNumberDOF();
    if(dofNd1 != 6)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; Node 1: " << theNodes.getTagNode(0)
                  << " has incorrect number of DOF: "
	          << dofNd1 << " instead of 6.\n";
        exit(-1);
      }
    const int dofNd2 = theNodes[1]->getNumberDOF();
    if(dofNd2 != 6)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; Node 1: " << theNodes.getTagNode(1)
                  << " has incorrect number of DOF: "
	          << dofNd2 << " instead of 6.\n";
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

	if(L == 0.0)
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
const XC::Vector &XC::ElasticBeam3dBase::getVDirStrongAxisGlobalCoord(bool initialGeometry) const
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
const XC::Vector &XC::ElasticBeam3dBase::getVDirWeakAxisGlobalCoord(bool initialGeometry) const
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
int XC::ElasticBeam3dBase::sendData(Communicator &comm)
  {
    int res= ProtoBeam3d::sendData(comm);
    res+= sendCoordTransf(8,9,10,comm);
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ElasticBeam3dBase::recvData(const Communicator &comm)
  {
    int res= ProtoBeam3d::recvData(comm);
    theCoordTransf= recvCoordTransf3d(8,9,10,comm);
    return res;
  }

//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! @param code: identifier of the requested value.
//! @param silent: if true dont complaint about non-existent property.
boost::python::list XC::ElasticBeam3dBase::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    if(code=="N")
      {
	retval.append(getN1());
	retval.append(getN2());
      }
    else if(code=="Vy" || code=="Qy")
      {
	retval.append(getVy1());
	retval.append(getVy2());
      }
    else if(code=="Vz" || code=="Qz")
      {
	retval.append(getVz1());
	retval.append(getVz2());
      }
    else if(code=="T")
      {
	retval.append(getT1());
	retval.append(getT2());
      }
    else if(code=="My")
      {
	retval.append(getMy1());
	retval.append(getMy2());
      }
    else if(code=="Mz")
      {
	retval.append(getMz1());
	retval.append(getMz2());
      }
    else
      retval= ProtoBeam3d::getValuesAtNodes(code, silent); 
    return retval;
  }
