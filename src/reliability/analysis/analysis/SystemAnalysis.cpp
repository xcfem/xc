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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
// $Date: 2003/10/27 23:45:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/SystemAnalysis.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/analysis/SystemAnalysis.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/analysis/analysis/ReliabilityAnalysis.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/analysis/misc/MatrixOperations.h>
#include <reliability/domain/distributions/NormalRV.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <cmath>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;

XC::SystemAnalysis::SystemAnalysis(ReliabilityDomain *passedReliabilityDomain,
							   const std::string &passedFileName)
:ReliabilityAnalysis()
{
	theReliabilityDomain = passedReliabilityDomain;
	fileName= passedFileName;
}


double XC::SystemAnalysis::getLowerBound(void)
{
	return minLowerBound;
}



double 
XC::SystemAnalysis::getUpperBound(void)
{
	return maxUpperBound;
}



int 
XC::SystemAnalysis::analyze(void)
{

	// Alert the user that the system analysis has started
	std::cerr << "System Reliability XC::Analysis is running ... " << std::endl;

	
	// Initial declarations
	double beta;
	double pf1;
	Vector alpha;
	LimitStateFunction *theLimitStateFunction;
	NormalRV aStdNormalRV(1, 0.0, 1.0, 0.0);
	int i, j, k, m, n;

	// Number of limit-state functions
	int numLsf = theReliabilityDomain->getNumberOfLimitStateFunctions();

	// Number of random variables
	int nrv = theReliabilityDomain->getNumberOfRandomVariables();

	// Allocate vectors to store ALL the betas and alphas
	Vector allBetas(numLsf);
	Vector allPf1s(numLsf);
	Matrix allAlphas(nrv,numLsf);

	// Loop over number of limit-state functions and collect results
	for (i=0; i<numLsf; i++ ) {

		// Get FORM results from the limit-state function
		theLimitStateFunction = theReliabilityDomain->getLimitStateFunctionPtr(i+1);
		beta = theLimitStateFunction->FORMReliabilityIndexBeta;
		pf1 = theLimitStateFunction->FORMProbabilityOfFailure_pf1;
		alpha = theLimitStateFunction->normalizedNegativeGradientVectorAlpha;

		// Put FORM results into vector of all betas and alphas
		// Note that the first index of 'allAlphas' here denote 'nrv'
		allBetas(i) = beta;
		allPf1s(i) = pf1;
		for (j=0; j<nrv; j++ ) {
			allAlphas(j,i) = alpha(j);
		}
	}


	// Compute vector of 'rhos', that is, dot products between the alphas
	// Note that only the upper diagonal is covered
	Matrix rhos(numLsf,numLsf);
	double dotProduct;
	for (i=0; i<numLsf; i++ ) {
		for (j=i+1; j<numLsf; j++ ) {
			dotProduct = 1.0;
			for (k=0; k<nrv; k++ ) {
				dotProduct=dotProduct*allAlphas(k,i)*allAlphas(k,j);
			}
			rhos(i,j) = dotProduct;
		}
	}


	// Compute the bi-variate normal distribution for all the pairs
	double beta1, beta2, integral;
	double a = 0.0;
	double h, fa, fb, sum_fx2j, sum_fx2j_1;
	int n_2;
	Matrix Pmn(numLsf,numLsf);
	for (m=0; m<numLsf; m++ ) {
		for (n=m+1; n<numLsf; n++ ) {
			beta1 = allBetas(m);
			beta2 = allBetas(n);
			// Composite Simpson's numerical integration
			n_2 = 400; // (half the number of intervals)
			h = rhos(m,n)/(2.0*n_2);
			fa = functionToIntegrate(a,beta1,beta2);
			fb = functionToIntegrate(rhos(m,n),beta1,beta2);
			sum_fx2j = 0.0;
			sum_fx2j_1 = 0.0;
			for (int j=1;  j<=n_2;  j++) {
				sum_fx2j   = sum_fx2j   + functionToIntegrate((a+(j*2.0)*h)    ,beta1,beta2);
				sum_fx2j_1 = sum_fx2j_1 + functionToIntegrate((a+(j*2.0-1.0)*h),beta1,beta2);
			}
			sum_fx2j = sum_fx2j - fb;
			integral = h/3.0*(fa + 2.0*sum_fx2j + 4.0*sum_fx2j_1 + fb);
			Pmn(m,n) = aStdNormalRV.getCDFvalue(-beta1)
				*aStdNormalRV.getCDFvalue(-beta2) + integral;
		}
	}

	// Arrange-envelope
	minLowerBound = 1.0;
	maxUpperBound = 0.0;
	Vector arrangement(numLsf);
	Matrix Pmn_arranged = Pmn;
	Vector allPf1s_arranged = allPf1s;

	for (i=0; i<(factorial(numLsf)); i++) {

		arrangement = arrange(numLsf);

		for (j=0; j<numLsf; j++) {
			allPf1s_arranged(j) = allPf1s(((int)arrangement(j)-1));
		}
		for (j=0; j<numLsf; j++) {
			for (k=0; k<numLsf; k++) {
				Pmn_arranged(j,k) = Pmn(((int)arrangement(j)-1),((int)arrangement(k)-1));
			}
		}

		// Compute lower probability bound
		double lowerBound = allPf1s_arranged(0);
		double temp1, temp2;
		for (m=2; m<=numLsf; m++) {
			temp1 = 0.0;
			for (n=1; n<=n-1; n++) {
				temp1 += Pmn_arranged(m-1,n-1);
			}
			temp2 = allPf1s_arranged(m-1) - temp1;
			if (temp2 < 0.0) {
				temp2 = 0.0;
			}
			lowerBound += temp2;
		}


		// Compute upper probability bound
		double upperBound = allPf1s_arranged(0);
		double maximus;
		for (m=2; m<=numLsf; m++) {
			maximus = 0.0;
			for (n=1; n<=m-1; n++) {
				if (Pmn_arranged(m-1,n-1) > maximus) {
					maximus = Pmn(m-1,n-1);
				}
			}
			upperBound += allPf1s_arranged(m-1) - maximus;
		}

		// Update bounds
		if (lowerBound < minLowerBound) {
			minLowerBound = lowerBound;
		}
		if (upperBound > maxUpperBound) {
			maxUpperBound = upperBound;
		}
	}


	// Print results  (should do this over all defined systems)
	std::ofstream outputFile( fileName.c_str(), ios::out );
	
	outputFile << "#######################################################################" << std::endl;
	outputFile << "#                                                                     #" << std::endl;
	outputFile << "#  SYSTEM ANALYSIS RESULTS                                            #" << std::endl;
	outputFile << "#                                                                     #" << std::endl;
	outputFile.setf(ios::scientific, ios::floatfield);
	outputFile << "#  Lower probability bound: ........................... " 
		<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<minLowerBound
		<< "  #" << std::endl;
	outputFile << "#  Upper probability bound: ........................... " 
		<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<maxUpperBound
		<< "  #" << std::endl;
	outputFile << "#                                                                     #" << std::endl;
	outputFile << "#  Warning: All permutations of the limit-state functions should      #" << std::endl;
	outputFile << "#           be checked. Currently, this is not done automatically.    #" << std::endl;
	outputFile << "#           The user is therefore advised to do this manually.        #" << std::endl;
	outputFile << "#           Contact the developer for further details/plans.          #" << std::endl;
	outputFile << "#                                                                     #" << std::endl;
	outputFile << "#######################################################################" << std::endl << std::endl << std::endl;

	outputFile.close();


	// INform the user on screen
	std::cerr << "System reliability analysis completed." <<std::endl;

	return 0;
}



double
XC::SystemAnalysis::functionToIntegrate(double rho, double beta1, double beta2)
{
	double pi = 3.14159265358979;
	return 1.0/(2.0*pi*sqrt(1.0-rho*rho)) 
		* exp(-(beta1*beta1+beta2*beta2-2*rho*beta1*beta2)
		/(2.0*(1.0-rho*rho)));
}

int
XC::SystemAnalysis::factorial(int num)
{
	int i = num-1;
	int result = num;

	while (i>0) {
		result = result * i;
		i--;
	}

	return result;
}

 XC::Vector
XC::SystemAnalysis::arrange(int num)
{
	Vector arrang(num);

	// This is not yet implemented

	for (int i=0; i<num; i++) {
		arrang(i) = i+1;
	}

	return arrang;
}
