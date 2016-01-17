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
//PropRecorder.h
                                                                        
#ifndef PropRecorder_h
#define PropRecorder_h

#include <utility/recorder/Recorder.h>

namespace XC {

class PropRecorder: public Recorder
  {
  protected:
    std::string CallbackRecord; //!< Instrucciones que se ejecutan en cada llamada a record.
    std::string CallbackRestart; //!< Instrucciones que se ejecutan en cada llamada a restart.
    int lastCommitTag; //!< CommitTag de la última llamada a record.
    double lastTimeStamp; //!< TimeStamp de la última llamada a record.

    Domain *theDomain;

  public:
    PropRecorder(int classTag, Domain *ptr_dom= NULL);

    int setDomain(Domain &theDomain);
    std::string getNombreCombActual(void) const;

    inline int getLastCommitTag(void) const
      { return lastCommitTag; }
    inline double getLastTimeStamp(void) const
      { return lastTimeStamp; }
    double getCurrentTime(void) const;
    double getCommittedTime(void) const;
    int getCommitTag(void) const;

    void setCallbackRecord(const std::string &);
    std::string getCallbackRecord(void);
    void setCallbackRestart(const std::string &);
    std::string getCallbackRestart(void);

  };
} // end of XC namespace

#endif
