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
//FiberDeque.cc

#include "FiberDeque.h"
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
#include <utility/matrix/Vector.h>
#include "xc_utils/src/geom/sis_ref/Ref3d3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/BND2d.h"
#include "xc_utils/src/geom/d1/Recta2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"
#include "xc_utils/src/geom/d2/Circulo2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/bool_op_poligono2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "xc_utils/src/geom/listas/utils_list_pos2d.h"
#include "material/section/interaction_diagram/DeformationPlane.h"


//! @brief Constructor.
XC::FiberDeque::FiberDeque(const size_t &num)
  : EntCmd(), fiber_ptrs_dq(num,static_cast<Fiber *>(nullptr)), yCDG(0.0), zCDG(0.0)
  {}

//! @brief Copy constructor.
XC::FiberDeque::FiberDeque(const FiberDeque &otro)
  : EntCmd(otro), fiber_ptrs_dq(otro), yCDG(otro.yCDG), zCDG(otro.zCDG)
  {}

//! @brief Assignment operator.
XC::FiberDeque &XC::FiberDeque::operator=(const FiberDeque &otro)
  {
    EntCmd::operator=(otro);
    fiber_ptrs_dq::operator=(otro);
    yCDG= otro.yCDG;
    zCDG= otro.zCDG;
    return *this;
  }

//! @brief Adds the fiber to the container.
XC::Fiber *XC::FiberDeque::inserta(const Fiber &f)
  {
    Fiber *retval= f.getCopy();
    push_back(retval);
    return retval;
  }

//! @brief Adds the fiber to the container.
void XC::FiberDeque::push_back(Fiber *f)
   { fiber_ptrs_dq::push_back(f); }


//! @brief Search for the fiber identified by the parameter.
const XC::Fiber *XC::FiberDeque::findFiber(const int &tag) const
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
XC::Fiber *XC::FiberDeque::findFiber(const int &tag)
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

//! @brief Returns minimal y coordinate value of the fibers.
double XC::FiberDeque::GetYMin(void) const
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
            std::cerr << "FiberDeque::GetYMin; Pointer to fiber is null." << std::endl;
      }
    else
      std::cerr << "FiberDeque::GetYMin; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns minimal z coordinate value of the fibers.
double XC::FiberDeque::GetZMin(void) const
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
            std::cerr << "FiberDeque::GetZMin; Pointer to fiber is null." << std::endl;
      }
    else
      std::cerr << "FiberDeque::GetZMin; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns maximal y coordinate value of the fibers.
double XC::FiberDeque::GetYMax(void) const
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
            std::cerr << "FiberDeque::GetYMax; Pointer to fiber is null." << std::endl;
      }
    else
      std::cerr << "FiberDeque::GetYMax; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns maximal z coordinate value of the fibers.
double XC::FiberDeque::GetZMax(void) const
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
            std::cerr << "FiberDeque::GetZMax; Pointer to fiber is null." << std::endl;
      }
    else
      std::cerr << "FiberDeque::GetZMax; fiber set is empty." << std::endl;
    return retval;
  }

//! @brief Returns fibers positions.
GeomObj::list_Pos2d XC::FiberDeque::getPosiciones(void) const
  {
    GeomObj::list_Pos2d retval;
    if(!empty())
      {
        for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
          if(*i)
            retval.push_back(Pos2d((*i)->getLocY(),(*i)->getLocZ()));
          else
            std::cerr << "FiberDeque::GetYMax; Pointer to fiber is null." << std::endl;
      }
    return retval;
  }

//! @brief Returns the upper right corner of the bounding rectangle.
Pos2d XC::FiberDeque::GetPMax(void) const
  { return Pos2d(GetYMax(),GetZMax()); }

//! @brief Returns the lower left corner of the bounding rectangle.
Pos2d XC::FiberDeque::GetPMin(void) const
  { return Pos2d(GetYMin(),GetZMin()); }

//! @brief Returns the bounding rectangle.
BND2d XC::FiberDeque::Bnd(void) const
  { return BND2d(GetPMin(),GetPMax()); }

//! @brief Returns the sum of the fibers areas.
double XC::FiberDeque::getSumaAreas(const double &factor) const
  {
    double retval= 0.0;
    const size_t numFibers= getNumFibers();
    for(register size_t i=0;i<numFibers;i++)
      retval+= (*this)[i]->getArea();
    return factor*retval;
  }

