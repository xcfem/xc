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
//SlidingVectorsSystem3d.cc

#include "SlidingVectorsSystem3d.h"
#include "SlidingVector3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/ref_sys/Ref3d3d.h"



//Equation of the cross product: x ^ a = b
//See book "Mecánica teórica de los sistemas de sólidos rígidos" de José Antonio Fernández Palacios.

//! @brief Condición para que la ecuación tenga solución.
inline bool cond_compat_eq_prod_vect(const Vector3d &a,const Vector3d &b)
  { return (dot(a,b)<=0); }

//! @brief Solución de la ecuación x ^ a = b siendo x, a y b sliding vectors
//! y ^ el producto vectorial.
Line3d sol_eq_prod_vect(const Vector3d &a,const Vector3d &b)
  {
    //Condición de compatibilidad.
    if(!cond_compat_eq_prod_vect(a,b))
      std::cerr << "La ecuación del producto vectorial no tiene solución." << std::endl;
    const GEOM_FT sqr_abs_a= Abs2(a);
    if(sqr_abs_a<=0)
      std::cerr << "La ecuación del producto vectorial no tiene solución: el vector a es nulo" << std::endl;
    const Pos3d org= Origin3d + ((a ^ b)*(1/sqr_abs_a));
    Pos3d dest= org+1000*a;
    return Line3d(org,dest);
  }

SlidingVectorsSystem3d::SlidingVectorsSystem3d(const Pos3d &O,const Vector3d &R,const Vector3d &Mo)
  : SlidingVector3d(O,R), mom(Mo) {}

SlidingVectorsSystem3d::SlidingVectorsSystem3d(const SlidingVector3d &v)
  : SlidingVector3d(v), mom(0,0,0) {}

//! @brief Moment field of the SlidingVectorsSystem3d.
//! Return themoment of the SlidingVectorsSystem3d with respect to the point P.
SlidingVector3d SlidingVectorsSystem3d::getMoment(const Pos3d &P) const
  {
    const SlidingVector3d m2= SlidingVector3d::getMoment(P);
    return SlidingVector3d(P,mom+m2);
  }

//! @brief Return the intersection of the zero moment line (if exists)
//! with the plane.
//! It's used, for example, to compunte the point of application of
//! the compression block in a reinforced concrete section.
Pos3d SlidingVectorsSystem3d::PointOfApplication(const Plane &p) const
  {
    Pos3d retval(NAN,NAN,NAN);
    if(existsZeroMomentLine())
      {
        Line3d r= getZeroMomentLine();
        GeomObj::list_Pos3d ptos= intersection(p,r);
        if(ptos.size()>0)
          retval= (*ptos.begin());
        else
          std::cerr << "The line of zero moment points is parallel to the plane."
		    << std::endl;
      }
    else
      std::cerr << "There are no points with zero moment." << std::endl;
    return retval;
  }

void SlidingVectorsSystem3d::PrintLtx(std::ostream &os,const std::string &ud_long,const GEOM_FT &f_long, const std::string &ud_f,const GEOM_FT &f_f) const
  {
    //Se asume que imprimimos en una tabla.
    os << "Point of application: " << org.VectorPos()*f_long << ud_long << "\\\\" << std::endl
       << "Resultant: " << getResultant()*f_f << ud_f << "\\\\" << std::endl 
       << "Moment: " << mom*f_f << ud_f << ud_long << "\\\\" << std::endl;
  }

//! @brief Moment with respect to an axis.
//! Is the moment with respect a point on the axis
//! projected onto the axis.
GEOM_FT SlidingVectorsSystem3d::getMoment(const Line3d &e) const
  { return dot(SlidingVectorsSystem3d::getMoment(e.Point()),e.VDir().getNormalized()); }

//! @brief Return el moment vector expressed in the reference
//! frame being passed as parameter.
Vector3d SlidingVectorsSystem3d::getMoment(const Ref3d3d &ref) const
  {
    SlidingVector3d m= getMoment(ref.Org());
    return ref.getLocalCoordinates(m);
  }

//! @brief Return the resultant vector expressed in the
//! reference frame argument.
Vector3d SlidingVectorsSystem3d::getResultant(const Ref3d3d &ref) const
  { return ref.getLocalCoordinates(getResultant()); } 

bool SlidingVectorsSystem3d::Nulo(void) const
  {
    bool retval= true;
    if(!SlidingVector3d::Nulo()) retval= false;
    if(!mom.Nulo()) retval= false;
    return retval;
  }

void SlidingVectorsSystem3d::Neg(void)
  {
    SlidingVector3d::Neg();
    mom=-mom;
  }

//! @brief Return the central axis of the system (moment paraller to resultant).
Line3d SlidingVectorsSystem3d::centralAxis(void) const
  { return sol_eq_prod_vect(getResultant(),mom); }

