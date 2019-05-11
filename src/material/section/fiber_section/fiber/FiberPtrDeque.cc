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
//FiberPtrDeque.cc

#include "FiberPtrDeque.h"
#include "utility/matrix/Vector.h"
#include "material/section/CrossSectionKR.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3dBase.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"
#include "material/section/repres/geom_section/GeomSection.h"
#include "material/uniaxial/UniaxialMaterial.h"

#include <domain/mesh/element/utils/Information.h>
#include "xc_utils/src/geom/sis_ref/Ref3d3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/BND2d.h"
#include "xc_utils/src/geom/d1/Line2d.h"
#include "xc_utils/src/geom/d1/Segment2d.h"
#include "xc_utils/src/geom/d2/Circle2d.h"
#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"
#include "xc_utils/src/geom/d2/2d_polygons/polygon2d_bool_op.h"
#include "xc_utils/src/geom/d2/HalfPlane2d.h"
#include "xc_utils/src/geom/lists/utils_list_pos2d.h"
#include "material/section/interaction_diagram/DeformationPlane.h"
#include "utility/actor/actor/MovableDeque.h"


//! @brief Constructor.
XC::FiberPtrDeque::FiberPtrDeque(const size_t &num)
  : CommandEntity(), fiber_ptrs_dq(num,static_cast<Fiber *>(nullptr)), MovableObject(0), yCenterOfMass(0.0), zCenterOfMass(0.0)
  {}

//! @brief Copy constructor.
XC::FiberPtrDeque::FiberPtrDeque(const FiberPtrDeque &other)
  : CommandEntity(other), fiber_ptrs_dq(other), MovableObject(other), yCenterOfMass(other.yCenterOfMass), zCenterOfMass(other.zCenterOfMass)
  {}

//! @brief Assignment operator.
XC::FiberPtrDeque &XC::FiberPtrDeque::operator=(const FiberPtrDeque &other)
  {
    CommandEntity::operator=(other);
    fiber_ptrs_dq::operator=(other);
    MovableObject::operator=(other);
    yCenterOfMass= other.yCenterOfMass;
    zCenterOfMass= other.zCenterOfMass;
    return *this;
  }

//! @brief Adds the fiber to the container.
void XC::FiberPtrDeque::push_back(Fiber *f)
   { fiber_ptrs_dq::push_back(f); }


//! @brief Search for the fiber identified by the parameter.
const XC::Fiber *XC::FiberPtrDeque::findFiber(const int &tag) const
  {
    const Fiber *retval= nullptr;
    std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getTag() == tag)
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for the fiber identified by the parameter.
XC::Fiber *XC::FiberPtrDeque::findFiber(const int &tag)
  {
    Fiber *retval= nullptr;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getTag() == tag)
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Returns true if the pointer is in the container.
bool XC::FiberPtrDeque::in(const Fiber *ptr) const
  {
    bool retval= false;
    for(const_iterator i= begin();i!= end();i++)
      if(*i==ptr)
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Returns minimal y coordinate value of the fibers.
double XC::FiberPtrDeque::GetYMin(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        register std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getLocY(); i++;
        for(;i!= end();i++)
          if(*i)
            retval= std::min(retval,(*i)->getLocY());
          else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to fiber." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns minimal z coordinate value of the fibers.
double XC::FiberPtrDeque::GetZMin(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        register std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getLocZ(); i++;
        for(;i!= end();i++)
          if(*i)
            retval= std::min(retval,(*i)->getLocZ());
          else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to fiber." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns maximal y coordinate value of the fibers.
double XC::FiberPtrDeque::GetYMax(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        register std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getLocY(); i++;
        for(;i!= end();i++)
          if(*i)
            retval= std::max(retval,(*i)->getLocY());
          else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to fiber." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns maximal z coordinate value of the fibers.
double XC::FiberPtrDeque::GetZMax(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        register std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getLocZ(); i++;
        for(;i!= end();i++)
          if(*i)
            retval= std::max(retval,(*i)->getLocZ());
          else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to fiber." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns fibers positions.
GeomObj::list_Pos2d XC::FiberPtrDeque::getPositions(void) const
  {
    GeomObj::list_Pos2d retval;
    if(!empty())
      {
        for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
          if(*i)
            retval.push_back(Pos2d((*i)->getLocY(),(*i)->getLocZ()));
          else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to fiber." << std::endl;
      }
    return retval;
  }

//! @brief Return the upper right corner of the bounding rectangle.
Pos2d XC::FiberPtrDeque::GetPMax(void) const
  { return Pos2d(GetYMax(),GetZMax()); }

//! @brief Return the lower left corner of the bounding rectangle.
Pos2d XC::FiberPtrDeque::GetPMin(void) const
  { return Pos2d(GetYMin(),GetZMin()); }

//! @brief Return the bounding rectangle.
BND2d XC::FiberPtrDeque::Bnd(void) const
  { return BND2d(GetPMin(),GetPMax()); }

//! @brief Return the sum of the fibers areas multiplied by the factor.
double XC::FiberPtrDeque::getArea(const double &factor) const
  {
    double retval= 0.0;
    const size_t numFibers= getNumFibers();
    for(register size_t i=0;i<numFibers;i++)
      retval+= (*this)[i]->getArea();
    return factor*retval;
  }

//! @brief Return the moment of inertia with respect to an axis parallel
//! the z axis at a distance y0 from the origin.
double XC::FiberPtrDeque::getIz(const double &factor,const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        retval+= (*i)->getArea()*sqr((*i)->getLocY()-y0);
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Return the moment of inertia with respect to an axis parallel
//! the y axis at a distance z0 from the origin.
double XC::FiberPtrDeque::getIy(const double &factor,const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        retval+= (*i)->getArea()*sqr((*i)->getLocZ()-z0);
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Return the product of inertia with respect to the parallel axes
//! at distances (y0,z0) from origin.
double XC::FiberPtrDeque::getPyz(const double &factor,const double &y0,const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        retval+= (*i)->getArea()*((*i)->getLocZ()-z0)*((*i)->getLocY()-y0);
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns homogenized section area.
double XC::FiberPtrDeque::getAreaHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;

    for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
      {
        const UniaxialMaterial *mat= (*i)->getMaterial();
        if(mat)
          retval+= (*i)->getArea()*(mat->getTangent()/E0);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; pointer to material nulo." << std::endl;
      }
    return retval;
  }

//! @brief Return the coordinates of the homogenized section centroid.
const XC::Vector &XC::FiberPtrDeque::getCenterOfMassHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << getClassName() << "::" << __FUNCTION__
	        << ";homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double Qy= 0.0,Qz= 0.0;
    double Atot= 0.0;
    double weightedFiberArea= 0.0;
    double yLoc= 0.0, zLoc= 0.0;

    for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
      {
        yLoc= (*i)->getLocY();
        zLoc= (*i)->getLocZ();
        const UniaxialMaterial *mat= (*i)->getMaterial();
        if(mat)
          {
            weightedFiberArea= (*i)->getArea()*(mat->getTangent()/E0);
            Atot+= weightedFiberArea;
            Qz+= -yLoc*weightedFiberArea; //Sign of y coordinate changed.
            Qy+= zLoc*weightedFiberArea;
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; null pointer to material." << std::endl;
      }
    static Vector retval(2);
    retval[0]= -Qz/Atot; //center of mass y coordinate  XXX ¿Signo menos?
    retval[1]= Qy/Atot; //center of mass z coordinate 
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respect to the axis parallel to y passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::FiberPtrDeque::getIyHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    const Vector &center_of_mass= getCenterOfMassHomogenizedSection(E0);
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const UniaxialMaterial *mat= (*i)->getMaterial();
          if(mat)
            retval+= (*i)->getArea()*sqr((*i)->getLocZ()-center_of_mass[1])*(mat->getTangent()/E0);
          else
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to material." << std::endl;
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respect to the axis parallel to z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::FiberPtrDeque::getIzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    const Vector &center_of_mass= getCenterOfMassHomogenizedSection(E0);
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const UniaxialMaterial *mat= (*i)->getMaterial();
          if(mat)
            retval+= (*i)->getArea()*sqr((*i)->getLocY()-center_of_mass[0])*(mat->getTangent()/E0);
          else
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to material." << std::endl;
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Returns homogenized product of inertia of the cross-section with respect to the axis parallel to y and z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::FiberPtrDeque::getPyzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    const Vector &center_of_mass= getCenterOfMassHomogenizedSection(E0);
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const UniaxialMaterial *mat= (*i)->getMaterial();
          if(mat)
            retval+= (*i)->getArea()*((*i)->getLocZ()-center_of_mass[1])*((*i)->getLocY()-center_of_mass[0])*(mat->getTangent()/E0);
          else
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "null pointer to material." << std::endl;
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Return the i,j component of the tensor of inertia calculado with respect to the CENTER_OF_MASS.
double XC::FiberPtrDeque::getIHomogenizedSection(const double &E0,const unsigned short int &i,const unsigned short int &j) const
  {
    unsigned short int k= i + (j-1)*2;
    double retval= 0.0;
    switch(k)
      {
        case 1: //(1,1)
          retval= getIyHomogenizedSection(E0);
          break;
        case 2: //(2,1)
        case 3: //(1,2)
          retval= -getPyzHomogenizedSection(E0);
          break;
	case 4:
          retval= getIzHomogenizedSection(E0);
          break;
      }
    return retval;
  }

//! @brief Return the tensor of inertia computed with respect to the object centroid.
XC::Matrix &XC::FiberPtrDeque::getIHomogenizedSection(const double &E0) const
  {
    static Matrix i(2,2);
    i(0,0)= getIyHomogenizedSection(E0); i(0,1)= -getPyzHomogenizedSection(E0);
    i(1,0)= i(0,1);   i(1,1)= getIzHomogenizedSection(E0);
    return i;
  }

//! @brief Return the tensor of inertia with respect to the point o.
XC::Matrix &XC::FiberPtrDeque::getIHomogenizedSection(const double &E0,const Pos2d &o) const
  {
    static Matrix retval(2,2);
    const Matrix Ig= getIHomogenizedSection(E0);
    Vector O(2); O[0]= o.x(); O[1]= o.y();
    const Vector og= getCenterOfMassHomogenizedSection(E0) - O;
    const double m= getAreaHomogenizedSection(E0);
    retval= Ig+m*(og.Norm2()*identity(Ig)-(og & og));
    return retval;
  }

//! @brief Return the moment of inertia of the homogenized section
//! with respect to the axis parallel to vector e through the point O.
//! @param E0: reference elastic modulus.
//! @param O: point for axis definition.
//! @param v: vector for axis definition.
double XC::FiberPtrDeque::getIHomogenizedSection(const double &E0,const Pos2d &O,const Vector &e) const
  {
    const Matrix Io(getIHomogenizedSection(E0,O));
    return dot(e,Io*e)/e.Norm2();
  }

//! @brief Return the moment of inertia with respect to the line being passed
//! as parameter.
double XC::FiberPtrDeque::getIHomogenizedSection(const double &E0,const Line2d &r) const
  { return getIHomogenizedSection(E0,r.Point(),Vector(r.VDir())); }

//! @brief Return the static moment of the cell areas that rely
//! above yf (y_fiber-yf > 0) with respect to the axis parallel to z
//! at the y coordinate being passed as parameter (lever arm= y_fiber-y0).
double XC::FiberPtrDeque::getSzPos(const double &yf,const double &y0,const double &factor) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const double y_fiber= (*i)->getLocY();
          if(y_fiber>yf)
            {
              const double brazo= y_fiber-y0;
              retval+= (*i)->getArea()*brazo;
            }
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Return the static moment of the cell areas that rely
//! below yf (y_fiber-yf < 0) with respect to the axis parallel to z
//! at the y coordinate being passed as parameter (lever arm= y_fiber-y0).
double XC::FiberPtrDeque::getSzNeg(const double &yf,const double &y0,const double &factor) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const double y_fiber= (*i)->getLocY();
          if(y_fiber<yf)
            {
              const double brazo= y_fiber-y0;
              retval+= (*i)->getArea()*brazo;
            }
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Return the static moment of the cell areas that rely
//! above zf (z_fiber-zf > 0) with respect to the axis parallel to y
//! at the z coordinate being passed as parameter (lever arm= z_fiber-z0).
double XC::FiberPtrDeque::getSyPos(const double &zf,const double &z0,const double &factor) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const double z_fiber= (*i)->getLocZ();
          if(z_fiber>zf)
            retval+= (*i)->getArea()*(z_fiber-z0);
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Return the static moment of the cell areas that rely
//! below zf (z_fiber-zf < 0) with respect to the axis parallel to y
//! at the z coordinate being passed as parameter (lever arm= z_fiber-z0).
double XC::FiberPtrDeque::getSyNeg(const double &zf,const double &z0,const double &factor) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const double z_fiber= (*i)->getLocZ();
          if(z_fiber<zf)
            retval+= (*i)->getArea()*(z_fiber-z0);
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Return the static moments of the fiber areas inside the halfplane
//! being passed as parameter.
double XC::FiberPtrDeque::getSPosHomogenizedSection(const double &E0,const HalfPlane2d &sp) const
  {
    if(fabs(E0)<1e-6)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    double d= 0.0;
    for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
      {
        const UniaxialMaterial *mat= (*i)->getMaterial();
        if(mat)
          {
            d= -sp.DistSigno((*i)->getPos());
            if(d>0)
              retval+= (*i)->getArea()*(mat->getTangent()/E0)*d;
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; null pointer to material." << std::endl;
      }
    return retval;
  }


//! @brief Return the static moments of the fiber areas outside the halfplane
//! being passed as parameter.
double XC::FiberPtrDeque::getSNegHomogenizedSection(const double &E0,const HalfPlane2d &sp) const
  {
    if(fabs(E0)<1e-6)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    double d= 0.0;
    for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
      {
        const UniaxialMaterial *mat= (*i)->getMaterial();
        if(mat)
          {
            d= sp.DistSigno((*i)->getPos());
            if(d>0)
              retval+= (*i)->getArea()*(mat->getTangent()/E0)*d;
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; null pointer to material!."
		    << std::endl;
      }
    return retval;
  }

//! @brief Auxiliary function to to compute principal moments of inertia.
inline double R(const double &Iy,const double &Iz,const double Pyz)
  { return sqrt(sqr((Iy-Iz)/2)+sqr(Pyz)); }

//! @brief Return the major principal moment of inertia.
double XC::FiberPtrDeque::getI1(const double &factor,const double &y0,const double &z0) const
  {
    const double iy= getIy(factor,z0);
    const double iz= getIz(factor,y0);
    const double pyz= getPyz(factor,y0,z0);
    const double r= R(iy,iz,pyz);
    return (iy+iz)/2+r;
  }


//! @brief Return the moment of inertia principal menor.
double XC::FiberPtrDeque::getI2(const double &factor,const double &y0,const double &z0) const
  {
    const double iy= getIy(factor,z0);
    const double iz= getIz(factor,y0);
    const double suma= (iy+iz)/2;
    const double pyz= getPyz();
    return suma-R(iy,iz,pyz);
  }

//! @brief Return the angle of the principal axis of inertia.
inline double theta_p(const double &Iy,const double &Iz,const double &Pyz)
  { return (atan(-2*Pyz/(Iy-Iz)))/2.0; }

//! @brief Return the angle between major principal axis and the y axis.
double XC::FiberPtrDeque::getTh1(const double &y0,const double &z0) const
  {
    const double Iy= getIy(1.0,z0);
    const double Iz= getIz(1.0,y0);
    const double Pyz= getPyz(1.0,y0,z0);
    double th1= theta_p(Iy,Iz,Pyz);
    const double media= (Iy+Iz)/2;
    const double dif2= (Iy-Iz)/2;
    const double th1_por2= 2*th1;
    const double seno= sin(th1_por2);
    const double coseno= cos(th1_por2);
    double i1= media+dif2*coseno-Pyz*seno;
    double i2= Iy+Iz-i1;
    if(i1<i2)
      {
        std::swap(i1,i2);
        th1+= (M_PI/2.0);
      }
    return th1;
  }

//! @brief Return the direction of the major axis.
XC::Vector XC::FiberPtrDeque::getAxis1(const double &y0,const double &z0) const
  {
    Vector retval(2);
    const double th1= getTh1(y0,z0);
    retval[0]= cos(th1);
    retval[1]= sin(th1);
    return retval;
  }

//! @brief Return the direction of the minor axis.
XC::Vector XC::FiberPtrDeque::getAxis2(const double &y0,const double &z0) const
  {
    Vector retval(2);
    const double th1= getTh1(y0,z0);
    retval[0]= sin(th1);
    retval[1]= -cos(th1);
    return retval;
  }

//! @brief Return the resultant of stresses.
double XC::FiberPtrDeque::getResultant(void) const
  {
    register double retval= 0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      retval+= (*i)->getForce();
    return retval;
  }

//! @brief Return the moment of the fibers forces
//! with respect to the axis parallel to "z" that passes through (y0,0).
double XC::FiberPtrDeque::getMz(const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      retval+= (*i)->getMz(y0); 
    return retval;
  }

//! @brief Return the moment of the fibers forces
//! with respect to the axis parallel to "y" that passes through (0,z0).
double XC::FiberPtrDeque::getMy(const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      retval+= (*i)->getMy(z0); 
    return retval;
  }

//! @brief Return the eccentricity of Mz (see getMz).
double XC::FiberPtrDeque::getExcentricidadMz(const double &y0) const
  { return getMz(y0)/getResultant(); }

//! @brief Return the eccentricity of My (see getMy).
double XC::FiberPtrDeque::getExcentricidadMy(const double &z0) const
  { return getMy(z0)/getResultant(); }

//! @brief Return the moment vector of the fibers forces
//! with respect to the axis parallel to "y" and z that passes through (y0,z0).
Vector2d XC::FiberPtrDeque::getMomentVector(const double &y0,const double &z0) const
  { return Vector2d(getMy(y0),getMz(y0)); }

//! @brief Return the position of the fibers forces resultant.
Pos2d XC::FiberPtrDeque::getResultantPosition(const double &y0,const double &z0) const
  {
    const double R= getResultant();
    return Pos2d(getMz(y0)/R,getMy(z0)/R);
  }

//! @brief Return an approximation of the neutral axis.
Line2d XC::FiberPtrDeque::getNeutralAxis(void) const
  {
    const Segment2d las= getLeverArmSegment();
    const double C= getCompressionResultant();
    const Pos2d CPos= las.getToPoint(); //Compression centroid.
    const double T= getTensionResultant();
    const Pos2d TPos= las.getFromPoint(); //Tension centroid.
    const double a= las.getLength()*T/(T-C); //Similar triangles.
    const Pos2d org= TPos+a*las.VDir().Normalizado();
    const Vector2d v= las.Normal(); //Direction of the neutral axis.
    return Line2d(org,v);
  }

//! @brief Returns true if the section is subject to a under bending moment.
bool XC::FiberPtrDeque::isSubjectedToBending(const double &tol) const
  {
    if(std::abs(getMz())>=tol)
      return true;
    else if(std::abs(getMy())>=tol)
      return true;
    else
      return false;
  }

//! @brief Return the resultant of the compressions in the fibers.
double XC::FiberPtrDeque::getCompressionResultant(void) const
  {
    register double retval= 0;
    register double f= 0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          f= (*i)->getForce();
          if(f<0.0) retval+= f;
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Return the moment of the compressed fibers
//! with respect to the z axis.
double XC::FiberPtrDeque::getCompressedFibersMz(const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()<0.0) retval+= (*i)->getMz(y0);
    return retval;
  }

//! @brief Return the passing point of the stress resultant for stresses
//! that are lower than «ref» (zero by default; compressions), if there are
//! not such stresses it returns (0,0).
const XC::Vector &XC::FiberPtrDeque::getCompressedFibersCentroid(void) const
  {
    static Vector retval(2);
    static double f,r;
    retval[0]= 0.0; retval[1]= 0.0; f= 0.0; r= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        f= (*i)->getForce();
        if(f<0.0)
          {
            r+= f;
            retval[0]+= (*i)->getMz();
            retval[1]+= (*i)->getMy();
          }
      }
    if(r!= 0.0)
      retval/= r;
    else
      { retval[0]= NAN; retval[1]= NAN; }
    return retval;
  }

//! @brief Return the centroid of the fibers whose strain is less than
//! the value passed as parameter.
const XC::Vector &XC::FiberPtrDeque::getCentroidFibersWithStrainSmallerThan(const double &defRef) const
  {
    static Vector retval(2);
    static double def,r;
    retval[0]= 0.0; retval[1]= 0.0; def= 0.0; r= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        def=  (*i)->getStrain();
        if(def<defRef)
          {
            r+= (*i)->getForce();
            retval[0]+= (*i)->getMz();
            retval[1]+= (*i)->getMy();
          }
      }
    if(r!= 0.0)
      retval/= r;
    else
      { retval[0]= NAN; retval[1]= NAN; }
    return retval;
  }

//! @brief Return the moment of the compressed fibers
//! with respect to the y axis.
double XC::FiberPtrDeque::getCompressedFibersMy(const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()<0.0) retval+= (*i)->getMy(z0);
    return retval;
  }

//! @brief Return the resultant of the tensions in the fibers.
double XC::FiberPtrDeque::getTensionResultant(void) const
  {
    register double retval= 0;
    register double f= 0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          f= (*i)->getForce();
          if(f>0.0) retval+= f;
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Return the moment of the tensioned fibers
//! with respect to the z axis.
double XC::FiberPtrDeque::getTensionedFibersMz(const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()>0.0) retval+= (*i)->getMz(y0);
    return retval;
  }

//! @brief Return the moment of the tensioned fibers
//! with respect to the y axis.
double XC::FiberPtrDeque::getTensionedFibersMy(const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()>0.0) retval+= (*i)->getMy(z0);
    return retval;
  }

//! @brief Return the centroid of the tensioned fibers, if 
//! there is no tensioned fibers returns (0,0).
const XC::Vector &XC::FiberPtrDeque::getTensionedFibersCentroid(void) const
  {
    static Vector retval(2);
    static double f,r;
    retval[0]= 0.0; retval[1]= 0.0; f= 0.0; r= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        f= (*i)->getForce();
        if(f>0)
          {
            r+= f;
            retval[0]+= (*i)->getMz();
            retval[1]+= (*i)->getMy();
          }
      }
    if(r!= 0.0)
      retval/= r;
    else
      { retval[0]= NAN; retval[1]= NAN; }
    return retval;
  }

//! @brief Return the centroid of the fibers whose strain is greater than
//! the value being passed as parameter.
const XC::Vector &XC::FiberPtrDeque::getCentroidFibersWithStrainGreaterThan(const double &defRef) const
  {
    static Vector retval(2);
    static double def,r;
    retval[0]= 0.0; retval[1]= 0.0; def= 0.0; r= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        def=  (*i)->getStrain();
        if(def>defRef)
          {
            r+= (*i)->getForce();
            retval[0]+= (*i)->getMz();
            retval[1]+= (*i)->getMy();
          }
      }
    if(r!= 0.0)
      retval/= r;
    else
      { retval[0]= NAN; retval[1]= NAN; }
    return retval;
  }

//! @brief Return the fiber that is closest to the given position.
size_t XC::FiberPtrDeque::nearest_fiber(const double &y,const double &z) const
  {
    size_t retval= 0;
    const size_t nf= getNumFibers();
    if(nf<1)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; fiber container empty." << std::endl;
        return retval;
      }
    register size_t i= 0;
    Fiber *f= (*this)[i];
    double yf= 0.0;
    double zf= 0.0;
    if(f)
      f->getFiberLocation(yf,zf);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; null pointer to fiber." << std::endl;
    double d2= sqr(yf-y)+sqr(zf-z);
    i++;
    for(;i<nf;i++)
      if((*this)[i])
        {
          (*this)[i]->getFiberLocation(yf,zf);
          double d2i= sqr(yf-y)+sqr(zf-z);
          if(d2i<d2)
            {
              d2= d2i;
              retval=i;
            }
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! Return the subfiber set of this one which material tag is
//! being passed as parameter.
void XC::FiberPtrDeque::SelMatTag(const int &matTag,FiberPtrDeque &retval,bool clear)
  {
    if(clear) retval.clear();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          Material *mat= (*i)->getMaterial();
          if(mat)
            {
              const int tag= mat->getTag();
              if(tag == matTag)
                retval.push_back(*i);
            }
        }
      else
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
  }

//! @brief Return the min strain.
double XC::FiberPtrDeque::getStrainMin(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getMaterial()->getStrain();
        i++;
        for(;i!= end();i++)
          retval= std::min(retval,(*i)->getMaterial()->getStrain());
      }
    return retval;
  }

//! @brief Return the max strain.
double XC::FiberPtrDeque::getStrainMax(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getMaterial()->getStrain();
        i++;
        for(;i!= end();i++)
	  retval= std::max(retval,(*i)->getMaterial()->getStrain());
      }
    return retval;
  }

//! @brief Return the average strain.
double XC::FiberPtrDeque::getStrainMed(void) const
  {
    double retval= 0.0;
    double a= 0.0;
    if(!empty())
      {
        for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
          {
            retval+= (*i)->getArea()*(*i)->getMaterial()->getStrain();
            a+= (*i)->getArea();
          }
        retval/=a;
      }
    return retval;
  }

//! @brief Return the deformation plane (least squares adjustment).
XC::DeformationPlane XC::FiberPtrDeque::getDeformationPlane(void) const
  {
    GeomObj::list_Pos3d points;
    if(!empty())
      {
        for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
          if(*i)
            points.push_back(Pos3d((*i)->getMaterial()->getStrain(),(*i)->getLocY(),(*i)->getLocZ()));
          else
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; null pointer to fiber." << std::endl;
      }
    DeformationPlane retval;
    retval.linearLeastSquaresFitting(points);
    return retval;
  }

//! @brief Return the generalized strain vector.
const XC::Vector &XC::FiberPtrDeque::getDeformation(void) const
  {
    const DeformationPlane pDef= getDeformationPlane();
    return pDef.getDeformation();
  }

//! @brief Return the minimal stress.
double XC::FiberPtrDeque::getStressMin(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getMaterial()->getStress();
        i++;
        for(;i!= end();i++)
          retval= std::min(retval,(*i)->getMaterial()->getStress());
      }
    return retval;
  }

//! @brief Return the minimal stress.
double XC::FiberPtrDeque::getStressMax(void) const
  {
    double retval= 0.0;
    if(!empty())
      {
        std::deque<Fiber *>::const_iterator i= begin();
        retval= (*i)->getMaterial()->getStress();
        i++;
        for(;i!= end();i++)
          retval= std::max(retval,(*i)->getMaterial()->getStress());
      }
    return retval;
  }

//! @brief Return the average stress.
double XC::FiberPtrDeque::getStressMed(void) const
  {
    double retval= 0.0;
    double a= 0.0;
    if(!empty())
      {
        for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
          {
            retval+= (*i)->getArea()*(*i)->getMaterial()->getStress();
            a+= (*i)->getArea();
          }
        retval/=a;
      }
    return retval;
  }

XC::ClaseEsfuerzo XC::FiberPtrDeque::getClaseEsfuerzo(const double &tol) const
  {
    ClaseEsfuerzo retval= ERROR;
    const double epsMin= getStrainMin();
    const double epsMax= getStrainMax();
    if(epsMin>0) //Tension.
      {
        const double r= (epsMax-epsMin)/epsMax;
        if(r<tol)
          retval= SIMPLE_TENSION;
        else
          retval= COMPOSED_TENSION;
      }
    else if(epsMax>0) //Bending.
      {
        const double R= getResultant()/epsMax;
        if(fabs(R)>tol)
          retval= FLEXION_COMPUESTA;
        else
          retval= FLEXION_SIMPLE;
      }
    else
      {
        const double r= (epsMin-epsMax)/epsMin;
        if(r<tol)
          retval= SIMPLE_COMPRESSION;
        else
          retval= COMPOSED_COMPRESSION;
      }
    return retval;
  }

//! @brief Returns true if all the fibers are tensioned.
bool XC::FiberPtrDeque::isTensioned(void) const
  { return ((getStrainMin()>=0) && (getStrainMax()>0)); }

//! @brief Returns true if some fibers are tensioned and other are compressed (bending with or without axial force).
bool XC::FiberPtrDeque::isBent(void) const
  { return ((getStrainMin()<0) && (getStrainMax()>0)); }

//! @brief Returns true if all the fibers are compressed.
bool XC::FiberPtrDeque::isCompressed(void) const
  { return ((getStrainMin()<0) && (getStrainMax()<=0)); }

//! @brief Return a string identifying the internal stress state.
std::string XC::FiberPtrDeque::getStrClaseEsfuerzo(const double &tol) const
  {
    std::string retval= "error";
    const ClaseEsfuerzo clase= getClaseEsfuerzo(tol);
    switch(clase)
      {
      case SIMPLE_TENSION:
	retval= "simple_tension";
        break;
      case COMPOSED_TENSION:
	retval= "composed_tension";
        break;
      case FLEXION_SIMPLE:
	retval= "flexion_simple";
        break;
      case FLEXION_COMPUESTA:
	retval= "flexion_compuesta";
        break;
      case SIMPLE_COMPRESSION:
	retval= "simple_compression";
        break;
      case COMPOSED_COMPRESSION:
	retval= "composed_compression";
        break;
      case ERROR:
	retval= "error";
        break;
      }
    return retval;
  }

//! @brief Return the position of the centroid.
Pos2d XC::FiberPtrDeque::getCenterOfMass(void) const
  { return Pos2d(getCenterOfMassY(),getCenterOfMassZ()); }

//! @brief Returns neutral axisr depth, i. e. distance from neutral axis to
//! the most compressed one.
//! The algorithm is based on the fact that get_dist_to_neutral_axis returns
//! negative distances for compressed fibers (negative strain).
double XC::FiberPtrDeque::getNeutralAxisDepth(const FiberSectionBase &Section) const
  {
    double retval= 0.0;
    if(empty())
      return retval;

    std::deque<Fiber *>::const_iterator i= begin();
    double yLoc= (*i)->getLocY();
    double zLoc= (*i)->getLocZ();
    retval= Section.get_dist_to_neutral_axis(yLoc,zLoc);
    if(!std::isnan(retval))
      {
        i++;
        double tmp;
        for(;i!= end();i++)
          {
            yLoc= (*i)->getLocY(); 
            zLoc= (*i)->getLocZ();
            tmp= Section.get_dist_to_neutral_axis(yLoc,zLoc);
            retval= std::min(retval,tmp);
          }
      }
    return retval;
  }

//! @brief Returns a vector oriented from the tension centroid
//ª to the compression centroid.
XC::Vector XC::FiberPtrDeque::getLeverArmVector(void) const
  {
    const Segment2d &bm= getLeverArmSegment();
    return Vector(bm.GetVector());
  }

//! @brief Returns a segment from the tension centroid to the compression
//! centroid.
Segment2d XC::FiberPtrDeque::getLeverArmSegment(void) const
  {
    Segment2d retval;
    retval.setExists(false);
    const double epsMin= getStrainMin();
    const double epsMax= getStrainMax();
    if((epsMin<0) && (epsMax>0))
      {
        const Vector &C= getCompressedFibersCentroid();
	if(!std::isnan(C[0]) && !std::isnan(C[1]))
	  {
            const Vector &T= getTensionedFibersCentroid();
	    if(!std::isnan(T[0]) && !std::isnan(T[1]))
	      retval= Segment2d(Pos2d(T[0],T[1]),Pos2d(C[0],C[1]));
	  }
      }
    else if(std::abs(epsMax-epsMin)>1e-6)
      {
        const double defRef= (epsMin+epsMax)/2.0;
        const Vector &C= getCentroidFibersWithStrainSmallerThan(defRef);
        const Vector &T= getCentroidFibersWithStrainGreaterThan(defRef);
        retval= Segment2d(Pos2d(T[0],T[1]),Pos2d(C[0],C[1]));
      }
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Return the intercept of the bending plane with
//! the plane that contains the section.
Line2d XC::FiberPtrDeque::getBendingPlaneTrace(void) const
  { return getLeverArmSegment().getSupportLine(); }

//! @brief Return the intercept of a plane perpendicular to the
//! bending plane through the tensions centroid with the plane
//! that contains the section.
Line2d XC::FiberPtrDeque::getTensionedPlaneTrace(void) const
  { 
    const Line2d bendingTrace= getBendingPlaneTrace();
    Pos2d pt(getCenterOfMassY(),getCenterOfMassZ());
    const double epsMax= getStrainMax();
    if(epsMax>0) //There are tractions.
      {
        const Vector &T= getTensionedFibersCentroid();
        pt= Pos2d(T[0],T[1]);
      }
    return bendingTrace.Perpendicular(pt);
  }

//! @brief Return the intercept of a plane perpendicular to the
//! bending plane through the compressions centroid with the plane
//! that contains the section.
Line2d XC::FiberPtrDeque::getCompressedPlaneTrace(void) const
  { 
    const Line2d bendingTrace= getBendingPlaneTrace();
    Pos2d pt(getCenterOfMassY(),getCenterOfMassZ());
    const double epsMin= getStrainMin();
    if(epsMin<0) //There are compressions.
      {
        const Vector &C= getCompressedFibersCentroid();
        pt= Pos2d(C[0],C[1]);
      }
    return bendingTrace.Perpendicular(pt);
  }

//! @brief Return the lever arm of forces in the section.
double XC::FiberPtrDeque::getMechanicLeverArm(void) const
  { return getLeverArmVector().Norm(); }

//! @brief Return the average distance between fibers.
double XC::FiberPtrDeque::getAverageDistanceBetweenFibers(void) const
  {
    double retval= 0;
    const size_t sz= seps.size();
    if(sz>0)
      {
        for(size_t i= 0;i<sz;i++)
          retval+= seps[i];
        retval/=sz;
      }
    return retval;
  }

//! @brief Computes crack effective areas on the fibers that represent reinforcing bars
//! (see article 49.2.4 from EHE-08 (hatched area in figure 49.2.4b).
//! See also figures 47.5 y 47.6 from volume II of the book
//! "Proyecto y cálculo de estructuras de hormigón" José Calavera.
//! Instead to consider an square shape we consider a dodecagone with the
//! same area. I think it's less anisotropic.
//! @param grossEffectiveConcreteAreaContour: Contour of the gross effective area obtained from the section.
//! @param factor: factor that multiplies rebar diameter to obtain the square prescribed by the standard (i.e. for EHE-08 factor= 15).
double XC::FiberPtrDeque::computeFibersEffectiveConcreteArea(const std::list<Polygon2d> &grossEffectiveConcreteAreaContour,const double &factor) const
  {
    double retval= 0.0;
    const size_t n= 12;
    double dm,L,R;
    const size_t sz= size();
    dq_ac_effective.clear();
    dq_ac_effective.resize(sz);
    Polygon2d tmp;
    //Clip the rebars areas with the effective area contour.
    for(size_t i= 0;i<sz;i++) //For each rebar in the family.
      {
        dm= getEquivalentDiameterOfFiber(i);
        L= factor*dm; //Side of the square prescribed by the standard.
        R= L*sqrt(2/(n*sin(2*M_PI/n)));
        const Pos2d pos= (*this)[i]->getPos();
        tmp= Circle2d(pos,R).getInscribedPolygon(n);
        if(tmp.Overlap(grossEffectiveConcreteAreaContour))
          for(std::list<Polygon2d>::const_iterator j= grossEffectiveConcreteAreaContour.begin();j!=grossEffectiveConcreteAreaContour.end();j++)
            {
              if((*j).In(tmp))
                dq_ac_effective[i].push_back(tmp);
              else if((*j).Overlap(tmp))
                {
                  std::list<Polygon2d> tmpLst= intersection(tmp,*j);
                  assert(!tmpLst.empty());
                  dq_ac_effective[i].insert(dq_ac_effective[i].end(),tmpLst.begin(),tmpLst.end());
                }
            }
      }

    //Clip computed intersections.
    for(size_t i= 0;i<sz;i++)
      {
	std::list<Polygon2d> &p1= dq_ac_effective[i];
        for(size_t j= i+1;j<sz;j++)
          {
	    std::list<Polygon2d> &p2= dq_ac_effective[j];
            if(overlap(p1,p2))
              {
                const Pos2d c1= (*this)[i]->getPos();
                const Pos2d c2= (*this)[j]->getPos();
                particiona(c1,p1,c2,p2);
              }
          }
      }
    //Sum areas.
    for(size_t i= 0;i<sz;i++)
      {
        if(!dq_ac_effective[i].empty())
          retval+= area(dq_ac_effective[i].begin(),dq_ac_effective[i].end());
      }
    const double area_contour= area(grossEffectiveConcreteAreaContour.begin(),grossEffectiveConcreteAreaContour.end());
    if(retval>1.01*area_contour)
      std::cerr << "Effective area: " << retval
                << " is greater than the theoretical maximum: "
		<< area_contour << std::endl;
    return retval;
  }

//! @brief Return the contours of the effective area of the fiber which index
//! is being passed as parameter.
const std::list<Polygon2d> &XC::FiberPtrDeque::getFiberEffectiveConcretAreaContour(const size_t &i) const
  { return dq_ac_effective[i]; }

//! @brief Return the effective area value of the fiber which index
//! is being passed as parameter.
double XC::FiberPtrDeque::getFiberEffectiveConcreteArea(const size_t &i) const
  {
    if(dq_ac_effective.size()!=size())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; effective areas are not computed yet."
                << std::endl;
    return area(dq_ac_effective[i].begin(),dq_ac_effective[i].end());
  }

//! @brief Return the sum of fibers effective areas.
double XC::FiberPtrDeque::getFibersEffectiveConcreteArea(void) const
  {
    const size_t sz= dq_ac_effective.size();
    if(sz!=size())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; effective areas are not computed yet."
                << std::endl;
    double retval= 0.0;
    for(size_t i= 0;i<sz;i++)
      retval+= area(dq_ac_effective[i].begin(),dq_ac_effective[i].end());
    return retval;
  }

//! @brief Computes the cover of the fibers.
void XC::FiberPtrDeque::computeCovers(const GeomSection &g) const
  {
    const GeomObj::list_Pos2d positions= getPositions();
    const Polygon2d contour= g.getRegionsContour();
    recubs= getRecubrimientos(positions,contour);
    const size_t sz= recubs.size();
    for(size_t i= 0;i<sz;i++)
      if(recubs[i]<0)
        std::clog << "Warning! position: " << positions[i]
                  << " is outside the section." << std::endl;
  }

//! @brief Computes the distance from each fiber to the nearest one.
void XC::FiberPtrDeque::computeSpacement(void) const
  { seps= getPositions().GetSeparaciones(); }

//! @brief Return the value of the concrete cover for the i-th fiber.
const double &XC::FiberPtrDeque::getFiberCover(const size_t &i) const
  {
    if(recubs.size()!=size())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; cover not computed."
                << std::endl;
    return recubs[i];
  }

//! @brief Return the spacing of the i-th fiber.
const double &XC::FiberPtrDeque::getFiberSpacing(const size_t &i) const
  {
    if(seps.size()!=size())
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; spacing not computed."
                << std::endl;
    return seps[i];
  }

//! @brief Return the diameter of the circle that has the same area
//! of the fiber which index is being passed as parameter.
double XC::FiberPtrDeque::getEquivalentDiameterOfFiber(const size_t &i) const
  { return 2.0*sqrt((*this)[i]->getArea()/M_PI); }



//! @brief Return the stress on the i-th fiber when cracking occurs
//! in its effective area.
double XC::FiberPtrDeque::getSigmaSRAtFiber(const size_t &i,const double &Ec,const double &Es,const double &fctm) const
  {
    double retval= 0.0;
    const double Ag= getFiberEffectiveConcreteArea(i);
    if(Ag>0.0)
      {
        const double As= (*this)[i]->getArea();
        const double ro= As/Ag;
        const double n= Es/Ec;
        retval= (1-ro+n*ro)*Ag*fctm/As;
      }
    return retval;
  }

//! @brief Updates the centroid position.
int XC::FiberPtrDeque::updateCenterOfMass(void)
  {
    double Qy= 0.0,Qz= 0.0;
    double Atot= 0.0;
    double fiberArea= 0.0;
    double yLoc= 0.0, zLoc= 0.0;

    // Recompute centroid
    for(std::deque<Fiber *>::iterator i= begin();i!= end();i++)
      {
        yLoc= (*i)->getLocY();
        zLoc= (*i)->getLocZ();
        fiberArea= (*i)->getArea();
        Atot+= fiberArea;
        Qz+= -yLoc*fiberArea; //minus y coordinate.
        Qy+= zLoc*fiberArea;
      }
    yCenterOfMass= -Qz/Atot; //center of mass y coordinate  XXX ¿Signo menos?
    zCenterOfMass= Qy/Atot; //center of mass z coordinate 
    return 0;
  }

//! @brief Update the parameters center of mass, stiffness and resultant.
int XC::FiberPtrDeque::updateKRCenterOfMass(FiberSection2d &Section2d,CrossSectionKR &kr2)
  {
    kr2.zero();
    double Qz= 0.0;
    double Atot= 0.0;//!< Total area of the fibers.
    double fiberArea= 0.0;//!< Area of a fiber.
    double yLoc= 0.0, zLoc= 0.0;

    double tangent,stress,fs0;
    // Recompute centroid
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        (*i)->getFiberLocation(yLoc, zLoc);
        fiberArea= (*i)->getArea();
        if(fiberArea!= 0.0)
          {
            Atot+= fiberArea;
            Qz+= -yLoc*fiberArea; //minus y coordinate.

            //Updating stiffness matrix.
            tangent= (*i)->getMaterial()->getTangent();
            kr2.updateK2d(fiberArea,yLoc,tangent);

            //Updating stress resultant.
            stress= (*i)->getMaterial()->getStress();
            fs0= stress * fiberArea;
            kr2.updateNMz(fs0,yLoc);
          }
      }
    yCenterOfMass= -Qz/Atot; //center or mass z coordinate 
    kr2.kData[2]= kr2.kData[1]; //Simetry.
    return 0;
  }

int XC::FiberPtrDeque::commitState(void)
  {
    int err= 0;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->commitState();
    return err;
  }

//! @brief Sets initial strains values.
int XC::FiberPtrDeque::setInitialSectionDeformation(const FiberSection2d &Section2d)
  {
    int retval= 0;
    UniaxialMaterial *theMat;
    double y; 
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        // determine material strain and set it
        retval+= theMat->setInitialStrain(Section2d.get_strain(y));
      }
    return retval;
  }

