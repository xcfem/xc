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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/01 00:07:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/SubdomainAnalysis.h,v $
                                                                        
                                                                        
// File: ~/analysis/method/SubdomainAnalysis.h
// 
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// SubdomainAnalysis. SubdomainAnalysis is a subclass 
// of AnalysisAnalysis, it is used when performing a domain decomposition
// analysis. It provides methods which can be invoked by a subdomain to
// perform the numerical computations required.
//
// What: "@(#) SubdomainAnalysis.h, revA"

#ifndef SubdomainAnalysis_h
#define SubdomainAnalysis_h

#include <solution/analysis/analysis/Analysis.h>
#include <utility/actor/actor/MovableObject.h>

namespace XC {
class Subdomain;
class Vector;

//! @ingroup TipoAnalisis
//
//! @brief SubdomainAnalysis is a subclass 
//! of AnalysisAnalysis, it is used when performing a domain decomposition
//! analysis. It provides methods which can be invoked by a subdomain to
//! perform the numerical computations required.
class SubdomainAnalysis: public Analysis, public MovableObject
  {
  private:
    Subdomain 		     *theSubdomain;
    DomainSolver	     *theSolver;
    Vector 		     *theResidual;
    int numEqn;
    int numExtEqn;

    // the following 2 variables are used to allow formResidual()
    // and formTangVectProduct() to be called before formTangent()
    // - this must be allowed as typical elements will not have to fromTangent
    // before being asked to form Residual(). 
    bool tangFormed;
    int tangFormedCount; // saves the expense of computing formTangent() 
	               // for same state of Subdomain.
    int domainStamp;			   
  protected: 
    Subdomain *getSubdomainPtr(void) const;
    DomainSolver        *getDomainSolverPtr(void) const;
    friend class ProcSolu;
    SubdomainAnalysis(Subdomain &theDomain);

    SubdomainAnalysis(int classTag, Subdomain &theDomain);    
    
    SubdomainAnalysis(Subdomain &theDomain,
				ConstraintHandler &theHandler,
				DOF_Numberer &theNumberer,
				AnalysisModel &theModel,
				DomainDecompAlgo &theSolnAlgo,		   
				IncrementalIntegrator &theIntegrator,	
				LinearSOE &theSOE,
				DomainSolver &theSolver);
    Analysis *getCopy(void) const;
  public:

    virtual ~SubdomainAnalysis();
    
    virtual int analyze(void);
    virtual int domainChanged(void);

    virtual int  getNumExternalEqn(void);
    virtual int  getNumInternalEqn(void);
    virtual int  newStep(double dT);
    virtual int  computeInternalResponse(void);
    virtual int  formTangent(void);
    virtual int  formResidual(void);
    virtual int  formTangVectProduct(Vector &force);
    virtual const Matrix &getTangent(void);
    virtual const Vector &getResidual(void);
    virtual const Vector &getTangVectProduct(void);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

inline Analysis *SubdomainAnalysis::getCopy(void) const
  { return new SubdomainAnalysis(*this); }
} // fin namespace XC

#endif











