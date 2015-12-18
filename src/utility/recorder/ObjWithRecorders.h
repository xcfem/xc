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
//ObjWithRecorders.h
                                                                        
                                                                        
#ifndef ObjWithRecorders_h
#define ObjWithRecorders_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/handler/DataOutputHandler.h"
#include <list>

namespace XC {
class Recorder;
 class Domain;

//! @brief Objeto capaz de gestionar Recorders.
class ObjWithRecorders: public EntCmd
  {
  public:
    typedef std::list<Recorder *> lista_recorders;
    typedef lista_recorders::iterator recorder_iterator;
    typedef lista_recorders::const_iterator const_recorder_iterator; 
  private:
    lista_recorders theRecorders; //!< Lista de recorders.
    DataOutputHandler::map_output_handlers *output_handlers; //!< Manejadores para salida de resultados.

    Recorder *procesa_cmd_recorder(const std::string &,CmdStatus &);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
    //! @brief Devuelve el número de recorders definidos.
    inline size_t getNumRecorders(void) const
      { return theRecorders.size(); }
    virtual Domain *get_domain_ptr(void)= 0;
  public:
    ObjWithRecorders(EntCmd *owr,DataOutputHandler::map_output_handlers *oh= NULL);
    virtual ~ObjWithRecorders(void);

    Recorder *newRecorder(const std::string &,DataOutputHandler *oh= NULL);
    virtual int addRecorder(Recorder &theRecorder);
    inline recorder_iterator recorder_begin(void)
      { return theRecorders.begin(); }
    inline const_recorder_iterator recorder_begin(void) const
      { return theRecorders.begin(); }
    inline recorder_iterator recorder_end(void)
      { return theRecorders.end(); }
    inline const_recorder_iterator recorder_end(void) const
      { return theRecorders.end(); }
    virtual int record(int track, double timeStamp= 0.0);
    void restart(void);
    virtual int removeRecorders(void);
    void setLinks(Domain *dom);
    void SetOutputHandlers(DataOutputHandler::map_output_handlers *oh);
  };
} // end of XC namespace

#endif


