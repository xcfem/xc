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
//MEDMeshing

#include "MEDMeshing.h"
#include "MEDDblFieldInfo.h"
#include "MEDIntFieldInfo.h"
#include "MEDGaussModel.h"


#include "preprocessor/set_mgmt/MapSet.h"
#include "post_process/MapFields.h"
#include "preprocessor/set_mgmt/Set.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "domain/mesh/node/Node.h"
#include "ProblemaEF.h"

const std::string XC::MEDMeshing::str_grupo_nodos= "_nodos";
const std::string XC::MEDMeshing::str_grupo_elementos= "_elementos";

// @brief Returns med mesh to export.
XC::MEDMeshing::MEDMeshing(const ProblemaEF &prb) 
  : sets(prb.getPreprocessor().get_sets()), fields(prb.getFields()), vertices(prb.getDomain()->getMesh()), cells(prb.getDomain()->getMesh(),vertices.getMapIndices())
  {}

//! Constructor.
XC::MEDMeshing::MEDMeshing(const Mesh &mesh,const MapSet &s, const MapFields &f)
  : sets(s), fields(f), vertices(mesh), cells(mesh,vertices.getMapIndices()) {} 

//! Returns a reference to the MEDMEM mesh.
MEDMEM::MESHING &XC::MEDMeshing::getMEDMesh(void) const
  { return mesh; }

//! Erases la mesh.
void XC::MEDMeshing::clear(void)
  {
    vertices.clear();
    cells.clear();
    med_groups.clear();
    for(std::deque<MEDFieldInfo *>::iterator i= med_fields.begin();i!=med_fields.end();i++)
      delete *i;
    med_fields.clear();
    //mesh= MEDMEM::MESHING(); //Not working 2012/10/03
  }
//! Constructor.
XC::MEDMeshing::~MEDMeshing(void)
  { clear(); }

const XC::MEDMapIndices &XC::MEDMeshing::getMapIndicesVertices(void) const
  { return vertices.getMapIndices(); }

const XC::MEDMapIndices &XC::MEDMeshing::getMapIndicesCeldas(void) const
  { return cells.getMapIndices(); }

//! @brief Define los grupos de MEDMEM correspondientes a the sets
//! de XC.
void XC::MEDMeshing::defineMEDGroups(void)
  {
    for(MapSet::const_iterator i= sets.begin();i!=sets.end();i++)
      {
        const Set *set= dynamic_cast<const Set *>(i->second);
        if(set)
          {
            const std::string nmb= set->GetNombre();
            const size_t numNodos= set->NumNodos();
            const size_t numElementos= set->NumElementos();
            if((numElementos==0) && (numNodos==0))
              continue;
            else if((numElementos==0) || (numNodos==0))
              med_groups.push_back(MEDGroupInfo(this,*set));
            else
              {
                Set set_nodos(*set);
                set_nodos.clearElementos();
                set_nodos.Nombre()= nmb+str_grupo_nodos;
                med_groups.push_back(MEDGroupInfo(this,set_nodos));
                Set set_elementos(*set);
                set_elementos.clearNodos();
                set_elementos.Nombre()= nmb+str_grupo_elementos;
                med_groups.push_back(MEDGroupInfo(this,set_elementos));
              }
          }
      }
  }

XC::MEDGroupInfo *XC::MEDMeshing::getGroupInfo(const std::string &nmb) const
  {
    MEDGroupInfo *retval= nullptr;
    std::deque<MEDGroupInfo>::iterator i= med_groups.begin();
    for(;i!=med_groups.end();i++)
      if(i->getNombre()==nmb)
        break;
    if(i!=med_groups.end())
      retval= &(*i);
    return retval;
  }

XC::MEDGroupInfo *XC::MEDMeshing::getGroupInfo(const Set &set,const FieldInfo &field) const
  {
    MEDGroupInfo *retval= nullptr;
    std::string nmb_grupo= "";
    if(field.isDefinedOnNodes())
      nmb_grupo= set.GetNombre()+str_grupo_nodos;
    else if(field.isDefinedOnElements())
      nmb_grupo= set.GetNombre()+str_grupo_elementos;
    retval= getGroupInfo(nmb_grupo);
    if(!retval)
      std::cerr << "MEDMeshing::getGroupInfo; no se encotró el grupo: "
                << nmb_grupo << std::endl;
    return retval;  
  }

