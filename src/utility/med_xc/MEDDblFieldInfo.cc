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
//MEDDblFieldInfo

#include "MEDDblFieldInfo.h"
#include "MEDGroupInfo.h"
#include "xc_basic/src/med_xc/MEDMEM_Group.hxx"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/xc_python_utils.h"
#include "xc_utils/src/nucleo/python_utils.h"

//! @brief Constructor.
XC::MEDDblFieldInfo::MEDDblFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : MEDTFieldInfo<double>(fi,grp) {}

//! @brief Asigna los valores del campo en los nodos.
void XC::MEDDblFieldInfo::populateOnNodes(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsNode &nodos= set.GetNodos();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<double> valor(dim);
    const std::string nmb_prop= fi.getComponentsProperty();
    if(!nmb_prop.empty())
      {
        for(DqPtrsNode::const_iterator j= nodos.begin();j!=nodos.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
	    if(tmp != boost::python::object())
              {
                valor= vector_double_from_py_object(tmp);
		checkVectorDimension(nmb_prop,valor,dim);
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,valor[k-1]);
              }
            else
	      std::cerr << "MEDDblFieldInfo::populateOnNodes; el nodo: "
                        << (*j)->getTag()
                        << " no pudo devolver la propiedad: '"
                        << nmb_prop << "'\n";
          }
      }
    else
      std::cerr << "Components property name is empty" << std::endl;
  }

//! @brief Asigna los valores del campo en los elementos.
void XC::MEDDblFieldInfo::populateOnElements(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsElem &elementos= set.GetElementos();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<double> valor(dim);
    const std::string nmb_prop= fi.getComponentsProperty();
    if(nmb_prop.empty())
      std::cerr << "Components property name is empty" << std::endl;
    else
      {
        for(DqPtrsElem::const_iterator j= elementos.begin();j!=elementos.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
	    if(tmp != boost::python::object())
              {
                valor= vector_double_from_py_object(tmp);
		checkVectorDimension(nmb_prop,valor,dim);
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,valor[k-1]);
              }
            else
	      std::cerr << "MEDDblFieldInfo::populateOnElements; the element: "
                        << (*j)->getTag()
                        << " can't return property: '"
                        << nmb_prop << "'\n";
          }
      }
  }

//! @brief Asigna los valores del campo en los puntos de Gauss.
void XC::MEDDblFieldInfo::populateOnGaussPoints(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsElem &elementos= set.GetElementos();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1;
    m_double valor;
    const std::string nmb_prop= fi.getComponentsProperty();
    if(nmb_prop.empty())
      std::cerr << "Components property name is empty" << std::endl;
    else
      {
        for(DqPtrsElem::const_iterator j= elementos.begin();j!=elementos.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
            if(tmp != boost::python::object())
              for(size_t k= 1;k<=dim;k++)
                {
                  valor= m_double_from_py_object(tmp);
                  const MED_EN::medGeometryElement tipo= (*j)->getMEDCellType();
                  const size_t num_ptos= (*j)->getGaussModel().getNumGaussPoints();
                  for(size_t l=1;l<=num_ptos;l++)
                    setValueIJK(conta,k,l,tipo,valor(l,k));
                }
          }
      }
  }
