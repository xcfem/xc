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
//ElementRecorderBase.h
                                                                        
                                                                        
#ifndef ElementRecorderBase_h
#define ElementRecorderBase_h


// A ElementRecorderBase is used to obtain a response from an element during 
// the analysis.
//
// What: "@(#) ElementRecorderBase.h, revA"

#include <utility/recorder/MeshCompRecorder.h>
#include <utility/matrix/ID.h>

namespace XC {
class Response;
class DataOutputHandler;

//! @brief Se emplea como base de los recorders que obtienen
//! la respuesta de uno o más elementos durante el análisis.
class ElementRecorderBase: public MeshCompRecorder
  {
  protected:        
    ID eleID;

    std::vector<Response *> theResponses;
    std::vector<std::string> responseArgs;

    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);
    void setup_responses(const std::string &);

  public:
    ElementRecorderBase(int classTag);
    ElementRecorderBase(int classTag, const ID &eleID, 
                        const std::vector<std::string> &, 
                        bool echoTime, 
                        Domain &theDomain, 
                        DataOutputHandler &theOutputHandler,
                        double deltaT = 0.0);
    ~ElementRecorderBase(void);
    inline size_t getNumArgs(void) const
      { return responseArgs.size(); }
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif
