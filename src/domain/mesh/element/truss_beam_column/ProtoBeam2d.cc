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

//! @brief Set values to section mass properties.
void XC::ProtoBeam2d::set_material(const Material *m)
  {
    if(m)
      {
        const BaseElasticSection2d *scc= dynamic_cast<const BaseElasticSection2d *>(m);
        if(scc)
          ctes_scc= scc->getCrossSectionProperties();
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
  :Element1D(tag,class_tag,0,0)
  { set_material(m); }

//! @brief Constructor.
XC::ProtoBeam2d::ProtoBeam2d(int tag, int class_tag, double a, double e, double i, int Nd1, int Nd2)
  :Element1D(tag,class_tag,Nd1,Nd2), ctes_scc(e,a,i) {}

int XC::ProtoBeam2d::getNumDOF(void) const
  { return 6; }


//! @brief Send members through the communicator argument.
int XC::ProtoBeam2d::sendData(Communicator &comm)
  {
    DbTagData &dt= getDbTagData();
    int res= Element1D::sendData(comm);
    res+= comm.sendMovable(ctes_scc,dt,CommMetaData(7));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ProtoBeam2d::recvData(const Communicator &comm)
  {
    int res= Element1D::recvData(comm);
    res+= comm.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Returns the direction vector of element strong axis
//! expressed in the local coordinate system.
XC::Vector XC::ProtoBeam2d::getVDirStrongAxisLocalCoord(void) const
  {
    const Vector2d sectionStrongAxis= ctes_scc.getVDirStrongAxis();
    Vector eF(3); eF(0)= 0.0; eF(1)= sectionStrongAxis.x(); eF(2)= sectionStrongAxis.y();
    return eF;
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the local coordinate system.
XC::Vector XC::ProtoBeam2d::getVDirWeakAxisLocalCoord(void) const
  {
    const Vector2d sectionWeakAxis= ctes_scc.getVDirWeakAxis();
    Vector eD(3); eD(0)= 0.0; eD(1)= sectionWeakAxis.x(); eD(2)= sectionWeakAxis.y();
    return eD;
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

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void XC::ProtoBeam2d::createInertiaLoad(const Vector &accel)
  {
    const Vector load= -accel*getLinearRho();
    vector2dUniformLoadGlobal(load);
  }
