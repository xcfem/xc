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
                                                                        
// $Revision: 1.6 $
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/AlgorithmIncrements.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 01/01
// Revision: A
//
// Description: This file contains the class definition for AlgorithmIncrements.
// A AlgorithmIncrements will display the X and B in the SOE associated with the
// algorithm on a record.

//
// What: "@(#) ModelBuilder.h, revA"

#ifndef AlgorithmIncrements_h
#define AlgorithmIncrements_h

#include <fstream>
#include "Recorder.h"
using std::ofstream;

namespace XC {
class EquiSolnAlgo;
class ID;
class Vector;

class AlgorithmIncrements : public Recorder
  {
  private:
    //    ColorMap *theMap;
    //Renderer *theRenderer;
    EquiSolnAlgo *theAlgo;

    int numRecord;
    bool displayRecord;
    char *fileName;
    ofstream theFile;     
  public:
    AlgorithmIncrements(EquiSolnAlgo *theAlgo,
			const char *windowTitle, 
			int xLoc, int yLoc, int width, int height,
			bool displayRecord = false,
			const char *fileName= NULL);
    ~AlgorithmIncrements(void);
    int plotData(const Vector &X, const Vector &B);
    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);    
  };
} // end of XC namespace

#endif







