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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/03/18 22:10:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/FilePlotter.h,v $
                                                                        
// Written: fmk 
// Created: 11/99
// Revision: A
//
// Description: This file contains the class definition for FilePlotter.
// A FilePlotter will create a line graph using xy points found in a file.

//
// What: "@(#) ModelBuilder.h, revA"

#ifndef FilePlotter_h
#define FilePlotter_h

#include <utility/recorder/Recorder.h>

namespace XC {
class ID;
class Vector;

class FilePlotter : public Recorder
  {
  private:
/*     ColorMap *theMap; */
/*     Renderer *theRenderer; */
    ID *cols;
    std::string fileName1;
    std::string fileName2;
    
    double deltaT;
    double nextTimeStampToRecord;    

    Vector *data1a;
    Vector *data1b;
    Vector *data2a;
    Vector *data2b;
  public:
    FilePlotter(const std::string &fileName,const std::string &windowTitle, 
		int xLoc, int yLoc, int width, int height, double dT);

    FilePlotter(const std::string &fileName1,const std::string &fileName2,
		const std::string &windowTitle, 
		int xLoc, int yLoc, int width, int height, double dT);
    
    ~FilePlotter(void);

    int plotFile(void);
    int plotFiles(void);

    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);    

    int setFile(const std::string &newFile);
    int setCol(const ID &theCols);
  };
} // end of XC namespace

#endif







