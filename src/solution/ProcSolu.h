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
class FEProblem;

//!  @defgroup Solu Solution of the finite element analysis problem.

//!  \ingroup Solu
//! 
//! @brief Solution procedure for the finite element problem.
//!
//! This objects allows the definition of the problem to be analyzed by
//! specifiying:.
//! - The type of analysis: static, dynamic, eigenvalues,...
//! - The parameters that control the solution procedure.
class ProcSolu: public EntCmd
  {
  private:
    ProcSoluControl solu_control;//!< Control of the solution procedure.
    Analysis *theAnalysis; //! Analysis type (static, dynamic, eigenvalues,...).
  protected:
    friend class FEProblem;

    void libera_analysis(void);
    bool alloc_analysis(const std::string &,const std::string &,const std::string &);
    void copia_analysis(Analysis *);
 
    void libera(void);

    void revertToStart(void);
    FEProblem *getFEProblem(void);
    const FEProblem *getFEProblem(void) const;

    ProcSolu(FEProblem *owr);
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
