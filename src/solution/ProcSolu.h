//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
//! dinámico, de eigenvalues,...) se hace mediante el objeto theAnalysis
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
