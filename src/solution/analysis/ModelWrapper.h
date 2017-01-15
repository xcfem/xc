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

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class Domain;
class AnalysisModel;
class ConstraintHandler;
class DOF_Numberer;
class FEM_ObjectBroker;
class ID;
class CommParameters;
class SoluMethod;
class Integrator;

//! @ingroup Analisis
//
//! @brief Envoltorio para el modelo de elementos finitos «visto» desde el «solver».
//! Queda definido por el dominio a analizar, el modelo de análisis
//! y el manejador de coacciones a emplear y el numerador de grados
//! de libertad.
//! Este objeto se encarga de crear, alojar y borrar el modelo de análisis,
//! el gestor de coacciones y el numerador de grados de libertad.
class ModelWrapper: public EntCmd
  {
    AnalysisModel *theModel; //!< Modelo de análisis
    ConstraintHandler *theHandler; //!< Manejador de coacciones.
    DOF_Numberer *theDOFNumberer;  //!< Numerador de grados de libertad.

    void libera_analysis_model(void);
    void alloc_analysis_model(void);
    void copia_analysis_model(AnalysisModel *);

    void libera_constraint_handler(void);
    bool alloc_constraint_handler(const std::string &);
    void copia_constraint_handler(const ConstraintHandler *);

    bool setup_numerador(void);
    void libera_numerador(void);
    bool alloc_numerador(const std::string &);
    void copia_numerador(const DOF_Numberer *);

    void copia(const ModelWrapper &otro);
    void libera(void);

    SoluMethod *getSoluMethod(void);
    const SoluMethod *getSoluMethod(void) const;
  protected:
    friend class ProblemaEF;
    friend class SoluMethod;

  public:
    ModelWrapper(SoluMethod *owr=NULL);
    ModelWrapper(const ModelWrapper &);
    ModelWrapper &operator=(const ModelWrapper &);
    ~ModelWrapper(void);

    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;

    //! @brief Devuelve un puntero al manejador de coacciones.
    inline ConstraintHandler *getConstraintHandlerPtr(void)
      { return theHandler; }
    //! @brief Devuelve un puntero al manejador de coacciones.
    inline const ConstraintHandler *getConstraintHandlerPtr(void) const
      { return theHandler; }
    //! @brief Devuelve un puntero al renumerador.
    inline DOF_Numberer *getDOF_NumbererPtr(void)
      { return theDOFNumberer; }
    //! @brief Devuelve un puntero al renumerador.
    inline const DOF_Numberer *getDOF_NumbererPtr(void) const
      { return theDOFNumberer; }
    //! @brief Devuelve un puntero al modelo de análisis.
    inline AnalysisModel *getAnalysisModelPtr(void)
      { return theModel; }
    //! @brief Devuelve un puntero al modelo de análisis.
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