//! @brief Return the line of the points with zero moment.
Line3d SlidingVectorsSystem3d::getZeroMomentLine(const double &tol) const
  {
    if(existsZeroMomentLine(tol))
      return sol_eq_prod_vect(getResultant(),mom);
    else
      return Line3d(Pos3d(NAN,NAN,NAN),Pos3d(NAN,NAN,NAN));
  }

//! @brief Return true if the line of the points with zero moment exists.
bool SlidingVectorsSystem3d::existsZeroMomentLine(const double &tol) const
  {
    if((SlidingVector3d::Nulo()) && !(mom.Nulo())) return false;
    if(dot(getResultant(),mom)>0) return false;
    return true;
  }

SlidingVectorsSystem3d SlidingVectorsSystem3d::reduceTo(const Pos3d &Q) const
  { return SlidingVectorsSystem3d(Q,getResultant(),getMoment(Q)); }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
std::vector<SlidingVector3d> SlidingVectorsSystem3d::distribute(const std::vector<Pos3d> &point_list) const
  {
    const size_t sz= point_list.size();
    // Calculate the position of the center of gravity:
    Vector3d num;
    const double W(sz);
    for(size_t i= 0;i<sz;i++)
      {
	const Pos3d p_i= point_list[i];
	num+= p_i.VectorPos();
      }
    Pos3d cg(0,0,0);
    cg+= (num/W);
    // Calculate the equivalent sliding vector system at cg:
    SlidingVectorsSystem3d newSVS= this->reduceTo(cg);
    const Vector3d Fcg= newSVS.getResultant();
    const Vector3d Mcg= newSVS.getMoment();
    std::vector<Vector3d> f_i(sz);
    std::vector<Vector3d> p_i(sz);
    double denom= 0.0; 
    for(size_t i= 0;i<sz;i++)
      {
	f_i[i]= Fcg/W; // force distribution.
	const Pos3d pt_i= point_list[i];
	const Vector3d r_i= pt_i-cg;
	p_i[i]= cross(Mcg,r_i); // moment distribution.
	denom+= dot(r_i,r_i);
      }
    std::vector<SlidingVector3d> retval(sz);
    const double factor= 1.0/denom;
    for(size_t i= 0;i<sz;i++)
      {
	const Pos3d pt_i= point_list[i];
	p_i[i]= factor*p_i[i];
	retval[i]= SlidingVector3d(pt_i, f_i[i]+p_i[i]);
      }
    return retval;
  }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
//! @param weights: list of weights (one for each position).
boost::python::list SlidingVectorsSystem3d::distributePyNoWeights(const boost::python::list &pt_list) const
  {
    const size_t sz= len(pt_list);
    std::vector<Pos3d> point_list(sz);
    for(size_t i= 0;i<sz;i++)
      { point_list[i]= boost::python::extract<Pos3d>(pt_list[i]); }
    std::vector<SlidingVector3d> tmp= distribute(point_list);
    boost::python::list retval;
    for(size_t i= 0;i<sz;i++)
      { retval.append(tmp[i]); }
    return retval;
  }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
//! @param weights: list of weights (one for each position).
std::vector<SlidingVector3d> SlidingVectorsSystem3d::distribute(const std::vector<Pos3d> &point_list, const std::vector<double> &weights) const
  {
    const size_t szpl= point_list.size();
    const size_t szw= weights.size();
    if(szpl!=szw)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point list and weight list of different sizes."
	        << std::endl;
    const size_t sz= std::min(szpl,szw);
    // Calculate the position of the center of gravity:
    Vector3d num;
    double W;
    for(size_t i= 0;i<sz;i++)
      {
	const double w_i= weights[i];
	const Pos3d p_i= point_list[i];
	num+= w_i*p_i.VectorPos();
	W+= w_i;
      }
    Pos3d cg(0,0,0);
    cg+= (num/W);
    // Calculate the equivalent sliding vector system at cg:
    SlidingVectorsSystem3d newSVS= this->reduceTo(cg);
    const Vector3d Fcg= newSVS.getResultant();
    const Vector3d Mcg= newSVS.getMoment();
    std::vector<Vector3d> f_i(sz);
    std::vector<Vector3d> p_i(sz);
    double denom= 0.0; 
    for(size_t i= 0;i<sz;i++)
      {
	const double w_i= weights[i];
	f_i[i]= Fcg*w_i/W; // force distribution.
	const Pos3d pt_i= point_list[i];
	const Vector3d r_i= pt_i-cg;
	p_i[i]= w_i*cross(Mcg,r_i); // moment distribution.
	denom+= w_i*dot(r_i,r_i);
      }
    std::vector<SlidingVector3d> retval(sz);
    const double factor= 1.0/denom;
    for(size_t i= 0;i<sz;i++)
      {
	const Pos3d pt_i= point_list[i];
	p_i[i]= factor*p_i[i];
	retval[i]= SlidingVector3d(pt_i, f_i[i]+p_i[i]);
      }
    return retval;
  }

