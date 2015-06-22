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
//MEDIntFieldInfo

#include "MEDIntFieldInfo.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/gauss_models/GaussModel.h"
#include "utility/xc_python_utils.h"
#include "xc_utils/src/nucleo/python_utils.h"

//! @brief Constructor.
XC::MEDIntFieldInfo::MEDIntFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : MEDTFieldInfo<int>(fi,grp) {}

//! @brief Lectura del objeto desde archivo.
bool XC::MEDIntFieldInfo::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MEDIntFieldInfo) Procesando comando: " << cmd << std::endl;

    if(cmd == "setValueIJ") //Asigna el valor a la componente j del elemento i.
      {
        if(!campo) to_med(); //Envía definición del campo a MEDMEM.
        const int valor= interpretaInt(status.GetBloque());
        const CmdParser &parser= status.Parser();
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(2,cmd))
              {
                const int j= convert_to_int(InterpreteRPN::Pila().Pop()); //Índice  de la componente.
                const int i= convert_to_int(InterpreteRPN::Pila().Pop()); //Índice  del elemento.
                setValueIJ(i,j,valor);
              }
          }
        return true;
      }
    else    
      return MEDTFieldInfo<int>::procesa_comando(status);
  }

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
                            << "' devuelve un vector de dimensión: "
                            << valor.size() << " se esperaba uno de dimensión "
                            << dim << std::endl;
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,convert_to_int(valor[k-1]));
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
                            << "' devuelve un vector de dimensión: "
                            << valor.size() << " se esperaba uno de dimensión "
                            << dim << std::endl;
                for(size_t k= 1;k<=dim;k++)
                  setValueIJ(conta,k,convert_to_int(valor[k-1]));
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
    //     valor= convert_to_m_int((*j)->GetProp(nmb_prop));
    //     const MED_EN::medGeometryElement tipo= (*j)->getMEDCellType();
    //     for(size_t k= 1;k<=dim;k++)
    //       {
    //         const size_t num_ptos= (*j)->getGaussModel().getNumGaussPoints();
    //         for(size_t l=1;l<=num_ptos;l++)
    //           setValueIJK(conta,k,l,tipo,valor(l,k));
    //       }
    //   }
  }
