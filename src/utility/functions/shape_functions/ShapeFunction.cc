//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
// Copyright (c) 1999, A.H. van den Boogaard

#include "ShapeFunction.h"
#include "IntegrationPoints.h"
#include <iostream>

//! @brief Return the integration point of a segment domain.
IntegrationPoints Segmento::get_integ_points() const
  {
    double w1;
    m_double natcor(1,1);

    IntegrationPoints integ_points(get_nr_integ_points());
    size_t ip = 0;

    for(size_t xip=1; xip<=int_scheme; xip++)
      {
        Gauss(xip, int_scheme, natcor[0], w1);
        integ_points.set_values(++ip, natcor, w1);
      }
    return integ_points;
  }

//! @brief ??
m_double Segmento::dVdA(size_t face_nr) const
  {
    m_double dvda(1,1,0);
    std::cerr << "Segmento::" << __FUNCTION__
              << " not implemented." << std::endl;
    return dvda;
  }

//! @brief Return the integration points for triangular domains.
IntegrationPoints Triangular::get_integ_points() const
  {
    double w;
    m_double Xi(3,1);

    IntegrationPoints integ_points(int_scheme);

    for(size_t ip=1; ip<=get_nr_integ_points(); ip++)
      {
        Hammer(ip, int_scheme, Xi[0], Xi[1], Xi[2], w);
        integ_points.set_values(ip, Xi, w);
      }
    return integ_points;
  }
