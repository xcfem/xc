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
//DeformationPlane.cc

#include "DeformationPlane.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/GeneralEquationOfPlane.h"
#include "utility/matrix/Vector.h"
#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d1/Recta3d.h"
#include "xc_utils/src/geom/d1/Recta2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "utility/actor/actor/MovableVector.h"

bool XC::DeformationPlane::check_positions(const Pos3d &pA,const Pos3d &pB, const Pos3d &pC)
  {
    const Pos2d p1= Pos2d(pA.y(),pA.z());
    const Pos2d p2= Pos2d(pB.y(),pB.z());
    const Pos2d p3= Pos2d(pC.y(),pC.z());
    return check_positions(p1,p2,p3);
  }

bool XC::DeformationPlane::check_positions(const Pos2d &p1,const Pos2d &p2, const Pos2d &p3)
  {
    bool retval= true;
    const GEOM_FT tol2= 1e-3;
    const GEOM_FT d12= p1.dist2(p2);
    if(d12<tol2)
      {
        retval= false;
        std::clog << "DeformationPlane; points p1= " << p1 << " and p2= " 
                  << p2 << " are too close d= " << sqrt(d12) << std::endl;
      }
    const GEOM_FT d13= p1.dist2(p3);
    if(d13<tol2)
      {
        retval= false;
        std::clog << "DeformationPlane; points p1= " << p1 << " and p3= " 
                  << p3 << " are too close d= " << sqrt(d13) << std::endl;
      }
    const GEOM_FT d23= p2.dist2(p3);
    if(d23<tol2)
      {
        retval= false;
        std::clog << "DeformationPlane; points p2= " << p2 << " and p3= " 
                  << p3 << " are too close d= " << sqrt(d23) << std::endl;
      }
    return retval;
  }

XC::DeformationPlane::DeformationPlane(const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  : Plano3d(p1,p2,p3), MovableObject(0) { check_positions(p1,p2,p3); }

XC::DeformationPlane::DeformationPlane(const Plano3d &p)
  : Plano3d(p), MovableObject(0) {}


//! @brief Strains at three fibers.
XC::DeformationPlane::DeformationPlane( const Pos2d &yz1, const double &e_1, 
                  const Pos2d &yz2, const double &e_2,
                  const Pos2d &yz3, const double &e_3)
  : Plano3d(), MovableObject(0)
  {
    const Pos3d p1(e_1,yz1.x(),yz1.y());
    const Pos3d p2(e_2,yz2.x(),yz2.y());
    const Pos3d p3(e_3,yz3.x(),yz3.y());
    check_positions(p1,p2,p3);
    TresPuntos(p1,p2,p3);
  }

//! Constant strain over the section.
XC::DeformationPlane::DeformationPlane(const double &eps) 
  :Plano3d(Pos3d(eps,100,0),Pos3d(eps,0,100),Pos3d(eps,100,100)), MovableObject(0) {}

XC::DeformationPlane::DeformationPlane(const Vector &e)
  : Plano3d(), MovableObject(0)
  {
    const double e_1= e(0); //strain at (0,0)
    const double e_2= e(0)+e(1); //Strain at (1,0)
    double e_3= e(0); //Strain at (0,1)
    if(e.Size()>2)
      e_3+= e(2); //Strain at (0,1)
    
    const Pos3d p1(e_1,0.0,0.0);
    const Pos3d p2(e_2,1.0,0.0);
    const Pos3d p3(e_3,0.0,1.0);
    TresPuntos(p1,p2,p3);
  }

void XC::DeformationPlane::ConstantStrain(const double &e)
  {
    const Pos3d p1(e,0.0,0.0);
    const Pos3d p2(e,100.0,0.0);
    const Pos3d p3(e,0.0,100.0);
    TresPuntos(p1,p2,p3);
  }

//! @brief Return the strain of the fiber at the position
//! being passed as parameter.
double XC::DeformationPlane::Strain(const Pos2d &p) const
  { return Plano3d::x(p); }

//! @brief Returns the generalized strains vector.
const XC::Vector &XC::DeformationPlane::getDeformation(void) const
  {
    static Vector retval(3);
    retval(0)= Strain(Pos2d(0,0));
    retval(1)= Strain(Pos2d(1,0))-retval(0);
    retval(2)= Strain(Pos2d(0,1))-retval(0);
    return retval;
  }

//! @brief Returns the generalized strains vector.
const XC::Vector &XC::DeformationPlane::getDeformation(const size_t &order,const ResponseId &code) const
  {
    static Vector retval;
    retval.resize(order);
    retval.Zero();
    const Vector &tmp= getDeformation();
    for(register size_t i= 0;i<order;i++)
      {
        if(code(i) == SECTION_RESPONSE_P)
          retval[i]+= tmp(0);
        if(code(i) == SECTION_RESPONSE_MZ)
          retval[i]+= tmp(1);
        if(code(i) == SECTION_RESPONSE_MY)
          retval[i]+= tmp(2);
      }
    return retval;
  }

//! @brief Return the neutral axis.
Recta2d XC::DeformationPlane::getNeutralAxis(void)const
  {
    const double a= angulo(*this,PlanoYZ3d);
    Recta2d retval;
    if(a>1e-4)
      {
        Recta3d traza= TrazaYZ();
        if(traza.exists())
          retval= traza.YZ2DProjection();
      }
    else //Almost parallel: can't find intersection.
      retval.setExists(false);
    return retval;
  }


//! @brief Returns (if possible) a pont in the tensioned side of the cross-section.
Pos2d XC::DeformationPlane::getPointOnTensionedHalfPlane(void) const
  {
    Pos2d retval(0,0);
    const Recta2d fn(getNeutralAxis());
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        //const Vector2d v(getMaximumSlopeLineYZ().YZ2DProjection().VDir());
        const Vector2d v= fn.VDir().Normal();
        retval= p0+1000*v;
        if(Strain(retval)<0) //Lado compresiones.
          retval= p0-1000*v;
      }
    else //Degenerated neutral axis.
      retval.setExists(Strain(retval)>0.0);
    return retval;
  }

