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
                                                                        
// $Revision: 1.12 $
// $Date: 2005/03/30 03:54:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/NodeRecorder.h,v $
                                                                        
#ifndef NodeRecorder_h
#define NodeRecorder_h

// Written: fmk 
// Created: 09/00
// Revision: A
//
// Description: This file contains the class definition for 
// NodeRecorder. A NodeRecorder is used to store the specified nodal dof responses
// for the specified nodes in a file.
//
// What: "@(#) NodeRecorder.h, revA"


#include <utility/recorder/NodeRecorderBase.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Domain;
class FE_Datastore;
class DataOutputHandler;
class Node;

class NodeRecorder: public NodeRecorderBase
  {
  private:	
    int initialize(void);
    Vector response;

    // AddingSensitivity:BEGIN //////////////////////////////
    int sensitivity;
    // AddingSensitivity:END ////////////////////////////////

    void setup_dofs(const XC::ID &dofs);
    void setup_nodes(const XC::ID &nodes);
    void setup_data_flag(const std::string &dataToStore);
  protected:
    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);

  public:
    NodeRecorder(void);
    NodeRecorder(const ID &theDof, const ID &theNodes, 
		 int sensitivity, const std::string &dataToStore,
		 Domain &theDomain, DataOutputHandler &theOutputHandler,
		 double deltaT = 0.0, bool echoTimeFlag = true); 

    int record(int commitTag, double timeStamp);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
