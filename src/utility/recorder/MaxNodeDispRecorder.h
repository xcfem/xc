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
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/MaxNodeDispRecorder.h,v $
                                                                        
                                                                        
#ifndef MaxNodeDispRecorder_h
#define MaxNodeDispRecorder_h

// File: ~/recorder/MaxNodeDispRecorder.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for MaxNodeDispRecorder.
// A MaxNodeDispRecorder is used to determine the max nodal displacement
// at a collection of nodes over an analysis. (between commitTag of 0 and
// last commitTag).
//
// What: "@(#) MaxNodeDispRecorder.h, revA"

#include <utility/recorder/DomainRecorderBase.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Domain;

class MaxNodeDispRecorder: public DomainRecorderBase
  {
  private:	
    ID theNodes;
    Vector maxDisp;
    int dof;
  public:
    MaxNodeDispRecorder(void);
    MaxNodeDispRecorder(int dof,const ID &theNodes, Domain &theDomain);

    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);    
    
  };
} // fin namespace XC


#endif