//! @brief Returns the moment of inertia respecto al paralelo al z a una
//! distancia y0 del origen.
double XC::FiberDeque::getIz(const double &factor,const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        retval+= (*i)->getArea()*sqr((*i)->getLocY()-y0);
      else
        std::cerr << "FiberDeque::getIz; Pointer to fiber is null." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns the moment of inertia respecto al eje paralelo al y a una
//! distancia z0 del origen.
double XC::FiberDeque::getIy(const double &factor,const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        retval+= (*i)->getArea()*sqr((*i)->getLocZ()-z0);
      else
        std::cerr << "FiberDeque::getIy; Pointer to fiber is null." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns the producto de inercia respecto a los ejes paralelos aa una
//! distancias (y0,z0) del origen.
double XC::FiberDeque::getPyz(const double &factor,const double &y0,const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        retval+= (*i)->getArea()*((*i)->getLocZ()-z0)*((*i)->getLocY()-y0);
      else
        std::cerr << "FiberDeque::getPyz; Pointer to fiber is null." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns homogenized section area.
double XC::FiberDeque::getAreaHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;

    for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
      {
        const UniaxialMaterial *mat= (*i)->getMaterial();
        if(mat)
          retval+= (*i)->getArea()*(mat->getTangent()/E0);
        else
          std::cerr << "FiberDeque::getIyHomogenizedSection; pointer to material nulo." << std::endl;
      }
    return retval;
  }

//! @brief Returns the coordinates of the homogenized section centroid.
const XC::Vector &XC::FiberDeque::getCdgHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << nombre_clase() << "::" << __FUNCTION__
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
          std::cerr << "FiberDeque::getIyHomogenizedSection; pointer to material nulo." << std::endl;
      }
    static Vector retval(2);
    retval[0]= -Qz/Atot; //Coordenada y del CDG  XXX ¿Signo menos?
    retval[1]= Qy/Atot; //Coordenada z del CDG 
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to y passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::FiberDeque::getIyHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    const Vector &cdg= getCdgHomogenizedSection(E0);
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const UniaxialMaterial *mat= (*i)->getMaterial();
          if(mat)
            retval+= (*i)->getArea()*sqr((*i)->getLocZ()-cdg[1])*(mat->getTangent()/E0);
          else
	    std::cerr << nombre_clase() << "::" << __FUNCTION__
		      << "; null pointer to material." << std::endl;
        }
      else
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::FiberDeque::getIzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << nombre_clase() << "::" << __FUNCTION__
		<< "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    const Vector &cdg= getCdgHomogenizedSection(E0);
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const UniaxialMaterial *mat= (*i)->getMaterial();
          if(mat)
            retval+= (*i)->getArea()*sqr((*i)->getLocY()-cdg[0])*(mat->getTangent()/E0);
          else
	    std::cerr << nombre_clase() << "::" << __FUNCTION__
		      << "; null pointer to material." << std::endl;
        }
      else
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Returns homogenized product of inertia of the cross-section with respecto to the axis parallel to y and z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::FiberDeque::getPyzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    const Vector &cdg= getCdgHomogenizedSection(E0);
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        {
          const UniaxialMaterial *mat= (*i)->getMaterial();
          if(mat)
            retval+= (*i)->getArea()*((*i)->getLocZ()-cdg[1])*((*i)->getLocY()-cdg[0])*(mat->getTangent()/E0);
          else
	    std::cerr << nombre_clase() << "::" << __FUNCTION__
		      << "null pointer to material." << std::endl;
        }
      else
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    return retval;
  }

//! @brief Returns the i,j component of the tensor of inertia calculado respecto al CDG.
double XC::FiberDeque::getIHomogenizedSection(const double &E0,const unsigned short int &i,const unsigned short int &j) const
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

//! @brief Returns the tensor of inertia computed with respect to the object centroid.
XC::Matrix &XC::FiberDeque::getIHomogenizedSection(const double &E0) const
  {
    static Matrix i(2,2);
    i(0,0)= getIyHomogenizedSection(E0); i(0,1)= -getPyzHomogenizedSection(E0);
    i(1,0)= i(0,1);   i(1,1)= getIzHomogenizedSection(E0);
    return i;
  }

//! @brief Returns the tensor of inertia respector al punto o.
XC::Matrix &XC::FiberDeque::getIHomogenizedSection(const double &E0,const Pos2d &o) const
  {
    static Matrix retval(2,2);
    const Matrix Ig= getIHomogenizedSection(E0);
    Vector O(2); O[0]= o.x(); O[1]= o.y();
    const Vector og= getCdgHomogenizedSection(E0) - O;
    const double m= getAreaHomogenizedSection(E0);
    retval= Ig+m*(og.Norm2()*identity(Ig)-(og & og));
    return retval;
  }

//! @brief Returns the moment of inertia of the homogenized section
//! with respect to the axis parallel to vector e through the point O.
//! @param E0: reference elastic modulus.
//! @param O: point for axis definition.
//! @param v: vector for axis definition.
double XC::FiberDeque::getIHomogenizedSection(const double &E0,const Pos2d &O,const Vector &e) const
  {
    const Matrix Io(getIHomogenizedSection(E0,O));
    return dot(e,Io*e)/e.Norm2();
  }

//! @brief Returns the moment of inertia with respect to the line being passed
//! as parameter.
double XC::FiberDeque::getIHomogenizedSection(const double &E0,const Recta2d &r) const
  { return getIHomogenizedSection(E0,r.Punto(),Vector(r.VDir())); }

//! @brief Returns the static moment of the cell areas that rely
//! above yf (y_fiber-yf > 0) with respect to the axis parallel to z
//! at the y coordinate being passed as parameter (lever arm= y_fiber-y0).
double XC::FiberDeque::getSzPos(const double &yf,const double &y0,const double &factor) const
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
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns the static moment of the cell areas that rely
//! below yf (y_fiber-yf < 0) with respect to the axis parallel to z
//! at the y coordinate being passed as parameter (lever arm= y_fiber-y0).
double XC::FiberDeque::getSzNeg(const double &yf,const double &y0,const double &factor) const
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
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns the static moment of the cell areas that rely
//! above zf (z_fiber-zf > 0) with respect to the axis parallel to y
//! at the z coordinate being passed as parameter (lever arm= z_fiber-z0).
double XC::FiberDeque::getSyPos(const double &zf,const double &z0,const double &factor) const
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
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns the static moment of the cell areas that rely
//! below zf (z_fiber-zf < 0) with respect to the axis parallel to y
//! at the z coordinate being passed as parameter (lever arm= z_fiber-z0).
double XC::FiberDeque::getSyNeg(const double &zf,const double &z0,const double &factor) const
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
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; null pointer to fiber." << std::endl;
    retval*= factor;
    return retval;
  }

//! @brief Returns the static moments of the fiber areas inside the halfplane
//! being passed as parameter.
double XC::FiberDeque::getSPosHomogenizedSection(const double &E0,const Semiplano2d &sp) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
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
          std::cerr << "FiberDeque::getSPosHomogenizedSection; pointer to material nulo." << std::endl;
      }
    return retval;
  }


//! @brief Returns the static moments of the fiber areas outside the halfplane
//! being passed as parameter.
double XC::FiberDeque::getSNegHomogenizedSection(const double &E0,const Semiplano2d &sp) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
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
          std::cerr << "FiberDeque::getSNegHomogenizedSection; pointer to material nulo." << std::endl;
      }
    return retval;
  }

//! @brief Funcion auxiliar para calcular los principal moments of inertia.
inline double R(const double &Iy,const double &Iz,const double Pyz)
  { return sqrt(sqr((Iy-Iz)/2)+sqr(Pyz)); }

//! @brief Returns the major principal moment of inertia.
double XC::FiberDeque::getI1(const double &factor,const double &y0,const double &z0) const
  {
    const double iy= getIy(factor,z0);
    const double iz= getIz(factor,y0);
    const double pyz= getPyz(factor,y0,z0);
    const double r= R(iy,iz,pyz);
    return (iy+iz)/2+r;
  }


//! @brief Returns the moment of inertia principal menor.
double XC::FiberDeque::getI2(const double &factor,const double &y0,const double &z0) const
  {
    const double iy= getIy(factor,z0);
    const double iz= getIz(factor,y0);
    const double suma= (iy+iz)/2;
    const double pyz= getPyz();
    return suma-R(iy,iz,pyz);
  }

//! @brief Returns the angle of the principal axis of inertia.
inline double theta_p(const double &Iy,const double &Iz,const double &Pyz)
  { return (atan(-2*Pyz/(Iy-Iz)))/2.0; }

//! @brief Returns the angle between major principal axis and the y axis.
double XC::FiberDeque::getTh1(const double &y0,const double &z0) const
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

//! @brief Returns the direction of the major axis.
XC::Vector XC::FiberDeque::getEje1(const double &y0,const double &z0) const
  {
    Vector retval(2);
    const double th1= getTh1(y0,z0);
    retval[0]= cos(th1);
    retval[1]= sin(th1);
    return retval;
  }

//! @brief Returns the direction of the minor axis.
XC::Vector XC::FiberDeque::getEje2(const double &y0,const double &z0) const
  {
    Vector retval(2);
    const double th1= getTh1(y0,z0);
    retval[0]= sin(th1);
    retval[1]= -cos(th1);
    return retval;
  }

//! @brief Returns the resultante de tensiones.
double XC::FiberDeque::Resultante(void) const
  {
    register double retval= 0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      retval+= (*i)->getForce();
    return retval;
  }

//! @brief Returns the moment of the fibers forces
//! with respect to the axis parallel to "z" that passes through (y0,0).
double XC::FiberDeque::getMz(const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      retval+= (*i)->getMz(y0); 
    return retval;
  }

//! @brief Returns the moment of the fibers forces
//! with respect to the axis parallel to "y" that passes through (0,z0).
double XC::FiberDeque::getMy(const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      retval+= (*i)->getMy(z0); 
    return retval;
  }

//! @brief Returns the eccentricity of Mz (see getMz).
double XC::FiberDeque::getExcentricidadMz(const double &y0) const
  { return getMz(y0)/Resultante(); }

//! @brief Returns the eccentricity of My (see getMy).
double XC::FiberDeque::getExcentricidadMy(const double &z0) const
  { return getMy(z0)/Resultante(); }

//! @brief Returns the moment vector of the fibers forces
//! with respect to the axis parallel to "y" and z that passes through (y0,z0).
Vector2d XC::FiberDeque::getMomentVector(const double &y0,const double &z0) const
  { return Vector2d(getMy(y0),getMz(y0)); }

//! @brief Returns the position of the fibers forces resultant.
Pos2d XC::FiberDeque::getPosResultante(const double &y0,const double &z0) const
  {
    const double R= Resultante();
    return Pos2d(getMz(y0)/R,getMy(z0)/R);
  }

//! @brief Returns the neutral axis.
Recta2d XC::FiberDeque::getNeutralAxis(void) const
  {
    const double R= Resultante();
    const double My= getMy();
    const double Mz= getMz();
    const Pos2d org(Mz/R,My/R);//Position of the resultant.
    const Vector2d v(My,Mz); //Direction of the neutral axis.
    return Recta2d(org,v);
  }

//! @brief Returns true if the section is subject to a under bending moment.
bool XC::FiberDeque::hayMomento(const double &tol) const
  {
    if(std::abs(getMz())>=tol)
      return true;
    else if(std::abs(getMy())>=tol)
      return true;
    else
      return false;
  }

//! @brief Returns the resultante de las compresiones en las fibers.
double XC::FiberDeque::ResultanteComp(void) const
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
        std::cerr << "FiberDeque::Resultante; Pointer to fiber is null." << std::endl;
    return retval;
  }

//! @brief Returns the moment of the compressed fibers
//! with respect to the z axis.
double XC::FiberDeque::getMzComp(const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()<0.0) retval+= (*i)->getMz(y0);
    return retval;
  }

//! @brief Returns the punto de paso de la resultante de tensiones menores
//! que «ref» (por defecto cero) compresiones, si no hay returns (0,0).
const XC::Vector &XC::FiberDeque::baricentroCompresiones(void) const
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

//! @brief Returns the centroid of the fibers whose strain is less than
//! the value passed as parameter.
const XC::Vector &XC::FiberDeque::baricentroDefMenores(const double &defRef) const
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

//! @brief Returns the moment of the compressed fibers
//! with respect to the y axis.
double XC::FiberDeque::getMyComp(const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()<0.0) retval+= (*i)->getMy(z0);
    return retval;
  }

//! @brief Returns the resultante de las tracciones en las fibers.
double XC::FiberDeque::ResultanteTracc(void) const
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
        std::cerr << "FiberDeque::Resultante; Pointer to fiber is null." << std::endl;
    return retval;
  }

//! @brief Returns the moment of the tensioned fibers
//! with respect to the z axis.
double XC::FiberDeque::getMzTracc(const double &y0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()>0.0) retval+= (*i)->getMz(y0);
    return retval;
  }

//! @brief Returns the moment of the tensioned fibers
//! with respect to the y axis.
double XC::FiberDeque::getMyTracc(const double &z0) const
  {
    double retval= 0.0;
    register std::deque<Fiber *>::const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->getForce()>0.0) retval+= (*i)->getMy(z0);
    return retval;
  }

//! @brief Returns the punto de paso de la resultante de tracciones, si
//! no hay tracciones returns (0,0).
const XC::Vector &XC::FiberDeque::baricentroTracciones(void) const
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

//! @brief Returns the centroid of the fibers whose strain is greater than
//! the value being passed as parameter.
const XC::Vector &XC::FiberDeque::baricentroDefMayores(const double &defRef) const
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

//! @brief Returns the fiber that is closest to the given position.
size_t XC::FiberDeque::nearest_fiber(const double &y,const double &z) const
  {
    size_t retval= 0;
    const size_t nf= getNumFibers();
    if(nf<1)
      {
        std::cerr << "FiberDeque::nearest_fiber; fiber container empty." << std::endl;
        return retval;
      }
    register size_t i= 0;
    Fiber *f= (*this)[i];
    double yf= 0.0;
    double zf= 0.0;
    if(f)
      f->getFiberLocation(yf,zf);
    else
      std::cerr << "FiberDeque::nearest_fiber; Pointer to fiber is null." << std::endl;
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
        std::cerr << "FiberDeque::nearest_fiber; Pointer to fiber is null." << std::endl;
    return retval;
  }

// //! Returns the fiber set that fulfill the condition
// //! being passed as parameter.
// void XC::FiberDeque::Cumplen(const std::string &cond,FiberDeque &retval,bool clear)
//   {
//     if(clear) retval.clear();
//     std::deque<Fiber *>::iterator i= begin();
//     for(;i!= end();i++)
//       if(*i)
//         {
//           if((*i)->verdadero(cond))
//             retval.push_back(*i);
//         }
//       else
//         std::cerr << "FiberDeque::Cumplen; Pointer to fiber is null." << std::endl;
//     retval.updateCDG();
//   }

//! Returns the subfiber set de éste cuyo material tiene por tag el que se
//! being passed as parameter.
void XC::FiberDeque::SelMatTag(const int &matTag,FiberDeque &retval,bool clear)
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
        std::cerr << "FiberDeque::SelMatTag; Pointer to fiber is null." << std::endl;
  }

//! @brief Returns the min strain.
double XC::FiberDeque::getStrainMin(void) const
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

//! @brief Returns the min strain.
double XC::FiberDeque::getStrainMax(void) const
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

//! @brief Returns the average strain.
double XC::FiberDeque::getStrainMed(void) const
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

//! @brief Returns the deformation plane (least squares adjustment).
XC::DeformationPlane XC::FiberDeque::getDeformationPlane(void) const
  {
    GeomObj::list_Pos3d puntos;
    if(!empty())
      {
        for(std::deque<Fiber *>::const_iterator i= begin();i!= end();i++)
          if(*i)
            puntos.push_back(Pos3d((*i)->getMaterial()->getStrain(),(*i)->getLocY(),(*i)->getLocZ()));
          else
            std::cerr << "FiberDeque::getDeformationPlane; Pointer to fiber is null." << std::endl;
      }
    DeformationPlane retval;
    retval.AjusteMinimosCuadrados(puntos);
    return retval;
  }

//! @brief Returns the generalized strain vector.
const XC::Vector &XC::FiberDeque::getDeformation(void) const
  {
    const DeformationPlane pDef= getDeformationPlane();
    return pDef.getDeformation();
  }

//! @brief Returns the minimal stress.
double XC::FiberDeque::getStressMin(void) const
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

//! @brief Returns the minimal stress.
double XC::FiberDeque::getStressMax(void) const
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

//! @brief Returns the average stress.
double XC::FiberDeque::getStressMed(void) const
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

XC::ClaseEsfuerzo XC::FiberDeque::getClaseEsfuerzo(const double &tol) const
  {
    ClaseEsfuerzo retval= ERROR;
    const double epsMin= getStrainMin();
    const double epsMax= getStrainMax();
    if(epsMin>0) //Tension.
      {
        const double r= (epsMax-epsMin)/epsMax;
        if(r<tol)
          retval= TRACCION_SIMPLE;
        else
          retval= TRACCION_COMPUESTA;
      }
    else if(epsMax>0) //Bending.
      {
        const double R= Resultante()/epsMax;
        if(fabs(R)>tol)
          retval= FLEXION_COMPUESTA;
        else
          retval= FLEXION_SIMPLE;
      }
    else
      {
        const double r= (epsMin-epsMax)/epsMin;
        if(r<tol)
          retval= COMPRESION_SIMPLE;
        else
          retval= COMPRESION_COMPUESTA;
      }
    return retval;
  }

//! @brief Returns true if all the fibers are tensioned.
bool XC::FiberDeque::enTraccion(void) const
  { return ((getStrainMin()>=0) && (getStrainMax()>0)); }

//! @brief Returns true if some fibers are tensioned and other are compressed (bending with or without axial force).
bool XC::FiberDeque::enFlexion(void) const
  { return ((getStrainMin()<0) && (getStrainMax()>0)); }

//! @brief Returns true if all the fibers are compressed.
bool XC::FiberDeque::enCompresion(void) const
  { return ((getStrainMin()<0) && (getStrainMax()<=0)); }

std::string XC::FiberDeque::getStrClaseEsfuerzo(const double &tol) const
  {
    std::string retval= "error";
    const ClaseEsfuerzo clase= getClaseEsfuerzo(tol);
    switch(clase)
      {
      case TRACCION_SIMPLE:
	retval= "traccion_simple";
        break;
      case TRACCION_COMPUESTA:
	retval= "traccion_compuesta";
        break;
      case FLEXION_SIMPLE:
	retval= "flexion_simple";
        break;
      case FLEXION_COMPUESTA:
	retval= "flexion_compuesta";
        break;
      case COMPRESION_SIMPLE:
	retval= "compresion_simple";
        break;
      case COMPRESION_COMPUESTA:
	retval= "compresion_compuesta";
        break;
      case ERROR:
	retval= "error";
        break;
      }
    return retval;
  }

//! @brief Returns the position of the centroid.
Pos2d XC::FiberDeque::getCdg(void) const
  { return Pos2d(getYCdg(),getZCdg()); }

//! @brief Returns neutral axisr depth, i. e. distance from neutral axis to
//! the most compressed one.
//! The algorithm is based on the fact that get_dist_to_neutral_axis returns
//! negative distances for compressed fibers (negative strain).
double XC::FiberDeque::getNeutralAxisDepth(const FiberSectionBase &Section) const
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

//! @brief Returns a vector orientado desde el centro de tracciones al de compresiones.
XC::Vector XC::FiberDeque::getVectorBrazoMecanico(void) const
  {
    const Segmento2d &bm= getSegmentoBrazoMecanico();
    return Vector(bm.GetVector());
  }

