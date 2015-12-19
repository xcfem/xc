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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/StaticDomainDecompositionAnalysis.h,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for 
// StaticDomainDecompositionAnalysis.StaticDomainDecompositionAnalysis is a subclass 
// of DomainDecompositionAnalysis used to perform a static analysis step on a subdomain.

//
// What: "@(#) StaticDomainDecompositionAnalysis.h, revA"

#ifndef StaticDomainDecompositionAnalysis_h
#define StaticDomainDecompositionAnalysis_h

#include <solution/analysis/analysis/DomainDecompositionAnalysis.h>
#include <utility/actor/actor/MovableObject.h>

namespace XC {
class Subdomain;

//! @ingroup TipoAnalisis
//
//! @brief StaticDomainDecompositionAnalysis is a subclass 
//! of DomainDecompositionAnalysis used to perform
//! a static analysis step on a subdomain.
class StaticDomainDecompositionAnalysis: public DomainDecompositionAnalysis
  {
    friend class ProcSolu;
    StaticDomainDecompositionAnalysis(Subdomain &theDomain,SoluMethod *s= NULL);
    Analysis *getCopy(void) const;
  public:
    ~StaticDomainDecompositionAnalysis(void);
    void clearAll(void);
    int initialize(void);
    int domainChanged(void);

    // methods for non standard domain deomposition analysis
    int analyze(double dT);
    bool doesIndependentAnalysis(void);    

    // methods for standard domain deomposition analysis
    // that do some form of condensation to the tangent
    int getNumExternalEqn(void);
    int getNumInternalEqn(void);
    int newStep(double dT);
    int computeInternalResponse(void);
    int formTangent(void);
    int formResidual(void);
    int formTangVectProduct(Vector &force);
    const Matrix &getTangent(void);
    const Vector &getResidual(void);
    const Vector &getTangVectProduct(void);

    // methods to change the analysis aggregates
    int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    int setIntegrator(IncrementalIntegrator &theIntegrator);
    int setLinearSOE(LinearSOE &theSOE);

    // methods to send/receive
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline Analysis *StaticDomainDecompositionAnalysis::getCopy(void) const
  { return new StaticDomainDecompositionAnalysis(*this); }
} // end of XC namespace

#endif











