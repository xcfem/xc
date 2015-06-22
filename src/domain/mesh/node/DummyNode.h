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
// $Date: 2003/02/14 23:00:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/DummyNode.h,v $
                                                                        
                                                                        
// File: ~/domain/mesh/node/DummyNode.h
//
// Written: fmk 
// Created: Fri Sep 20 15:34:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for DummyNode.
// DummyNodes are a type of node created and used by Subdomains for their
// exterior nodes. They reference a real node and most methods invoked on
// them are in turn invoked by the dummy node on the real node. The calls
// asking the real node to change its current state are ignored. The
// calls involving DOF\_Group are handled by the dummy node.
//
// 
// What: "@(#) DummyNode.h, revA"

#ifndef DummyNode_h
#define DummyNode_h

#include "domain/mesh/node/Node.h"

namespace XC {
class DOF_Group;
class Element;

//! \ingroup Nod
//
//! @brief DummyNodes are a type of node created and used by
//! Subdomains for their exterior nodes. They reference a real
//! node and most methods invoked on them are in turn invoked
//! by the dummy node on the real node. The calls asking the
//! real node to change its current state are ignored. The
//! calls involving DOF_Group are handled by the dummy node.
class DummyNode: public Node
  {
  private:
    Node *theRealNode;
    DOF_Group *theDOFGroup;
  public:
    DummyNode(void);    
    DummyNode(Node &theRealNode);
    
    Node *getNode(void) const;
    
    void setDOF_GroupPtr(DOF_Group *theDOF_Grp);
    DOF_Group *getDOF_GroupPtr(void);
    int  getNumberDOF(void) const;

    const Matrix &getMass(void);
    int setMass(const Matrix &);
    
    const Vector &getCrds(void) const;

    const Vector &getDisp(void) const;
    const Vector &getVel(void) const;
    const Vector &getAccel(void) const;   

    const Vector &getTrialDisp(void) const;    
    const Vector &getTrialVel(void) const;    
    const Vector &getTrialAccel(void) const;    
    
    int setTrialDisp(const Vector &);
    int setTrialVel(const Vector &);   
    int setTrialAccel(const Vector &);        

    int setIncrTrialDisp(const Vector &);    
    int setIncrTrialVel(const Vector &);    
    int setIncrTrialAccel(const Vector &);        
    
    void addUnbalancedLoad(const Vector &);        
    const Vector &getUnbalancedLoad(void) const;    
    void zeroUnbalancedLoad(void);
    int commitState();

    void Print(std::ostream &s) const;
    friend std::ostream &operator<<(std::ostream &s, const DummyNode &N);

    int addElementPtr(Element *);

    void setColor(int newColor);
    int getColor(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);    
  };
} // fin namespace XC

#endif