//! @brief Returns a segmento orientado desde el centro de tracciones al de compresiones.
Segmento2d XC::FiberDeque::getSegmentoBrazoMecanico(void) const
  {
    Segmento2d retval;
    const double epsMin= getStrainMin();
    const double epsMax= getStrainMax();
    if((epsMin<0) && (epsMax>0))
      {
        const Vector &C= baricentroCompresiones();
        const Vector &T= baricentroTracciones();
        retval= Segmento2d(Pos2d(T[0],T[1]),Pos2d(C[0],C[1]));
      }
    else if(std::abs(epsMax-epsMin)>1e-6)
      {
        const double defRef= (epsMin+epsMax)/2.0;
        const Vector &C= baricentroDefMenores(defRef);
        const Vector &T= baricentroDefMayores(defRef);
        retval= Segmento2d(Pos2d(T[0],T[1]),Pos2d(C[0],C[1]));
      }
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Returns the intercept of the bending plane with
//! the plane that contains the section.
Recta2d XC::FiberDeque::getTrazaPlanoFlexion(void) const
  { return getSegmentoBrazoMecanico().RectaSoporte(); }

//! @brief Returns the intercept of a plane perpendicular to the
//! bending plane through the tensions centroid with the plane
//! that contains the section.
Recta2d XC::FiberDeque::getTrazaPlanoTraccion(void) const
  { 
    const Recta2d trazaFlexion= getTrazaPlanoFlexion();
    Pos2d pt(getYCdg(),getZCdg());
    const double epsMax= getStrainMax();
    if(epsMax>0) //Thera are tractions.
      {
        const Vector &T= baricentroTracciones();
        pt= Pos2d(T[0],T[1]);
      }
    return trazaFlexion.Perpendicular(pt);
  }

//! @brief Returns the intercept of a plane perpendicular to the
//! bending plane through the compressions centroid with the plane
//! that contains the section.
Recta2d XC::FiberDeque::getTrazaPlanoCompresion(void) const
  { 
    const Recta2d trazaFlexion= getTrazaPlanoFlexion();
    Pos2d pt(getYCdg(),getZCdg());
    const double epsMin= getStrainMin();
    if(epsMin<0) //There are compresions.
      {
        const Vector &C= baricentroCompresiones();
        pt= Pos2d(C[0],C[1]);
      }
    return trazaFlexion.Perpendicular(pt);
  }

//! @brief Returns the lever arm of forces in the section.
double XC::FiberDeque::getBrazoMecanico(void) const
  { return getVectorBrazoMecanico().Norm(); }

//! @brief Returns the distancia media entre fibers.
double XC::FiberDeque::getAverageDistanceBetweenFibers(void) const
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
double XC::FiberDeque::computeFibersEffectiveConcreteArea(const std::list<Poligono2d> &grossEffectiveConcreteAreaContour,const double &factor) const
  {
    double retval= 0.0;
    const size_t n= 12;
    double dm,L,R;
    const size_t sz= size();
    dq_ac_effective.clear();
    dq_ac_effective.resize(sz);
    Poligono2d tmp;
    for(size_t i= 0;i<sz;i++)
      {
        dm= getEquivalentDiameterOfFiber(i);
        L= factor*dm; //Side of the square prescribed by the standard.
        R= L*sqrt(2/(n*sin(2*M_PI/n)));
        const Pos2d pos= (*this)[i]->getPos();
        tmp= Circulo2d(pos,R).getPoligonoInscrito(n);
        if(tmp.Overlap(grossEffectiveConcreteAreaContour))
          for(std::list<Poligono2d>::const_iterator j= grossEffectiveConcreteAreaContour.begin();j!=grossEffectiveConcreteAreaContour.end();j++)
            {
              if((*j).In(tmp))
                dq_ac_effective[i].push_back(tmp);
              else if((*j).Overlap(tmp))
                {
                  std::list<Poligono2d> tmpLst= interseccion(tmp,*j);
                  assert(!tmpLst.empty());
                  dq_ac_effective[i].insert(dq_ac_effective[i].end(),tmpLst.begin(),tmpLst.end());
                }
            }
      }

    //Clip intersections.
    for(size_t i= 0;i<sz;i++)
      {
	std::list<Poligono2d> &p1= dq_ac_effective[i];
        for(size_t j= i+1;j<sz;j++)
          {
	    std::list<Poligono2d> &p2= dq_ac_effective[j];
            if(overlap(p1,p2))
              {
                const Pos2d c1= (*this)[i]->getPos();
                const Pos2d c2= (*this)[j]->getPos();
                particiona(c1,p1,c2,p2);
              }
          }
      }
    for(size_t i= 0;i<sz;i++)
      {
        if(!dq_ac_effective[i].empty())
          retval+= area(dq_ac_effective[i].begin(),dq_ac_effective[i].end());
      }
    const double area_contour= area(grossEffectiveConcreteAreaContour.begin(),grossEffectiveConcreteAreaContour.end());
    if(retval>1.01*area_contour)
      std::cerr << "Effective area: " << retval
                << " is greater than the theoretical maximum: " << area_contour << std::endl;
    return retval;
  }

//! @brief Returns the contours of the effective area of the fiber which index
//! is being passed as parameter.
const std::list<Poligono2d> &XC::FiberDeque::getFiberEffectiveConcretAreaContour(const size_t &i) const
  { return dq_ac_effective[i]; }

//! @brief Returns the effective area value of the fiber which index
//! is being passed as parameter.
double XC::FiberDeque::getFiberEffectiveConcreteArea(const size_t &i) const
  {
    if(dq_ac_effective.size()!=size())
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; effective areas are not computed yet."
                << std::endl;
    return area(dq_ac_effective[i].begin(),dq_ac_effective[i].end());
  }

//! @brief Returns the sum of fibers effective areas.
double XC::FiberDeque::getFibersEffectiveConcreteArea(void) const
  {
    const size_t sz= dq_ac_effective.size();
    if(sz!=size())
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; effective areas are not computed yet."
                << std::endl;
    double retval= 0.0;
    for(size_t i= 0;i<sz;i++)
      retval+= area(dq_ac_effective[i].begin(),dq_ac_effective[i].end());
    return retval;
  }

//! @brief Computes the cover of the fibers.
void XC::FiberDeque::computeCovers(const GeomSection &g) const
  {
    const GeomObj::list_Pos2d posiciones= getPosiciones();
    const Poligono2d contour= g.getRegionsContour();
    recubs= getRecubrimientos(posiciones,contour);
    const size_t sz= recubs.size();
    for(size_t i= 0;i<sz;i++)
      if(recubs[i]<0)
        std::clog << "Warning! position: " << posiciones[i]
                  << " is outside the section." << std::endl;
  }

//! @brief Computes the distance from each fiber to the nearest one.
void XC::FiberDeque::computeSpacement(void) const
  { seps= getPosiciones().GetSeparaciones(); }

//! @brief Returns the value of concrete cover for the i-th fiber.
const double &XC::FiberDeque::getFiberCover(const size_t &i) const
  {
    if(recubs.size()!=size())
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; cover not computed."
                << std::endl;
    return recubs[i];
  }

//! @brief Returns the spacing of the i-th fiber.
const double &XC::FiberDeque::getFiberSpacing(const size_t &i) const
  {
    if(seps.size()!=size())
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; spacing not computed."
                << std::endl;
    return seps[i];
  }

//! @brief Returns the diameter of the circle that has the same area
//! of the fiber which index is being passed as parameter.
double XC::FiberDeque::getEquivalentDiameterOfFiber(const size_t &i) const
  { return 2.0*sqrt((*this)[i]->getArea()/M_PI); }



//! @brief Returns the stress on the i-th fiber when cracking occurs
//! in its effective area.
double XC::FiberDeque::getSigmaSRAtFiber(const size_t &i,const double &Ec,const double &Es,const double &fctm) const
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
int XC::FiberDeque::updateCDG(void)
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
        Qz+= -yLoc*fiberArea; //Coordenada y cambiada de signo.
        Qy+= zLoc*fiberArea;
      }
    yCDG= -Qz/Atot; //Coordenada y del CDG  XXX ¿Signo menos?
    zCDG= Qy/Atot; //Coordenada z del CDG 
    return 0;
  }

//! @brief Update the parameters CDG, stiffness and resultant.
int XC::FiberDeque::updateKRCDG(FiberSection2d &Section2d,CrossSectionKR &kr2)
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
            Qz+= -yLoc*fiberArea; //Coordenada y cambiada de signo.

            //Updating stiffness matrix.
            tangent= (*i)->getMaterial()->getTangent();
            kr2.updateK2d(fiberArea,yLoc,tangent);

            //Updating stress resultant.
            stress= (*i)->getMaterial()->getStress();
            fs0= stress * fiberArea;
            kr2.updateNMz(fs0,yLoc);
          }
      }
    yCDG= -Qz/Atot; //Coordenada y del CDG 
    kr2.kData[2]= kr2.kData[1]; //Simetría.
    return 0;
  }

