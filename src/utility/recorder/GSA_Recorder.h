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
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/GSA_Recorder.h,v $
                                                                        
#ifndef GSA_Recorder_h
#define GSA_Recorder_h

// Written: fmk 
// Created: 02/03
//
// Description: This file contains the class definition for 
// GSA_Recorder. A GSA_Recorder is used to create an o/p file
// which can be read by the Ove Arup's GSA program for
// postprocessing.
//
// What: "@(#) GSA_Recorder.h, revA"


#include <utility/recorder/DomainRecorderBase.h>
#include <utility/handler/FileStream.h>

namespace XC {
class Domain;
class FE_Datastore;

class GSA_Recorder: public DomainRecorderBase
  {
  private:	
    int ndm, ndf;
    int counter;
    FileStream theFile;
    double deltaT;
    double nextTimeStampToRecord;
  public:
    GSA_Recorder(Domain &theDomain, 
		 const std::string &fileName,
		 const std::string &title1,
		 const std::string &title2,
		 const std::string &title3,
		 const std::string &jobno,
		 const std::string &initials,
		 const std::string &spec,
		 const std::string &currency,
		 const std::string &length,
		 const std::string &force,
		 const std::string &temp,
		 double deltaT = 0.0); 

    ~GSA_Recorder(void);
    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);    
  };
} // end of XC namespace

#endif
