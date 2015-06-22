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
                                                                        


// $Revision: 1.5 $
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/GSA_Recorder.cpp,v $

// Written: fmk 
// Created: 02/03
//
// What: "@(#) GSA_Recorder.C, revA"

#include "GSA_Recorder.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include <domain/constraints/SP_Constraint.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <domain/constraints/SP_ConstraintIter.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>

XC::GSA_Recorder::GSA_Recorder(Domain &theDom, 
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
			   double dT)
  : DomainRecorderBase(RECORDER_TAGS_GSA_Recorder,&theDom),
    ndm(3), ndf(6), counter(0), deltaT(dT), nextTimeStampToRecord(0.0)
  {
    // open file 
    if(theFile.setFile(fileName.c_str(), OVERWRITE) < 0)
      {
        std::cerr << "WARNING - XC::GSA_Recorder::GSA_Recorder()";
        std::cerr << " - could not open file " << fileName << std::endl;
        exit(-1);
      } 

    // spit out header data
    if(title1.size())
      theFile.stream() << "TITLE\t" << title1;
    else
      theFile.stream() << "TITLE\t" << "No Title";


    if(title2.size())
      theFile.stream() << "\t" << title2;
    else
      theFile.stream() << "\t" << "BLANK";


    if(title3.size())
      theFile.stream() << "\t" << title3;
    else
      theFile.stream() << "\t" << "BLANK";  


    if(jobno.size())
      theFile.stream() << "\t" << jobno;
    else
      theFile.stream() << "\t" << "0000";


    if(initials.size())
      theFile.stream() << "\t" << initials << std::endl;
    else
      theFile.stream() << "\t" << "ANOTHER\n";  

    if(spec.size())
      theFile.stream() << "SPEC\t" << spec << std::endl;

    if(currency.size())
      theFile.stream() << "CURRENCY\t" << currency << std::endl;

    if(length.size())
      theFile.stream() << "UNIT_DATA\tLENGTH\t" << length << std::endl; 

    if(force.size())
      theFile.stream() << "UNIT_DATA\tFORCE\t" << force << std::endl;

    if(temp.size())
      theFile.stream() << "UNIT_DATA\tTEMP\t" << temp << std::endl;


    // spit out nodal data
    NodeIter &theNodes = theDomain->getNodes();
    Node *theNode;
    while((theNode=theNodes()) != 0)
      {
        int nodeTag = theNode->getTag();
        theFile.stream() << "NODE\t" << nodeTag;
        const XC::Vector &crds = theNode->getCrds();
        if(crds.Size() != ndm)
          {
            std::cerr << "WARNING - XC::GSA_Recorder::GSA_Recorder() - node: " <<  nodeTag ;
            std::cerr << " has invalid number of coordinates, expecting: " << ndm 
                      << " got: " << crds.Size() << std::endl;
            exit(-1);
          }
        const XC::Vector &disp = theNode->getTrialDisp();
        if(disp.Size() != ndf)
          {
            std::cerr << "WARNING - XC::GSA_Recorder::GSA_Recorder() - node: " <<  nodeTag ;
            std::cerr << " has invalid number of dof, expecting: " << ndf << " got: " << disp.Size() << std::endl;
            exit(-1);
          }
        for(int i=0; i<ndm; i++)
          theFile.stream() << "\t" << crds(i);
        theFile.stream() << std::endl;
      }
        
    // open file and spit out the initial data
    SP_ConstraintIter &theSPs = theDomain->getCondsContorno().getSPs();
    SP_Constraint *theSP;
    ID theConstrainedNodes(0,6);
    ID theSpMatrix(0, 6*ndf);
    int numNodesWithSP = 0;
    while((theSP=theSPs()) != 0)
      {
        int nodeTag =  theSP->getNodeTag();
        int location = theConstrainedNodes.getLocation(nodeTag);
        if(location < 0)
          {
            theConstrainedNodes[numNodesWithSP] = nodeTag;
            for(int i=0; i<ndf; i++)
	    theSpMatrix[numNodesWithSP*ndf+i] = 0;	  
            location = numNodesWithSP++;
          }
        int id = theSP->getDOF_Number();
        theSpMatrix[location*ndf + id] = 1;
      }
    
    for(int j=0; j<numNodesWithSP; j++)
      {
        theFile.stream() << "SPC\t" <<  theConstrainedNodes[j] << "\t0";
        for(int i=0; i<ndf; i++)
          theFile.stream() << "\t" << theSpMatrix[j*ndf+i];
        theFile.stream() << std::endl;
      }
    
    ElementIter &theElements = theDomain->getElements();
    Element *theElement;
    while((theElement=theElements()) != 0)
      { theElement->Print(theFile.stream(), -1); }
  }

XC::GSA_Recorder::~GSA_Recorder()
  {
    theFile.stream() << "END\n";
    theFile.close();
  }

int XC::GSA_Recorder::record(int commitTag, double timeStamp)
  {

  if (deltaT == 0.0 || timeStamp >= nextTimeStampToRecord) {
      
    if (deltaT != 0.0) 
      nextTimeStampToRecord = timeStamp + deltaT;

    counter++;

    theFile.stream() << "ANAL_CASE\t" << counter << "\tStep" << counter << "\tL" << counter << 
      "\tGSS\tSTATIC\tPOST\t" << counter << "\topensees\t" << "20030204165318	0" << std::endl;

    theFile.stream() << "!\n!RESULTS FOR ANALYSIS CASE\t" << counter << "\n!\n";

    // spit out nodal displacements
    NodeIter &theNodes = theDomain->getNodes();
    Node *theNode;
    while ((theNode=theNodes()) != 0) {
      int nodeTag = theNode->getTag();
      const XC::Vector &disp = theNode->getTrialDisp();
      if (ndm == 3 && ndf == 6) {
	theFile.stream() << "DISP\t" << nodeTag << "\t" << counter;
	for (int i=0; i<ndm; i++)
	  theFile.stream() << "\t" << disp(i);
	theFile.stream() << std::endl;
	theFile.stream() << "ROTN\t" << nodeTag << "\t" << counter;
	for (int j=0; j<ndm; j++)
	  theFile.stream() << "\t" << disp(ndm+j);
	theFile.stream() << std::endl;
      }
    }

    ElementIter &theElements = theDomain->getElements();
    Element *theElement;
    while ((theElement=theElements()) != 0) {
      theElement->Print(theFile.stream(), (counter+1)*-1);  // starts at -2, as already using -1

    }
  }
  
  return 0;
}

int XC::GSA_Recorder::playback(int commitTag)
{
  // does nothing
  return 0;
}

int XC::GSA_Recorder::restart(void)
{
  return 0;
}





