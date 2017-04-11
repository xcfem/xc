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
//ProtoElementLoader.h

#ifndef PROTOELEMENTLOADER_H
#define PROTOELEMENTLOADER_H

#include "Loader.h"
#include "MaterialLoader.h"
#include "TransfCooLoader.h"
#include "BeamIntegratorLoader.h"
#include <map>

namespace XC {
class Element;

//!  \ingroup Ldrs
//! 
//! @brief Finite element creation.
//!
//! This objetct allows the creation of finite element obbjets. It can be done
//! directly (specifiying the finite element type, material, nodes,...) creating
//! one element after another or you can specify a "seed element" that will be
//! used by the meshing routines.
class ProtoElementLoader: public Loader
  {
    Element *create_element(const std::string &cmd,int tag_elem);

    std::string nmb_mat; //!< When appropriate, material name for the new element.
    int num_sec; //!< When appropriate, number of sections (integration points) on one-dimensional elements.
    int dim_elem; //!< If required, dimension of the new element (0,1,2 or 3)
    std::string nmb_transf; //!< If required, transformation identifier for the new element.
    std::string nmb_integ; //!< If required, integrator identifier for the new element.
    int dir; //!< If required (i.e. for zero length elements), direction of the element material.
  protected:
    virtual void add(Element *)= 0;
    MaterialLoader &get_material_loader(void) const;
    MaterialLoader::const_iterator get_iter_material(void) const;
    Material *get_ptr_material(void) const;
    TransfCooLoader &get_transf_coo_loader(void) const;
    TransfCooLoader::const_iterator get_iter_transf_coo(void) const;
    CrdTransf *get_ptr_transf_coo(void) const;
    BeamIntegratorLoader &get_beam_integrator_loader(void) const;
    BeamIntegratorLoader::const_iterator get_iter_beam_integrator(void) const;
    BeamIntegration *get_ptr_beam_integrator(void) const;

    friend class Preprocessor;

  public:
    ProtoElementLoader(Preprocessor *preprocessor);
    virtual int getDefaultTag(void) const= 0;

    void setDefaultMaterial(const std::string &);
    const std::string &getDefaultMaterial(void) const;
    void setNumSections(const int &);
    int getNumSections(void) const;
    void setDimElem(const int &);
    int getDimElem(void) const;
    void setDefaultTransf(const std::string &);
    const std::string &getDefaultTransf(void) const;
    void setDefaultIntegrator(const std::string &);
    const std::string &getDefaultIntegrator(void) const;

    Element *newElement(const std::string &,const ID &);

  };

} // end of XC namespace

#endif
