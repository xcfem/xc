//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//Vector3d.cc

#include "Vector3d.h"
#include "Pos3d.h"
#include "Dir3d.h"

#include "utility/geom/FT_matrix.h"
#include "utility/matrices/op_tensor.h"
//#include "Inventor/SbVec3f.h"



// Vector3d::Vector3d(const double &x,const double &y,const double &z)
//   : cgvct(Vector_3_from_doubles(x,y,z)) {}
Vector3d::Vector3d(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &z)
  : cgvct(x,y,z) {}

Vector3d::Vector3d(const FT_matrix &m)
  : cgvct(0,0,0)
  {
    assert(m.getNumberOfRows()==3);
    assert(m.getNumberOfColumns()==1); 
    cgvct= CGVector_3(m(1),m(2),m(3));
  }
Vector3d::Vector3d(const Pos3d &p1,const Pos3d &p2)
  : cgvct(p1.ToCGAL(),p2.ToCGAL()) {}

//! @brief Comparison operator.
bool Vector3d::operator==(const Vector3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= ProtoGeom::operator==(other);
        if(retval)
          retval= (cgvct==other.cgvct); 
       }
    return retval;
  }

bool Vector3d::Nulo(void) const
  { return ((*this)==VectorNulo3d); }
FT_matrix Vector3d::getMatrix(void) const
  //Return el column vector.
  {
    FT_matrix retval(3,1,0);
    retval(1)= x(); retval(2)= y(); retval(3)= z();
    return retval;
  }
Dir3d Vector3d::getDirection(void) const
  { return Dir3d(*this); }

bool Vector3d::EsUnitario(const double &tol) const
  {
    const GEOM_FT md2= GetModulus2();
    if((md2-1)>tol)
      return false;
    else
      return true;
  }

//! @brief Return the normalized vector.
Vector3d Vector3d::getNormalized(void) const
  {
    Vector3d retval(*this);
    const double m= GetModulus();
    if(m!=0.0)
      retval= retval/m;
    return retval; }

//! @brief Normalize vector.
void Vector3d::Normalize(void)
  { operator=(this->getNormalized()); }

void Vector3d::SetX(const GEOM_FT &vx)
  { cgvct= CGVector_3(vx,y(),z()); }
void Vector3d::SetY(const GEOM_FT &vy)
  { cgvct= CGVector_3(x(),vy,z()); }
void Vector3d::SetZ(const GEOM_FT &vz)
  { cgvct= CGVector_3(x(),y(),vz); }
void Vector3d::Set(unsigned short int i,const GEOM_FT &v)
  {
    switch(i)
      {
      case 1:
        SetX(v);
        break;
      case 2:
        SetY(v);
        break;
      case 3:
        SetZ(v);
        break;
      default:
	std::cerr << "Se esperaba que el índice fuera 1, 2 ó 3." << std::endl;
        break;
      }
  }

//! @brief Return el módulo del vector.
GEOM_FT Vector3d::GetModulus(void) const
  { return sqrt_FT(GetModulus2()); }

//! @brief Return el producto escalar.
GEOM_FT Vector3d::GetDot(const Vector3d &v) const
  { return (x()*v.x()+y()*v.y()+z()*v.z()); }

//! @brief Return the product of the vector times the matrix.
GEOM_FT Vector3d::GetDot(const FT_matrix &m) const
  {
    assert(m.getNumberOfRows()==3);
    assert(m.getNumberOfColumns()==3);
    return (x()*m(1)+y()*m(2)+z()*m(3));
  }

//! @brief Return the cross product.
Vector3d Vector3d::getCross(const Vector3d &v) const
  { return Vector3d(cross_product(ToCGAL(),v.ToCGAL())); }

//! @brief Product of the vector times the matrix.
FT_matrix operator*(const FT_matrix &m,const Vector3d &v)
  { return m*v.getMatrix(); }

Vector3d &Vector3d::operator+=(const Vector3d &v)
  {
    (*this)= (*this)+v;
    return *this;
  }

Vector3d &Vector3d::operator-=(const Vector3d &v)
  {
    (*this)= (*this)-v;
    return *this;
  }

//! @brief Return la suma de los vectores.
Vector3d Vector3d::operator+(const Vector3d &v) const
  {  return Vector3d(ToCGAL()+v.ToCGAL()); }

//! @brief Return la resta de los vectores.
Vector3d Vector3d::operator-(const Vector3d &v) const
  { return Vector3d(ToCGAL()-v.ToCGAL()); }

//! @brief Return el producto del vector por el escalar.
Vector3d Vector3d::operator*(const GEOM_FT &d) const
  { return Vector3d(ToCGAL()*d); }

//! @brief Return el producto del vector por el inverso del escalar.
Vector3d Vector3d::operator/(const GEOM_FT &d) const
  { return Vector3d(ToCGAL()*(1/d)); }

//! @brief Return the vector obtained from projecting v
//! onto the perpendicular direction to this vector.
//! Used to orthogonalize a base.
Vector3d Vector3d::Perpendicular(const Vector3d &v) const
  {
    Vector3d retval(0,0,0);
    if( Nulo() || v.Nulo() )
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; one of the vectors: "
                  << *this << " o " << v << " is null."
                  << " Null vector returned." << std::endl;
      }
    else
      if(parallel(*this,v))
        {
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; vector :" << v
                    << " is parallel to this one: " << *this
                    << ", null vector returned." << std::endl;
        }
      else
        {
          const GEOM_FT alpha= -(dot(v,(*this))/GetModulus2());
          retval= v+alpha*(*this);
        }
    return retval;
  }

