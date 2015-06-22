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
//HandlerRecorder.cpp

#include <utility/recorder/HandlerRecorder.h>
#include <utility/handler/DataOutputHandler.h>
#include "xc_utils/src/base/CmdStatus.h"

XC::HandlerRecorder::HandlerRecorder(int classTag)
  :DomainRecorderBase(classTag,nullptr), theHandler(nullptr), initializationDone(false), echoTimeFlag(false)
  {}

XC::HandlerRecorder::HandlerRecorder(int classTag,Domain &theDom,DataOutputHandler &theOutputHandler,bool tf)
  :DomainRecorderBase(classTag,&theDom), theHandler(&theOutputHandler), initializationDone(false), echoTimeFlag(tf) {}

//! @brief Lee un objeto XC::HandlerRecorder desde archivo
bool XC::HandlerRecorder::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(HandlerRecorder) Procesando comando: " << cmd << std::endl;
    if(cmd == "echo_time_on")
      {
        status.GetString(); //Ignoramos entrada.
        echoTimeFlag= true;
        return true;
      }
    else if(cmd == "echo_time_off")
      {
        status.GetString(); //Ignoramos entrada.
        echoTimeFlag= false;
        return true;
      }
    else
      return DomainRecorderBase::procesa_comando(status);
  }

//! @brief Asigna el manejador de salida de datos
void XC::HandlerRecorder::SetOutputHandler(DataOutputHandler *tH)
  { theHandler= tH; }


//! @brief Envia el objet por el canal definido en el parámetro.
int XC::HandlerRecorder::sendData(CommParameters &cp)
  {
    cp.sendBrokedPtr(theHandler,getDbTagData(),BrokedPtrCommMetaData(0,1,2));
    cp.sendBool(echoTimeFlag,getDbTagData(),CommMetaData(3));
    cp.sendBool(initializationDone,getDbTagData(),CommMetaData(4));
    return 0;
  }

//! @brief Recibe el objeto por el canal definido en el parámetro.
int XC::HandlerRecorder::receiveData(const CommParameters &cp)
  {
    if(theHandler)
      {
        delete theHandler;
        theHandler= nullptr;
      }
    theHandler= cp.getBrokedDataOutputHandler(theHandler,getDbTagData(),BrokedPtrCommMetaData(0,1,2));
    cp.receiveBool(echoTimeFlag,getDbTagData(),CommMetaData(3));
    cp.receiveBool(initializationDone,getDbTagData(),CommMetaData(4));
    return 0;
  }