//! @brief Assigns a vector to each point to obtain an equivalent
//! sliding vector system.
//! @param point_list: list of positions.
//! @param weights: list of weights (one for each position).
boost::python::list SlidingVectorsSystem3d::distributePy(const boost::python::list &pt_list, const boost::python::list &w) const
  {
    const size_t szpl= len(pt_list);
    const size_t szw= len(w);
    if(szpl!=szw)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; point list and weight list of different sizes."
                << std::endl;
    const size_t sz= std::min(szpl,szw);
    std::vector<Pos3d> point_list(sz);
    std::vector<double> weights(sz);
    for(size_t i= 0;i<sz;i++)
      {
	point_list[i]= boost::python::extract<Pos3d>(pt_list[i]);
	weights[i]= boost::python::extract<double>(w[i]);
      }
    std::vector<SlidingVector3d> tmp= distribute(point_list, weights);
    boost::python::list retval;
    for(size_t i= 0;i<sz;i++)
      { retval.append(tmp[i]); }
    return retval;
  }

SlidingVectorsSystem3d &SlidingVectorsSystem3d::operator+=(const SlidingVector3d &v)
  {
    Vector3d::operator+=(v);
    mom= mom + v.getMoment(org);
    return *this;
  }
SlidingVectorsSystem3d &SlidingVectorsSystem3d::operator-=(const SlidingVector3d &v)
  {
    SlidingVector3d::operator-=(v);
    mom= mom - v.getMoment(org);
    return *this;
  }
SlidingVectorsSystem3d &SlidingVectorsSystem3d::operator+=(const SlidingVectorsSystem3d &s)
  //The org point is preserved.
  {
    SlidingVector3d::operator+=(s);
    mom= mom + s.getMoment(org);
    return *this;
  }

SlidingVectorsSystem3d &SlidingVectorsSystem3d::operator-=(const SlidingVectorsSystem3d &s)
  //The org point is preserved.
  {
    SlidingVector3d::operator-=(s);
    mom= mom - s.getMoment(org);
    return *this;
  }
SlidingVectorsSystem3d &SlidingVectorsSystem3d::operator*=(const GEOM_FT &d)
  {
    SlidingVector3d::operator*=(d);
    mom= mom * d;
    return *this;
  }

//! @brief Return the sum of the sliding vectors systems.
SlidingVectorsSystem3d operator+(const SlidingVectorsSystem3d &s1,const SlidingVectorsSystem3d &s2)
  {
    SlidingVectorsSystem3d retval(s1);
    retval+=s2;
    return retval;
  }

//! @brief Return the difference of sliding vectors systems.
SlidingVectorsSystem3d operator-(const SlidingVectorsSystem3d &s1,const SlidingVectorsSystem3d &s2)
  {
    SlidingVectorsSystem3d retval(s1);
    retval-=s2;
    return retval;
  }

//! @brief Return the product of the sliding vectors system by a scalar.
SlidingVectorsSystem3d operator*(const GEOM_FT &d, const SlidingVectorsSystem3d &s)
  {
    SlidingVectorsSystem3d retval(s);
    return retval*=d;
  }
//! @brief Return the product of the sliding vectors system by a scalar.
SlidingVectorsSystem3d operator*(const SlidingVectorsSystem3d &s,const GEOM_FT &d)
  { return d*s; }

//! @brief Imprime el sliding vectors system.
void SlidingVectorsSystem3d::Print(std::ostream &os) const
  {
    os << "Resultant R=" << getResultant()
       << " , moment with respect to " << org << " Mo= " << mom; 
  }

//! @brief Return the suma de los sliding vectors.
SlidingVectorsSystem3d operator+(const SlidingVector3d &v1,const SlidingVector3d &v2)
  {
    SlidingVectorsSystem3d suma(v1);
    suma+=v2;
    return suma;
  }

//! @brief Return the sum of the sliding vector system
//! with the vector being passed as parameter.
SlidingVectorsSystem3d operator+(const SlidingVectorsSystem3d &s,const SlidingVector3d &v)
  {
    SlidingVectorsSystem3d suma(s);
    suma+=v;
    return suma;
  }

//! @brief Return the sum of the sliding vector system
//! with the vector being passed as parameter.
SlidingVectorsSystem3d operator+(const SlidingVector3d &v,const SlidingVectorsSystem3d &s)
  { return s+v; }

//! @brief Cambia de signo al sliding vector.
SlidingVectorsSystem3d operator-(const SlidingVectorsSystem3d &svd3d)
  {
    SlidingVectorsSystem3d retval(svd3d);
    retval.Neg();
    return retval;
  }

//! @brief Prints.
std::ostream &operator<<(std::ostream &os, const SlidingVectorsSystem3d &svd3d)
  {
    svd3d.Print(os);
    return os;
  }
