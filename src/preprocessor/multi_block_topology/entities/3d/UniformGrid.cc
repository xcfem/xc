//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos3dArray3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

//! @brief Constructor.
XC::UniformGrid::UniformGrid(Preprocessor *m,const size_t &i)
  : EntMdlr(m,i),Lx(0.0),Ly(0.0),Lz(0.0),ndiv_x(0),ndiv_y(0),ndiv_z(0)
  {}

//! @brief Constructor.
//! @param name: Object identifier.
//! @param m: Pointer to preprocessor.
//! @param i: index for VTK routines.
XC::UniformGrid::UniformGrid(const std::string &name,Preprocessor *m,const size_t &i)
  : EntMdlr(name,i,m),Lx(0.0),Ly(0.0),Lz(0.0),ndiv_x(0),ndiv_y(0),ndiv_z(0)
  {}

//! @brief Comparison operator.
bool XC::UniformGrid::operator==(const UniformGrid &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= EntMdlr::operator==(other);
        if(retval)
          retval= (Lx==other.Lx);
        if(retval)
          retval= (Ly==other.Ly);
        if(retval)
          retval= (Lz==other.Lz);
        if(retval)
          retval= (ndiv_x==other.ndiv_x);
        if(retval)
          retval= (ndiv_y==other.ndiv_y);
        if(retval)
          retval= (ndiv_z==other.ndiv_z);
       } 
    return retval;
  }

//! @brief Virtual constructor.
XC::SetEstruct *XC::UniformGrid::getCopy(void) const
  { return new UniformGrid(*this); }

void XC::UniformGrid::update_topology(void)
  {
  }

//! @brief Returns mesh dimension (1,2 or 3).
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
Pos3d XC::UniformGrid::getCenter(void) const
  { return Pos3d(org.x()+Lx/2,org.y()+Ly/2,org.z()+Lz/2); }

//! @brief Returns the sets to which this object belongs.
std::set<XC::SetBase *> XC::UniformGrid::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; preprocessor needed." << std::endl;
    return retval;
  }

//! @brief Adds the uniform grid to the set being passed as parameter.
void XC::UniformGrid::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->getUniformGrids().push_back(this);
      }
  }

//! @brief Creates mesh nodes.
void XC::UniformGrid::create_nodes(void)
  {
    Pos3dArray3d ptos= create_uniform_grid(Bnd(),ndiv_x,ndiv_y,ndiv_z);
    EntMdlr::create_nodes(ptos);
  }

//! @brief Triggers mesh creation.
void XC::UniformGrid::genMesh(meshing_dir dm)
  {
    create_nodes();
    create_elements(dm);
  }



