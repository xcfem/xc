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
//ElementRecorderBase.cpp
                                                                        
#include <utility/recorder/ElementRecorderBase.h>
#include <utility/recorder/response/Response.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_basic/src/texto/cadena_carac.h"
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/handler/DataOutputHandler.h>
#include <utility/actor/message/Message.h>

XC::ElementRecorderBase::ElementRecorderBase(int classTag)
  : MeshCompRecorder(classTag), eleID(0), theResponses(), responseArgs()
  {}

XC::ElementRecorderBase::ElementRecorderBase(int classTag,const XC::ID &ele, const std::vector<std::string> &argv, 
                                 bool echoTime, 
                                 Domain &theDom, DataOutputHandler &theOutputHandler,
                                 double dT)
  :MeshCompRecorder(classTag,theDom,theOutputHandler,dT,echoTime),
   eleID(ele), theResponses(), responseArgs(argv) {}

//! @brief set the responses to get in a record
void XC::ElementRecorderBase::setup_responses(const std::string &dataToStore)
  {
    std::deque<std::string> campos= separa_cadena(dataToStore," ");
    const int argc= campos.size();
    responseArgs.resize(argc);
    for(int i=0; i<argc; i++)
      responseArgs[i]= campos[i];
  }

//! @brief Lee un objeto XC::ElementRecorderBase desde archivo
bool XC::ElementRecorderBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElementRecorderBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "elementos")
      {
        std::vector<int> elems= crea_vector_int(status.GetString());
        const int sz= elems.size();
        eleID= ID(sz);
        for(int i= 0;i<sz;i++)
          eleID[i]= elems[i];
        return true;
      }
    else if(cmd == "data_to_store")
      {
        const std::string tmp= interpretaString(status.GetString());
        setup_responses(tmp);
        return true;
      }
    else
      return MeshCompRecorder::procesa_comando(status);
  }

//@brief Destructor.
XC::ElementRecorderBase::~ElementRecorderBase(void)
  {
    const int numEle= eleID.Size();
    for(int i= 0;i<numEle;i++)
      {
        if(theResponses[i])
          {
            delete theResponses[i];
            theResponses[i]= nullptr;
          }
      }
  }

//! @brief Envía el objeto a otro proceso.
int XC::ElementRecorderBase::sendData(CommParameters &cp)
  {
    int res= MeshCompRecorder::sendData(cp);
    res+= cp.sendID(eleID,getDbTagData(),CommMetaData(6));
    res+= cp.sendStrings(responseArgs,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Recibe el objeto desde otro proceso.
int XC::ElementRecorderBase::receiveData(const CommParameters &cp)
  {
    int res= MeshCompRecorder::receiveData(cp);
    res+= cp.receiveID(eleID,getDbTagData(),CommMetaData(6));
    res+= cp.receiveStrings(responseArgs,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Envía el objeto a otro proceso.
int XC::ElementRecorderBase::sendSelf(CommParameters &cp)
  {
    int res= 0;
    if(cp.isDatastore() == 1)
      std::cerr << "ElementRecorderBase::sendSelf() - does not send data to a datastore\n";
    else
      {
        setDbTag(cp);
        const int dataTag= getDbTag();
        inicComm(8);
        res= sendData(cp);
        if(cp.sendIdData(getDbTagData(),dataTag)< 0)
          {
            std::cerr << "ElementRecorderBase::sendSelf() "
                      << "- failed to send idData\n";
            return -1;
          }
      }
    return res;
  }

//! @brief Recibe el objeto desde otro proceso.
int XC::ElementRecorderBase::recvSelf(const CommParameters &cp)
  {
    int res= 0;
    if(cp.isDatastore() == 1)
      std::cerr << "ElementRecorderBase::recvSelf() - does not recv data to a datastore\n";
    else
      {
        inicComm(8);
        const int dataTag= getDbTag();
        res= cp.receiveIdData(getDbTagData(),dataTag);
        if(res < 0)
          {
            std::cerr << "ElementRecorderBase::recvSelf() - failed to recv idData\n";
            return res;
          }
        res= receiveData(cp);
      }
    return res;
  }