//! @brief Set the integration Gauss model for the elements on the set.
//! @param set: element set.
void XC::MEDMeshing::defineMEDGaussModels(const Set &set,MEDFieldInfo &med_fi) const
  {
    med_fi.defineGaussModels(set);
  }

//! @brief Defines a field over an element set.
void XC::MEDMeshing::defineMEDDblField(const Set &set,const FieldInfo &fi,MEDGroupInfo *grupo) const
  {
    MEDDblFieldInfo *med_fi= new MEDDblFieldInfo(fi,grupo);
    med_fields.push_back(med_fi);
    med_fi->to_med();
    if(fi.isDefinedOnNodes())
      { med_fi->populateOnNodes(set,fi); }
    else if(fi.isDefinedOnElements())
      {
        if(fi.isDefinedOnGaussPoints())
          {
            defineMEDGaussModels(set,*med_fi);
            med_fi->populateOnGaussPoints(set,fi);
          }
        else
          med_fi->populateOnElements(set,fi);
      }
  }

//! @brief Defines a field over a set.
void XC::MEDMeshing::defineMEDIntField(const Set &set,const FieldInfo &fi,MEDGroupInfo *grupo) const
  {
    MEDIntFieldInfo *med_fi= new MEDIntFieldInfo(fi,grupo);
    med_fields.push_back(med_fi);
    med_fi->to_med();
    if(fi.isDefinedOnNodes())
      { med_fi->populateOnNodes(set,fi); }
    else if(fi.isDefinedOnElements())
      {
        if(fi.isDefinedOnGaussPoints())
          {
            defineMEDGaussModels(set,*med_fi);
            med_fi->populateOnGaussPoints(set,fi);
          }
        else
          med_fi->populateOnElements(set,fi);
      }
  }

//! @brief Define los campos de MEDMEM correspondientes a los de XC.
void XC::MEDMeshing::defineMEDFields(void) const
  {
    for(MapFields::const_iterator fieldIter= fields.begin();fieldIter!=fields.end();fieldIter++)
      {
        const FieldInfo &fi= *fieldIter;
        const Set *set= dynamic_cast<const Set *>(sets.busca_set(fi.getSetName()));
        if(set)
          {
            MEDGroupInfo *grupo= getGroupInfo(*set,fi);
            if(grupo)
              {
                const std::string tipo_componentes= fi.getComponentsType();
                if(tipo_componentes=="double")
                  { defineMEDDblField(*set,fi,grupo); }
                else if(tipo_componentes=="int")
                  { defineMEDIntField(*set,fi,grupo); }
                else
                  std::cerr << "MEDMeshing::defineMEDFields; el tipo: " << tipo_componentes
                            << " es desconocido." << std::endl;
	      }
          }
        else
	  std::cerr << "MEDMeshing::defineMEDFields; set: '" 
                    << fi.getSetName() << "' for the field : '"
                    << fi.GetNombre() << "' not found." << std::endl;
      }
  }

//! @brief Dumps vertices and cells definition onto
//! mesh MED
void XC::MEDMeshing::to_med(void)
  {
    vertices.to_med(mesh);
    cells.to_med(mesh);
    defineMEDGroups();
    for(std::deque<MEDGroupInfo>::const_iterator i=med_groups.begin();i!=med_groups.end();i++)
      i->to_med();
    defineMEDFields();
  }

void XC::MEDMeshing::write(const std::string &fileName)
  {
    to_med();
    const int id= mesh.addDriver(MEDMEM::MED_DRIVER,fileName,mesh.getName());
    mesh.write(id);
    for(std::deque<MEDFieldInfo *>::iterator i= med_fields.begin();i!=med_fields.end();i++)
      (*i)->write(fileName);
  }