//! @brief Return the integration points corresponding to a side 
//! of a quadrilateral domain.
IntegrationPoints Segmento::get_face_integ_points(size_t face_nr) const
  {
    std::cerr << "illegal face number in Segmento::get_face_integ_points()" 
              << std::endl;
    IntegrationPoints integ_points(0);
    return integ_points;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Seg_lin::get_N_vec(const m_double &natcor) const
  {
    const double xi = natcor(1,1);

    m_double N(4,1);

    N(1,1) = 0.5*(1-xi);
    N(2,1) = 0.5*(1+xi);

    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Seg_lin::getPartialDerivatives(const m_double &natcor) const
  {
    m_double P(1,2);
    P(1,1) = -0.5;
    P(1,2) =  0.5;
    return P;
  }

//! @brief Return the integration points of a triangular domain.
IntegrationPoints Triangular::get_face_integ_points(size_t face_nr) const
  {
    size_t i=0, j=0, k=0;
    m_double Xi(3,1);

    switch(face_nr)
      {  /* L(i) 0->1, L(j) 1->0, L(k)=0 */
      case 1:
        i=2; j=1; k=3;
        break;
      case 2:
        i=3; j=2; k=1;
        break;
      case 3:
        i=1; j=3; k=2;
        break;
      default:
	std::cerr << "illegal face number in Triangular::get_face_integ_points()" << std::endl;
      }

    size_t nr_line_ip= 0;
    if(int_scheme <= 3)
      nr_line_ip = 2;
    else
      if(int_scheme <= 7)
        nr_line_ip = 3;
      else
        nr_line_ip = 4;

    IntegrationPoints integ_points(nr_line_ip);

    double xi,w,L;
    for(size_t ip=1; ip<=nr_line_ip; ip++)
      {
        Gauss(ip, nr_line_ip, xi, w);
        L = 0.5*(xi+1);
        Xi(i,1) = L;
        Xi(j,1) = 1.0-L;
        Xi(k,1) = 0.0;
        integ_points.set_values(ip, Xi, w);
      }
    return integ_points;
  }

//! @brief ??
m_double Triangular::dVdA(size_t face_nr) const
  {
    m_double dvda(2,1);

    switch(face_nr)
      {
      case 1:
        dvda(1,1) = -0.5;
        dvda(2,1) =  0.5;
        break;
      case 2:
        dvda(1,1) =  0.0;
        dvda(2,1) = -0.5;
        break;
      case 3:
        dvda(1,1) =  0.5;
        dvda(2,1) =  0.0;
        break;
      default:
	std::cerr << "illegal face number in Triangular::dVda()" << std::endl;
      }
    return dvda;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Tri_lin::get_N_vec(const m_double &Xi) const
  {
    const double L1 = Xi[0], L2 = Xi[1], L3 = Xi[2];

    m_double N(3,1);

    N(1) = L1;
    N(2) = L2;
    N(3) = L3;
    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Tri_lin::getPartialDerivatives(const m_double &/*natcor*/) const
  {
    m_double P(2,3);

    P(1,1) =  1.0; P(2,1) =  0.0;
    P(1,2) =  0.0; P(2,2) =  1.0;
    P(1,3) = -1.0; P(2,3) = -1.0;

    return P;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Tri_quad::get_N_vec(const m_double &Xi) const
  {
    const double L1 = Xi[0], L2 = Xi[1], L3 = Xi[2];

    m_double N(6,1);

    N(1,1) = L1*(2*L1 - 1);
    N(2,1) = 4*L1*L2;
    N(3,1) = L2*(2*L2 - 1);
    N(4,1) = 4*L2*L3;
    N(5,1) = L3*(2*L3 - 1);
    N(6,1) = 4*L3*L1;

    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Tri_quad::getPartialDerivatives(const m_double &Xi) const
  {
    const double L1 = Xi[0], L2 = Xi[1];//, L3 = Xi[2];

    m_double P(2,6);

    P(1,1) =  4*L1 - 1;        P(2,1) =  0.0;
    P(1,2) =  4*L2;            P(2,2) =  4*L1;
    P(1,3) =  0.0;             P(2,3) =  4*L2 - 1;
    P(1,4) = -4*L2;            P(2,4) =  4 - 8*L2 - 4*L1;
    P(1,5) = -3 + 4*(L1+L2);   P(2,5) = -3 + 4*(L1+L2);
    P(1,6) =  4 - 8*L1 - 4*L2; P(2,6) = -4*L1;

    return P;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Tri_quad_alt::get_N_vec(const m_double &Xi) const
  {
    const double L1 = Xi[0], L2 = Xi[1], L3 = Xi[2];

    m_double N(6,1);

    N(1,1) = L1*(2*L1 - 1);
    N(4,1) = 4*L1*L2;
    N(2,1) = L2*(2*L2 - 1);
    N(5,1) = 4*L2*L3;
    N(3,1) = L3*(2*L3 - 1);
    N(6,1) = 4*L3*L1;

    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Tri_quad_alt::getPartialDerivatives(const m_double &Xi) const
  {
    const double L1 = Xi[0], L2 = Xi[1];//, L3 = Xi[2];

    m_double P(2,6);

    P(1,1) =  4*L1 - 1;        P(2,1) =  0.0;
    P(1,4) =  4*L2;            P(2,4) =  4*L1;
    P(1,2) =  0.0;             P(2,2) =  4*L2 - 1;
    P(1,5) = -4*L2;            P(2,5) =  4 - 8*L2 - 4*L1;
    P(1,3) = -3 + 4*(L1+L2);   P(2,3) = -3 + 4*(L1+L2);
    P(1,6) =  4 - 8*L1 - 4*L2; P(2,6) = -4*L1;

    return P;
  }

//! @brief Return the integration points of a quadrilateral domain.
IntegrationPoints Quadrilateral::get_integ_points() const
  {
    double w1, w2;
    m_double natcor(2,1);

    IntegrationPoints integ_points(get_nr_integ_points());
    size_t ip = 0;

    for(size_t xip=1; xip<=int_scheme[0]; xip++)
      {
        Gauss(xip, int_scheme[0], natcor[0], w1);
        for(size_t eip=1; eip<= int_scheme[1]; eip++)
          {
            Gauss(eip, int_scheme[1], natcor[1], w2);
            integ_points.set_values(++ip, natcor, w1*w2);
          }
      }
    return integ_points;
  }

//! @brief Return the integration points corresponding to a side of a
// quadrilateral domain.
IntegrationPoints Quadrilateral::get_face_integ_points(size_t face_nr) const
  {
    size_t nr_line_ip=0, i=0;
    m_double Xi(2,1,0.0);

    switch(face_nr)
      {  // i is active local coordinate
      case 1:
        i=1; Xi(2)=-1; nr_line_ip=int_scheme[0];
        break;
      case 2:
        i=2; Xi(1)=1;  nr_line_ip=int_scheme[1];
        break;
      case 3:
        i=1; Xi(2)=1;  nr_line_ip=int_scheme[0];
        break;
      case 4:
        i=2; Xi(1)=-1; nr_line_ip=int_scheme[1];
        break;
      default:
        std::cerr << "illegal face number in Quadrilateral::get_face_integ_points()" << std::endl;
      }

    IntegrationPoints integ_points(nr_line_ip);

    double w;
    for(size_t ip=1; ip<=nr_line_ip; ip++)
      {
        Gauss(ip, nr_line_ip, Xi(i), w);
        integ_points.set_values(ip, Xi, w);
      }
    return integ_points;
  }

//! @brief ??
m_double Quadrilateral::dVdA(size_t face_nr) const
  {
    m_double dvda(2,1);

    switch(face_nr)
      {
      case 1:
        dvda(1,1) =  1.0;
        dvda(2,1) =  0.0;
        break;
      case 2:
        dvda(1,1) =  0.0;
        dvda(2,1) =  1.0;
        break;
      case 3:
        dvda(1,1) = -1.0;
        dvda(2,1) =  0.0;
        break;
      case 4:
        dvda(1,1) =  0.0;
        dvda(2,1) = -1.0;
        break;
      default:
	std::cerr << "illegal face number in Quadrilateral::dVdA()" << std::endl;
      }
    return dvda;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Quad_lin::get_N_vec(const m_double &natcor) const
  {
    const double xi = natcor(1,1), eta=natcor(2,1);

    m_double N(4,1);

    N(1,1) = 0.25*(1-xi)*(1-eta);
    N(2,1) = 0.25*(1+xi)*(1-eta);
    N(3,1) = 0.25*(1+xi)*(1+eta);
    N(4,1) = 0.25*(1-xi)*(1+eta);

    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Quad_lin::getPartialDerivatives(const m_double &natcor) const
  {
    const double xi = natcor(1,1), eta=natcor(2,1);

    m_double P(2,4);

    P(1,1) = -0.25*(1-eta); P(2,1) = -0.25*(1-xi);
    P(1,2) =  0.25*(1-eta); P(2,2) = -0.25*(1+xi);
    P(1,3) =  0.25*(1+eta); P(2,3) =  0.25*(1+xi);
    P(1,4) = -0.25*(1+eta); P(2,4) =  0.25*(1-xi);
    return P;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Quad_quad::get_N_vec(const m_double &natcor) const
  {
    const double xi = natcor(1,1), eta=natcor(2,1);

    m_double N(8,1);

    N(1,1) = -0.25*(1-xi)*(1-eta)*(1+xi+eta);
    N(2,1) = 0.5*(1-xi*xi)*(1-eta);
    N(3,1) = -0.25*(1+xi)*(1-eta)*(1-xi+eta);
    N(4,1) = 0.5*(1+xi)*(1-eta*eta);
    N(5,1) = -0.25*(1+xi)*(1+eta)*(1-xi-eta);
    N(6,1) = 0.5*(1-xi*xi)*(1+eta);
    N(7,1) = -0.25*(1-xi)*(1+eta)*(1+xi-eta);
    N(8,1) = 0.5*(1-xi)*(1-eta*eta);

    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Quad_quad::getPartialDerivatives(const m_double &natcor) const
  {
    const double xi = natcor(1), eta=natcor(2);

    m_double P(2,8);

    P(1,1) = 0.25*(1-eta)*(2*xi+eta); P(2,1) = 0.25*(1-xi)*(2*eta+xi);
    P(1,2) = xi*(eta-1);              P(2,2) = 0.5*(xi*xi-1);
    P(1,3) = 0.25*(1-eta)*(2*xi-eta); P(2,3) = 0.25*(1+xi)*(2*eta-xi);
    P(1,4) = 0.5*(1-eta*eta);         P(2,4) = -eta*(1+xi);
    P(1,5) = 0.25*(1+eta)*(2*xi+eta); P(2,5) = 0.25*(1+xi)*(2*eta+xi);
    P(1,6) = -xi*(1+eta);             P(2,6) = 0.5*(1-xi*xi);
    P(1,7) = 0.25*(1+eta)*(2*xi-eta); P(2,7) = 0.25*(1-xi)*(2*eta-xi);
    P(1,8) = 0.5*(eta*eta-1);         P(2,8) = eta*(xi-1);
    return P;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Quad_quad_alt::get_N_vec(const m_double &natcor) const
  {
    const double xi = natcor(1,1), eta=natcor(2,1);

    m_double N(8,1);

    N(1,1) = -0.25*(1-xi)*(1-eta)*(1+xi+eta);
    N(5,1) = 0.5*(1-xi*xi)*(1-eta);
    N(2,1) = -0.25*(1+xi)*(1-eta)*(1-xi+eta);
    N(6,1) = 0.5*(1+xi)*(1-eta*eta);
    N(3,1) = -0.25*(1+xi)*(1+eta)*(1-xi-eta);
    N(7,1) = 0.5*(1-xi*xi)*(1+eta);
    N(4,1) = -0.25*(1-xi)*(1+eta)*(1+xi-eta);
    N(8,1) = 0.5*(1-xi)*(1-eta*eta);

    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Quad_quad_alt::getPartialDerivatives(const m_double &natcor) const
  {
    const double xi = natcor(1), eta=natcor(2);

    m_double P(2,8);

    P(1,1) = 0.25*(1-eta)*(2*xi+eta); P(2,1) = 0.25*(1-xi)*(2*eta+xi);
    P(1,5) = xi*(eta-1);              P(2,5) = 0.5*(xi*xi-1);
    P(1,2) = 0.25*(1-eta)*(2*xi-eta); P(2,2) = 0.25*(1+xi)*(2*eta-xi);
    P(1,6) = 0.5*(1-eta*eta);         P(2,6) = -eta*(1+xi);
    P(1,3) = 0.25*(1+eta)*(2*xi+eta); P(2,3) = 0.25*(1+xi)*(2*eta+xi);
    P(1,7) = -xi*(1+eta);             P(2,7) = 0.5*(1-xi*xi);
    P(1,4) = 0.25*(1+eta)*(2*xi-eta); P(2,4) = 0.25*(1-xi)*(2*eta-xi);
    P(1,8) = 0.5*(eta*eta-1);         P(2,8) = eta*(xi-1);
    return P;
  }

//! @brief Return the integration points of a hexahedron.
IntegrationPoints Hexaedrico::get_integ_points() const
  {
    double w1, w2, w3;
    m_double natcor(3,1);

    IntegrationPoints integ_points(get_nr_integ_points());
    size_t ip = 0;

    for(size_t xip=1; xip<=int_scheme[0]; xip++)
      {
        Gauss(xip, int_scheme[0], natcor(1), w1);
        for(size_t eip=1; eip<= int_scheme[1]; eip++)
          {
            Gauss(eip, int_scheme[1], natcor(2), w2);
            for(size_t zip=1; zip<=int_scheme[2]; zip++)
              {
                Gauss(zip, int_scheme[2], natcor(3), w3);
                integ_points.set_values(++ip, natcor, w1*w2*w3);
              }
          }
      }
    return integ_points;
  }

//! @brief Devuelve los puntos de una cara de un dominio hexahédrico.
IntegrationPoints Hexaedrico::get_face_integ_points(size_t face_nr) const
  {
    size_t i=0, j=0;
    m_double Xi(3,1,0);

    switch(face_nr)
      {  /* i and j are the active local coordinates */
      case 1:
        i=1; j=3; Xi(2)=-1;
        break;
      case 2:
        i=1; j=3; Xi(2)=+1;
        break;
      case 3:
        i=1; j=2; Xi(3)=-1;
        break;
      case 4:
        i=2; j=3; Xi(1)= 1;
        break;
      case 5:
        i=1; j=2; Xi(3)= 1;
        break;
      case 6:
        i=2; j=3; Xi(1)=-1;
        break;
      default:
	std::cerr << "illegal face number in Hexaedrico::get_face_integ_points()" << std::endl;
      }

    size_t nr_face_ip = int_scheme[i-1]*int_scheme[j-1];
    IntegrationPoints integ_points(nr_face_ip);

    double w1, w2;
    size_t ip=0;
    for(size_t xip=1; xip<=int_scheme[i-1]; xip++)
      {
        Gauss(xip, int_scheme[i-1], Xi(i), w1);
        for(size_t eip=1; eip<=int_scheme[j-1]; eip++)
          {
            Gauss(eip, int_scheme[j-1], Xi(j), w2);
            integ_points.set_values(++ip, Xi, w1*w2);
          }
      }
    return integ_points;
  }

//! @brief ??
m_double Hexaedrico::dVdA(size_t face_nr) const
  {
    m_double dvda(3,2,0.0);

    switch(face_nr)
      {
      case 1:
        dvda(1,1) =  1.0;
        dvda(3,2) =  1.0;
        break;
      case 2:
        dvda(1,1) = -1.0;
        dvda(3,2) =  1.0;
        break;
      case 3:
        dvda(1,1) = -1.0;
        dvda(2,2) =  1.0;
        break;
      case 4:
        dvda(2,1) =  1.0;
        dvda(3,2) =  1.0;
        break;
      case 5:
        dvda(1,1) =  1.0;
        dvda(2,2) =  1.0;
        break;
      case 6:
        dvda(2,1) = -1.0;
        dvda(3,2) =  1.0;
        break;
      default:
	std::cerr << "illegal face number in Hexaedrico::dVdA()" << std::endl;
      }
    return dvda;
  }

//! @brief Return the values of the shape functions at the point
//! with the natural coordinates being passed as parameter.
m_double Hex_lin::get_N_vec(const m_double &natcor) const
  {
    const double xi = natcor(1,1), eta=natcor(2,1), zeta=natcor(3,1);

    m_double N(8,1);

    N(1,1) = 0.125*(1-xi)*(1-eta)*(1-zeta);
    N(2,1) = 0.125*(1+xi)*(1-eta)*(1-zeta);
    N(3,1) = 0.125*(1+xi)*(1+eta)*(1-zeta);
    N(4,1) = 0.125*(1-xi)*(1+eta)*(1-zeta);
    N(5,1) = 0.125*(1-xi)*(1-eta)*(1+zeta);
    N(6,1) = 0.125*(1+xi)*(1-eta)*(1+zeta);
    N(7,1) = 0.125*(1+xi)*(1+eta)*(1+zeta);
    N(8,1) = 0.125*(1-xi)*(1+eta)*(1+zeta);
    return N;
  }

//! @brief Return the values of the partial derivatives of the shape functions
//! at the point wich natural coordinates are being passed as parameter.
//! The (i,j) component of the matrix is equal to d(Nj)/d(xi)
m_double Hex_lin::getPartialDerivatives(const m_double &natcor) const
  {
    const double xi = natcor(1,1), eta=natcor(2,1), zeta=natcor(3,1);

    m_double P(3,8);

    P(1,1) = -0.125*(1-eta)*(1-zeta); P(2,1) = -0.125*(1-xi)*(1-zeta);  P(3,1) = -0.125*(1-xi)*(1-eta);
    P(1,2) =  0.125*(1-eta)*(1-zeta); P(2,2) = -0.125*(1+xi)*(1-zeta);  P(3,2) = -0.125*(1+xi)*(1-eta);
    P(1,3) =  0.125*(1+eta)*(1-zeta); P(2,3) =  0.125*(1+xi)*(1-zeta);  P(3,3) = -0.125*(1+xi)*(1+eta);
    P(1,4) = -0.125*(1+eta)*(1-zeta); P(2,4) =  0.125*(1-xi)*(1-zeta);  P(3,4) = -0.125*(1-xi)*(1+eta);
    P(1,5) = -0.125*(1-eta)*(1+zeta); P(2,5) = -0.125*(1-xi)*(1+zeta);  P(3,5) =  0.125*(1-xi)*(1-eta);
    P(1,6) =  0.125*(1-eta)*(1+zeta); P(2,6) = -0.125*(1+xi)*(1+zeta);  P(3,6) =  0.125*(1+xi)*(1-eta);
    P(1,7) =  0.125*(1+eta)*(1+zeta); P(2,7) =  0.125*(1+xi)*(1+zeta);  P(3,7) =  0.125*(1+xi)*(1+eta);
    P(1,8) = -0.125*(1+eta)*(1+zeta); P(2,8) =  0.125*(1-xi)*(1+zeta);  P(3,8) =  0.125*(1-xi)*(1+eta);
    return P;
  }
