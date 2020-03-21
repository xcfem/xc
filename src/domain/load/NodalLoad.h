//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/04 00:48:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/NodalLoad.h,v $
                                                                        
                                                                        
#ifndef NodalLoad_h
#define NodalLoad_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class interface for NodalLoad.
// NodalLoad is a class for applying nodal loads to the model.

#include "domain/load/Load.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Node;
class Vector;

//! @ingroup Loads
//!
//! @defgroup NodeLoads Loads over nodes.
//
//! @ingroup NodeLoads
//
//! @brief Load over a node.
//!
//! NodalLoads are loads acting on Nodes. The public methods are
//! all declared as virtual to allow subclasses to be introduced for the
//! provision of time varying loads. Each NodalLoad object is associated
//! with a single Node object and has a Vector object corresponding to the
//! load acting on this Node object as a result of the NodalLoad.
class NodalLoad: public Load
  {
  private:
    int  loadedNode; //!< tag of loaded node.
    mutable Node *loadedNodePtr; //!< pointer to the loaded node
    Vector load;  //!< load vector.
    bool  konstant; //!< true if load is load factor independent (pushover analysis).
    // AddingSensitivity:BEGIN /////////////////////////////////////
    int parameterID;
    static Vector gradientVector;
    // AddingSensitivity:END ///////////////////////////////////////

    Node *get_node_ptr(void);
    const Node *get_node_ptr(void) const;

  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    NodalLoad(int tag, int theClassTag=  LOAD_TAG_NodalLoad);
    NodalLoad(int tag, int node, int classTag);
    NodalLoad(int tag, int node, const Vector &load, bool isLoadConstant = false);

    virtual void setDomain(Domain *newDomain);
    inline const Node *getNode(void) const
      { return get_node_ptr(); }
    virtual int getNodeTag(void) const;
    virtual void applyLoad(double loadFactor);
    
    const Vector &getForce(void) const;
    const Vector &getMoment(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    virtual void Print(std::ostream &s, int flag =0) const;   
    
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int            setParameter(const std::vector<std::string> &argv, Parameter &param);
    int            updateParameter(int parameterID, Information &info);
    int            activateParameter(int parameterID);
    const Vector & getExternalForceSensitivity(int gradNumber);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif

