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
  : nombre(set.getName()), med_group(nullptr)
  { 
    set_owner(mesh);
    indices= mesh->getMapIndicesCeldas();
    const size_t numNodes= set.getNumberOfNodes();
    const size_t numElements= set.getNumberOfElements();
    if(numElements==0) //node set.
      {
        tipo_entidad= MED_EN::MED_NODE;
	const DqPtrsNode nodes= set.getNodes();
        const MEDMapIndices &node_indexes= mesh->getMapIndicesVertices();
        for(Set::nod_const_iterator i= nodes.begin();i!=nodes.end();i++)
          new_vertice((*i)->getTag(),node_indexes);
      }
    else if(numNodes==0) //element set.
      {
        tipo_entidad= MED_EN::MED_CELL;   
	const DqPtrsElem elements= set.getElements();
        for(Set::elem_const_iterator i= elements.begin();i!=elements.end();i++)
          nueva_celda((*i)->getTag(),(*i)->getMEDCellType());
      }
    else
      std::cerr << "Group cannot contain nodes and cells at the same time."
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

//! @brief Returns the vector with the indexes of the elements
//! that match the type being passed as parameter.
std::vector<int> &XC::MEDGroupInfo::getElementOfTypeIndices(const MED_EN::medGeometryElement &tipo)
  {
    map_indices_tipo::iterator i= indices_tipo.find(tipo);
    if(i!=indices_tipo.end())
      return i->second;
    else
      return (indices_tipo[tipo]= std::vector<int>());
  }

//! @brief Appends a vertex to the group.
void XC::MEDGroupInfo::new_vertice(size_t tag,const MEDMapIndices &ind)
  {
    tipo_entidad= MED_EN::MED_NODE;
    std::vector<int> &c= getElementOfTypeIndices(MED_EN::MED_NONE);
    const int idx= ind.getMEDIndice(tag);
    c.push_back(idx);
    MEDCellBaseInfo::new_cell(tag,MED_EN::MED_NONE);
  }

//! @brief Appends a cell to the group.
void XC::MEDGroupInfo::nueva_celda(size_t tag,const MED_EN::medGeometryElement &tipo)
  {
    tipo_entidad= MED_EN::MED_CELL;
    std::vector<int> &c= getElementOfTypeIndices(tipo);
    const int idx= indices.getMEDIndice(tag);
    c.push_back(idx);
    MEDCellBaseInfo::new_cell(tag,tipo);
  }

//! @brief Returns the nombre del grupo.
const std::string &XC::MEDGroupInfo::getNombre(void) const
  { return nombre; }

//! @brief Returns the vector with the positions of the different element types
//! int the vector of element indexes.
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

//! @brief Returns a vector with the indexes of the elements ordered
//! by type.
std::vector<int> XC::MEDGroupInfo::getElementIndices(void) const
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
    const std::vector<int> value= getElementIndices();
    med_group->setNumber(&index[0],&value[0]);
    mesh.getMEDMesh().addGroup(*med_group);
  }

