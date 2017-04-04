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
//NMyMzPointCloud.cc

#include "NMyMzPointCloud.h"
#include "NMPointCloud.h"


//! @brief Constructor.
XC::NMyMzPointCloud::NMyMzPointCloud(const double &u)
  : NMPointCloudBase(u), GeomObj::list_Pos3d()
  {
    lastInserted= nullptr;
  }

void XC::NMyMzPointCloud::clear(void)
  {
    GeomObj::list_Pos3d::clear();
    NMPointCloudBase::clear();
    lastInserted= nullptr;
  }

//! @brief Appends the cross-section normal stresses resultant
//! being passed as parameter.
const Pos3d *XC::NMyMzPointCloud::append(const Pos3d &NMyMz)
  {
    if(lastInserted)
      {
        if(dist(NMyMz,*lastInserted)>umbral)
          lastInserted= Agrega(NMyMz);
      }
    else
      lastInserted= Agrega(NMyMz);
    return lastInserted;  
  }

XC::NMPointCloud XC::NMyMzPointCloud::getNMy(void) const
  {
    NMPointCloud retval(umbral);
    for(const_iterator i= begin();i!=end();i++)
      {
        Pos3d p3d= *i;
        retval.push_back(Pos2d(p3d.x(),p3d.y()));
      }
    return retval;
  }

XC::NMPointCloud XC::NMyMzPointCloud::getNMz(void) const
  {
    NMPointCloud retval(umbral);
    for(const_iterator i= begin();i!=end();i++)
      {
        Pos3d p3d= *i;
        retval.push_back(Pos2d(p3d.x(),p3d.z()));
      }
    return retval;
  }

XC::NMPointCloud XC::NMyMzPointCloud::getNM(const double &theta) const
  {
    NMPointCloud retval(umbral);
    const double PImed= M_PI/2.0; 
    if(theta==PImed)
      retval= getNMy();
    else if(theta==0.0)
      retval= getNMz();
    else
      {
        const double c= cos(theta);
	const double s= sin(theta);
        double M= 0.0;
        for(const_iterator i= begin();i!=end();i++)
          {
            Pos3d p3d= *i;
            M= p3d.y()*s+p3d.z()*c; //My*sin(theta)+Mz*cos(theta)
            retval.push_back(Pos2d(p3d.x(),M));
          }
      }
    return retval; 
  }
