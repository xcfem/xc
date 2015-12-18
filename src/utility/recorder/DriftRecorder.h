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
                                                                        
// $Revision: 1.9 $
// $Date: 2005/03/30 20:12:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DriftRecorder.h,v $
                                                                        
#ifndef DriftRecorder_h
#define DriftRecorder_h

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for 
// DriftRecorder. 

#include "HandlerRecorder.h"
#include <vector>

namespace XC {
class Domain;
class DataOutputHandler;
class Node;
class Vector;
class ID;

//! @brief Obtiene una medida del desplazamiento relativo entre dos nodos. The drift is taken 
//! as the ratio between the prescribed relative displacement and the specified distance between the nodes.
class DriftRecorder: public HandlerRecorder
  {
  private:	
    int initialize(void);

    ID *ndI;
    ID *ndJ;
    std::vector<Node *> theNodes; // i & j nodes
    int dof;
    int perpDirn;
    Vector *oneOverL;
    Vector *data;
    int numNodes;
  protected:
    void libera_nodes(void);
    int alloc_nodes(const int &,const int &);
    void libera_ndIJ(void);
    void alloc_ndIJ(const int &);
    void set_ndIJ(const ID &nI,const ID &);
    void setup_ndIJ(const int &);
    bool procesa_comando(CmdStatus &status);
  public:
    DriftRecorder(void);
    DriftRecorder(int ndI, int ndJ, int dof, int perpDirn,
		Domain &theDomain, 
		DataOutputHandler &theHandler,
		bool echoTime = false);

    DriftRecorder(const ID &ndI, const ID &ndJ, int dof, int perpDirn,
		Domain &theDomain, 
		DataOutputHandler &theHandler,
		bool echoTime = false);
  
    ~DriftRecorder(void);

    int record(int commitTag, double timeStamp);
    int restart(void);    

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
