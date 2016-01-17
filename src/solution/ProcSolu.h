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
//ProcSolu.h

#ifndef PROCSOLU_H
#define PROCSOLU_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "ProcSoluControl.h"


namespace XC {

class SolutionAlgorithm;
class Integrator;
class ConstraintHandler;
class DOF_Numberer;
class SystemOfEqn;
class Analysis;
class ProblemaEF;

//!  @defgroup Solu Solution of the finite element analysis problem.

//!  \ingroup Solu
//! 
//! @brief Procedimiento de solución del problema de elementos finitos.
//!
//! La definición del análisis que se pretende realizar (problema estático,
//! dinámico, de autovalores,...) se hace mediante el objeto theAnalysis
//! de la clase Analysis. La definición de los algoritmos a emplear para
//! realizar el análisis se hace mediante el objeto solu_control
class ProcSolu: public EntCmd
  {
  private:
    ProcSoluControl solu_control;//!< Modo de realizar el análisis.
    Analysis *theAnalysis; //! Análisis a realizar (estático, dinámico,...).
  protected:
    friend class ProblemaEF;

    void libera_analysis(void);
    bool alloc_analysis(const std::string &,const std::string &,const std::string &);
    void copia_analysis(Analysis *);
 
    void libera(void);

    void revertToStart(void);
    ProblemaEF *getProblemaEF(void);
    const ProblemaEF *getProblemaEF(void) const;

    ProcSolu(ProblemaEF *owr);
    ProcSolu(const ProcSolu &otro);
    ProcSolu &operator=(const ProcSolu &otro);
  public:
    ~ProcSolu(void);
    void clearAll(void);

    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;
    const DomainSolver *getDomainSolverPtr(void) const;
    DomainSolver *getDomainSolverPtr(void);
    const Subdomain *getSubdomainPtr(void) const;
    Subdomain *getSubdomainPtr(void);
    ProcSoluControl &getSoluControl(void);
    const ProcSoluControl &getSoluControl(void) const;
    Analysis *getAnalysisPtr(void);
    const Analysis *getAnalysisPtr(void) const;
    Analysis &getAnalysis(void);
    Analysis &newAnalysis(const std::string &,const std::string &,const std::string &);

    DataOutputHandler::map_output_handlers *getOutputHandlers(void) const;
  };

} // end of XC namespace

#endif
