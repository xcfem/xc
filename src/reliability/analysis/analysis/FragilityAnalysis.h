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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/04/28 20:51:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/FragilityAnalysis.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef FragilityAnalysis_h
#define FragilityAnalysis_h

#include <reliability/analysis/analysis/ReliabilityAnalysis.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/analysis/designPoint/FindDesignPointAlgorithm.h>
#include <tcl.h>

#include <fstream>
using std::ofstream;

namespace XC {
class GFunEvaluator;
class GradGEvaluator;

class FragilityAnalysis : public ReliabilityAnalysis
{
private:
	ReliabilityDomain *theReliabilityDomain;
	FindDesignPointAlgorithm *theFindDesignPointAlgorithm;
	GradGEvaluator *theGradGEvaluator;
	std::string fileName;
	double first, last;
	int parameterNumber, numIntervals;
	Tcl_Interp *theTclInterp;

public:
	FragilityAnalysis(ReliabilityDomain *theReliabilityDomain,
					  FindDesignPointAlgorithm *theFindDesignPointAlgorithm,
					  GradGEvaluator *theGradGEvaluator,
					  int parameterNumber,
					  double first,
					  double last,
					  int numIntervals,
			  const std::string &fName,
					  Tcl_Interp *theTclInterp);

	int analyze(void);
};
} // fin namespace XC

#endif
