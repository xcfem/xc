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
//Vector2d.cc

#include "Vector2d.h"
#include "Pos2d.h"
#include "Dir2d.h"
#include <plotter.h>
#include "utility/matrices/giros.h"
#include "utility/geom/FT_matrix.h"

// Vector2d::Vector2d(const double &x,const double &y)
//   : ProtoGeom(), cgvct(Vector_2_from_doubles(x,y)) {}
Vector2d::Vector2d(const GEOM_FT &x,const GEOM_FT &y)
  : ProtoGeom(), cgvct(x,y) {}

Vector2d::Vector2d(const FT_matrix &m)
  : cgvct(0,0)
  {
    assert(m.getNumberOfRows()==2);
    assert(m.getNumberOfColumns()==1); 
    cgvct= CGVector_2(m(1),m(2));
  }
Vector2d::Vector2d(const Pos2d &p1,const Pos2d &p2)
  : ProtoGeom(), cgvct(p1.ToCGAL(),p2.ToCGAL()) {}

//! @brief Comparison operator.
bool Vector2d::operator==(const Vector2d &other) const
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

bool Vector2d::Nulo(void) const
  { return ((*this)==VectorNulo2d); }
void Vector2d::SetX(const GEOM_FT &vx)
  { cgvct= CGVector_2(vx,y()); }
void Vector2d::SetY(const GEOM_FT &vy)
  { cgvct= CGVector_2(x(),vy); }
void Vector2d::Set(unsigned short int i,const GEOM_FT &v)
  {
    switch(i)
      {
      case 1:
        SetX(v);
        break;
      case 2:
        SetY(v);
        break;
      default:
	std::cerr << "Se esperaba que el índice fuera 1, 2 ó 3." << std::endl;
        break;
      }
  }

Vector2d &Vector2d::operator+=(const Vector2d &v)
  {
    (*this)= (*this)+v;
    return *this;
  }

Vector2d &Vector2d::operator-=(const Vector2d &v)
  {
    (*this)= (*this)-v;
    return *this;
  }
  

//! @brief Return la suma de los vectores.
Vector2d Vector2d::operator+(const Vector2d &v) const
  {  return Vector2d(ToCGAL()+v.ToCGAL()); }

//! @brief Return la resta de los vectores.
Vector2d Vector2d::operator-(const Vector2d &v) const
  { return Vector2d(ToCGAL()-v.ToCGAL()); }

//! @brief Return el producto del vector por el escalar.
Vector2d Vector2d::operator*(const GEOM_FT &d) const
  { return Vector2d(ToCGAL()*d); }

//! @brief Return el producto del vector por el inverso del escalar.
Vector2d Vector2d::operator/(const GEOM_FT &d) const
  { return Vector2d(ToCGAL()*(1/d)); }

FT_matrix Vector2d::getMatrix(void) const
  {
    FT_matrix retval(2,1,0);
    retval(1)= x(); retval(2)= y();
    return retval;
  }

//! @brief Return el vector normal a éste.
Vector2d Vector2d::Normal(void) const
  { return Vector2d(y(),-x()); }

//! @brief Normalize vector.
void Vector2d::Normalize(void)
  { operator=(this->getNormalized()); }

Dir2d Vector2d::getDirection(void) const
  { return Dir2d(*this); }

//! @brief Return the angle with the vector argument.
GEOM_FT Vector2d::getSignedAngle(const Vector2d &v) const
  { return signedAngle(*this,v); }

//! @brief Return the angle with the argument vector.
GEOM_FT Vector2d::getAngle(const Vector2d &v) const
  { return angle(*this,v); }

GEOM_FT Vector2d::XAxisAngle(void) const
  {
    double retval= atan2(y(),x());
    if(retval<0)
      retval+= 2*M_PI;
    return retval;
  }

GEOM_FT Vector2d::YAxisAngle(void) const
  { return M_PI/2.0-XAxisAngle(); }


bool Vector2d::EsUnitario(const double &tol) const
  {
    const GEOM_FT md2= GetModulus2();
    if((md2-1)>tol)
      return false;
    else
      return true;
  }

//! @brief Return el módulo del vector.
GEOM_FT Vector2d::GetModulus(void) const
  { return sqrt_FT(GetModulus2()); }

//! @brief Product of the vector times the matrix.
FT_matrix operator*(const FT_matrix &m,const Vector2d &v)
  { return m*v.getMatrix(); }

