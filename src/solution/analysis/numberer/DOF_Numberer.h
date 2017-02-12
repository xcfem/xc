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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/DOF_Numberer.h,v $
                                                                        
                                                                        
// File: ~/analysis/numberer/DOF_Numberer.h
// 
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for DOF_Numberer.
// DOF_Numberer is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) DOF_Numberer.h, revA"

#ifndef DOF_Numberer_h
#define DOF_Numberer_h

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class AnalysisModel;
class GraphNumberer;
class FEM_ObjectBroker;
class ID;
class ModelWrapper;

//! @ingroup Analysis
//
//! @defgroup Numerador DOF numbering.
//
//! @ingroup Numerador
//
//! @brief Base class for DOF numbererers.
//! Para realizar su función esta clase contiene un numerador de grafos
//! y un apuntador al analysis model.
//! Este es el objeto encargado de crear, almacenar y destruir el
//! numerador de grafos.
class DOF_Numberer: public MovableObject, public EntCmd
  {
  private:
    ModelWrapper *getModelWrapper(void);
    const ModelWrapper *getModelWrapper(void) const;

    GraphNumberer *theGraphNumberer; //!< DOF numberer.
  protected:
    AnalysisModel *getAnalysisModelPtr(void);
    GraphNumberer *getGraphNumbererPtr(void);
    const AnalysisModel *getAnalysisModelPtr(void) const;
    const GraphNumberer *getGraphNumbererPtr(void) const;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    void alloc(const std::string &);
    void copia(const GraphNumberer &);
    void libera(void);

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    DOF_Numberer(ModelWrapper *,int classTag);
    DOF_Numberer(ModelWrapper *);
    DOF_Numberer(const DOF_Numberer &);
    DOF_Numberer &operator=(const DOF_Numberer &);
    virtual DOF_Numberer *getCopy(void) const;
  public:
    virtual ~DOF_Numberer(void);

    // pure virtual functions
    virtual int numberDOF(int lastDOF_Group = -1);
    virtual int numberDOF(ID &lastDOF_Groups);

    void useAlgorithm(const std::string &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

