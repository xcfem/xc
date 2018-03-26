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

//! @brief Set the field values at the nodes.
void XC::MEDIntFieldInfo::populateOnNodes(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsNode &nodes= set.getNodes();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<int> value(dim);
    const std::string nmb_prop= fi.getComponentsProperty();
    if(nmb_prop.empty())
      std::cerr << "Components property name is empty" << std::endl;
    else
      {
        for(DqPtrsNode::const_iterator j= nodes.begin();j!=nodes.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
            if(tmp != boost::python::object())
              {
                value= vector_int_from_py_object(tmp);
		checkVectorDimension(nmb_prop,value,dim);
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,value[k-1]);
              }
            else
	      std::cerr << "MEDIntFieldInfo::populateOnNodes; the node: "
                        << (*j)->getTag()
                        << " can't return property: '"
                        << nmb_prop << "'\n";
          }
      }
  }

//! @brief Sets the values of the fields in the elements.
void XC::MEDIntFieldInfo::populateOnElements(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsElem &elements= set.getElements();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<int> value(dim);
    const std::string nmb_prop= fi.getComponentsProperty();
    if(nmb_prop.empty())
      std::cerr << "Components property name is empty" << std::endl;
    else
      {
        for(DqPtrsElem::const_iterator j= elements.begin();j!=elements.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
            if(tmp != boost::python::object())
              {
                value= vector_int_from_py_object(tmp);
 		checkVectorDimension(nmb_prop,value,dim);
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,value[k-1]);
              }
            else
	      std::cerr << "MEDIntFieldInfo::populateOnElements; the element: "
                        << (*j)->getTag()
                        << " no pudo devolver la propiedad: '"
                        << nmb_prop << "'\n";
          }
      }
  }

//! @brief Set the field values at the Gauss points.
void XC::MEDIntFieldInfo::populateOnGaussPoints(const Set &set,const FieldInfo &fi)
  {
    std::cerr << "MEDIntFieldInfo::populateOnGaussPoints not implemented" << std::endl;

    // const DqPtrsElem &elements= set.getElements();
    // const size_t dim= fi.getNumberOfComponents();
    // int conta= 1; m_int value;
    // const std::string nmb_prop= fi.getComponentsProperty();
    // for(DqPtrsElem::const_iterator j= elements.begin();j!=elements.end();j++,conta++)
    //   {
    //     value= convert_to_m_int((*j)->GetPrp(nmb_prop));
    //     const MED_EN::medGeometryElement tipo= (*j)->getMEDCellType();
    //     for(size_t k= 1;k<=dim;k++)
    //       {
    //         const size_t num_ptos= (*j)->getGaussModel().getNumGaussPoints();
    //         for(size_t l=1;l<=num_ptos;l++)
    //           setValueIJK(conta,k,l,tipo,value(l,k));
    //       }
    //   }
  }
