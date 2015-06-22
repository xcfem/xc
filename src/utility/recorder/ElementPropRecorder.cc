//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
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
//ElementPropRecorder.cc

#include <utility/recorder/ElementPropRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "xc_utils/src/nucleo/python_utils.h"


//! @brief Constructor.
XC::ElementPropRecorder::ElementPropRecorder(Domain *ptr_dom)
  :PropRecorder(RECORDER_TAGS_ElementPropRecorder,ptr_dom) {}

//! @brief Asigns elements to recorder.
void XC::ElementPropRecorder::setElements(const ID &iElements)
  {
    const int sz= iElements.Size();
    if(sz)
      {
        for(int i= 0;i<sz;i++)
          elements.push_back(theDomain->getElement(iElements(i)));
      }
    else
      std::cerr << "Error; setElements"
                << "la lista de elementos está vacia." << std::endl;
  }

//! @brief Registra la(s) propiedad(es) del objeto tras el commit.
int XC::ElementPropRecorder::record(int commitTag, double timeStamp)
  {
    lastCommitTag= commitTag;
    lastTimeStamp= timeStamp;
    for(dq_elements::iterator i= elements.begin();i!=elements.end();i++)
      {
        Element *tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << "XC::ElementPropRecorder::record(); se intentó llamar a un puntero nulo." << std::endl;
      }
    return 0;
  }

//! @brief Reinicia el recorder.
int XC::ElementPropRecorder::restart(void)
  {
    for(dq_elements::iterator i= elements.begin();i!=elements.end();i++)
      {
        Element *tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << "XC::ElementPropRecorder::restart(); se intentó llamar a un puntero nulo." << std::endl;
      }
    return 0;
  }