//! @brief Adds a fiber XXX Enhance parameter updating.
XC::Fiber *XC::FiberDeque::addFiber(FiberSection2d &Section2d,Fiber &newFiber,CrossSectionKR &kr2)
  {
    Fiber *retval= inserta(newFiber);
    updateKRCDG(Section2d,kr2);
    return retval;
  }

int XC::FiberDeque::commitState(void)
  {
    int err= 0;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->commitState();
    return err;
  }

//! @brief Sets initial strains values.
int XC::FiberDeque::setInitialSectionDeformation(const FiberSection2d &Section2d)
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
int XC::FiberDeque::setTrialSectionDeformation(const FiberSection2d &Section2d,CrossSectionKR &kr2)
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
    kr2.kData[2]= kr2.kData[1]; //Simetría.
    return retval;
  }

//! @brief Returns the fibers to its last commited state.
int XC::FiberDeque::revertToLastCommit(FiberSection2d &Section2d,CrossSectionKR &kr2)
  {
    int err= 0;
    kr2.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToLastCommit();
    err+= updateKRCDG(Section2d,kr2);
    return err;
  }

//! @brief Returns the fibers to its initial state.
int XC::FiberDeque::revertToStart(FiberSection2d &Section2d,CrossSectionKR &kr2)
  {
    int err= 0;
    kr2.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToStart();
    err+= updateKRCDG(Section2d,kr2);
    return err;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberDeque::getInitialTangent(const FiberSection2d &Section2d) const
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

    kInitial[2]= kInitial[1]; //Simetría.
    return kInitialMatrix;
  }

const XC::Vector &XC::FiberDeque::getStressResultantSensitivity(int gradNumber, bool conditional)
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

int XC::FiberDeque::commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads)
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

//! @brief Update the parameters CDG, stiffness matrix and resultant.
int XC::FiberDeque::updateKRCDG(FiberSection3d &Section3d,CrossSectionKR &kr3)
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
            Qz+= -yLoc*fiberArea; //Coordenada y cambiada de signo.
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
    yCDG= -Qz/Atot; //Coordenada y del CDG  XXX ¿Signo menos?
    zCDG= Qy/Atot; //Coordenada z del CDG 
    kr3.kData[3]= kr3.kData[1]; //Stiffness matrix symmetry.
    kr3.kData[6]= kr3.kData[2];
    kr3.kData[7]= kr3.kData[5];
    return 0;
  }

//! @brief Adds a fiber to the section XXX Enhance parameter updating.
XC::Fiber *XC::FiberDeque::addFiber(FiberSection3d &Section3d,Fiber &newFiber,CrossSectionKR &kr3)
  {
    Fiber *retval= inserta(newFiber);
    updateKRCDG(Section3d,kr3);
    return retval;
  }

//! @brief Establece los valores de las initial strains.
int XC::FiberDeque::setInitialSectionDeformation(const FiberSection3d &Section3d)
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

//! @brief Establece los valores de las trial strains.
int XC::FiberDeque::setTrialSectionDeformation(FiberSection3d &Section3d,CrossSectionKR &kr3)
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

//! @brief Returns to the last commited state.
int XC::FiberDeque::revertToLastCommit(FiberSection3d &Section3d,CrossSectionKR &kr3)
  {
    int err= 0;
    kr3.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToLastCommit(); // invoke revertToLastCommit on the material
    err+= updateKRCDG(Section3d,kr3);
    return err;
  }

//! @brief Returns to the initial state.
int XC::FiberDeque::revertToStart(FiberSection3d &Section3d,CrossSectionKR &kr3)
  {
    // revert the fibers to start
    int err= 0;
    kr3.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToStart(); // invoke revertToStart on the material
    err+= updateKRCDG(Section3d,kr3);
    return err;
  }

//! @brief Returns the tangent stiffness matrix inicial.
const XC::Matrix &XC::FiberDeque::getInitialTangent(const FiberSection3d &Section3d) const
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

//! @brief Update the parameters CDG, stiffness and resultant.
int XC::FiberDeque::updateKRCDG(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
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
            Qz+= -yLoc*fiberArea; //Coordenada y cambiada de signo.
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
    yCDG= -Qz/Atot; //Coordenada y del CDG  XXX ¿Signo menos?
    zCDG= Qy/Atot; //Coordenada z del CDG 
    krGJ.kData[4]= krGJ.kData[1]; //Stiffness matrix symmetry.
    krGJ.kData[8]= krGJ.kData[2];
    krGJ.kData[9]= krGJ.kData[6];
    krGJ.kData[15]= SectionGJ.getGJ(); //(3,3)->15 //The remaining six elements of krGJ.kData are zero.

    krGJ.rData[3]= SectionGJ.getGJ()*SectionGJ.getSectionDeformation()(3); //Torsion.
    return 0;
  }

//! @brief Adds a fiber to the container. XXX Enhance parameter updating.
XC::Fiber *XC::FiberDeque::addFiber(FiberSectionGJ &SectionGJ,Fiber &newFiber,CrossSectionKR &krGJ)
  {
    Fiber *retval= inserta(newFiber);
    updateKRCDG(SectionGJ,krGJ);
    return retval;
  }

