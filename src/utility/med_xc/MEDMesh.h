//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDMesh.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDMESH_H
#define MEDMESH_H

#include "MEDObject.h"
#include "xc_basic/src/med_xc/MEDMEM_Mesh.hxx"

namespace XC {
  class Matrix;
//! @ingroup MED
//
//!  @brief Envoltorio para el objeto MESHING de MED.
class MEDMesh: public MEDObject
  {
    std::string meshName;
    mutable MEDMEM::MESH *malla;
    void libera(void) const;
    void alloc(const std::string &) const;
    void alloc(const MEDMesh &) const;
    MEDMesh(const MEDMesh &);
    MEDMesh &operator=(const MEDMesh &);
  protected:

  public:
    MEDMesh(void);
    ~MEDMesh(void);
    inline std::string getMeshName(void) const
      { return meshName; }
    inline void setMeshName(const std::string &s)
      { meshName= s; }

    size_t getSpaceDimension(void);
    size_t getMeshDimension(void);
    size_t getNumberOfNodes(void);
    boost::python::list getCoordinatesNames(void);
    boost::python::list getCoordinatesUnits(void);
    Matrix getCoordinates(void);
    size_t getNumberOfCellTypes();
    boost::python::list getCellTypes(void);
    boost::python::list getCellTypeNames(void);
    size_t getNumCellsOfType(int tipo);
    Matrix getConnectivityCellsOfType(int tipo);
    size_t getNumberOfGroups(int tipo_entidad= MED_EN::MED_ALL_ENTITIES);
    size_t getNumberOfFamilies(int tipo_entidad= MED_EN::MED_ALL_ENTITIES);

    void read(const std::string &);
  };
} // end of XC namespace
#endif
