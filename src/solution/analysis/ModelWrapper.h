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
//ModelWrapper.h

#ifndef MODELWRAPPER_H
#define MODELWRAPPER_H

#include "xc_utils/src/kernel/CommandEntity.h"

namespace XC {

class Domain;
class AnalysisModel;
class ConstraintHandler;
class DOF_Numberer;
class FEM_ObjectBroker;
class ID;
class CommParameters;
class AnalysisAggregation;
class Integrator;

//! @ingroup Analysis
//
//! @brief Wrapper for the finite element model "as seen" from the solver.
//
//! The model wrapper contains:
//! - The analysis model. A container object holding the FE\_Element and
//!   DOF\_Group objects created by the ConstraintHandler object.
//! - The constraint handler. An object which creates the DOF\_Group
//!   and FE\_Element objects, the type of objects created depending on how
//!   the specified constraints in the domain are to be handled. 
//! - The DOF numberer.  An object responsible for providing equation
//!   numbers to the individual degrees of freedom in each DOF\_Group object.
class ModelWrapper: public CommandEntity
  {
    AnalysisModel *theModel; //!< Analysis model
    ConstraintHandler *theHandler; //!< Constrainnt handler.
    DOF_Numberer *theDOFNumberer;  //!< DOF numberer.

    void free_analysis_model(void);
    void alloc_analysis_model(void);
    void copy_analysis_model(AnalysisModel *);

    void free_constraint_handler(void);
    bool alloc_constraint_handler(const std::string &);
    void copy_constraint_handler(const ConstraintHandler *);

    bool setup_numerador(void);
    void free_numerador(void);
    bool alloc_numerador(const std::string &);
    void copy_numerador(const DOF_Numberer *);

    void copy(const ModelWrapper &);
    void free_mem(void);

    AnalysisAggregation *getAnalysisAggregation(void);
    const AnalysisAggregation *getAnalysisAggregation(void) const;
  protected:
    friend class FEProblem;
    friend class AnalysisAggregation;

  public:
    ModelWrapper(AnalysisAggregation *owr= nullptr);
    ModelWrapper(const ModelWrapper &);
    ModelWrapper &operator=(const ModelWrapper &);
    ~ModelWrapper(void);

    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;

    //! @brief Return a pointer to the constraints handler.
    inline ConstraintHandler *getConstraintHandlerPtr(void)
      { return theHandler; }
    //! @brief Return a pointer to the constraints handler.
    inline const ConstraintHandler *getConstraintHandlerPtr(void) const
      { return theHandler; }
    //! @brief Return a pointer to the DOF numberer.
    inline DOF_Numberer *getDOF_NumbererPtr(void)
      { return theDOFNumberer; }
    //! @brief Return a pointer to the DOF numberer.
    inline const DOF_Numberer *getDOF_NumbererPtr(void) const
      { return theDOFNumberer; }
    //! @brief Return a pointer to the analysis model.
    inline AnalysisModel *getAnalysisModelPtr(void)
      { return theModel; }
    //! @brief Return a pointer to the analysis model.
    inline const AnalysisModel *getAnalysisModelPtr(void) const
      { return theModel; }
    int setNumberer(DOF_Numberer &theNumberer);
    void brokeConstraintHandler(const CommParameters &,const ID &);
    void brokeNumberer(const CommParameters &,const ID &);
    void brokeAnalysisModel(const CommParameters &,const ID &);
    bool CheckPointers(void);

    DOF_Numberer &newNumberer(const std::string &);
    ConstraintHandler &newConstraintHandler(const std::string &);

    void clearAll(void);
  };

} // end of XC namespace

#endif
