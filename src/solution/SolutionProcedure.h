// -*-c++-*-
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
//SolutionProcedure.h

#ifndef PROCSOLU_H
#define PROCSOLU_H

#include "utility/kernel/CommandEntity.h"
#include "SolutionProcedureControl.h"


namespace XC {

class SolutionAlgorithm;
class Integrator;
class ConstraintHandler;
class DOF_Numberer;
class SystemOfEqn;
class Analysis;
class FEProblem;

//!  @defgroup Solu Solution of the finite element analysis problem.

//! 
//! @brief Solution procedure for the finite element problem.
//!
//! This objects allows the definition of the problem to be analyzed by
//! specifying:.
//! - The type of analysis: static, dynamic, eigenvalues,...
//! - The parameters that control the solution procedure.
//!  @ingroup Solu
class SolutionProcedure: public CommandEntity
  {
  private:
    SolutionProcedureControl solu_control;//!< Control of the solution procedure.
    Analysis *theAnalysis; //! Analysis type (static, dynamic, eigenvalues,...).
  protected:
    friend class FEProblem;

    void free_analysis(void);
    bool alloc_analysis(const std::string &,const std::string &,const std::string &);
    void copy_analysis(Analysis *);
 
    void free_mem(void);

    void revertToStart(void);
    FEProblem *getFEProblem(void);
    const FEProblem *getFEProblem(void) const;

    SolutionProcedure(FEProblem *owr);
    SolutionProcedure(const SolutionProcedure &);
    SolutionProcedure &operator=(const SolutionProcedure &);
  public:
    ~SolutionProcedure(void);
    void clearAll(void);

    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;
    const DomainSolver *getDomainSolverPtr(void) const;
    DomainSolver *getDomainSolverPtr(void);
    const Subdomain *getSubdomainPtr(void) const;
    Subdomain *getSubdomainPtr(void);
    SolutionProcedureControl &getSoluControl(void);
    const SolutionProcedureControl &getSoluControl(void) const;
    Analysis *getAnalysisPtr(void);
    const Analysis *getAnalysisPtr(void) const;
    Analysis &getAnalysis(void);
    Analysis &newAnalysis(const std::string &,const std::string &,const std::string &);

    DataOutputHandler::map_output_handlers *getOutputHandlers(void) const;
  };

} // end of XC namespace

#endif
