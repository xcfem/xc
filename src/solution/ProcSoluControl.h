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
//ProcSoluControl.h

#ifndef PROCSOLUCONTROL_H
#define PROCSOLUCONTROL_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "analysis/MapModelWrapper.h"
#include "MapSoluMethod.h"

namespace XC {

class ProcSolu;
class Domain;

//!  \ingroup Solu
//! 
//! @brief Contenedor de objetos para el control del 
//! procedimiento de solución del problema de elementos finitos.
class ProcSoluControl: public EntCmd
  {
  private:
    MapModelWrapper solu_models; //!< Modelos para la solución.
    MapSoluMethod solu_methods; //!< Métodos para la solución.

    ProcSolu *getProcSolu(void);
    const ProcSolu *getProcSolu(void) const;
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    ProcSoluControl(ProcSolu *owr);

    Domain *getDomain(void);
    const Domain *getDomain(void) const;
    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;

    DataOutputHandler::map_output_handlers *getOutputHandlers(void) const;
    const ModelWrapper *getModelWrapper(const std::string &) const;
    ModelWrapper *getModelWrapper(const std::string &);
    const SoluMethod *getSoluMethod(const std::string &) const;
    SoluMethod *getSoluMethod(const std::string &);

    MapModelWrapper &getModelWrapperContainer(void);
    MapSoluMethod &getSoluMethodContainer(void);

    void revertToStart(void);
    void clearAll(void);
  };

} // end of XC namespace

#endif