//! @brief Sets trial strains values.
int XC::FiberPtrDeque::setTrialSectionDeformation(const FiberSection2d &Section2d,CrossSectionKR &kr2)
  {
    int retval= 0;
    kr2.zero();
    UniaxialMaterial *theMat;
    double y,fiberArea,strain,tangent,stress, fs0; 
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        fiberArea= (*i)->getArea();
        if(fiberArea!=0.0)
          {
            // determine material strain and set it
            strain= Section2d.get_strain(y);
            retval+= theMat->setTrial(strain, stress, tangent);

            //Updating stiffness matrix.
            kr2.updateK2d(fiberArea,y,tangent);

            //Updating stress resultant.
            fs0= stress * fiberArea;
            kr2.updateNMz(fs0,y);
          }
      }
    kr2.kData[2]= kr2.kData[1]; //Simetry.
    return retval;
  }

//! @brief Return the fibers to its last committed state.
int XC::FiberPtrDeque::revertToLastCommit(FiberSection2d &Section2d,CrossSectionKR &kr2)
  {
    int err= 0;
    kr2.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToLastCommit();
    err+= updateKRCenterOfMass(Section2d,kr2);
    return err;
  }

//! @brief Return the fibers to its initial state.
int XC::FiberPtrDeque::revertToStart(FiberSection2d &Section2d,CrossSectionKR &kr2)
  {
    int err= 0;
    kr2.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToStart();
    err+= updateKRCenterOfMass(Section2d,kr2);
    return err;
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberPtrDeque::getInitialTangent(const FiberSection2d &Section2d) const
  {
    static double kInitial[4];
    kInitial[0]= 0.0; kInitial[1]= 0.0;
    kInitial[2]= 0.0; kInitial[3]= 0.0;
    static Matrix kInitialMatrix(kInitial, 2, 2);

    std::deque<Fiber *>::const_iterator i= begin();
    UniaxialMaterial *theMat= nullptr;
    double y,fiberArea,tangent; 
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        fiberArea= (*i)->getArea();

        if(fiberArea!=0.0)
          {
            tangent= theMat->getInitialTangent();
            CrossSectionKR::updateK2d(kInitial,fiberArea,y,tangent);
          }
      }

    kInitial[2]= kInitial[1]; //Simetry.
    return kInitialMatrix;
  }

