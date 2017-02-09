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
//MaterialLoader.h

#ifndef MATERIALLOADER_H
#define MATERIALLOADER_H

#include "Loader.h"
#include <map>

namespace XC {
class Material;
class GeomSection;
class InteractionDiagram;
class InteractionDiagram2d;
class InteractionDiagramData;

//!  \ingroup Ldrs
//! 
//! @brief Material handler (definition, searching,...).
class MaterialLoader: public Loader
  {
  public:
    typedef std::map<std::string,Material *> map_materials;
    typedef map_materials::const_iterator const_iterator;
    typedef map_materials::iterator iterator;

    typedef std::map<std::string,GeomSection *> map_geom_secc;
    typedef map_geom_secc::const_iterator const_geom_secc_iterator;
    typedef map_geom_secc::iterator geom_secc_iterator;

    typedef std::map<std::string,InteractionDiagram *> map_interaction_diagram;
    typedef map_interaction_diagram::const_iterator const_diag_interacc_iterator;
    typedef map_interaction_diagram::iterator diag_interacc_iterator;

    typedef std::map<std::string,InteractionDiagram2d *> map_interaction_diagram2d;
    typedef map_interaction_diagram2d::const_iterator const_diag_interacc2d_iterator;
    typedef map_interaction_diagram2d::iterator diag_interacc2d_iterator;
  private:
    map_materials materials; //!< Material definitions.
    int tag_mat; //!< Tag por defecto para el material.
    map_geom_secc geom_secciones; //!< Section geometries.
    map_interaction_diagram diagramas_interaccion; //!< 3D interaction diagrams.
    map_interaction_diagram2d diagramas_interaccion2d; //!< 2D interaction diagrams.
  protected:
    friend class ElementLoader;
  public:
    MaterialLoader(Preprocessor *owr);
    const map_materials &Map(void) const;
    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);
    const_iterator find(const std::string &str) const;
    iterator find(const std::string &str);
    Material *find_ptr(const std::string &str);
    const Material *find_ptr(const std::string &nmb) const;
    Material *find_ptr(const int &tag);
    const Material *find_ptr(const int &tag) const;
    GeomSection *find_ptr_geom_section(const std::string &nmb);
    const GeomSection *find_ptr_geom_section(const std::string &nmb) const;
    InteractionDiagram *find_ptr_interaction_diagram(const std::string &nmb);
    const InteractionDiagram *find_ptr_interaction_diagram(const std::string &nmb) const;
    InteractionDiagram2d *find_ptr_interaction_diagram2d(const std::string &nmb);
    const InteractionDiagram2d *find_ptr_interaction_diagram2d(const std::string &nmb) const;
    bool existeMaterial(const std::string &nmb) const;
    bool existeGeomSection(const std::string &nmb) const;
    bool InteractionDiagramExists(const std::string &nmb) const;
    bool InteractionDiagramExists2d(const std::string &nmb) const;
    Material *nuevoMaterial(const std::string &,const std::string &);
    Material &getMaterial(const std::string &);
    GeomSection *newSectionGeometry(const std::string &);
    GeomSection &getGeomSection(const std::string &);
    InteractionDiagram *newInteractionDiagram(const std::string &);
    InteractionDiagram *calcInteractionDiagram(const std::string &,const InteractionDiagramData &datos_diag);
    InteractionDiagram &getInteractionDiagram(const std::string &);
    InteractionDiagram2d *new2DInteractionDiagram(const std::string &);
    InteractionDiagram2d *calcInteractionDiagramNMy(const std::string &,const InteractionDiagramData &datos_diag);
    InteractionDiagram2d *calcInteractionDiagramNMz(const std::string &,const InteractionDiagramData &datos_diag);
    InteractionDiagram2d &getNMzInteractionDiagram(const std::string &);
    ~MaterialLoader(void);
    void clearAll(void);

  };

} // end of XC namespace

#endif
