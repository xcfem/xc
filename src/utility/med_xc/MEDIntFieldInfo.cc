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
//MEDIntFieldInfo

#include "MEDIntFieldInfo.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/xc_python_utils.h"
#include "xc_utils/src/nucleo/python_utils.h"

//! @brief Constructor.
XC::MEDIntFieldInfo::MEDIntFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : MEDTFieldInfo<int>(fi,grp) {}

//! @brief Asigna los valores del campo en los nodos.
void XC::MEDIntFieldInfo::populateOnNodes(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsNode &nodos= set.GetNodos();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<int> valor(dim);
    const std::string nmb_prop= fi.getComponentsProperty();
    if(nmb_prop.empty())
      std::cerr << "Components property name is empty" << std::endl;
    else
      {
        for(DqPtrsNode::const_iterator j= nodos.begin();j!=nodos.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
            if(tmp != boost::python::object())
              {
                valor= vector_int_from_py_object(tmp);
                if(valor.size()!=dim)
  	          std::cerr << "La propiedad: '" << nmb_prop
                            << "' returns un vector de dimensión: "
                            << valor.size() << " se esperaba uno de dimensión "
                            << dim << std::endl;
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,valor[k-1]);
              }
            else
	      std::cerr << "MEDIntFieldInfo::populateOnNodes; el nodo: "
                        << (*j)->getTag()
                        << " no pudo devolver la propiedad: '"
                        << nmb_prop << "'\n";
          }
      }
  }

//! @brief Asigna los valores del campo en los elementos.
void XC::MEDIntFieldInfo::populateOnElements(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsElem &elementos= set.GetElementos();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<int> valor(dim);
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
                valor= vector_int_from_py_object(tmp);
                if(valor.size()!=dim)
	          std::cerr << "La propiedad: '" << nmb_prop
                            << "' returns un vector de dimensión: "
                            << valor.size() << " se esperaba uno de dimensión "
                            << dim << std::endl;
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,valor[k-1]);
              }
            else
	      std::cerr << "MEDIntFieldInfo::populateOnElements; el elemento: "
                        << (*j)->getTag()
                        << " no pudo devolver la propiedad: '"
                        << nmb_prop << "'\n";
          }
      }
  }

//! @brief Asigna los valores del campo en los puntos de Gauss.
void XC::MEDIntFieldInfo::populateOnGaussPoints(const Set &set,const FieldInfo &fi)
  {
    std::cerr << "MEDIntFieldInfo::populateOnGaussPoints no implementada" << std::endl;

    // const DqPtrsElem &elementos= set.GetElementos();
    // const size_t dim= fi.getNumberOfComponents();
    // int conta= 1; m_int valor;
    // const std::string nmb_prop= fi.getComponentsProperty();
    // for(DqPtrsElem::const_iterator j= elementos.begin();j!=elementos.end();j++,conta++)
    //   {
    //     valor= convert_to_m_int((*j)->GetPrp(nmb_prop));
    //     const MED_EN::medGeometryElement tipo= (*j)->getMEDCellType();
    //     for(size_t k= 1;k<=dim;k++)
    //       {
    //         const size_t num_ptos= (*j)->getGaussModel().getNumGaussPoints();
    //         for(size_t l=1;l<=num_ptos;l++)
    //           setValueIJK(conta,k,l,tipo,valor(l,k));
    //       }
    //   }
  }
