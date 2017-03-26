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
//UniformGrid.cc

#include "UniformGrid.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

//! @brief Constructor.
XC::UniformGrid::UniformGrid(Preprocessor *m,const size_t &i)
  : EntMdlr(m,i),Lx(0.0),Ly(0.0),Lz(0.0),ndiv_x(0),ndiv_y(0),ndiv_z(0)
  {}

//! @brief Constructor.
//! @param nombre: Object identifier.
//! @param m: Pointer to preprocesador.
//! @param i: Índice para gráficos.
XC::UniformGrid::UniformGrid(const std::string &nombre,Preprocessor *m,const size_t &i)
  : EntMdlr(nombre,i,m),Lx(0.0),Ly(0.0),Lz(0.0),ndiv_x(0),ndiv_y(0),ndiv_z(0)
  {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::UniformGrid::getCopy(void) const
  { return new UniformGrid(*this); }

void XC::UniformGrid::actualiza_topologia(void)
  {
  }

//! @brief Returns mesh dimension (1,2 ó 3).
unsigned short int XC::UniformGrid::GetDimension(void) const
  {
    unsigned short int retval= 0;
    if(ndiv_x > 0) retval++;
    if(ndiv_y > 0) retval++;
    if(ndiv_z > 0) retval++;
    return retval;
  }

//! @brief Returns mesh BND.
BND3d XC::UniformGrid::Bnd(void) const
  {
    Vector3d v(Lx,Ly,Lz);
    return BND3d(org,org+v);
  }

//! @brief Returns mesh centroid.
Pos3d XC::UniformGrid::GetCentro(void) const
  { return Pos3d(org.x()+Lx/2,org.y()+Ly/2,org.z()+Lz/2); }

//! @brief Returns the sets a los que pertenece este objeto.
std::set<XC::SetBase *> XC::UniformGrid::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "UniformGrid::get_sets; no se ha definido el preprocesador." << std::endl;
    return retval;
  }

//! @brief Agrega el «uniform grid» a the sets being passed as parameters.
void XC::UniformGrid::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->GetUniformGrids().push_back(this);
      }
  }

//! @brief Creates mesh nodes.
void XC::UniformGrid::crea_nodos(void)
  {
    TritrizPos3d ptos= crea_uniform_grid(Bnd(),ndiv_x,ndiv_y,ndiv_z);
    EntMdlr::crea_nodos(ptos);
  }

//! @brief Triggers mesh creation.
void XC::UniformGrid::genMesh(meshing_dir dm)
  {
    crea_nodos();
    crea_elementos(dm);
  }



