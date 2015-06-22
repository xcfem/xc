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
#include <reliability/analysis/gFunction/TclMatlabGFunEvaluator.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
#include <reliability/domain/components/ReliabilityDomain.h>

#include <tcl.h>
#include <cstring>

#include <fstream>
//#include <engine.h>
//#include <mex.h>



XC::TclMatlabGFunEvaluator::TclMatlabGFunEvaluator(Tcl_Interp *passedTclInterp,
											   ReliabilityDomain *passedReliabilityDomain)

:GFunEvaluator()
{
	theTclInterp			= passedTclInterp;
	theReliabilityDomain	= passedReliabilityDomain;
}

int
XC::TclMatlabGFunEvaluator::evaluate_g(Vector x)
{

	// "Download" limit-state function from reliability domain
	int lsf = theReliabilityDomain->getTagOfActiveLimitStateFunction();
	LimitStateFunction *theLimitStateFunction = 
		theReliabilityDomain->getLimitStateFunctionPtr(lsf);

	if (lsf == 1) {

		// Print the realization of x to file called 'realization.txt'
	  std::ofstream outputFile( "realization.txt", std::ios::out );
		for (int i=0; i<x.Size(); i++) {
			outputFile << x(i) << std::endl;
		}
		outputFile.close();


		// Execute a Tcl file called 'tclgfun.tcl' (remember to "reset" analysis!)
		char theTclCommand[30];
		sprintf(theTclCommand,"source tclgfun.tcl");
		Tcl_Eval( theTclInterp, theTclCommand );

		
		// Start a Matlab engine
		Engine *ep;
		ep = engOpen("\0");


		// Execute a Matlab function called 'matlabgfun'
		char theMatlabCommand[50];
		sprintf(theMatlabCommand,"matlabgfun");
		engEvalString(ep, theMatlabCommand);


		// Shut down the Matlab engine
		engClose(ep);

	}
	else {
		// Does nothing
	}


	// Read value of limit-state functions from file called 'gfun.txt'
	double gvalue;
	ifstream inputFile( "gfun.txt", ios::in );

	for (int i=1; i<=lsf; i++) {
		inputFile >> gvalue;
	}


	// Store the value of the g-function
	g = gvalue;

	return 0;
}


double
XC::TclMatlabGFunEvaluator::get_g()
{
	return g;
}
