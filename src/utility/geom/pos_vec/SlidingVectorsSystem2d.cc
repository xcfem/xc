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
//SlidingVectorsSystem2d.cc

#include "SlidingVectorsSystem2d.h"
#include "SlidingVector2d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/ref_sys/Ref2d2d.h"


SlidingVectorsSystem2d::SlidingVectorsSystem2d(const Pos2d &O,const Vector2d &R,const GEOM_FT &Mo)
  : SlidingVector2d(O,R), mom(Mo) {}

SlidingVectorsSystem2d::SlidingVectorsSystem2d(const SlidingVector2d &v)
  : SlidingVector2d(v), mom(0.0) {}

//! @brief Return the moment about P.
GEOM_FT SlidingVectorsSystem2d::getMoment(const Pos2d &P) const
  {
    const GEOM_FT m2= SlidingVector2d::getMoment(P);
    return mom+m2;
  }

void SlidingVectorsSystem2d::Print(std::ostream &os) const
  {
    os << "Resultant R=" << getResultant()
       << " , moment with respect to " << getOrg() << " Mo= " << mom; 
  }

void SlidingVectorsSystem2d::PrintLtx(std::ostream &os,const std::string &ud_long,const GEOM_FT &f_long, const std::string &ud_f,const GEOM_FT &f_f) const
  {
    //Se asume que imprimimos en una tabla.
    os << "Point of application: " << getOrg().VectorPos()*f_long << ud_long << "\\\\" << std::endl
       << "Resultant: " << getResultant()*f_f << ud_f << "\\\\" << std::endl 
       << "Moment: " << mom*f_f << ud_f << ud_long << "\\\\" << std::endl;
  }
Vector2d SlidingVectorsSystem2d::getResultant(const Ref2d2d &ref) const
  { return ref.getLocalCoordinates(getResultant()); } 

SlidingVectorsSystem2d SlidingVectorsSystem2d::reduceTo(const Pos2d &Q) const
  { return SlidingVectorsSystem2d(Q,getResultant(),getMoment(Q)); }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
std::vector<SlidingVector2d> SlidingVectorsSystem2d::distribute(const std::vector<Pos2d> &point_list) const
  {
    const size_t sz= point_list.size();
    // Calculate the position of the center of gravity:
    Vector2d num;
    const double W(sz);
    for(size_t i= 0;i<sz;i++)
      {
	const Pos2d p_i= point_list[i];
	num+= p_i.VectorPos();
      }
    Pos2d cg(0,0);
    cg+= (num/W);
    // Calculate the equivalent sliding vector system at cg:
    SlidingVectorsSystem2d newSVS= this->reduceTo(cg);
    const Vector2d Fcg= newSVS.getResultant();
    const double Mcg= newSVS.getMoment();
    std::vector<Vector2d> f_i(sz);
    std::vector<Vector2d> p_i(sz);
    double denom= 0.0; 
    for(size_t i= 0;i<sz;i++)
      {
	f_i[i]= Fcg/W; // force distribution.
	const Pos2d pt_i= point_list[i];
	const Vector2d r_i= pt_i-cg;
	p_i[i]= Mcg*Vector2d(r_i.y(),r_i.x()); // moment distribution.
	denom+= dot(r_i,r_i);
      }
    std::vector<SlidingVector2d> retval(sz);
    const double factor= 1.0/denom;
    for(size_t i= 0;i<sz;i++)
      {
	const Pos2d pt_i= point_list[i];
	p_i[i]= factor*p_i[i];
	retval[i]= SlidingVector2d(pt_i, f_i[i]+p_i[i]);
      }
    return retval;
  }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
//! @param weights: list of weights (one for each position).
boost::python::list SlidingVectorsSystem2d::distributePyNoWeights(const boost::python::list &pt_list) const
  {
    const size_t sz= len(pt_list);
    std::vector<Pos2d> point_list(sz);
    for(size_t i= 0;i<sz;i++)
      { point_list[i]= boost::python::extract<Pos2d>(pt_list[i]); }
    std::vector<SlidingVector2d> tmp= distribute(point_list);
    boost::python::list retval;
    for(size_t i= 0;i<sz;i++)
      { retval.append(tmp[i]); }
    return retval;
  }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