const XC::Vector &XC::FiberPtrDeque::getStressResultantSensitivity(int gradNumber, bool conditional)
  {
    static XC::Vector ds(2);
    ds.Zero();
    double y, fiberArea, stressGradient;
    std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        y= (*i)->getLocY();
        fiberArea= (*i)->getArea();
        stressGradient= (*i)->getMaterial()->getStressSensitivity(gradNumber,true);
        stressGradient= stressGradient * fiberArea;
        ds(0)+= stressGradient;
        ds(1)+= stressGradient * y;
      }
    return ds;
  }

int XC::FiberPtrDeque::commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads)
  {
    double d0= defSens(0);
    double d1= defSens(1);
    std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        UniaxialMaterial *theMat= (*i)->getMaterial();
        double y= (*i)->getLocY();

        // determine material strain and set it
        const double strainSens= d0 + y*d1;
        theMat->commitSensitivity(strainSens,gradNumber,numGrads);
      }
    return 0;
  }

//! @brief Update the parameters center of mass, stiffness matrix and resultant.
int XC::FiberPtrDeque::updateKRCenterOfMass(FiberSection3d &Section3d,CrossSectionKR &kr3)
  {
    kr3.zero();
    double Qy= 0.0,Qz= 0.0;
    double Atot= 0.0;
    double fiberArea= 0.0;
    double yLoc= 0.0, zLoc= 0.0;

    // Recompute centroid
    double stress,tangent,fs0;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        yLoc= (*i)->getLocY();
        zLoc= (*i)->getLocZ();
        fiberArea= (*i)->getArea();
        if(fiberArea!=0.0)
          {
            Atot+= fiberArea;
            Qz+= -yLoc*fiberArea; //minus y coordinate.
            Qy+= zLoc*fiberArea;

            //Updating stiffness matrix.
            tangent= (*i)->getMaterial()->getTangent();

            kr3.updateK3d(fiberArea,yLoc,zLoc,tangent);

            //Updating stress resultant.
            stress= (*i)->getMaterial()->getStress();
            fs0= stress * fiberArea;
            kr3.updateNMzMy(fs0,yLoc,zLoc);
	  }
      }
    yCenterOfMass= -Qz/Atot; //center of mass y coordinate  XXX ¿Signo menos?
    zCenterOfMass= Qy/Atot; //center of mass z coordinate 
    kr3.kData[3]= kr3.kData[1]; //Stiffness matrix symmetry.
    kr3.kData[6]= kr3.kData[2];
    kr3.kData[7]= kr3.kData[5];
    return 0;
  }

