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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/DomainDecompositionAnalysis.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// DomainDecompositionAnalysis. DomainDecompositionAnalysis is a subclass 
// of AnalysisAnalysis, it is used when performing a domain decomposition
// analysis. It provides methods which can be invoked by a subdomain to
// perform the numerical computations required.
//
// What: "@(#) DomainDecompositionAnalysis.h, revA"

#ifndef DomainDecompositionAnalysis_h
#define DomainDecompositionAnalysis_h

#include <solution/analysis/analysis/Analysis.h>
#include "utility/matrix/Vector.h"
#include <utility/actor/actor/MovableObject.h>

namespace XC {
class Subdomain;
 class Matrix;
 class DomainSolver;

//! @ingroup TipoAnalisis
//
//! @brief Análisis por descomposición del dominio.
class DomainDecompositionAnalysis: public Analysis, public MovableObject
  {
  private:
    Subdomain *theSubdomain; //!< Apuntador al subdominio. 
    DomainSolver *theSolver; //!< Apuntador al solver de tipo DomainSolver.
    Vector theResidual; //!< Vector residuo.
    int numEqn; //!< Número de ecuaciones.
    int numExtEqn; //!< Número de ecuaciones externas.

    // the following 2 variables are used to allow formResidual()
    // and formTangVectProduct() to be called before formTangent()
    // - this must be allowed as typical elements will not have to fromTangent
    // before being asked to form Residual(). 
    bool tangFormed; //!< Verdadero si la matriz de rigidez tangente ya está formada.
    int tangFormedCount; // saves the expense of computing formTangent() 
	               // for same state of Subdomain.
  protected:
    int domainStamp;
    //! @brief Devuelve un puntero al subdominio.
    inline Subdomain *getSubdomainPtr(void) const
      { return theSubdomain; }

    void set_all_links(void);

    friend class ProcSolu;
    friend class FEM_ObjectBroker;
    DomainDecompositionAnalysis(Subdomain &theDomain,SoluMethod *s= NULL);
    DomainDecompositionAnalysis(Subdomain &theDomain,DomainSolver &theSolver,SoluMethod *s= NULL);
    DomainDecompositionAnalysis(int classTag, Subdomain &theDomain,SoluMethod *s= NULL);
    DomainDecompositionAnalysis(int classTag, Subdomain &theDomain,DomainSolver &theSolver,SoluMethod *s);
    Analysis *getCopy(void) const;
  public:

    virtual void clearAll(void);	    
    virtual int initialize(void);
    virtual int domainChanged(void);

    // methods for non standard domain deomposition analysis
    virtual bool doesIndependentAnalysis(void);    
    virtual int analyze(double dT);

    // methods for standard domain deomposition analysis
    // that do some form of condensation to the tangent
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
    
    virtual const DomainSolver *getDomainSolver(void) const;
    virtual DomainSolver *getDomainSolver(void);
    virtual const Subdomain *getSubdomain(void) const;
    virtual Subdomain *getSubdomain(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    // methods to change the analysis aggregates
    virtual int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    virtual int setIntegrator(IncrementalIntegrator &theIntegrator);
    virtual int setLinearSOE(LinearSOE &theSOE);
    virtual int setConvergenceTest(ConvergenceTest &theTest);
  };
inline Analysis *DomainDecompositionAnalysis::getCopy(void) const
  { return new DomainDecompositionAnalysis(*this); }
} // end of XC namespace

#endif











