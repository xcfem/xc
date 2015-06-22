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
//NodePropRecorder.cc

#include <utility/recorder/NodePropRecorder.h>
#include "xc_utils/src/base/CmdStatus.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "xc_utils/src/nucleo/python_utils.h"

//! @brief Constructor.
XC::NodePropRecorder::NodePropRecorder(Domain *ptr_dom)
  :PropRecorder(RECORDER_TAGS_NodePropRecorder,ptr_dom) {}

//! @brief Asigns nodes to recorder.
void XC::NodePropRecorder::setNodes(const ID &iNodos)
  {
    const int sz= iNodos.Size();
    if(sz)
      {
        for(int i= 0;i<sz;i++)
          nodes.push_back(theDomain->getNode(iNodos(i)));
      }
    else
      std::cerr << "Error; setNodes"
                << "la lista de nodos está vacia." << std::endl;
  }

//! @brief Registra la(s) propiedad(es) del objeto tras el commit.
int XC::NodePropRecorder::record(int commitTag, double timeStamp)
  {
    lastCommitTag= commitTag;
    lastTimeStamp= timeStamp;
    for(dq_nodes::iterator i= nodes.begin();i!=nodes.end();i++)
      {
        Node *tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << "XC::NodePropRecorder::record(); se intentó llamar a un puntero nulo." << std::endl;
      }
    return 0;
  }

//! @brief Reinicia el recorder.
int XC::NodePropRecorder::restart(void)
  {
    for(dq_nodes::iterator i= nodes.begin();i!=nodes.end();i++)
      {
        Node *tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << "XC::NodePropRecorder::restart(); se intentó llamar a un puntero nulo." << std::endl;
      }
    return 0;
  }