//! @brief Set the initial strains.
int XC::FiberPtrDeque::setInitialSectionDeformation(const FiberSection3d &Section3d)
  {
    int retval= 0;
    UniaxialMaterial *theMat;
    double y,z; 
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        z= (*i)->getLocZ();
        // determine material strain and set it
        retval+= theMat->setInitialStrain(Section3d.get_strain(y,z));
      }
    return retval;
  }

//! @brief Set the trial strains.
int XC::FiberPtrDeque::setTrialSectionDeformation(FiberSection3d &Section3d,CrossSectionKR &kr3)
  {
    int retval= 0;
    kr3.zero();
    std::deque<Fiber *>::iterator i= begin();
    UniaxialMaterial *theMat;
    double y,z,fiberArea,tangent,stress, fs0; 
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        z= (*i)->getLocZ();

        // determine material strain and set it
        retval+= theMat->setTrial(Section3d.get_strain(y,z), stress, tangent);

        //Updating stiffness matrix.
        fiberArea= (*i)->getArea();
        if(fiberArea!=0.0)
          {
            kr3.updateK3d(fiberArea,y,z,tangent);

            //Updating stress resultant.
            fs0= stress * fiberArea;
            kr3.updateNMzMy(fs0,y,z);
          }
      }
    kr3.kData[3]= kr3.kData[1]; //Stiffness matrix symmetry.
    kr3.kData[6]= kr3.kData[2];
    kr3.kData[7]= kr3.kData[5];
    return retval;
  }

