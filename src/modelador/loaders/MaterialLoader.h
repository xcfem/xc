//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MaterialLoader.h

#ifndef MATERIALLOADER_H
#define MATERIALLOADER_H

#include "Loader.h"
#include <map>

namespace XC {
class Material;
class GeomSection;
class DiagInteraccion;
class DiagInteraccion2d;
class DatosDiagInteraccion;

//!  \ingroup Ldrs
//! 
//! @brief Objeto encargado de la definición de materiales.
class MaterialLoader: public Loader
  {
  public:
    typedef std::map<std::string,Material *> map_materiales;
    typedef map_materiales::const_iterator const_iterator;
    typedef map_materiales::iterator iterator;

    typedef std::map<std::string,GeomSection *> map_geom_secc;
    typedef map_geom_secc::const_iterator const_geom_secc_iterator;
    typedef map_geom_secc::iterator geom_secc_iterator;

    typedef std::map<std::string,DiagInteraccion *> map_diag_interaccion;
    typedef map_diag_interaccion::const_iterator const_diag_interacc_iterator;
    typedef map_diag_interaccion::iterator diag_interacc_iterator;

    typedef std::map<std::string,DiagInteraccion2d *> map_diag_interaccion2d;
    typedef map_diag_interaccion2d::const_iterator const_diag_interacc2d_iterator;
    typedef map_diag_interaccion2d::iterator diag_interacc2d_iterator;
  private:
    map_materiales materiales; //!< Materiales para el problema.
    int tag_mat; //!< Tag por defecto para el material.
    map_geom_secc geom_secciones; //!< Geometrías de secciones.
    map_diag_interaccion diagramas_interaccion; //!< Diagramas de interacción de secciones.
    map_diag_interaccion2d diagramas_interaccion2d; //!< Diagramas de interacción de secciones.
  protected:
    friend class ElementLoader;
    void ejecuta_bloque_for_each(CmdStatus &,const std::string &);
  public:
    MaterialLoader(Modelador *owr);
    const map_materiales &Map(void) const;
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
    DiagInteraccion *find_ptr_diag_interaccion(const std::string &nmb);
    const DiagInteraccion *find_ptr_diag_interaccion(const std::string &nmb) const;
    DiagInteraccion2d *find_ptr_diag_interaccion2d(const std::string &nmb);
    const DiagInteraccion2d *find_ptr_diag_interaccion2d(const std::string &nmb) const;
    bool existeMaterial(const std::string &nmb) const;
    bool existeGeomSection(const std::string &nmb) const;
    bool existeDiagInteraccion(const std::string &nmb) const;
    bool existeDiagInteraccion2d(const std::string &nmb) const;
    Material *nuevoMaterial(const std::string &,const std::string &);
    Material &getMaterial(const std::string &);
    GeomSection *newSectionGeometry(const std::string &);
    GeomSection &getGeomSection(const std::string &);
    DiagInteraccion *newInteractionDiagram(const std::string &);
    DiagInteraccion *calcInteractionDiagram(const std::string &,const DatosDiagInteraccion &datos_diag);
    DiagInteraccion &getDiagInteraccion(const std::string &);
    DiagInteraccion2d *new2DInteractionDiagram(const std::string &);
    DiagInteraccion2d *calcInteractionDiagramNMy(const std::string &,const DatosDiagInteraccion &datos_diag);
    DiagInteraccion2d *calcInteractionDiagramNMz(const std::string &,const DatosDiagInteraccion &datos_diag);
    DiagInteraccion2d &getDiagInteraccionNMz(const std::string &);
    ~MaterialLoader(void);
    void clearAll(void);

  };

} // fin namespace XC

#endif
