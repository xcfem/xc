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
//BeamColumnWithSectionFDTrf3d.cpp

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.h>
#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>
#include "material/section/PrismaticBarCrossSection.h"


#include "xc_utils/src/geom/pos_vec/Vector2d.h"


void XC::BeamColumnWithSectionFDTrf3d::set_transf(const CrdTransf *trf)
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
            std::cerr << "BeamColumnWithSectionFDTrf3d::set_transf -- failed to get copy of coordinate transformation\n";
            exit(1);
          }
      }
    else
      std::cerr << "BeamColumnWithSectionFDTrf3d::set_transf; pointer to coordinate transformation is null." << std::endl;
  }

XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec)
  :BeamColumnWithSectionFD(tag, classTag,numSec), theCoordTransf(nullptr)
  {}

XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec,const Material *mat,const CrdTransf *coordTransf)
  :BeamColumnWithSectionFD(tag, classTag,numSec,mat), theCoordTransf(nullptr)
  { set_transf(coordTransf); }

XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf3d &coordTransf)
  :BeamColumnWithSectionFD(tag, classTag,numSec,nullptr,nodeI,nodeJ), theCoordTransf(nullptr)
  { set_transf(&coordTransf); }

//! @brief Copy constructor.
XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(const XC::BeamColumnWithSectionFDTrf3d &other)
  :BeamColumnWithSectionFD(other), theCoordTransf(nullptr)
  { set_transf(other.theCoordTransf); }

//! @brief Assignment operator.
XC::BeamColumnWithSectionFDTrf3d &XC::BeamColumnWithSectionFDTrf3d::operator=(const XC::BeamColumnWithSectionFDTrf3d &other)
  {
    //BeamColumnWithSectionFD::operator=(other);
    //set_transf(other.theCoordTransf);
    std::cerr << "BeamColumnWithSectionFDTrf3d; assignment operator not implemented." << std::endl;
    return *this;
  }

//! @brief Destructor.
XC::BeamColumnWithSectionFDTrf3d::~BeamColumnWithSectionFDTrf3d(void)
  {
    if(theCoordTransf) delete theCoordTransf;
    theCoordTransf= nullptr;
  }

void XC::BeamColumnWithSectionFDTrf3d::initialize_trf(void)
  {
    if(theCoordTransf->initialize(theNodes[0], theNodes[1]))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to initialize coordinate transformation\n";
        exit(-1);
      }
  }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
XC::CrdTransf *XC::BeamColumnWithSectionFDTrf3d::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::CrdTransf *XC::BeamColumnWithSectionFDTrf3d::getCoordTransf(void) const
  { return theCoordTransf; }

//! @brief Returns i-th cross section strong axis direction vector expressed in local coordinates.
XC::Vector XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisLocalCoord(const size_t &i) const
  {
    Vector retval(3);
    retval(2)= 1;
    const PrismaticBarCrossSection *tmp= dynamic_cast<const PrismaticBarCrossSection *>(theSections[i]);
    if(tmp)
      {
        const Vector2d sectionStrongAxis= tmp->getVDirStrongAxis();
        retval(0)= 0.0; retval(1)= sectionStrongAxis.x(); retval(2)= sectionStrongAxis.y();
      }
    else
      {
        std::cerr << getClassName() << __FUNCTION__
	          << "; section: "
                  << i << " doesn't returns principal axes." << std::endl;
      }
    return retval;
  }

//! @brief Returns i-th cross section weak axis direction vector expressed in local coordinates.
XC::Vector XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisLocalCoord(const size_t &i) const
  {
    Vector retval(3);
    retval(2)= 1;
    const PrismaticBarCrossSection *tmp= dynamic_cast<const PrismaticBarCrossSection *>(theSections[i]);
    if(tmp)
      {
        const Vector2d sectionWeakAxis= tmp->getVDirWeakAxis();
        retval(0)= 0.0; retval(1)= sectionWeakAxis.x(); retval(2)= sectionWeakAxis.y();
      }
    else
      {
        std::cerr << getClassName() << __FUNCTION__
	          << "; section: "
                  << i << " doesn't returns principal axes." << std::endl;
        return retval;
      }
    return retval;
  }

//! @brief Returns the angle between i-th cross section strong axis
//! and the local XZ plane.
double XC::BeamColumnWithSectionFDTrf3d::getStrongAxisAngle(const size_t &i) const
  {
    Vector eF= getVDirStrongAxisLocalCoord(i);
    return atan2(eF(2),eF(1));
  }

//! @brief Returns the angle between i-th cross section weak axis
//! and the local XZ plane.
double XC::BeamColumnWithSectionFDTrf3d::getWeakAxisAngle(const size_t &i) const
  {
    Vector eD= getVDirWeakAxisLocalCoord(i);
    return atan2(eD(2),eD(1));
  }

//! @brief Returns i-th cross section strong axis direction vector expressed in global coordinates.
const XC::Vector &XC::BeamColumnWithSectionFDTrf3d::getVDirStrongAxisGlobalCoord(const size_t &i, bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; not implemented for deformed geometry." << std::endl;
    const CrdTransf *ct= checkCoordTransf();
    if(ct)
      {
        const Vector eF= getVDirStrongAxisLocalCoord(i);
        return theCoordTransf->getVectorGlobalCoordFromLocal(eF);
      }
    else
      {
        static Vector tmp(3);
        return tmp;
      }
  }

//! @brief Returns i-th cross section weak axis direction vector expressed in global coordinates.
const XC::Vector &XC::BeamColumnWithSectionFDTrf3d::getVDirWeakAxisGlobalCoord(const size_t &i, bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; not implemented for deformed geometry." << std::endl;
    const CrdTransf *ct= checkCoordTransf();
    if(ct)
      {
        const Vector eD= getVDirWeakAxisLocalCoord(i);
        return theCoordTransf->getVectorGlobalCoordFromLocal(eD);
      }
    else
      {
        static Vector tmp(3);
        return tmp;
      }
  }

//! @brief Send members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFDTrf3d::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFD::sendData(cp);
    res+= sendCoordTransf(10,11,12,cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFDTrf3d::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFD::recvData(cp);
    theCoordTransf= recvCoordTransf3d(10,11,12,cp);
    return res;
  }

