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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2004/11/25 00:54:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/message/Message.cpp,v $
                                                                        
                                                                        
// File: ~/actor/Message.C
//

#include "utility/actor/message/Message.h"
#include <cstring>
#include <iostream>

XC::Message::Message(void)
  :length(0),data(nullptr)
  {}

XC::Message::Message(double *Ptr,int size)
  {
    length = size*sizeof(double);
    data = reinterpret_cast<char *>(Ptr);
  }

XC::Message::Message(int *Ptr, int size)
  {
    length = size*sizeof(int);
    data = reinterpret_cast<char *>(Ptr);
  }

XC::Message::Message(char *Ptr, int size)
  {
    length = size*sizeof(char);
    data = Ptr;
  }

int XC::Message::putData(char *theData, int startLoc, int endLoc)
  {
    if(startLoc > 0 && startLoc <= length && endLoc <= length && endLoc > startLoc)
      {
	int theLength = endLoc - startLoc;
	char *dataPos = &data[startLoc];
	bcopy(theData, dataPos, theLength);
	return 0;
      }
    else
      {
	std::cerr << "XC::Message::putData() - invalid length of data given\n";
	return -1;
      }
  }

const char *XC::Message::getData(void)
  { return data; }

int XC::Message::getSize(void)
  { return length; }


