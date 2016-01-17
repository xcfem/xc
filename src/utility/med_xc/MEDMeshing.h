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
//MEDMeshing.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDMESHING_H
#define MEDMESHING_H

#include "MEDObject.h"
#include "xc_basic/src/med_xc/MEDMEM_Meshing.hxx"
#include "MEDCellInfo.h"
#include "MEDVertexInfo.h"
#include "MEDGroupInfo.h"
#include "MEDFieldInfo.h"
#include <deque>

namespace XC {
class MapSet;
class MapFields;
class MEDDblFieldInfo;
class ProblemaEF;

//! @ingroup MED
//
//!  @brief Envoltorio para el objeto MESHING de MED.
class MEDMeshing: public MEDObject
  {
    const MapSet &sets;
    const MapFields &fields;
    MEDVertexInfo vertices;
    MEDCellInfo cells;
    mutable std::deque<MEDGroupInfo> med_groups;
    mutable std::deque<MEDFieldInfo *> med_fields;

    static const std::string str_grupo_nodos;
    static const std::string str_grupo_elementos;

    mutable MEDMEM::MESHING malla;
    MEDMeshing(const MEDMeshing &);
    MEDMeshing &operator=(const MEDMeshing &);
  protected:
    void nuevo_nodo(size_t i,const std::vector<double> &coo);

    friend class MEDGroupInfo;
    MEDMEM::MESHING &getMallaMED(void) const;
    MEDGroupInfo *getGroupInfo(const std::string &) const;
    MEDGroupInfo *getGroupInfo(const Set &,const FieldInfo &) const;

    void defineMEDGroups(void);
    void defineMEDGaussModels(const Set &,MEDFieldInfo &) const;
    void defineMEDDblField(const Set &,const FieldInfo &,MEDGroupInfo *) const;
    void defineMEDIntField(const Set &,const FieldInfo &,MEDGroupInfo *) const;
    void defineMEDFields(void) const;
    
  public:
    MEDMeshing(const Mesh &,const MapSet &, const MapFields &);
    MEDMeshing(const ProblemaEF &);
    ~MEDMeshing(void);

    void clear(void);
    void write(const std::string &);

    inline const std::string getMeshName(void) const
      { return malla.getName(); }
    inline void setMeshName(const std::string &str) const
      { return malla.setName(str); }
    inline MEDVertexInfo &getVertices(void)
      { return vertices; }
    inline void setVertices(const MEDVertexInfo &vtx)
      { vertices= vtx; }
    inline MEDCellInfo &getCells(void)
      { return cells; }
    inline void setCells(const MEDCellInfo &cls)
      { cells= cls; }  

    const MEDMapIndices &getMapIndicesVertices(void) const;
    const MEDMapIndices &getMapIndicesCeldas(void) const;


    void to_med(void);
  };
} // end of XC namespace
#endif
