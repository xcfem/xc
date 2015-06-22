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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/Analysis.h,v $
                                                                        
                                                                        
#ifndef Analysis_h
#define Analysis_h

// File: ~/analysis/analysis/Analysis.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the interface for the Analysis class.
// Analysis is an abstract class, i.e. no objects of it's type can be created. 
//
// What: "@(#) Analysis.h, revA"

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class SoluMethod;
class ProcSolu;

class Domain;
class AnalysisModel;
class ConstraintHandler;
class DOF_Numberer;

class Integrator;
class IncrementalIntegrator;
class EigenIntegrator;
class LinearBucklingIntegrator;
class TransientIntegrator;
class StaticIntegrator;

class LinearSOE;
class EigenSOE;

class SolutionAlgorithm;
class EigenAlgorithm;
class EquiSolnAlgo;
class DomainDecompAlgo;
class DomainSolver;
class Subdomain;

class ConvergenceTest;

class FEM_ObjectBroker;
class ID;

class CommParameters;

//! @ingroup Analisis
//
//! @defgroup TipoAnalisis Análisis a realizar (estático, dinámico, autovalores,...).
//
//! @ingroup TipoAnalisis
//
//! @brief Clase Base para los objetos que realizan el análisis.
class Analysis: public EntCmd
  {
  protected:
    int analysis_result; //!< igual a cero si se analizó con éxito.
    SoluMethod *metodo_solu; //!< Estrategia de solución.

    int newStepDomain(AnalysisModel *theModel,const double &dT =0.0);
    ProcSolu *getProcSolu(void);
    const ProcSolu *getProcSolu(void) const;    

    friend class ProcSolu;
    Analysis(SoluMethod *metodo);
    virtual Analysis *getCopy(void) const= 0;
  public:
    inline virtual ~Analysis(void) {}

    inline SoluMethod *getSoluMethodPtr(void)
      { return metodo_solu; }
    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    ConstraintHandler *getConstraintHandlerPtr(void);
    DOF_Numberer *getDOF_NumbererPtr(void) const;
    AnalysisModel *getAnalysisModelPtr(void) const;

    LinearSOE *getLinearSOEPtr(void) const;
    EigenSOE *getEigenSOEPtr(void) const;

    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;
    IncrementalIntegrator *getIncrementalIntegratorPtr(void);
    EigenIntegrator *getEigenIntegratorPtr(void);
    LinearBucklingIntegrator *getLinearBucklingIntegratorPtr(void);
    TransientIntegrator *getTransientIntegratorPtr(void);
    StaticIntegrator *getStaticIntegratorPtr(void);

    SolutionAlgorithm *getSolutionAlgorithmPtr(void);
    EigenAlgorithm *getEigenSolutionAlgorithmPtr(void);
    EquiSolnAlgo *getEquiSolutionAlgorithmPtr(void);
    DomainDecompAlgo *getDomainDecompSolutionAlgorithmPtr(void);

    virtual ConvergenceTest *getConvergenceTestPtr(void);
    virtual const ConvergenceTest *getConvergenceTestPtr(void) const;

    virtual const DomainSolver *getDomainSolver(void) const;
    virtual DomainSolver *getDomainSolver(void);
    virtual const Subdomain *getSubdomain(void) const;
    virtual Subdomain *getSubdomain(void);

    // pure virtual functions
    virtual int domainChanged(void) = 0;
    virtual void clearAll(void);
    virtual int setNumberer(DOF_Numberer &theNumberer);
    virtual int setLinearSOE(LinearSOE &theSOE); 
    virtual int setEigenSOE(EigenSOE &theSOE);
    virtual int setIntegrator(Integrator &theNewIntegrator);
    virtual int setAlgorithm(SolutionAlgorithm &theNewAlgorithm);

    void brokeConstraintHandler(const CommParameters &,const ID &);
    void brokeNumberer(const CommParameters &,const ID &);
    void brokeAnalysisModel(const CommParameters &,const ID &);

    void brokeDDLinearSOE(const CommParameters &,const ID &);
    void brokeLinearSOE(const CommParameters &,const ID &);

    void brokeIncrementalIntegrator(const CommParameters &,const ID &);
    void brokeStaticIntegrator(const CommParameters &,const ID &);
    void brokeTransientIntegrator(const CommParameters &,const ID &);

    void brokeDomainDecompAlgo(const CommParameters &,const ID &);
    void brokeEquiSolnAlgo(const CommParameters &,const ID &);

    inline int getAnalysisResult(void) const
      { return analysis_result; }

  };
} // fin namespace XC

#endif

