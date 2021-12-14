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
//SolidExtruPolygon3d.h

#ifndef SOLIDEXTRUPLGNO3D_H
#define SOLIDEXTRUPLGNO3D_H

#include "SolidExtru3d.h"

//! @ingroup GEOM
//
//! @brief Sólido de extrusión generado por un polígono.
template<class PG>
class SolidExtruPolygon3d : public SolidExtru3d<PG>
  {
  public:
    typedef SolidExtru3d<PG> solid_extru_pg;
    inline unsigned int get_num_vertices_plgno(void) const
      { return this->scc.getNumVertices(); }
  public:
    SolidExtruPolygon3d(void) {}
    SolidExtruPolygon3d(const PG &secc,const GEOM_FT &lng): solid_extru_pg(secc,lng) {}

    virtual GeomObj *getCopy(void) const
      { return new SolidExtruPolygon3d<PG>(*this); }
    virtual unsigned int getNumVertices(void) const
      { return 2*get_num_vertices_plgno(); }
    virtual unsigned int getNumVerticesFaceta(unsigned int faceta) const;
    inline virtual unsigned int getNumEdges(void) const
      { return 3*this->scc.getNumEdges(); }
    inline virtual unsigned int GetNumFacetas(void) const
      { return 2+this->scc.getNumEdges(); }
    Pos3d Vertice(unsigned int i) const;
/*     virtual Polyhedron::v_ind_vertices IndVerticesArista(unsigned int arista) const; */
/*     virtual Polyhedron::v_ind_vertices IndVerticesFaceta(unsigned int faceta) const; */
    inline BND3d Bnd(void) const
    //Return la extension del objeto.
      { return solid_extru_pg::Bnd(); }
    inline FT_matrix I(void) const
      { return solid_extru_pg::I(); }
    inline Pos3d getCenterOfMass(void) const
      { return solid_extru_pg::getCenterOfMass(); }
    
  };

template <class PG>
unsigned int SolidExtruPolygon3d<PG>::getNumVerticesFaceta(unsigned int faceta) const
  {
    unsigned int retval= 4;
    unsigned int nf= GetNumFacetas();
    unsigned int ifc= faceta%nf;
    if((ifc==0)||(ifc==(nf-1))) retval= get_num_vertices_plgno();
    return retval;
  }

template <class PG>
Pos3d SolidExtruPolygon3d<PG>::Vertice(unsigned int i) const
  {
    Pos3d p;
    const unsigned int num_vert_plgno= get_num_vertices_plgno();
    unsigned int j=i;
    if(i>num_vert_plgno) j-=num_vert_plgno;
    if(i<num_vert_plgno)
      p= this->GetBase(0).Vertice(j);
    else
      p= this->GetBase(1).Vertice(j);
    return p;
  }

/* template <class PG> */
/* Polyhedron::v_ind_vertices SolidExtruPolygon3d<PG>::IndVerticesArista(unsigned int arista) const */
/*   { */
/*     const unsigned int num_edges= getNumEdges(); */
/*     Polyhedron::v_ind_vertices va(2); */
/*     const unsigned int indice= arista%num_edges; */
/*     const unsigned int num_vert_plgno= get_num_vertices_plgno(); */
/*     if(indice<num_vert_plgno) //Si es una arista de la base */
/*       { */
/*         va[0]= indice; va[1]= indice + 1; */
/*         if(va[1]>=num_vert_plgno) va[1]= 0; //Cierra con el primer vértice de la base. */
/*         return va; */
/*       } */
/*     if(indice>=(2*num_vert_plgno)) //Si es una arista de la tapa. */
/*       { */
/*         va[0]= indice-num_vert_plgno; */
/*         va[1]= va[0] + 1; */
/*         if(va[1]>=2*num_vert_plgno) va[1]= num_vert_plgno; //Cierra con el primer vértice de la tapa. */
/*         return va; */
/*       } */
/* 	//En other caso, es una arista lateral. */
/*     va[1]= indice; */
/*     va[0]= indice - num_vert_plgno; */
/*     return va; */
/*   } */
/* template <class PG> */
/* Polyhedron::v_ind_vertices SolidExtruPolygon3d<PG>::IndVerticesFaceta(unsigned int faceta) const */
/*   { */
/*     const unsigned int num_facetas= GetNumFacetas(); */
/*     const unsigned int num_vert_plgno= get_num_vertices_plgno(); */
/*     const unsigned int nv= getNumVertices(); */
/*     const unsigned int ifac= faceta%num_facetas; */
/*     if(ifac == 0) //Es la base. */
/*       { */
/* 	    Polyhedron::v_ind_vertices iv(num_vert_plgno); */
/*         for(size_t i= 0;i<num_vert_plgno;i++) */
/*           iv[i]= i; */
/*         return iv; */
/*       } */
/*     if(ifac == num_facetas-1) //Es la tapa. */
/*       { */
/* 	    Polyhedron::v_ind_vertices iv(num_vert_plgno); */
/*         for(size_t i= nv-1,j=0;i>=num_vert_plgno;i--,j++) */
/*           iv[j]= i; */
/*         return iv; */
/*       } */
/*     //Es un lateral. */
/*     Polyhedron::v_ind_vertices iv(4); */
/*     iv[0]= ifac-1; //En la base. */
/*     iv[1]= iv[0]+num_vert_plgno; //En la tapa. */
/*     iv[2]= iv[1]+1; //En la tapa. */
/*     if(iv[2]==nv) iv[2]= num_vert_plgno; */
/*     iv[3]= iv[0]+1; //En la base. */
/*     if(iv[3]==num_vert_plgno) iv[3]= 0; */
/*     return iv; */
/*   } */


#endif