//! @brief Returns to the last committed state.
int XC::FiberPtrDeque::revertToLastCommit(FiberSection3d &Section3d,CrossSectionKR &kr3)
  {
    int err= 0;
    kr3.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToLastCommit(); // invoke revertToLastCommit on the material
    err+= updateKRCenterOfMass(Section3d,kr3);
    return err;
  }

//! @brief Returns to the initial state.
int XC::FiberPtrDeque::revertToStart(FiberSection3d &Section3d,CrossSectionKR &kr3)
  {
    // revert the fibers to start
    int err= 0;
    kr3.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToStart(); // invoke revertToStart on the material
    err+= updateKRCenterOfMass(Section3d,kr3);
    return err;
  }

//! @brief Return the tangent stiffness matrix inicial.
const XC::Matrix &XC::FiberPtrDeque::getInitialTangent(const FiberSection3d &Section3d) const
  {
    static double kInitialData[9];
    static XC::Matrix kInitial(kInitialData, 3, 3);

    kInitialData[0]= 0.0; kInitialData[1]= 0.0; kInitialData[2]= 0.0;
    kInitialData[3]= 0.0; kInitialData[4]= 0.0; kInitialData[5]= 0.0;
    kInitialData[6]= 0.0; kInitialData[7]= 0.0; kInitialData[8]= 0.0;

    UniaxialMaterial *theMat;
    double y,z,fiberArea,tangent; 
    std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        z= (*i)->getLocZ();
        fiberArea= (*i)->getArea();

        if(fiberArea!=0.0)
	  {
            tangent= theMat->getInitialTangent();
            CrossSectionKR::updateK3d(kInitialData,fiberArea,y,z,tangent);
          }
      }
    kInitialData[3]= kInitialData[1]; //Stiffness matrix symmetry.
    kInitialData[6]= kInitialData[2];
    kInitialData[7]= kInitialData[5];

    return kInitial;
  }