//! @brief Establece los valores de las initial strains.
int XC::FiberDeque::setInitialSectionDeformation(const FiberSectionGJ &SectionGJ)
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
int XC::FiberDeque::setTrialSectionDeformation(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
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

//! @brief Returns to the last commited state.
int XC::FiberDeque::revertToLastCommit(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
  {
    int err= 0;
    krGJ.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToLastCommit(); // invoke revertToLastCommit on the material
    err+= updateKRCDG(SectionGJ,krGJ);
    return err;
  }

//! @brief Returns to the initial state.
int XC::FiberDeque::revertToStart(FiberSectionGJ &SectionGJ,CrossSectionKR &krGJ)
  {
    // revert the fibers to start
    int err= 0;
    krGJ.zero();
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      err+= (*i)->getMaterial()->revertToStart(); // invoke revertToStart on the material
    err+= updateKRCDG(SectionGJ,krGJ);
    return err;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberDeque::getInitialTangent(const FiberSectionGJ &SectionGJ) const
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
    kInitialData[4]= kInitialData[1]; //Simetría.
    kInitialData[8]= kInitialData[2]; //Simetría.
    kInitialData[9]= kInitialData[6]; //Simetría.

    kInitialData[15]= SectionGJ.GJ; //(3,3)->15 //The remaining six elements of krGJ.kData are zero.
    return kInitial;
  }

//! @brief Gets one of the response parameters of the section.
XC::Response *XC::FiberDeque::setResponse(const std::vector<std::string> &argv, Information &sectInfo)
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

// //! Returns the max value of the property being passed as parameter over the fiber set.
// //! being passed as parameter.
// size_t XC::FiberDeque::IMaxProp(const std::string &nmb_prop) const
//   {
//     size_t retval= 0;
//     double vmax= 0.0;
//     const size_t nf= getNumFibers();
//     if(nf<1)
//       {
//         std::cerr << "FiberDeque::IMaxProp; No hay fibers definidas." << std::endl;
//         return retval;
//       }
//     register size_t i= 0;
//     (*this)[i]->interpreta(nmb_prop);
//     boost::any valor_prop= InterpreteRPN::Pila().Pop();
//     if(boost_any_is_number(valor_prop))
//       {
//         vmax= convert_to_double(valor_prop);
//         retval= i;
//       }
//     i++;
//     double tmp= vmax;
//     for(;i<nf;i++)
//       if((*this)[i])
//         {
//           (*this)[i]->interpreta(nmb_prop);
//           boost::any valor_prop= InterpreteRPN::Pila().Pop();
//           if(boost_any_is_number(valor_prop))
//             {
//               tmp= convert_to_double(valor_prop);
//               if(tmp>vmax)
//                 {
//                   retval= i;
//                   vmax= tmp;
//                 }
//             }
//         }
//       else
//         std::cerr << "FiberDeque::IMaxProp; Pointer to fiber is null." << std::endl;
//     return retval;
//   }

// //! Returns the index of the fiber that presents the minimal value
// //! of the property which name is being passed as parameter.
// size_t XC::FiberDeque::IMinProp(const std::string &nmb_prop) const
//   {
//     size_t retval= 0;
//     double vmin= 0.0;
//     const size_t nf= getNumFibers();
//     if(nf<1)
//       {
//         std::cerr << "FiberDeque::IMinProp; No hay fibers definidas." << std::endl;
//         return retval;
//       }
//     register size_t i= 0;
//     (*this)[i]->interpreta(nmb_prop);
//     boost::any valor_prop= InterpreteRPN::Pila().Pop();
//     if(boost_any_is_number(valor_prop))
//       {
//         vmin= convert_to_double(valor_prop);
//         retval= i;
//       }
//     i++;
//     double tmp= vmin;
//     for(;i<nf;i++)
//       if((*this)[i])
//         {
//           (*this)[i]->interpreta(nmb_prop);
//           valor_prop= InterpreteRPN::Pila().Pop();
//           if(boost_any_is_number(valor_prop))
//             {
//               tmp= convert_to_double(valor_prop);
//               if(tmp<vmin)
//                 {
//                   retval= i;
//                   vmin= tmp;
//                 }
//             }
//         }
//       else
//         std::cerr << "FiberDeque::IMinProp; Pointer to fiber is null." << std::endl;
//     return retval;
//   }

//! @brief Returns the identifier of the fiber with maximum value for the iCoo
//! coordinate with respect to the system being passed as parameter.
size_t XC::FiberDeque::getFiberWithMaxCoord(const Ref3d3d &r,const size_t &iCoo) const
  {
    size_t retval= 0;
    const size_t nf= getNumFibers();
    if(nf<1)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; there is no fibers defined." << std::endl;
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

//! @brief Returns the identifier of the fiber with minimum value for the iCoo
//! coordinate with respect to the system being passed as parameter.
size_t XC::FiberDeque::getFiberWithMinCoord(const Ref3d3d &r,const size_t &iCoo) const
  {
    size_t retval= 0;
    const size_t nf= getNumFibers();
    if(nf<1)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
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

int XC::FiberDeque::setParameter(const int &paramMatTag,const std::vector<std::string> &argv, Parameter &param)
  {
    int parameterID= -1;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      if(paramMatTag == (*i)->getMaterial()->getTag())
        parameterID= (*i)->getMaterial()->setParameter(argv, param);
    return parameterID;
  }

int XC::FiberDeque::updateParameter(const int &paramMatTag,int parameterID, Information &info)
  {
    int ok= -1;
    std::deque<Fiber *>::iterator i= begin();
    for(;i!= end();i++)
      if(paramMatTag == (*i)->getMaterial()->getTag())
        ok =(*i)->getMaterial()->updateParameter(parameterID, info);
    return ok;
  }

int XC::FiberDeque::activateParameter(int passedParameterID)
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

void XC::FiberDeque::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of Fibers: " << getNumFibers() << std::endl;
    s << "\tCentroid: (" << -yCDG << ", " << zCDG << ')' << std::endl;
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



