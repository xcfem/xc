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
//MeshCompRecorder.h
                                                                        
#ifndef MeshCompRecorder_h
#define MeshCompRecorder_h

#include <utility/recorder/HandlerRecorder.h>

namespace XC {
class Domain;
class FE_Datastore;
class DataOutputHandler;
class Node;

//! @brief Se emplea como base de los recorders que obtienen
//! la respuesta de uno o más nodos durante el análisis.
class MeshCompRecorder: public HandlerRecorder
  {
  protected:
    double deltaT;
    double nextTimeStampToRecord;

    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);

  public:
    MeshCompRecorder(int classTag);
    MeshCompRecorder(int classTag, Domain &theDomain, DataOutputHandler &theOutputHandler,double deltaT = 0.0, bool echoTimeFlag = true); 
  };
} // end of XC namespace

#endif
