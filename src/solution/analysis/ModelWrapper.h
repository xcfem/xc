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
    bool procesa_cmd_constraint_handler(const std::string &cmd,CmdStatus &status);
    bool procesa_cmd_numerador(const std::string &cmd,CmdStatus &status);
    bool procesa_comando(CmdStatus &status);
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

} // fin namespace XC

#endif
