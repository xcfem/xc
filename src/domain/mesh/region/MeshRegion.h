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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:02 $
// $Source: domain/mesh/region/MeshRegion.h,v $
                                                                        
                                                                        
// Written: fmk 
//
// Description: This file contains the class definition for MeshRegion.
// A Region is a part of the domain which is defined by a set of
// Elements and Nodes (all the end nodes of the elements are in the region, 
// as are all elements whose end nodes are in the region)
//
// What: "@(#) Region.h, revA"

#ifndef MeshRegion_h
#define MeshRegion_h

#include "domain/component/ContinuaReprComponent.h"
#include "domain/mesh/element/utils/RayleighDampingFactors.h"

namespace XC {
class Element;
class Node;
class ElementRecorder;
class NodeRecorder;
class ID;

class MeshRegion : public ContinuaReprComponent
  {
  private:
    RayleighDampingFactors rayFactors; //!< Rayleigh damping factors

    ID *theNodes;
    ID *theElements;

    int	currentGeoTag;
    int lastGeoSendTag;

    void libera(void);
    void copia(const MeshRegion &otra);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    MeshRegion(int tag);
    MeshRegion(int tag, int classTag);    
    MeshRegion(const MeshRegion &);
    MeshRegion &operator=(const MeshRegion &);
    virtual ~MeshRegion(void);
    virtual MeshRegion *getCopy(void) const;

    // methods dealing with setting up the region
    virtual int setNodes(const ID &theNodes);
    virtual int setElements(const ID &theEles);

    // methods getting the ID's of nodes & ele
    virtual const ID &getNodes(void);
    virtual const ID &getElements(void);

    // methods dealing with setting parameters in the region
    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF);

    // methods to send & recv data for database/parallel applications
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    virtual void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace


#endif