//! @brief Return the angle between this vector and the argument.
GEOM_FT Vector3d::getSignedAngle(const Vector3d &v) const
  {
    if( this->Nulo() || v.Nulo() )
      {
	std::clog << getClassName() << "::" << __FUNCTION__
		  << "(Vector3d) one of the vectors: v1= "
                  << (*this) << " or v2= " << v
		  << " is null. Zero returned." << std::endl;
        return 0.0;
      }
    const GEOM_FT prod_mod= sqrt_FT(this->GetModulus2()*v.GetModulus2());
    const GEOM_FT prod_escalar= this->GetDot(v);
    const GEOM_FT coseno= prod_escalar/prod_mod;
    const Vector3d cross_product= this->getCross(v);
    const GEOM_FT seno= cross_product.GetModulus()/prod_mod;
    std::cout << "v1= " << *this << " v2= " << v
              << " cos= " << coseno << " sin= " << seno << std::endl;
    return atan2(seno,coseno);
  }

//! @brief Return the angle between this vector and the argument.
GEOM_FT Vector3d::getAngle(const Vector3d &v) const
  {
    GEOM_FT retval= 0;
    //XXX Modify by splitting in "getAngle" and "getSignedAngle"
    //como en Vector2d.
    if( Nulo() || v.Nulo() )
      {
	std::clog << getClassName() << "::" << __FUNCTION__
	          << "; one of the vector: this= "
                  << *this << " or v= " << v 
                  << " is null. Zero returned." << std::endl;
        retval= 0.0;
      }
    else
      {
        const GEOM_FT dot_product= dot(*this,v);
	const GEOM_FT cross_product= cross(*this,v).GetModulus();
	retval= atan2(cross_product,dot_product);
      }
    return retval;
  }

//! @brief Get the polar angle (spherical coordinates)
GEOM_FT Vector3d::getPolarAngle(void) const
  { return acos(z()/GetModulus()); }
      
//! @brief Get the azimuthal angle (spherical coordinates)
GEOM_FT Vector3d::getAzimuthalAngle(void) const
    { return atan2(y(),x()); }

bool parallel(const Vector3d &v1,const Vector3d &v2)
  { return parallel(v1.getDirection(),v2.getDirection()); } 
bool coplanarios(const Vector3d &v1,const Vector3d &v2,const Vector3d &v3)
  {
    const Pos3d p1= Origin3d + v1;
    const Pos3d p2= Origin3d + v2;
    const Pos3d p3= Origin3d + v3;
    return coplanarios(Origin3d,p1,p2,p3);
  }

GEOM_FT Abs(const Vector3d &v)
//Return el módulo del vector.
  { return v.GetModulus(); }

FT_matrix Traspuesta(const Vector3d &v)
  {
    FT_matrix retval(1,3);
    retval(1,1)= v.x(); retval(1,2)= v.y(); retval(1,3)= v.z();
    return retval;
  }
std::ostream &operator<<(std::ostream &stream,const Vector3d &n)
  {
    stream << "[[" << n.x() << "][" << n.y() << "]["
           << n.z() << "]]";
    return stream;
  }
    
//! @brief Return a Python dictionary with the object members values.
boost::python::dict Vector3d::getPyDict(void) const
  {
    boost::python::dict retval= ProtoGeom::getPyDict();
    retval["x"]= x();
    retval["y"]= y();
    retval["z"]= z();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Vector3d::setPyDict(const boost::python::dict &d)
  {
    ProtoGeom::setPyDict(d);
    SetX(boost::python::extract<GEOM_FT>(d["x"]));
    SetY(boost::python::extract<GEOM_FT>(d["y"]));
    SetZ(boost::python::extract<GEOM_FT>(d["z"]));
  }

GEOM_FT signedAngle(const Vector3d &v1,const Vector3d &v2)
  { return v1.getSignedAngle(v2); }

//! @brief Return el angle between vectors.
GEOM_FT angle(const Vector3d &v1,const Vector3d &v2)
  { return v1.getAngle(v2); }


FT_matrix prod_tensor(const Vector3d &u,const Vector3d &v)
  { return prod_tensor(u.getMatrix(),v.getMatrix()); }

FT_matrix operator&(const Vector3d &u,const Vector3d &v)
  { return prod_tensor(u,v); }

//! @brief For the vector argument v, return the matrix that
//! post-multiplied by any vector u returns the cross product: (u x v)
//!
//! see "Geometric tools for computer graphics"
//! Philip J. Schneider Morgan Kaufmann Publishers, páginas 119 y 120.
FT_matrix skew_symm_matrix_post(const Vector3d &v)
  {
    FT_matrix retval(3,3,double_to_FT(0.0));
    const GEOM_FT &v1= v(1);
    const GEOM_FT &v2= v(2);
    const GEOM_FT &v3= v(3);
    retval(1,2)= -v3; retval(1,3)= -v2;
    retval(2,1)= v3; retval(2,3)=-v1;
    retval(3,1)= -v2; retval(3,2)=v1;
    return retval;
  }

//! @brief For the vector argument v, return the matrix that
//! pre-multiplied by any vector u returns the cross product: (v x u)
//!
//! see "Geometric tools for computer graphics"
//! Philip J. Schneider Morgan Kaufmann Publishers, páginas 119 y 120.
FT_matrix skew_symm_matrix_pre(const Vector3d &v)
  { return -skew_symm_matrix_post(v); }