//! @brief Update the parameters center of mass, stiffness and resultant.
int XC::FiberPtrDeque::updateKRCenterOfMass(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
  {
    krGJ.zero();
    double Qy= 0.0,Qz= 0.0;
    double Atot= 0.0;
    double fiberArea= 0.0;
    double yLoc= 0.0, zLoc=0.0;

    // Recompute centroid
    double stress,tangent,fs0;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        yLoc= (*i)->getLocY();
        zLoc= (*i)->getLocZ();
        fiberArea= (*i)->getArea();
        if(fiberArea!=0.0)
          {
            Atot+= fiberArea;
            Qz+= -yLoc*fiberArea; //minus y coordinate.
            Qy+= zLoc*fiberArea;

            tangent= (*i)->getMaterial()->getTangent();

            //Updating stiffness matrix.
            krGJ.updateKGJ(fiberArea,yLoc,zLoc,tangent);

            //Updating stress resultant.
            stress= (*i)->getMaterial()->getStress();
            fs0= stress * fiberArea;
            krGJ.updateNMzMy(fs0,yLoc,zLoc);
          }
      }
    yCenterOfMass= -Qz/Atot; //center of mass y coordinate  XXX ¿Signo menos?
    zCenterOfMass= Qy/Atot; //center of mass z coordinate 
    krGJ.kData[4]= krGJ.kData[1]; //Stiffness matrix symmetry.
    krGJ.kData[8]= krGJ.kData[2];
    krGJ.kData[9]= krGJ.kData[6];
    krGJ.kData[15]= SectionGJ.getGJ(); //(3,3)->15 //The remaining six elements of krGJ.kData are zero.

    krGJ.rData[3]= SectionGJ.getGJ()*SectionGJ.getSectionDeformation()(3); //Torsion.
    return 0;
  }

//! @brief Set the initial strains.
int XC::FiberPtrDeque::setInitialSectionDeformation(const FiberSectionGJ &SectionGJ)
  {
    int retval= 0;
    UniaxialMaterial *theMat;
    double y,z; 
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        z= (*i)->getLocZ();
        // determine material strain and set it
        retval+= theMat->setInitialStrain(SectionGJ.get_strain(y,z));
      }
    return retval;
  }

//! @brief Sets generalized trial strains values.
int XC::FiberPtrDeque::setTrialSectionDeformation(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
  {
    int retval= 0;
    krGJ.zero();
    UniaxialMaterial *theMat;
    double y,z,fiberArea,tangent,stress, fs0; 
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        z= (*i)->getLocZ();
        fiberArea= (*i)->getArea();

        if(fiberArea!=0.0)
          {
            // determine material strain and set it
            retval= theMat->setTrial(SectionGJ.get_strain(y,z), stress, tangent);

            //Updating stiffness matrix.
            krGJ.updateKGJ(fiberArea,y,z,tangent);

            //Updating stress resultant.
            fs0= stress * fiberArea;
            krGJ.updateNMzMy(fs0,y,z);
	  }
      }
    krGJ.kData[4]= krGJ.kData[1]; //Stiffness matrix symmetry.
    krGJ.kData[8]= krGJ.kData[2];
    krGJ.kData[9]= krGJ.kData[6];
    krGJ.kData[15]= SectionGJ.getGJ(); //(3,3)->15 //The remaining six elements of krGJ.kData are zero.

    krGJ.rData[3]= SectionGJ.getGJ()*SectionGJ.getSectionDeformation()(3); //Torsion.
    return retval;
  }

