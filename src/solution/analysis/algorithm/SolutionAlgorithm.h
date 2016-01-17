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
                                                                        
// $Revision: 1.3 $
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/SolutionAlgorithm.h,v $
                                                                        
                                                                        
#ifndef SolutionAlgorithm_h
#define SolutionAlgorithm_h

// File: ~/OOP/analysis/algorithm/SolutionAlgorithm.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for SolutionAlgorithm.
// SolutionAlgorithm is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) SolutionAlgorithm.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "utility/recorder/ObjWithRecorders.h"
#include "utility/handler/DataOutputHandler.h"

namespace XC {
class FEM_ObjectBroker;
class AnalysisModel;
class Integrator;
class SystemOfEqn;
class Recorder;
class SoluMethod;

//! @ingroup Analisis
//
//! @defgroup AnalAlgo Algoritmos a emplear para el análisis.
//
//! @ingroup AnalAlgo
//
//! @brief Clase base para los algoritmos de solución.
class SolutionAlgorithm: public MovableObject, public ObjWithRecorders
  {
  protected:
    SoluMethod *getSoluMethod(void);
    const SoluMethod *getSoluMethod(void) const;

    virtual Domain *get_domain_ptr(void);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


    SolutionAlgorithm(SoluMethod *,int classTag);
    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    virtual SolutionAlgorithm *getCopy(void) const= 0;
  public:
    virtual int domainChanged(void);

    virtual AnalysisModel *getAnalysisModelPtr(void);
    virtual const AnalysisModel *getAnalysisModelPtr(void) const;
    virtual Integrator *getIntegratorPtr(void);
    virtual const Integrator *getIntegratorPtr(void) const;
    virtual LinearSOE *getLinearSOEPtr(void);
    virtual const LinearSOE *getLinearSOEPtr(void) const;
    virtual EigenSOE *getEigenSOEPtr(void);
    virtual const EigenSOE *getEigenSOEPtr(void) const;
    virtual const DomainSolver *getDomainSolverPtr(void) const;
    virtual DomainSolver *getDomainSolverPtr(void);
    virtual const Subdomain *getSubdomainPtr(void) const;
    virtual Subdomain *getSubdomainPtr(void);
  };
} // end of XC namespace

#endif