//! @param weights: list of weights (one for each position).
std::vector<SlidingVector2d> SlidingVectorsSystem2d::distribute(const std::vector<Pos2d> &point_list, const std::vector<double> &weights) const
  {
    const size_t szpl= point_list.size();
    const size_t szw= weights.size();
    if(szpl!=szw)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point list and weight list of different sizes."
	        << std::endl;
    const size_t sz= std::min(szpl,szw);
    // Calculate the position of the center of gravity:
    Vector2d num;
    double W;
    for(size_t i= 0;i<sz;i++)
      {
	const double w_i= weights[i];
	const Pos2d p_i= point_list[i];
	num+= w_i*p_i.VectorPos();
	W+= w_i;
      }
    Pos2d cg(0,0);
    cg+= (num/W);
    // Calculate the equivalent sliding vector system at cg:
    SlidingVectorsSystem2d newSVS= this->reduceTo(cg);
    const Vector2d Fcg= newSVS.getResultant();
    const double Mcg= newSVS.getMoment();
    std::vector<Vector2d> f_i(sz);
    std::vector<Vector2d> p_i(sz);
    double denom= 0.0; 
    for(size_t i= 0;i<sz;i++)
      {
	const double w_i= weights[i];
	f_i[i]= Fcg*w_i/W; // force distribution.
	const Pos2d pt_i= point_list[i];
	const Vector2d r_i= pt_i-cg;
	p_i[i]= w_i*(Mcg*Vector2d(r_i.y(),r_i.x())); // moment distribution.
	denom+= w_i*dot(r_i,r_i);
      }
    std::vector<SlidingVector2d> retval(sz);
    const double factor= 1.0/denom;
    for(size_t i= 0;i<sz;i++)
      {
	const Pos2d pt_i= point_list[i];
	p_i[i]= factor*p_i[i];
	retval[i]= SlidingVector2d(pt_i, f_i[i]+p_i[i]);
      }
    return retval;
  }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
//! @param weights: list of weights (one for each position).
boost::python::list SlidingVectorsSystem2d::distributePy(const boost::python::list &pt_list, const boost::python::list &w) const
  {
    const size_t szpl= len(pt_list);
    const size_t szw= len(w);
    if(szpl!=szw)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; point list and weight list of different sizes."
                << std::endl;
    const size_t sz= std::min(szpl,szw);
    std::vector<Pos2d> point_list(sz);
    std::vector<double> weights(sz);
    for(size_t i= 0;i<sz;i++)
      {
	point_list[i]= boost::python::extract<Pos2d>(pt_list[i]);
	weights[i]= boost::python::extract<double>(w[i]);
      }
    std::vector<SlidingVector2d> tmp= distribute(point_list, weights);
    boost::python::list retval;
    for(size_t i= 0;i<sz;i++)
      { retval.append(tmp[i]); }
    return retval;
  }

SlidingVectorsSystem2d &SlidingVectorsSystem2d::operator+=(const SlidingVector2d &v)
  {
    SlidingVector2d::operator+=(v);
    mom-= v.getMoment(getOrg());
    return *this;
  }
SlidingVectorsSystem2d &SlidingVectorsSystem2d::operator-=(const SlidingVector2d &v)
  {
    SlidingVector2d::operator-=(v);
    mom-= v.getMoment(getOrg());
    return *this;
  }
SlidingVectorsSystem2d &SlidingVectorsSystem2d::operator+=(const SlidingVectorsSystem2d &s)
  //The origin is preserved.
  {
    SlidingVector2d::operator+=(s);
    mom+= s.getMoment(getOrg());
    return *this;
  }
