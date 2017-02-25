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
//MEDGroupInfo

#include "MEDGroupInfo.h"
#include "MEDMeshing.h"

#include "preprocessor/set_mgmt/Set.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "xc_basic/src/med_xc/MEDMEM_Group.hxx"

//! @brief Constructor.
XC::MEDGroupInfo::MEDGroupInfo(MEDMeshing *mesh,const std::string &nmb)
  : nombre(nmb), med_group(nullptr)
  {
    assert(mesh);
    set_owner(mesh);
    indices= mesh->getMapIndicesCeldas();
  }

//! Constructor.
XC::MEDGroupInfo::MEDGroupInfo(MEDMeshing *mesh,const Set &set)
  : nombre(set.GetNombre()), med_group(nullptr)
  { 
    set_owner(mesh);
    indices= mesh->getMapIndicesCeldas();
    const size_t numNodos= set.NumNodos();
    const size_t numElementos= set.NumElementos();
    if(numElementos==0) //node set.
      {
        tipo_entidad= MED_EN::MED_NODE;
	const DqPtrsNode nodos= set.GetNodos();
        const MEDMapIndices &indices_nodos= mesh->getMapIndicesVertices();
        for(Set::nod_const_iterator i= nodos.begin();i!=nodos.end();i++)
          nuevo_vertice((*i)->getTag(),indices_nodos);
      }
    else if(numNodos==0) //Set of elementos.
      {
        tipo_entidad= MED_EN::MED_CELL;   
	const DqPtrsElem elementos= set.GetElementos();
        for(Set::elem_const_iterator i= elementos.begin();i!=elementos.end();i++)
          nueva_celda((*i)->getTag(),(*i)->getMEDCellType());
      }
    else
      std::cerr << "El grupo no puede conterner nodos y celdas simultáneamente."
                << std::endl;
   }

//! Destructor.
XC::MEDGroupInfo::~MEDGroupInfo(void)
  {
    if(med_group)
      {
        med_group->removeReference();
      }
  } 

const XC::MEDMeshing &XC::MEDGroupInfo::getMesh(void) const
  { 
    const MEDMeshing *retval= dynamic_cast<const MEDMeshing *>(Owner());
    return *retval;
  }

MEDMEM::GROUP *XC::MEDGroupInfo::getGrupoMED(void) const
  { return med_group; }

//! @brief Returns the vector con los índices de los elementos
//! being passed as parameter.
std::vector<int> &XC::MEDGroupInfo::getIndicesElementosTipo(const MED_EN::medGeometryElement &tipo)
  {
    map_indices_tipo::iterator i= indices_tipo.find(tipo);
    if(i!=indices_tipo.end())
      return i->second;
    else
      return (indices_tipo[tipo]= std::vector<int>());
  }

//! @brief Agrega un vértice al grupo.
void XC::MEDGroupInfo::nuevo_vertice(size_t tag,const MEDMapIndices &ind)
  {
    tipo_entidad= MED_EN::MED_NODE;
    std::vector<int> &c= getIndicesElementosTipo(MED_EN::MED_NONE);
    const int idx= ind.getMEDIndice(tag);
    c.push_back(idx);
    MEDCellBaseInfo::new_cell(tag,MED_EN::MED_NONE);
  }

//! @brief Agrega una celda al grupo.
void XC::MEDGroupInfo::nueva_celda(size_t tag,const MED_EN::medGeometryElement &tipo)
  {
    tipo_entidad= MED_EN::MED_CELL;
    std::vector<int> &c= getIndicesElementosTipo(tipo);
    const int idx= indices.getMEDIndice(tag);
    c.push_back(idx);
    MEDCellBaseInfo::new_cell(tag,tipo);
  }

//! @brief Returns the nombre del grupo.
const std::string &XC::MEDGroupInfo::getNombre(void) const
  { return nombre; }

//! @brief Returns the vector con las posiciones de los distintos
//! tipos de elemento en el vector de indices de elementos.
std::vector<int> XC::MEDGroupInfo::getVectorIndicesTipos(void) const
  {
    const size_t num_tipos= indices_tipo.size();
    std::vector<int> retval(num_tipos+1);
    int conta= 1; retval[0]= conta; int j= 1;
    for(map_indices_tipo::const_iterator i=indices_tipo.begin();i!=indices_tipo.end();i++,j++)
      {
        conta+= i->first;
        retval[j]= conta;
      }
    return retval;
  }

//! @brief Returns a vector con los índices de los elementos ordenados
//! por tipo.
std::vector<int> XC::MEDGroupInfo::getIndicesElementos(void) const
  {
    std::vector<int> retval;
    for(map_indices_tipo::const_iterator i=indices_tipo.begin();i!=indices_tipo.end();i++)
      retval.insert(retval.end(),i->second.begin(),i->second.end());
    return retval;
  }

//! @brief Dumps group definition on mesh MED.
void XC::MEDGroupInfo::to_med(void) const
  {
    med_group= new MEDMEM::GROUP;
    med_group->setName(nombre);
    const MEDMeshing &mesh= getMesh();
    med_group->setMesh(&mesh.getMEDMesh());
    med_group->setEntity(tipo_entidad);
    MEDCellBaseInfo::to_support_med(*med_group);
    const std::vector<int> index= getVectorIndicesTipos();
    const std::vector<int> value= getIndicesElementos();
    med_group->setNumber(&index[0],&value[0]);
    mesh.getMEDMesh().addGroup(*med_group);
  }

