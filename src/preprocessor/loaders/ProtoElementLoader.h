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
//! @brief Objeto encargado de leer elementos finitos desde el archivo de entrada.
//!
//! Se emplea tanto para cargar directamente los elementos del dominio como
//! para cargar la definición del elemento semilla que se empleará en las rutinas
//! de mallado semi-automático. Con este fin se redefine el método agrega en las
//! clases hijas.
class ProtoElementLoader: public Loader
  {
    Element *crea_elemento(const std::string &cmd,int tag_elem);

    std::string nmb_mat; //!< En su caso, nombre del material para el nuevo elemento.
    int num_sec; //!< En su caso, número de secciones (puntos de integración) en elementos lineales.
    int dim_elem; //!< Dimensión, en su caso, para el nuevo elemento. 
    std::string nmb_transf; //!< En su caso, nombre de la transformación para el nuevo elemento.
    std::string nmb_integ; //!< En su caso, nombre del integrador para el nuevo elemento.
    int dir; //!< En su caso (elementos de longitud cero), dirección en la que actúa el material del elemento.
  protected:
    virtual void agrega(Element *)= 0;
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

    Element *nuevoElemento(const std::string &,const ID &);

  };

} // end of XC namespace

#endif
