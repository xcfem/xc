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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2003/10/27 23:04:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/LimitStateFunction.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef LimitStateFunction_h
#define LimitStateFunction_h

#include <reliability/domain/components/ReliabilityDomainComponent.h>
#include <utility/matrix/Vector.h>
#include <string>

namespace XC {
class LimitStateFunction : public ReliabilityDomainComponent
  {
  private:
    int tokenizeIt(const std::string &expression);

    std::string originalExpression;
    std::string tokenizedExpression;
    std::string expressionWithAddition;
  public:
    // FORM analysis:
    double GFunValueAtStartPt;
    double GFunValueAtEndPt;
    double FORMReliabilityIndexBeta;
    double FORMProbabilityOfFailure_pf1;
    Vector designPoint_x_inOriginalSpace;
    Vector designPoint_u_inStdNormalSpace;
    Vector normalizedNegativeGradientVectorAlpha;
    Vector importanceVectorGamma;
    int numberOfStepsToFindDesignPointAlgorithm;
    
    // From Simulation analysis:
    double SimulationReliabilityIndexBeta;
    double SimulationProbabilityOfFailure_pfsim;
    double CoefficientOfVariationOfPfFromSimulation;
    int NumberOfSimulations;
    
    // From SORM analysis:
    double SORMCurvatureFittingBetaBreitung;
    double SORMCurvatureFittingPf2Breitung;
    double SORMPointFittingBetaBreitung;
    double SORMPointFittingPf2Breitung;
    double SORMUsingSearchBetaBreitung;
    double SORMUsingSearchPf2Breitung;
    Vector lastSearchDirection;
    int numberOfCurvatauresUsed;
    Vector secondLast_u;
    Vector secondLastAlpha;
  public:
    LimitStateFunction(int tag,const std::string &expression);

    // Method to get/add limit-state function
    const std::string &getExpression(void) const;
    const std::string &getTokenizedExpression(void) const;
    int addExpression(const std::string &expression);
    int removeAddedExpression(void);
  };
} // end of XC namespace

#endif
