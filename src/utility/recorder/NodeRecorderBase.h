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
//NodeRecorderBase.h
                                                                        
#ifndef NodeRecorderBase_h
#define NodeRecorderBase_h

#include "utility/recorder/MeshCompRecorder.h"
#include <vector>

namespace XC {
class Domain;
class FE_Datastore;
class DataOutputHandler;
 class ID;

//! @brief Se emplea como base de los recorders que obtienen
//! la respuesta de uno o más nodos durante el análisis.
class NodeRecorderBase: public MeshCompRecorder
  {
  protected:	
    ID *theDofs;
    ID *theNodalTags;
    std::vector<Node *> theNodes;


    int dataFlag; //!< flag indicating what it is to be stored in recorder.
    int numValidNodes;

  protected:
    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);

  public:
    NodeRecorderBase(int classTag);
    NodeRecorderBase(int classTag, const ID &theDof, 
		     const ID &theNodes, Domain &theDomain, DataOutputHandler &theOutputHandler,
		     double deltaT = 0.0, bool echoTimeFlag = true); 
    ~NodeRecorderBase(void);

  };
} // end of XC namespace

#endif