SlidingVectorsSystem2d &SlidingVectorsSystem2d::operator-=(const SlidingVectorsSystem2d &s)
  //The origin is preserved.
  {
    SlidingVector2d::operator-=(s);
    mom-= s.getMoment(getOrg());
    return *this;
  }
SlidingVectorsSystem2d &SlidingVectorsSystem2d::operator*=(const GEOM_FT &d)
  {
    SlidingVector2d::operator*=(d);
    mom*= d;
    return *this;
  }

SlidingVectorsSystem2d operator+(const SlidingVectorsSystem2d &s1,const SlidingVectorsSystem2d &s2)
  {
    SlidingVectorsSystem2d retval(s1);
    retval+=s2;
    return retval;
  }
SlidingVectorsSystem2d operator-(const SlidingVectorsSystem2d &s1,const SlidingVectorsSystem2d &s2)
  {
    SlidingVectorsSystem2d retval(s1);
    retval-=s2;
    return retval;
  }
SlidingVectorsSystem2d operator*(const GEOM_FT &d, const SlidingVectorsSystem2d &s)
  {
    SlidingVectorsSystem2d retval(s);
    return retval*=d;
  }

SlidingVectorsSystem2d operator*(const SlidingVectorsSystem2d &s,const GEOM_FT &d)
  { return d*s; }

SlidingVectorsSystem2d operator+(const SlidingVector2d &v1,const SlidingVector2d &v2)
  {
    SlidingVectorsSystem2d suma(v1);
    suma+=v2;
    return suma;
  }

SlidingVectorsSystem2d operator+(const SlidingVectorsSystem2d &s,const SlidingVector2d &v)
  {
    SlidingVectorsSystem2d suma(s);
    suma+=v;
    return suma;
  }

SlidingVectorsSystem2d operator+(const SlidingVector2d &v,const SlidingVectorsSystem2d &s)
  { return s+v; }

bool SlidingVectorsSystem2d::Nulo(void) const
  {
    bool retval= true;
    if(!SlidingVector2d::Nulo()) retval= false;
    if(mom!=0) retval= false;
    return retval;
  }

void SlidingVectorsSystem2d::Neg(void)
  {
    SlidingVector2d::Neg();
    mom= -mom;
  }

//! @brief Cambia de signo al sliding vector.
SlidingVectorsSystem2d operator-(const SlidingVectorsSystem2d &svd2d)
  {
    SlidingVectorsSystem2d retval(svd2d);
    retval.Neg();
    return retval;
  }

//! @brief Line of points with zero moment.
Line2d SlidingVectorsSystem2d::getZeroMomentLine(void) const
  {
    Line2d retval; //= Line2d(Pos2d(NAN,NAN),Pos2d(NAN,NAN));
    const Vector2d resul= getResultant();
    const GEOM_FT rx= resul.x();
    const GEOM_FT ry= resul.y();
    const GEOM_FT k= rx*getOrg().y()-ry*getOrg().x()-mom; 
    if(rx!=0.0)
      {
	const GEOM_FT xA= 1.0;
	const GEOM_FT yA= (ry*xA+k)/rx;
	const GEOM_FT xB= 1.0e3;
	const GEOM_FT yB= (ry*xB+k)/rx;
        retval= Line2d(Pos2d(xA,yA),Pos2d(xB,yB));
      }
    else if(ry!=0.0)
      {
	const GEOM_FT x= -k/ry;
        retval= Line2d(Pos2d(x,0.0),Pos2d(x,1e3));
      }
    else if(mom==0.0)
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; all points have zero moment." << std::endl; 
    else
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; no point has zero moment." << std::endl;
    const Pos2d p= retval.Point();
    const GEOM_FT m= getMoment(p);
    if(abs(m)>1E-6)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "ERROR in computing zero moment line; "
                << " moment for point: " << p
	        << " m= " << m << " not zero." << std::endl;
    return retval;
  }

std::ostream &operator<<(std::ostream &os, const SlidingVectorsSystem2d &svd2d)
  {
    svd2d.Print(os);
    return os;
  }
