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
