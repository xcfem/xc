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
                                                                        
// $Revision: 1.2 $
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DamageRecorder.h,v $
                                                                        
#ifndef DamageRecorder_h
#define DamageRecorder_h


// Written: Arash Altoontash, Gregory Deierlein, 04/04
// Created: 04/04
// Revision: Arash Altoontash
//
// Description: This file contains the class definition for DamageRecorder.
// A DamageRecorder is used to obtain a response from an element section/material during 
// the analysis and apply the information to the damage model and record the damage index.
//
// What: "@(#) DamageRecorder.h, revA"

#include <utility/recorder/DomainRecorderBase.h>
#include <fstream>
#include <utility/matrix/ID.h>


namespace XC {
class Domain;
class Vector;
class Matrix;
class Element;
class Response;
class FE_Datastore;
class DamageModel;

class DamageRecorder: public DomainRecorderBase
  {
  private:	
    int eleID, numSec, dofID;
    ID responseID;                 // integer element returns in setResponse
    ID sectionTags;

    std::vector<Response *> theResponses;
    std::vector<DamageModel *> theDamageModels;

    bool echoTimeFlag; //!< flag indicating if pseudo time also printed
    std::string fileName; //!< file name  
    std::ofstream theFile; //!< output stream

    double deltaT;
    double nextTimeStampToRecord;

    void libera(void);
    void alloc(const size_t &,DamageModel *);
  public:
    DamageRecorder(int elemid, ID &secIDs, int dofid, DamageModel *dmgPtr, Domain &,
		   bool echotimeflag, double deltat , const std::string &);

    ~DamageRecorder(void);
    int record(int commitTag, double timeStamp);
    int playback(int commitTag);

    int restart(void);    
  };
} // fin namespace XC


#endif
