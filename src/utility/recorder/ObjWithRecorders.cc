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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/ObjWithRecorders.cpp,v $
                                                                        
                                                                        
#include "ObjWithRecorders.h"
#include <utility/recorder/Recorder.h>
#include <utility/recorder/DamageRecorder.h>
#include <utility/recorder/DatastoreRecorder.h>
#include <utility/recorder/DriftRecorder.h>
#include <utility/recorder/ElementRecorder.h>
#include <utility/recorder/EnvelopeElementRecorder.h>
#include <utility/recorder/EnvelopeNodeRecorder.h>
#include <utility/recorder/FilePlotter.h>
#include <utility/recorder/NodeRecorder.h>
#include <utility/recorder/GSA_Recorder.h>
#include <utility/recorder/MaxNodeDispRecorder.h>
#include <utility/recorder/PatternRecorder.h>
#include <utility/recorder/NodePropRecorder.h>
#include <utility/recorder/ElementPropRecorder.h>


#include "boost/any.hpp"

XC::ObjWithRecorders::ObjWithRecorders(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  : EntCmd(owr), theRecorders(), output_handlers(oh) {}


//! @brief Lee un objeto Recorder desde archivo.
XC::Recorder *XC::ObjWithRecorders::newRecorder(const std::string &cod,DataOutputHandler *output_handler)
  {
    Recorder *retval= nullptr;
    if(cod == "data_store_recorder")
      {
        DatastoreRecorder *tmp= new DatastoreRecorder();
        retval= tmp;
      }
    else if(cod == "drift_recorder")
      {
        DriftRecorder *tmp= new DriftRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << "ObjWithRecorders; new recorder: " << cod 
                    << " no hay gestores de salida." << std::endl;
        retval= tmp;
      }
    else if(cod == "element_recorder")
      {
        ElementRecorder *tmp= new ElementRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << "ObjWithRecorders; new recorder: " << cod 
                    << " no hay gestores de salida." << std::endl;
        retval= tmp;
      }
    else if(cod == "envelope_element_recorder")
      {
        EnvelopeElementRecorder *tmp= new EnvelopeElementRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << "ObjWithRecorders; new recorder: " << cod 
                    << " no hay gestores de salida." << std::endl;
        retval= tmp;
      }
    else if(cod == "envelope_node_recorder")
      {
        EnvelopeNodeRecorder *tmp= new EnvelopeNodeRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << "ObjWithRecorders; new recorder: " << cod 
                    << " no hay gestores de salida." << std::endl;
        retval= tmp;
      }
    else if(cod == "max_node_disp_recorder")
      {
        MaxNodeDispRecorder *tmp= new MaxNodeDispRecorder();
        retval= tmp;
      }
    else if(cod == "node_recorder")
      {
        NodeRecorder *tmp= new NodeRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << "ObjWithRecorders; new recorder: " << cod 
                    << " no hay gestores de salida." << std::endl;
        retval= tmp;
      }
    else if(cod == "pattern_recorder")
      {
        PatternRecorder *tmp= new PatternRecorder();
        retval= tmp;
      }
    else if(cod == "node_prop_recorder")
      {
        NodePropRecorder *tmp= new NodePropRecorder(get_domain_ptr());
        retval= tmp;
      }
    else if(cod == "element_prop_recorder")
      {
        ElementPropRecorder *tmp= new ElementPropRecorder(get_domain_ptr());
        retval= tmp;
      }
    else
      std::cerr << "Recorder type: '" << cod
                << "' unknown." << std::endl;
    if(retval)
      addRecorder(*retval);
    return retval;
  }

//! @brief Destructor.
XC::ObjWithRecorders::~ObjWithRecorders(void)
  {
    for(lista_recorders::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      delete *i; 
    theRecorders.erase(theRecorders.begin(),theRecorders.end());
  }

//! @brief Agrega un objeto recorder.
int XC::ObjWithRecorders::addRecorder(Recorder &theRecorder)
  {
    theRecorders.push_back(&theRecorder);
    return 0;
  }

//! @brief Ejecuta los recorders sobre el tag being passed as parameter.
int XC::ObjWithRecorders::record(int cTag, double timeStamp)
  {
    for(lista_recorders::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      (*i)->record(cTag, timeStamp);
    return 0;
  }

//! @brief Rearranca los recorders.
void XC::ObjWithRecorders::restart(void)
  {
    for(lista_recorders::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      (*i)->restart();
  }

//! @brief Elimina los recorders.
int XC::ObjWithRecorders::removeRecorders(void)
  {
    for(lista_recorders::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      delete *i; 
    theRecorders.erase(theRecorders.begin(),theRecorders.end());
    return 0;
  }

//! @brief Asigna el dominio a los recorders.
void XC::ObjWithRecorders::setLinks(Domain *ptr_dom)
  {
    if(ptr_dom)
      {
        for(lista_recorders::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
          {
            DomainRecorderBase *tmp= dynamic_cast<DomainRecorderBase *>(*i);
            if(tmp)
              tmp->setDomain(*ptr_dom);
          }
      }
    else
      std::cerr << "ObjWithRecorders::setLinks; el dominio no está asignado." << std::endl;
  }

//! @brief Asigna el almacén de gestores de salida.
void XC::ObjWithRecorders::SetOutputHandlers(DataOutputHandler::map_output_handlers *oh)
  { output_handlers= oh; }

int XC::ObjWithRecorders::sendData(CommParameters &cp)
  {
    //Reservamos las posiciones 0 y 1 de data para los miembros
    //de esta clase.
    return 0;
  }

int XC::ObjWithRecorders::recvData(const CommParameters &cp)
  {
    //Reservamos las posiciones 0 y 1 de data para los miembros
    //de esta clase.
    return 0;
  }
