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

//! @brief Set the field values at the nodes.
void XC::MEDDblFieldInfo::populateOnNodes(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsNode &nodes= set.getNodes();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<double> value(dim);
    const std::string nmb_prop= fi.getComponentsProperty();
    if(!nmb_prop.empty())
      {
        for(DqPtrsNode::const_iterator j= nodes.begin();j!=nodes.end();j++,conta++)
          {
            boost::python::object pyObj(boost::ref(*j));
            boost::python::object tmp= EntCmd_eval(pyObj,nmb_prop);
	    if(tmp != boost::python::object())
              {
                value= vector_double_from_py_object(tmp);
		checkVectorDimension(nmb_prop,value,dim);
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,value[k-1]);
              }
            else
	      std::cerr << "MEDDblFieldInfo::populateOnNodes; the node: "
                        << (*j)->getTag()
                        << " can't return the property: '"
                        << nmb_prop << "'\n";
          }
      }
    else
      std::cerr << "Components property name is empty" << std::endl;
  }

//! @brief Set the field values for the elements.
void XC::MEDDblFieldInfo::populateOnElements(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsElem &elements= set.getElements();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1; std::vector<double> value(dim);
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
                value= vector_double_from_py_object(tmp);
		checkVectorDimension(nmb_prop,value,dim);
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,value[k-1]);
              }
            else
	      std::cerr << "MEDDblFieldInfo::populateOnElements; the element: "
                        << (*j)->getTag()
                        << " can't return property: '"
                        << nmb_prop << "'\n";
          }
      }
  }

//! @brief Set the field values at the Gauss points.
void XC::MEDDblFieldInfo::populateOnGaussPoints(const Set &set,const FieldInfo &fi)
  {
    const DqPtrsElem &elements= set.getElements();
    const size_t dim= fi.getNumberOfComponents();
    int conta= 1;
    m_double value;
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
              for(size_t k= 1;k<=dim;k++)
                {
                  value= m_double_from_py_object(tmp);
                  const MED_EN::medGeometryElement tipo= (*j)->getMEDCellType();
                  const size_t num_ptos= (*j)->getGaussModel().getNumGaussPoints();
                  for(size_t l=1;l<=num_ptos;l++)
                    setValueIJK(conta,k,l,tipo,value(l,k));
                }
          }
      }
  }