//! @brief Returns (if possible) un punto en el que las tensiones son de compresion.
Pos2d XC::DeformationPlane::getPointOnCompressedHalfPlane(void) const
  {
    Pos2d retval(0,0);
    const Recta2d fn(getNeutralAxis());
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        //const Vector2d v(getMaximumSlopeLineYZ().YZ2DProjection().VDir());
        const Vector2d v= fn.VDir().Normal();
        retval= p0+1000*v;
        if(Strain(retval)>0) //Tensioned side.
          retval= p0-1000*v;
      }
    else //Degenerated neutral axis.
      retval.setExists(Strain(retval)<0.0);
    return retval;
  }

//! @brief Returns the half plane which border is the line being passed
//! as parameter and is contained in the half plane in traction.
Semiplano2d XC::DeformationPlane::getTensionedHalfPlane(const Recta2d &r) const
  {
    const Semiplano2d spt= getTensionedHalfPlane();
    assert(spt.exists());
    const Pos2d p0(r.Punto());
    const Vector2d v= r.VDir().Normal();
  
    Pos2d p(p0+1000*v);
    Semiplano2d retval;
    if(spt.In(p))
      retval= Semiplano2d(r,p);
    else
      {
        p= p0-1000*v;
        if(spt.In(p))
          retval= Semiplano2d(r,p);
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << " couldn't find the tensioned half-plane"
                    << " with the border r= " << r << std::endl;
      }
    return retval;
  }

//! @brief Returns the tensioned half-plane.
Semiplano2d XC::DeformationPlane::getTensionedHalfPlane(void) const
  {
    const Recta2d fn= getNeutralAxis();
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    Pos2d tmp(0,0);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        const Vector2d v= fn.VDir().Normal();
        tmp= p0+1000*v;
        if(Strain(tmp)<0) //Lado compresiones.
          tmp= p0-1000*v;
      }
    else //Degenerated neutral axis.
      exists= (Strain(tmp)>0.0);

    Semiplano2d retval;
    if(exists)
      retval= Semiplano2d(fn,tmp);
    return retval;
  }

//! @brief Returns the compressed half-plane.
Semiplano2d XC::DeformationPlane::getCompressedHalfPlane(const Recta2d &r) const
  {
    const Recta2d fn= getNeutralAxis();
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    Pos2d tmp(0,0);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        const Vector2d v= fn.VDir().Normal();
        tmp= p0+1000*v;
        if(Strain(tmp)>0) //Tensioned side.
          tmp= p0-1000*v;
      }
    else //Degenerated neutral axis.
      exists= (Strain(tmp)<0.0);

    Semiplano2d retval;
    if(exists)
      retval= Semiplano2d(fn,tmp);
    return retval;
  }

//! @brief Returns the compressed half plane.
Semiplano2d XC::DeformationPlane::getCompressedHalfPlane(void) const
  {
    const Recta2d fn= getNeutralAxis();
    bool exists= fn.exists();
    Semiplano2d retval;
    if(exists)
      retval= getCompressedHalfPlane(fn);
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::DeformationPlane::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    Vector ec_gen(4);
    GeneralEquationOfPlane ec= getGeneralEquation();
    ec_gen(0)= ec.a(); ec_gen(1)= ec.b(); ec_gen(2)= ec.c(); ec_gen(3)= ec.d(); 
    int res= cp.sendVector(ec_gen,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::DeformationPlane::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0));
    Vector ec_gen(4);
    int res= cp.receiveVector(ec_gen,getDbTagData(),CommMetaData(1));
    GeneralEquationOfPlane ec(ec_gen(0),ec_gen(1),ec_gen(2),ec_gen(3));
    GeneralEquation(ec);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DeformationPlane::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DeformationPlane::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
