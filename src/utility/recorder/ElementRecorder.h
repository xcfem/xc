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
                                                                        
// $Revision: 1.11 $
// $Date: 2004/11/24 22:43:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/ElementRecorder.h,v $
                                                                        
                                                                        
#ifndef ElementRecorder_h
#define ElementRecorder_h


// Written: fmk 
// Created: 09/99
// Revision: A
//
// Description: This file contains the class definition for ElementRecorder.
// A ElementRecorder is used to obtain a response from an element during 
// the analysis.
//
// What: "@(#) ElementRecorder.h, revA"

#include "ElementRecorderBase.h"

namespace XC {
class Vector;
class Element;

class ElementRecorder: public ElementRecorderBase
  {
  private:	
    int initialize(void);
    Vector *data;

  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    ElementRecorder(void);
    ElementRecorder(const ID &eleID, 
		    const std::vector<std::string> &, 
		    bool echoTime, 
		    Domain &theDomain, 
		    DataOutputHandler &theOutputHandler,
		    double deltaT = 0.0);

    ~ElementRecorder(void);

    int record(int commitTag, double timeStamp);
    int restart(void);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC


#endif
