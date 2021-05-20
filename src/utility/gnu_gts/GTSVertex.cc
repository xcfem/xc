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
//GTSVertex.cc

#include "GTSVertex.h"
#include "GTSSurface.h"
#include "GTSSplit.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include <iostream>

using namespace std;

void GTSVertex::crea(const double &x,const double &y,const double &z)
  {
    if(vt) borra();
    vt = gts_vertex_new(gts_vertex_class(),x,y,z);
    borrar= true;
  }
void GTSVertex::borra(void)
  {
    if(!borrar) return;
    if(vt) gts_object_destroy(GTS_OBJECT(vt));
    vt= nullptr;
    borrar= false;
  }

//! @brief Constructor.
GTSVertex::GTSVertex(GtsVertex *pvt)
  : vt(pvt), borrar(false) {}

//! @brief Constructor.
GTSVertex::GTSVertex(const double &x,const double &y,const double &z)
  : vt(nullptr), borrar(false)
  { crea(x,y,z); }

//! @brief Constructor.
GTSVertex::GTSVertex(const Pos3d &p)
  : vt(nullptr), borrar(false)
  { crea(p.x(),p.y(),p.z()); }

//! @brief Copy constructor.
GTSVertex::GTSVertex(const GTSVertex &other)
  : vt(other.vt), borrar(false)
  {}

//! @brief Assignment operator.
GTSVertex &GTSVertex::operator=(const GTSVertex &other)
  {
    borra();
    vt= other.vt;
    borrar= false;
    return *this;
  }

GTSSplit GTSVertex::GetSplit(const GTSVertex &v1,const GTSVertex &v2)
  { return GTSSplit(*this,v1,v2); }

double GTSVertex::GaussianCurvature(const GTSSurface &surf) const
  {
    double retval;
    bool error= gts_vertex_gaussian_curvature(vt,surf.s,&retval);
    if(error)
      cerr << "GaussianCurvature: error al determinar la aproximación" << endl;
    return retval;
  }
Vector3d GTSVertex::MeanCurvatureNormal(const GTSSurface &surf) const
  {
    GtsVector tmp;
    bool error= gts_vertex_mean_curvature_normal(vt,surf.s,tmp);
    if(error)
      cerr << "MeanCurvatureNormal: error al determinar la aproximación" << endl;
    return Vector3d(tmp[0],tmp[1],tmp[2]);
  }
void GTSVertex::PrincipalCurvatures(const GTSSurface &surf,double &k1,double &k2) const
// Computes the principal curvatures at a point given the mean and Gaussian curvatures at that point.

// The mean curvature can be computed as one-half the magnitude of the vector computed by gts_vertex_mean_curvature_normal().

// The Gaussian curvature can be computed with gts_vertex_gaussian_curvature().

// Kh :	mean curvature.
// Kg :	Gaussian curvature.
// k1 :	first principal curvature.
// k2 :	second principal curvature.
  {
    const double Kg= GaussianCurvature(surf);
    const Vector3d v= MeanCurvatureNormal(surf);
    const double Kh= 0.5*Abs(v);
    gts_vertex_principal_curvatures(Kh,Kg,&k1,&k2);
  }

void GTSVertex::PrincipalDirections(const GTSSurface &surf,Vector3d &e1,Vector3d &e2) const
//Computes the principal curvature directions at a point given Kh and Kg, the mean
//curvature normal and Gaussian curvatures at that point, computed with gts_vertex_mean_curvature_normal() 
//and gts_vertex_gaussian_curvature(), respectively.

// Note that this computation is very approximate and tends to be unstable. Smoothing of the surface or the principal directions may be necessary to achieve reasonable results.

// v :	a GtsVertex.
// s :	a GtsSurface.
// Kh :	mean curvature normal (a GtsVector).
// Kg :	Gaussian curvature (a gdouble).
// e1 :	first principal curvature direction (direction of largest curvature).
// e2 :	second principal curvature direction.
  {
    GtsVector tmp1,tmp2;
    const Vector3d vMCN= MeanCurvatureNormal(surf);
    GtsVector Kh={vMCN.x(),vMCN.y(),vMCN.z()};
    const double Kg= GaussianCurvature(surf);
    gts_vertex_principal_directions(vt,surf.s,Kh,Kg,tmp1,tmp2);
    e1= Vector3d(tmp1[0],tmp1[1],tmp1[2]);
    e2= Vector3d(tmp2[0],tmp2[1],tmp2[2]);
  }

Pos3d GTSVertex::Posicion(void) const
  { return Pos3d(vt->p.x,vt->p.y,vt->p.z); }

GTSVertex::~GTSVertex(void)
  { borra(); }