//! @brief Returns to the last committed state.
int XC::FiberPtrDeque::revertToLastCommit(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
  {
    int err= 0;
    krGJ.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToLastCommit(); // invoke revertToLastCommit on the material
    err+= updateKRCenterOfMass(SectionGJ,krGJ);
    return err;
  }

//! @brief Returns to the initial state.
int XC::FiberPtrDeque::revertToStart(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
  {
    // revert the fibers to start
    int err= 0;
    krGJ.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToStart(); // invoke revertToStart on the material
    err+= updateKRCenterOfMass(SectionGJ,krGJ);
    return err;
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberPtrDeque::getInitialTangent(const FiberSectionGJ &SectionGJ) const
  {
    static double kInitialData[16];

    kInitialData[0]= 0.0; kInitialData[1]= 0.0; kInitialData[2]= 0.0; kInitialData[3]= 0.0;
    kInitialData[4]= 0.0; kInitialData[5]= 0.0; kInitialData[6]= 0.0; kInitialData[7]= 0.0;
    kInitialData[8]= 0.0; kInitialData[9]= 0.0; kInitialData[10]= 0.0; kInitialData[11]= 0.0;
    kInitialData[12]= 0.0; kInitialData[13]= 0.0; kInitialData[14]= 0.0; kInitialData[15]= 0.0;

    static XC::Matrix kInitial(kInitialData, 4, 4);
    UniaxialMaterial *theMat;
    double y,z,fiberArea,tangent; 
    std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      {
        theMat= (*i)->getMaterial();
        y= (*i)->getLocY();
        z= (*i)->getLocZ();
        fiberArea= (*i)->getArea();

        if(fiberArea!= 0.0)
          {
            tangent= theMat->getInitialTangent();
            //Updating stiffness matrix.
            CrossSectionKR::updateKGJ(kInitialData,fiberArea,y,z,tangent);
          }
      }
    kInitialData[4]= kInitialData[1]; //Simetry.
    kInitialData[8]= kInitialData[2]; //Simetry.
    kInitialData[9]= kInitialData[6]; //Simetry.

    kInitialData[15]= SectionGJ.GJ; //(3,3)->15 //The remaining six elements of krGJ.kData are zero.
    return kInitial;
  }

//! @brief Gets one of the response parameters of the section.
XC::Response *XC::FiberPtrDeque::setResponse(const std::vector<std::string> &argv, Information &sectInfo)
  {
    const size_t numFibers= getNumFibers();
    if(argv[0] == "fiber")
      {
        size_t key= numFibers;
        int passarg= 2;
        const int argc= argv.size();
        if(argv.size() <= 2)          // not enough data input
          return 0;
        if(argv.size() <= 3)
          {// fiber number was input directly
            key= atoi(argv[1]);
            if(key < numFibers && key >= 0)
              {
		std::vector<std::string> argv2(argv);
                argv2.erase(argv2.begin(),argv2.begin()+passarg);
                return (*this)[key]->getMaterial()->setResponse(argv2,sectInfo);
              }
            else
              return nullptr;
          }
        if(argc > 4)
          { // find fiber closest to coord. with mat tag
            int matTag= atoi(argv[3]);
            const double yCoord= atof(argv[1]);
            const double zCoord= atof(argv[2]);
            double closestDist2,ySearch, zSearch, dy, dz;
            double distance2;
            size_t j;
            // Find first fiber with specified material tag
            for(j= 0;j<numFibers;j++)
              {
                if(matTag == (*this)[j]->getMaterial()->getTag())
                  {
                    ySearch= -(*this)[j]->getLocY();
                    zSearch=  (*this)[j]->getLocZ();
                    dy= ySearch-yCoord;
                    dz= zSearch-zCoord;
                    closestDist2= dy*dy + dz*dz;
                    key= j;
                    break;
                  }
              }
            // Search the remaining fibers
            for(;j<numFibers;j++)
              {
                if(matTag == (*this)[j]->getMaterial()->getTag())
                  {
                    ySearch= -(*this)[j]->getLocY();
                    zSearch=  (*this)[j]->getLocZ();
                    dy= ySearch-yCoord;
                    dz= zSearch-zCoord;
                    distance2= dy*dy + dz*dz;
                    if(distance2 < closestDist2)
                      {
                        closestDist2= distance2;
                        key= j;
                      }
                  }
              }
            passarg= 4;
          }
        else
          {                  // fiber near-to coordinate specified
            const double yCoord= atof(argv[1]);
            const double zCoord= atof(argv[2]);
            double distance2;
            double ySearch= -(*this)[0]->getLocY();
            double zSearch=  (*this)[0]->getLocZ();
            double dy= ySearch-yCoord;
            double dz= zSearch-zCoord;
            double closestDist2= dy*dy + dz*dz;
            key= 0;
            for(size_t j= 1; j < numFibers; j++)
              {
                ySearch= -(*this)[j]->getLocY();
                zSearch=  (*this)[j]->getLocZ();
                dy= ySearch-yCoord;
                dz= zSearch-zCoord;
                distance2= dy*dy + dz*dz;
                if(distance2 < closestDist2)
                  {
                    closestDist2= distance2;
                    key= j;
                  }
              }
            passarg= 3;
          }
        if(key < numFibers)
          {
            std::vector<std::string> argv2(argv);
            argv2.erase(argv2.begin(),argv2.begin()+passarg);
            return (*this)[key]->getMaterial()->setResponse(argv2,sectInfo);
          }
        else
          return 0;
      }
    // otherwise response quantity is unknown for the XC::FiberSection class
    else
      return 0;
  }

// //! Return the max value of the property being passed as parameter over the fiber set.
// //! being passed as parameter.
// size_t XC::FiberPtrDeque::IMaxProp(const std::string &nmb_prop) const
//   {
//     size_t retval= 0;
//     double vmax= 0.0;
//     const size_t nf= getNumFibers();
//     if(nf<1)
//       {
//         std::cerr << getClassName() << "::" << __FUNCTION__
//                   << "; no fibers." << std::endl;
//         return retval;
//       }
//     register size_t i= 0;
//     (*this)[i]->interpreta(nmb_prop);
//     boost::any propertyValue= InterpreteRPN::Pila().Pop();
//     if(boost_any_is_number(propertyValue))
//       {
//         vmax= convert_to_double(propertyValue);
//         retval= i;
//       }
//     i++;
//     double tmp= vmax;
//     for(;i<nf;i++)
//       if((*this)[i])
//         {
//           (*this)[i]->interpreta(nmb_prop);
//           boost::any propertyValue= InterpreteRPN::Pila().Pop();
//           if(boost_any_is_number(propertyValue))
//             {
//               tmp= convert_to_double(propertyValue);
//               if(tmp>vmax)
//                 {
//                   retval= i;
//                   vmax= tmp;
//                 }
//             }
//         }
//       else
//         std::cerr << getClassName() << "::" << __FUNCTION__
//                   << "; null pointer to fiber." << std::endl;
//     return retval;
//   }

// //! Return the index of the fiber that presents the minimal value
// //! of the property which name is being passed as parameter.
// size_t XC::FiberPtrDeque::IMinProp(const std::string &nmb_prop) const
//   {
//     size_t retval= 0;
//     double vmin= 0.0;
//     const size_t nf= getNumFibers();
//     if(nf<1)
//       {
//         std::cerr << getClassName() << "::" << __FUNCTION__
//                   << "; no fibers." << std::endl;
//         return retval;
//       }
//     register size_t i= 0;
//     (*this)[i]->interpreta(nmb_prop);
//     boost::any propertyValue= InterpreteRPN::Pila().Pop();
//     if(boost_any_is_number(propertyValue))
//       {
//         vmin= convert_to_double(propertyValue);
//         retval= i;
//       }
//     i++;
//     double tmp= vmin;
//     for(;i<nf;i++)
//       if((*this)[i])
//         {
//           (*this)[i]->interpreta(nmb_prop);
//           propertyValue= InterpreteRPN::Pila().Pop();
//           if(boost_any_is_number(propertyValue))
//             {
//               tmp= convert_to_double(propertyValue);
//               if(tmp<vmin)
//                 {
//                   retval= i;
//                   vmin= tmp;
//                 }
//             }
//         }
//       else
//         std::cerr << getClassName() << "::" << __FUNCTION__
//                   << "; null pointer to fiber." << std::endl;
//     return retval;
//   }

//! @brief Return the identifier of the fiber with maximum value for the iCoo
//! coordinate with respect to the system being passed as parameter.
size_t XC::FiberPtrDeque::getFiberWithMaxCoord(const Ref3d3d &r,const size_t &iCoo) const
  {
    size_t retval= 0;
    const size_t nf= getNumFibers();
    if(nf<1)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; there is no fibers." << std::endl;
        return retval;
      }
    register size_t i= 0;
    retval= i;
    Pos3d p(0.0,(*this)[i]->getLocY(),(*this)[i]->getLocZ());
    double v_max= r.GetPosLocal(p)(iCoo);
    double v= v_max;
    i++;
    for(;i<nf;i++)
      {
        p= Pos3d(0.0,(*this)[i]->getLocY(),(*this)[i]->getLocZ());
        v= r.GetPosLocal(p)(iCoo);
        if(v>v_max)
          {
            v_max= v;
            retval= i;
          }
      }
    return retval;
  }

//! @brief Return the identifier of the fiber with minimum value for the iCoo
//! coordinate with respect to the system being passed as parameter.
size_t XC::FiberPtrDeque::getFiberWithMinCoord(const Ref3d3d &r,const size_t &iCoo) const
  {
    size_t retval= 0;
    const size_t nf= getNumFibers();
    if(nf<1)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; container is empty." << std::endl;
        return retval;
      }
    register size_t i= 0;
    retval= i;
    Pos3d p(0.0,(*this)[i]->getLocY(),(*this)[i]->getLocZ());
    double v_min= r.GetPosLocal(p)(iCoo);
    double v= v_min;
    i++;
    for(;i<nf;i++)
      {
        p= Pos3d(0.0,(*this)[i]->getLocY(),(*this)[i]->getLocZ());
        v= r.GetPosLocal(p)(iCoo);
        if(v<v_min)
          {
            v_min= v;
            retval= i;
          }
      }
    return retval;
  }

int XC::FiberPtrDeque::setParameter(const int &paramMatTag,const std::vector<std::string> &argv, Parameter &param)
  {
    int parameterID= -1;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      if(paramMatTag == (*i)->getMaterial()->getTag())
        parameterID= (*i)->getMaterial()->setParameter(argv, param);
    return parameterID;
  }

int XC::FiberPtrDeque::updateParameter(const int &paramMatTag,int parameterID, Information &info)
  {
    int ok= -1;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      if(paramMatTag == (*i)->getMaterial()->getTag())
        ok =(*i)->getMaterial()->updateParameter(parameterID, info);
    return ok;
  }

int XC::FiberPtrDeque::activateParameter(int passedParameterID)
  {
    std::deque<Fiber *>::iterator i= begin();
    if(passedParameterID == 0 )
      {
        // "Zero out" the identifier in all materials
        for(;i!= end();i++)
          (*i)->getMaterial()->activateParameter(passedParameterID);
      }
    else
      {
        // Extract section and material tags
        int activeSectionTag= (int)( floor((double)passedParameterID) / (100000) );
        passedParameterID -= activeSectionTag*100000;
        int activeMaterialTag= (int)( floor((double)passedParameterID) / (1000) );
        passedParameterID -= activeMaterialTag*1000;

        // Go down to the sections and set appropriate flags
        for(;i!= end();i++)
          if(activeMaterialTag == (*i)->getMaterial()->getTag())
            (*i)->getMaterial()->activateParameter(passedParameterID);
      }
    return 0;
  }

void XC::FiberPtrDeque::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of Fibers: " << getNumFibers() << std::endl;
    s << "\tCentroid: (" << -yCenterOfMass << ", " << zCenterOfMass << ')' << std::endl;
    std::deque<Fiber *>::const_iterator i= begin();
    if(flag == 2)
      for(;i!= end();i++)
        {
          s << -(*i)->getLocY() << " "  << (*i)->getLocZ() << " "  << (*i)->getArea() << " " ;
          s << (*i)->getMaterial()->getStress() << " "  << (*i)->getMaterial()->getStrain() << std::endl;
        }
    else if(flag == 1)
      for(;i!= end();i++)
        {
           s << "\nLocation (y, z)= (" << -(*i)->getLocY() << ", " << (*i)->getLocZ() << ")";
           s << "\nArea= " << (*i)->getArea() << std::endl;
           (*i)->getMaterial()->Print(s, flag);
        }
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::FiberPtrDeque::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(yCenterOfMass,zCenterOfMass,getDbTagData(),CommMetaData(0));
    res+= sendDeque(*this,cp,getDbTagData(),CommMetaData(1));
    std::clog << getClassName() << "::" << __FUNCTION__
	      << "; not fully implemented yet." << std::endl;
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::FiberPtrDeque::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(yCenterOfMass,zCenterOfMass,getDbTagData(),CommMetaData(0));
    res+= receiveDeque(*this,cp,getDbTagData(),CommMetaData(1),&FEM_ObjectBroker::getNewFiber);
    std::clog << getClassName() << "::" << __FUNCTION__
	      << "; not fully implemented yet." << std::endl;
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::FiberPtrDeque::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int res= sendData(cp);
    const int dataTag=getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to send ID"
	        << std::endl;
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::FiberPtrDeque::recvSelf(const CommParameters &cp)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }


