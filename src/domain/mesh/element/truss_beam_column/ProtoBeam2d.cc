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
//ProtoBeam2d.cc

#include "ProtoBeam2d.h"

#include "utility/actor/actor/MovableVector.h"
#include "material/section/elastic_section/BaseElasticSection2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "material/section/repres/CrossSectionProperties2d.h"

//! @brief Set values to section mass properties.
void XC::ProtoBeam2d::set_material(const Material *m)
  {
    if(m)
      {
        const BaseElasticSection2d *scc= dynamic_cast<const BaseElasticSection2d *>(m);
        if(scc)
          physicalProperties.set(0,scc->getCrossSectionProperties());
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; material type is not valid.\n";
          }
      }
    else
      if(verbosity>0)
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; pointer to material is null." << std::endl;
  }

//! @brief Default constructor.
XC::ProtoBeam2d::ProtoBeam2d(int tag,int class_tag,const Material *m)
  :Element1D(tag,class_tag,0,0), physicalProperties(1)
  { set_material(m); }

//! @brief Constructor.
XC::ProtoBeam2d::ProtoBeam2d(int tag, int class_tag, double a, double e, double i, int Nd1, int Nd2)
  :Element1D(tag,class_tag,Nd1,Nd2), physicalProperties(1)
  { setSectionProperties(CrossSectionProperties2d(e,a,i)); }

//! @brief Return the number of degrees of freedom of the element.
int XC::ProtoBeam2d::getNumDOF(void) const
  { return 6; }

//! @brief Return section properties.
const XC::CrossSectionProperties2d &XC::ProtoBeam2d::getSectionProperties(void) const
  { return (*physicalProperties[0]).getCrossSectionProperties(); }

//! @brief Return section properties.
XC::CrossSectionProperties2d &XC::ProtoBeam2d::getSectionProperties(void)
  { return (*physicalProperties[0]).getCrossSectionProperties(); }

//! @brief Set section properties.
void XC::ProtoBeam2d::setSectionProperties(const CrossSectionProperties2d &csp)
  {
    physicalProperties.set(0,csp);
  }

//! @brief Return density.
double XC::ProtoBeam2d::getRho(void) const
  { return getSectionProperties().getRho(); }

//! @brief Sets density.
void XC::ProtoBeam2d::setRho(const double &r)
  { getSectionProperties().setRho(r); }

//! @brief Return linear density.
double XC::ProtoBeam2d::getLinearRho(void) const
  { return getSectionProperties().getLinearRho(); }

//! @brief Send members through the communicator argument.
int XC::ProtoBeam2d::sendData(Communicator &comm)
  {
    DbTagData &dt= getDbTagData();
    int res= Element1D::sendData(comm);
    res+= comm.sendMovable(physicalProperties,dt,CommMetaData(7));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ProtoBeam2d::recvData(const Communicator &comm)
  {
    int res= Element1D::recvData(comm);
    res+= comm.receiveMovable(physicalProperties,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Returns the direction vector of element strong axis
//! expressed in the local coordinate system.
XC::Vector XC::ProtoBeam2d::getVDirStrongAxisLocalCoord(void) const
  {
    const Vector2d sectionStrongAxis= getSectionProperties().getVDirStrongAxis();
    Vector eF(3); eF(0)= 0.0; eF(1)= sectionStrongAxis.x(); eF(2)= sectionStrongAxis.y();
    return eF;
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the local coordinate system.
XC::Vector XC::ProtoBeam2d::getVDirWeakAxisLocalCoord(void) const
  {
    const Vector2d sectionWeakAxis= getSectionProperties().getVDirWeakAxis();
    Vector eD(3); eD(0)= 0.0; eD(1)= sectionWeakAxis.x(); eD(2)= sectionWeakAxis.y();
    return eD;
  }

//! @brief Compute the current strain.
const XC::Vector &XC::ProtoBeam2d::computeCurrentStrain(void) const
  {
    static Vector retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; not implemented yet.\n";
    return retval;
  }

//! @brief Set the element initial strain.
int XC::ProtoBeam2d::setInitialSectionDeformation(const Vector &def)
  {
    (*physicalProperties[0]).setInitialSectionDeformation(def);
    return 0;
  }

//! @brief Return the section generalized strain.
const XC::Vector &XC::ProtoBeam2d::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= computeCurrentStrain();
    const Vector &e0= getInitialSectionDeformation();
    // retval(0)= (dx2-dx1)/L: Element elongation/L.
    // retval(1)= (dy1-dy2)/L: Rotation about z/L.
    // retval(2)= (dy1-dy2)/L: Rotation about z/L.
    retval(0)-= e0(0);
    retval(1)-= e0(1);
    retval(2)-= e0(1);
    return retval;
  }

//! @brief Update element state.
int XC::ProtoBeam2d::update(void)
  {
    int retval= Element1D::update();
    // determine the current strain given trial displacements at nodes
    const Vector strain= this->computeCurrentStrain();
    retval+= (*physicalProperties[0]).setTrialSectionDeformation(strain);
    return retval;
  }

//! @brief Commit the element state.
int XC::ProtoBeam2d::commitState(void)
  {
    int retVal = Element1D::commitState();
    // call element commitState to do any base class stuff
    if(retVal != 0)
      { std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class."; }
    retVal+= physicalProperties.commitState();
    return retVal;
  }

//! @brief Revert the element to the its last commited state.
int XC::ProtoBeam2d::revertToLastCommit()
  {
    //int retval= Element1D::revertToLastCommit(); // pure virtual.
    int retval= physicalProperties.revertToLastCommit();
    return retval;
  }

//! @brief Revert the the element to the its start state.
int XC::ProtoBeam2d::revertToStart()
  {
    int retval= Element1D::revertToStart();
    retval+= physicalProperties.revertToStart();
    return retval;
  }

//! @brief Returns the angle between element strong axis
//! and local XZ plane.
double XC::ProtoBeam2d::getStrongAxisAngle(void) const
  {
    Vector eF= getVDirStrongAxisLocalCoord();
    return atan2(eF(2),eF(1));
  }

//! @brief Returns the angle between element weak axis
//! and local XZ plane.
double XC::ProtoBeam2d::getWeakAxisAngle(void) const
  {
    Vector eD= getVDirWeakAxisLocalCoord();
    return atan2(eD(2),eD(1));
  }

//! @brief Removes the element loads.
void XC::ProtoBeam2d::zeroLoad(void)
  {
    Element1D::zeroLoad();
    (*physicalProperties[0]).zeroInitialSectionDeformation(); //Removes also initial strains.
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void XC::ProtoBeam2d::createInertiaLoad(const Vector &accel)
  {
    const Vector load= -accel*getLinearRho();
    vector2dUniformLoadGlobal(load);
  }
