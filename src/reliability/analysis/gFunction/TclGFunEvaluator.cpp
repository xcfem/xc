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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/27 23:45:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/gFunction/TclGFunEvaluator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/gFunction/TclGFunEvaluator.h>
#include <utility/matrix/Vector.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/domain/components/LimitStateFunction.h>
#include <reliability/domain/components/RandomVariablePositioner.h>
//#include <fstream>
#include <tcl.h>
#include <cstring>


XC::TclGFunEvaluator::TclGFunEvaluator(Tcl_Interp *passedTclInterp,
					ReliabilityDomain *passedReliabilityDomain,
					const std::string &passed_fileName)
:GFunEvaluator(passedTclInterp, passedReliabilityDomain)
{
	fileName= passed_fileName;
}


int
XC::TclGFunEvaluator::runGFunAnalysis(Vector x)
{	
	// Initial declarations
	char theCommand[100];
	int i;


////// IN CASE AN OPENSEES MODEL EXISTS ////////////////////////////////

	// Zero out the response in the structural domain to make ready for next analysis
	char theRevertToStartCommand[10] = "reset";
	Tcl_Eval( theTclInterp, theRevertToStartCommand );


	// Put random variables into the structural domain according to the RandomVariablePositioners
	int numberOfRandomVariablePositioners = theReliabilityDomain->getNumberOfRandomVariablePositioners();
	RandomVariablePositioner *theRandomVariablePositioner;
	int rvNumber;
	for ( i=1 ; i<=numberOfRandomVariablePositioners ; i++ )  {
		theRandomVariablePositioner = theReliabilityDomain->getRandomVariablePositionerPtr(i);
		rvNumber				= theRandomVariablePositioner->getRvNumber();
		theRandomVariablePositioner->update(x(rvNumber-1));
	}

//////////////////////////////////////////////////////////////////////////


	// Set values of random variables in the Tcl intepreter
	for (i=0; i<x.Size(); i++) {
		sprintf(theCommand,"set x_%d %20.12e",(i+1),x(i));
		Tcl_Eval( theTclInterp, theCommand );
	}


	// Source the code file that the user has provided
	sprintf(theCommand,"source %s",fileName.c_str());
	Tcl_Eval( theTclInterp, theCommand );


	return 0;
}




int
XC::TclGFunEvaluator::tokenizeSpecials(const std::string &theExpression)
{

	return 0;
}

