//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDGroupInfo.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

#ifndef MEDGROUPINFO_H
#define MEDGROUPINFO_H

#include "MEDCellBaseInfo.h"

namespace XC {
class Set;
class MEDMeshing;

//! @ingroup MED
//
//!  @brief Información sobre conjuntos de nodos y elementos.
class MEDGroupInfo: public MEDCellBaseInfo
  {
  public:
    typedef std::map<int,std::vector<int> > map_indices_tipo;
  private:
    std::string nombre; //!< nombre del conjunto.
    map_indices_tipo indices_tipo; //!< índices de los elementos por tipo. 
    MED_EN::medEntityMesh tipo_entidad; //!< tipo de entidad que almacena el conjunto (node,cell,face,edge,...).
    mutable MEDMEM::GROUP *med_group; //!< Definición del grupo en MEDMEM.
  protected:
    friend class MEDMeshing;
    friend class MEDFieldInfo;
    MEDGroupInfo(MEDMeshing *malla,const std::string &);
    MEDGroupInfo(MEDMeshing *malla,const Set &);

    std::vector<int> &getIndicesElementosTipo(const MED_EN::medGeometryElement &);
    std::vector<int> getVectorIndicesTipos(void) const;
    std::vector<int> getIndicesElementos(void) const;

    const MEDMeshing &getMalla(void) const;
    MEDMEM::GROUP *getGrupoMED(void) const;
  public:
    ~MEDGroupInfo(void);
    const std::string &getNombre(void) const;

    void nuevo_vertice(size_t i,const MEDMapIndices &);
    void nueva_celda(size_t i,const MED_EN::medGeometryElement &);
    void to_med(void) const;
  };
} // end of XC namespace
#endif