//! @brief Producto escalar.
GEOM_FT Vector2d::GetDot(const FT_matrix &m) const
  {
    assert(m.getNumberOfRows()==2);
    assert(m.getNumberOfColumns()==1);
    return (x()*m(1)+y()*m(2));
  }

//! @brief Return the product of the vector times the matrix.
GEOM_FT dot(const Vector2d &v1, const FT_matrix &m)
  { return v1.GetDot(m); }

//! @brief Return the product of the matrix times the vector.
GEOM_FT dot(const FT_matrix &m, const Vector2d &v1)
  { return v1.GetDot(m); }


// void Vector2d::Gira(const GEOM_FT &ang_rad)
//   { (*this)= GetGirado(ang_rad); }

// Vector2d Vector2d::GetGirado(const double &ang_rad) const
//   { 
//     Vector2d retval;
//     CGTrfAfin_2 giro(CGAL::ROTATION,sin(ang_rad),cos(ang_rad));
//     retval.cgvct = giro(cgvct);
//     return retval;
//   }
Vector2d Vector2d::Perpendicular(const Orientacion &o) const
  {
    Vector2d retval;
    retval.cgvct = cgvct.perpendicular(o);
    return retval;
  }
//! @brief Return the vector obtained from projecting v
//! onto the perpendicular direction to this vector.
//! Used to orthogonalize a base.
Vector2d Vector2d::Perpendicular(const Vector2d &v) const
  {
    Vector2d retval(0,0);
    if( Nulo() || v.Nulo() )
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; One of the vectors: "
                  << *this << " o " << v << " is null."
                  << " La operación no tiene sentido, se devuelve un vector nulo." << std::endl;
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
    
//! @brief Return a Python dictionary with the object members values.
boost::python::dict Vector2d::getPyDict(void) const
  {
    boost::python::dict retval= ProtoGeom::getPyDict();
    retval["x"]= x();
    retval["y"]= y();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Vector2d::setPyDict(const boost::python::dict &d)
  {
    ProtoGeom::setPyDict(d);
    SetX(boost::python::extract<GEOM_FT>(d["x"]));
    SetY(boost::python::extract<GEOM_FT>(d["y"]));
  }    

void Vector2d::Print(std::ostream &stream) const
  {
    stream << "[[" << x() << "]["
           << y() << "]]";
  }
void Vector2d::Plot(Plotter &plotter) const
  {
    plotter.fline(0,0,x(),y()); //Dibuja una línea.
    plotter.fmarker(x(),y(),7,4.0); //With a triangle at its end.
  }
std::ostream &operator<<(std::ostream &stream,const Vector2d &n)
  {
    n.Print(stream);
    return stream;
  }
bool colineales(const Vector2d &v1,const Vector2d &v2)
  {
    Pos2d p1= Origin2d + v1;
    Pos2d p2= Origin2d + v2;
    return colineales(Origin2d,p1,p2);
  }

//! @brief Return true if vector are parallel.
bool parallel(const Vector2d &v1,const Vector2d &v2)
  { return parallel(v1.getDirection(),v2.getDirection()); } 

double signedAngle(const Vector2d &v1,const Vector2d &v2)
  {
    if( v1.Nulo() || v2.Nulo() )
      {
	std::clog << __FUNCTION__
		  << "(Vector2d,Vector2d) one of the vectors: v1= "
                  << v1 << " or v2= " << v2
		  << " is null. Zero returned." << std::endl;
        return 0.0;
      }
    const GEOM_FT prod_mod= sqrt_FT(v1.GetModulus2()*v2.GetModulus2());
    const GEOM_FT prod_escalar= dot(v1,v2);
    const GEOM_FT coseno= prod_escalar/prod_mod;
    const GEOM_FT seno= -(v1.x()*v2.y()-v1.y()*v2.x())/prod_mod;
    return atan2(seno,coseno);
  }

double angle(const Vector2d &v1,const Vector2d &v2)
  {
    double retval= signedAngle(v1,v2);
    if(retval<0)
      retval+= 2*M_PI;
    return retval;
  }

FT_matrix prod_tensor(const Vector2d &u,const Vector2d &v)
  { return prod_tensor(u.getMatrix(),v.getMatrix()); }
FT_matrix operator&(const Vector2d &u,const Vector2d &v)
  { return prod_tensor(u,v); }






