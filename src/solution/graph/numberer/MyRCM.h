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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/MyRCM.h,v $
                                                                        
                                                                        
// File: ~/graph/numberer/MyRCM.h
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for MyRCM.
// MyRCM is an object to perform the Reverse Cuthill-McKee numbering
// scheme on the vertices of a graph. This is done by invoking the
// number() method with the Graph to be numbered.
//
// Side effects: numberer() changes the Tmp values of the vertices to
// the number assigned to that vertex.
//
// What: "@(#) MyRCM.h, revA"

#ifndef MyRCM_h
#define MyRCM_h

#include "BaseNumberer.h"

namespace XC {
//! @ingroup Graph
//
//! @brief Class which performs the Reverse Cuthill-McKee numbering
//! scheme on the vertices of a graph. This is done by invoking the
//! number() method with the Graph to be numbered.
class MyRCM: public BaseNumberer
  {
  private:
    int startVertexTag;
    bool minDegree;
  protected:
    friend class FEM_ObjectBroker;
    friend class DOF_Numberer;
    MyRCM(int startVertex = -1, bool minDegreeFlag = false); 
    GraphNumberer *getCopy(void) const;
  public:

    void setStartVertex(int startVertex);
    void setMinDegreeFlag(bool flag);    

    const ID &number(Graph &theGraph, int lastVertex = -1);
    const ID &number(Graph &theGraph, const ID &lastVertices);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif

