//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Xd3dCooSys.cc

#include "Xd3dCooSys.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Dir3d.h"
#include "../pos_vec/Pos3d.h"


//! @brief Defines a coordinate system of dimension i
//! the x axis will have the orientation and sense of the vector
//! argument.
//! @param i: dimension of the coordinate system.
//! @param v: direction of the x axis.
Xd3dCooSys::Xd3dCooSys(const size_t &i,const VGlobal &v)
  : CooSys(i,3)
  { XAxisVector(v); }

//! @brief Defines a coordinate system of dimension i
//! the x axis will have the orientation and sense of the v1
//! argument.
//! @param i: dimension of the coordinate system.
//! @param v1: direction of the x axis.
//! @param v2: direction of the y axis.
Xd3dCooSys::Xd3dCooSys(const size_t &i,const VGlobal &v1,const VGlobal &v2)
  : CooSys(i,3)
  { vectores_unitarios(v1,v2,v1 ^ v2); }

//! @brief Defines a coordinate system of dimension i.
//! @param i: dimension of the coordinate system.
Xd3dCooSys::Xd3dCooSys(const size_t &i,const PGlobal &o,const PGlobal &p)
  : CooSys(i,3)
  { TwoPoints(o,p); }

//! @brief Defines a coordinate system of dimension i.
//! @param i: dimension of the coordinate system.
Xd3dCooSys::Xd3dCooSys(const size_t &i,const PGlobal &p1,const PGlobal &p2, const PGlobal &p3)
  : CooSys(i,3)
  { ThreePoints(p1,p2,p3); }

//! @brief Defines a coordinate system of dimension i.
//! @param i: dimension of the coordinate system.
Xd3dCooSys::Xd3dCooSys(const size_t &i,const VGlobal &v1,const VGlobal &v2,const VGlobal &v3)
  : CooSys(i,3)
  { vectores_unitarios(v1,v2,v3); }

void Xd3dCooSys::putRow(const size_t &axis,const VGlobal &v)
  { CooSys::putRow(axis,traspuesta(v.getMatrix())); }

//! @brief Return the dirección of the axis being passed as parameter.
Xd3dCooSys::DGlobal Xd3dCooSys::getAxisDir(const size_t &axis) const
  { return DGlobal(getAxisVDir(1)); }

//! @brief Return the direction vector of the axis being passed as parameter.
Xd3dCooSys::VGlobal Xd3dCooSys::getAxisVDir(const size_t &axis) const
  { 
    const FT_matrix row= getRow(axis);
    return VGlobal(row(1,1),row(1,2),row(1,3));
  }
//! @brief Return the global coordinates of the vector.
//!
//! @param v: local coordinates of the vector.
Xd3dCooSys::VGlobal Xd3dCooSys::getGlobalCoordinates(const FT_matrix &v) const
  {
    const FT_matrix tmp= CooSys::getGlobalCoordinates(v);
    return VGlobal(tmp(1),tmp(2),tmp(3)); 
  }

//! @brief Return the local coordinates of the vector.
//!
//! @param v: global coordinates of the vector.
FT_matrix Xd3dCooSys::getLocalCoordinates(const Xd3dCooSys::VGlobal &v) const
  { return CooSys::getLocalCoordinates(v.getMatrix()); }

//! Makes the system of coordinates have the following unit vectors:
//! - The versor corresponding to vector i_, being passed as parameter.
//! - The versor corresponding to vector j_, being passed as parameter.
//! - The versor corresponding to vector k_, being passed as parameter.
//!
//! Controls that the three vectors are not coplanar nor parallel (2 by 2).
void Xd3dCooSys::vectores_unitarios(const VGlobal &i_,const VGlobal &j_,const VGlobal &k_)
  {
    if(coplanarios(i_,j_,k_))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; vectors: " 
                  << i_ << ' ' << j_ << ' ' << k_ 
                  << " are coplanar. No changes were made." << std::endl;
        return;
      }
    if(parallel(i_,j_))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vectors: i= " << i_ << " y j= " << j_
                  << " are parallel. No changes were made." << std::endl;
        return;
      }
    if(parallel(i_,k_))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vectors: i= " << i_ << " y k= " << k_
                  << " are parallel. No changes were made." << std::endl;
        return;
      }
    if(parallel(j_,k_))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vectors: j= " << j_ << " y k= " << k_
                  << " are parallel. No changes were made." << std::endl;
        return;
      }
    const size_t ne= numberOfAxis();
    const VGlobal e1= ::normalize(i_);
    putRow(1,e1);
    if(ne>1)
      {
        const VGlobal e2= ::normalize(j_);
        putRow(2,e2);
      }
    if(ne>2)
      {
        const VGlobal e3= ::normalize(k_);
        putRow(3,e3);
      }
  }

//! @brief Builds the coordinate system formed by the vectors:
//! i_ the vector being passed as parameter.
//! j_ the horizontal vector which is perpendicular to i_ (if it is unique).
//! k_ the cross producto of both vectors.
void Xd3dCooSys::XAxisVector(const VGlobal &i_)
  {
    if(i_.Nulo())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vector: " << i_ << " is zero."
	          << " System parallel to global axis will be returned."
                  << std::endl;
        identity();
      }
    else
      {
        const GEOM_FT imod= Abs2(i_); //Módulo de i.
        //const GEOM_FT tol= imod/1e8;
        VGlobal k_;
        //if( (fabs(i_(1))<tol) && (fabs(i_(2))<tol) ) //If i is almost parallel to global z axis.
        if(parallel(i_,Vector3d(0,0,1))) //If i is parallel to global z axis.
          k_= imod*J_3d; //k parallel to global y axis.
        else
          k_= imod*K_3d; //k parallel to global z axis.
        const VGlobal j_= k_ ^ i_;
        k_= i_ ^ j_;
        vectores_unitarios(i_,j_,k_); //Lets normalize.
      }
  }

//! @brief Build the coordinate system formed by the vectors:
//! i_ the op vector (see XAxisVector).
void Xd3dCooSys::TwoPoints(const PGlobal &o,const PGlobal &p)
  { XAxisVector(p-o); }

//! @brief Create the coordinate system with axis oriented as follows:
//! x axis: from o to p1;
//! y axis: normal to x and contained in the plane defined 
//! by the three points and pointing to p2.
void Xd3dCooSys::ThreePoints(const PGlobal &o,const PGlobal &p1,const PGlobal &p2)
  {
    if(colineales(o,p1,p2))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__ 
             << o << ' ' << p1 << ' ' << p2 
             << " are collinear. Doing nothing."
             << std::endl;
        return;
      }
    const VGlobal i_= p1 - o;
    VGlobal j_= p2 - o;
    const VGlobal k_= i_ ^ j_;
    j_= k_ ^ i_;
    vectores_unitarios(i_,j_,k_);
  }
