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
//MEDGroupInfo

#include "MEDGroupInfo.h"
#include "MEDMeshing.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "preprocessor/set_mgmt/Set.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "xc_basic/src/med_xc/MEDMEM_Group.hxx"

//! @brief Constructor.
XC::MEDGroupInfo::MEDGroupInfo(MEDMeshing *malla,const std::string &nmb)
  : nombre(nmb), med_group(nullptr)
  {
    assert(malla);
    set_owner(malla);
    indices= malla->getMapIndicesCeldas();
  }

//! Constructor.
XC::MEDGroupInfo::MEDGroupInfo(MEDMeshing *malla,const Set &set)
  : nombre(set.GetNombre()), med_group(nullptr)
  { 
    set_owner(malla);
    indices= malla->getMapIndicesCeldas();
    const size_t numNodos= set.NumNodos();
    const size_t numElementos= set.NumElementos();
    if(numElementos==0) //Conjunto de nodos.
      {
        tipo_entidad= MED_EN::MED_NODE;
	const DqPtrsNode nodos= set.GetNodos();
        const MEDMapIndices &indices_nodos= malla->getMapIndicesVertices();
        for(Set::nod_const_iterator i= nodos.begin();i!=nodos.end();i++)
          nuevo_vertice((*i)->getTag(),indices_nodos);
      }
    else if(numNodos==0) //Conjunto de elementos.
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

const XC::MEDMeshing &XC::MEDGroupInfo::getMalla(void) const
  { 
    const MEDMeshing *retval= dynamic_cast<const MEDMeshing *>(Owner());
    return *retval;
  }

MEDMEM::GROUP *XC::MEDGroupInfo::getGrupoMED(void) const
  { return med_group; }

//! @brief Devuelve el vector con los índices de los elementos que se pasan como
//! parámetro.
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

//! @brief Devuelve el nombre del grupo.
const std::string &XC::MEDGroupInfo::getNombre(void) const
  { return nombre; }

//! @brief Devuelve el vector con las posiciones de los distintos
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

//! @brief Devuelve un vector con los índices de los elementos ordenados
//! por tipo.
std::vector<int> XC::MEDGroupInfo::getIndicesElementos(void) const
  {
    std::vector<int> retval;
    for(map_indices_tipo::const_iterator i=indices_tipo.begin();i!=indices_tipo.end();i++)
      retval.insert(retval.end(),i->second.begin(),i->second.end());
    return retval;
  }

//! @brief Vuelca la definición del grupo en MEDMEM
//! malla MED
void XC::MEDGroupInfo::to_med(void) const
  {
    med_group= new MEDMEM::GROUP;
    med_group->setName(nombre);
    const MEDMeshing &malla= getMalla();
    med_group->setMesh(&malla.getMallaMED());
    med_group->setEntity(tipo_entidad);
    MEDCellBaseInfo::to_support_med(*med_group);
    const std::vector<int> index= getVectorIndicesTipos();
    const std::vector<int> value= getIndicesElementos();
    med_group->setNumber(&index[0],&value[0]);
    malla.getMallaMED().addGroup(*med_group);
  }

any_const_ptr XC::MEDGroupInfo::GetProp(const std::string &cod) const
  {
    return MEDCellBaseInfo::GetProp(cod);
  }
