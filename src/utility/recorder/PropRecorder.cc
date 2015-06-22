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
//PropRecorder.cc

#include <utility/recorder/PropRecorder.h>
#include "xc_utils/src/base/CmdStatus.h"
#include <domain/domain/Domain.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_basic/src/texto/cadena_carac.h"

//! @brief Constructor.
XC::PropRecorder::PropRecorder(int classTag,Domain *ptr_dom)
  :Recorder(classTag), CallbackRecord(), CallbackRestart(),
  lastCommitTag(-1),lastTimeStamp(-1.0),theDomain(ptr_dom) {}

//! @brief Lee un objeto PropRecorder desde archivo
bool XC::PropRecorder::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(PropRecorder) Procesando comando: " << cmd << std::endl;
    if(cmd == "callback_record")
      {
        CallbackRecord= status.GetBloque();
        return true;
      }   
    else if(cmd == "callback_restart")
      {
        CallbackRestart= status.GetBloque();
        return true;
      }
    // else if(cmd == "nodos")
    //   {
    //     if(!theDomain)
    // 	  std::cerr << "PropRecorder::procesa_comando; el recorder no tiene asignado el dominio." << std::endl;
    //     else
    //       {
    //         std::vector<boost::any> param= interpretaVectorAny(status.GetString());
    //         std::vector<int> iNodos= convert_to_vector_int(param);
    //         const int sz= iNodos.size();
    //         if(sz)
    //           {
    //             for(int i= 0;i<sz;i++)
    //               objetos.push_back(theDomain->getNode(iNodos[i]));
    //           }
    //         else
    // 	      std::cerr << "Error; " << cmd << "la lista de nodos está vacia." << std::endl;
    //       }
    //     return true;
    //   }
    // else if(cmd == "elementos")
    //   {
    //     if(!theDomain)
    // 	  std::cerr << "PropRecorder::procesa_comando; el recorder no tiene asignado el dominio." << std::endl;
    //     else
    //       {
    //         std::vector<boost::any> param= interpretaVectorAny(status.GetString());
    //         std::vector<int> iElementos= convert_to_vector_int(param);
    //         const int sz= iElementos.size();
    //         if(sz)
    //           {
    //             for(int i= 0;i<sz;i++)
    //               objetos.push_back(theDomain->getElement(iElementos[i]));
    //           }
    //         else
    // 	      std::cerr << "Error; " << cmd << "la lista de elementos está vacia." << std::endl;
    //       }
    //     return true;
    //   }
    else
      return Recorder::procesa_comando(status);
  }

//! @brief Asigna el dominio al recorder.
int XC::PropRecorder::setDomain(Domain &theDom)
  {
    theDomain = &theDom;
    return 0;
  }

double XC::PropRecorder::getCurrentTime(void) const
  { return theDomain->getTimeTracker().getCurrentTime(); }
double XC::PropRecorder::getCommittedTime(void) const
  { return theDomain->getTimeTracker().getCommittedTime(); }
int XC::PropRecorder::getCommitTag(void) const
  { return theDomain->getCommitTag(); }

//! @brief Devuelve el nombre de la combinación actual.
std::string XC::PropRecorder::getNombreCombActual(void) const
  {
    std::string retval= "";
    if(theDomain)
      retval= theDomain->getNombreCombActual();
    return retval;
  }

void XC::PropRecorder::setCallbackRecord(const std::string &str)
  { CallbackRecord= str; }
std::string XC::PropRecorder::getCallbackRecord(void)
  { return CallbackRecord; }
void XC::PropRecorder::setCallbackRestart(const std::string &str)
  { CallbackRestart= str; }
std::string XC::PropRecorder::getCallbackRestart(void)
  { return CallbackRestart; }

