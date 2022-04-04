// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//ShapeFunction.h
// ORIGINAL: shape.h Copyright (c) 1999, A.H. van den Boogaard
//FeaTure is put on the internet as free software. I would very 
//much like to hear your feedback and if possible improvements. See 
//also the 'Future plans' part in the manual.
//Modified by LCPT to integrate it in XC.

#ifndef SHAPEFUNCTION_H
#define SHAPEFUNCTION_H

#include <cstddef>
#include "utility/matrices/m_double.h"

//!
//! \ingroup Elems
//!
//! \defgroup ShapeFunctionsGroup Shape functions.


class IntegrationPoints;

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Base class for shape functions.
class ShapeFunction
  {
  protected:
    //! @brief Constructor por defecto
    ShapeFunction(){}

  public:
    //! Destructor.
    virtual ~ShapeFunction(){}
    //! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
    virtual m_double get_N_vec(const m_double &natcor) const=0;
    //! @brief Return the values of the partial derivatives of the shape
    //! functions at the point which natural coordinates are being passed as
    //! parameter. The (i,j) component of the matrix is equal to d(Nj)/d(xi)
    virtual m_double getPartialDerivatives(const m_double &natcor) const=0;
    //! @brief Return the integration points for the shape functions domain.
    virtual IntegrationPoints get_integ_points(void) const=0;
    //! @brief Return the integration points for the shape functions domain.
    virtual size_t get_nr_integ_points() const=0;

    //! @brief Return the integration points for a side (2D domains) or a
    //! face (3D domains) of the shape functions.
    virtual IntegrationPoints get_face_integ_points(size_t face_nr) const=0;
    //! ??
    virtual m_double dVdA(size_t face_nr) const=0;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Clase base para dominios de tipo segmento.
class Segmento: public ShapeFunction
  {
  private:
    size_t int_scheme;//!< Number of integration points.

  protected:
    //! @brief Constructor.
    Segmento(size_t intscheme)
      :int_scheme(intscheme){}
  public:
    //! @brief Destructor.
    ~Segmento(){}
    IntegrationPoints get_integ_points(void) const;
    //! @brief Return the number of integration points.
    size_t get_nr_integ_points(void) const { return int_scheme; }

    m_double dVdA(size_t face_nr) const;
    IntegrationPoints get_face_integ_points(size_t face_nr) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Functiones de forma para segmento lineal (2 nodos).
class Seg_lin:public Segmento
  {
  public:
    //! @brief Constructor
    Seg_lin(size_t nxi)
      : Segmento(nxi){}
    //! @brief Destructor
    ~Seg_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Clase base para dominios triangulares.
class Triangular: public ShapeFunction
  {
  private:
    size_t int_scheme;//!< Number of integration points.

  protected:
    //! @brief Constructor.
    Triangular(size_t intscheme)
      :int_scheme(intscheme){}
  public:
    //! @brief Destructor.
    ~Triangular(){}
    IntegrationPoints get_integ_points(void) const;
    //! @brief Return the number of integration points.
    size_t get_nr_integ_points(void) const { return int_scheme; }

    m_double dVdA(size_t face_nr) const;
    IntegrationPoints get_face_integ_points(size_t face_nr) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for linear triangles (3 nodes).
class Tri_lin:public Triangular
  {
  public:
    //! @brief Constructor.
    Tri_lin(size_t intscheme)
      : Triangular(intscheme){}
    //! @brief Destructor.
    ~Tri_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for quadratic triangles (6 nodes).
class Tri_quad:public Triangular
  {
  public:
    //! @brief Constructor
    Tri_quad(size_t intscheme)
      : Triangular(intscheme){}
    //! @brief Destructor
    ~Tri_quad(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for another quadratic triangle (6 nodes).
class Tri_quad_alt:public Triangular
  {
  public:
    //! @brief Constructor
    Tri_quad_alt(size_t intscheme):Triangular(intscheme){}
    //! @brief Destructor
    ~Tri_quad_alt(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Base class for quadrilateral domains.
class Quadrilateral:public ShapeFunction
  {
  private:
    size_t int_scheme[2];

  protected:
    //! @brief Constructor
    Quadrilateral(size_t nxi, size_t neta)
      { int_scheme[0]=nxi; int_scheme[1]=neta; }

  public:
    //! @brief Destructor
    ~Quadrilateral(){}
    IntegrationPoints get_integ_points() const;
    //! @brief Return the number of integration points.
    size_t get_nr_integ_points() const
      { return int_scheme[0]*int_scheme[1]; }

    m_double dVdA(size_t face_nr) const;
    IntegrationPoints get_face_integ_points(size_t face_nr) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for linear quads (4 nodes).
class Quad_lin:public Quadrilateral
  {
  public:
    //! @brief Constructor
    Quad_lin(size_t nxi, size_t neta)
      : Quadrilateral(nxi, neta){}
    //! @brief Destructor
    ~Quad_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for quadratic quads (8 nodes).
class Quad_quad:public Quadrilateral
  {
  public:
    //! @brief Constructor
    Quad_quad(size_t nxi, size_t neta)
      : Quadrilateral(nxi, neta){}
    //! @brief Destructor
    ~Quad_quad(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for another quadratic quad (8 nodes).
class Quad_quad_alt:public Quadrilateral
  {
  public:
    //! @brief Constructor
    Quad_quad_alt(size_t nxi, size_t neta)
      : Quadrilateral(nxi, neta){}
    //! @brief Destructor
    ~Quad_quad_alt(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Base class for shape functions over hexahedrons.
class Hexaedrico:public ShapeFunction
  {
  private:
    size_t int_scheme[3];

  protected:
    //! @brief Constructor
    Hexaedrico(size_t nxi, size_t neta, size_t nzeta)
      { int_scheme[0]=nxi; int_scheme[1]=neta; int_scheme[2]=nzeta; }

  public:
    //! @brief Destructor
    ~Hexaedrico(){}
    IntegrationPoints get_integ_points() const;
    //! @brief Return the numberof integration points.
    size_t get_nr_integ_points() const
      { return int_scheme[0]*int_scheme[1]*int_scheme[2]; }

    m_double dVdA(size_t face_nr) const;
    IntegrationPoints get_face_integ_points(size_t face_nr) const;
  };

//! @ingroup ShapeFunctionsGroup
//!
//! @brief Shape functions for linear hexahedrons (8 nodes).
class Hex_lin:public Hexaedrico
  {
  public:
    //! @brief Constructor.
    Hex_lin(size_t nxi, size_t neta, size_t nzeta)
      : Hexaedrico(nxi, neta, nzeta){}
    //! @brief Destructor.
    ~Hex_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

#endif // SHAPEFUNCTION_H
