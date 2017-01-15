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
//DistributedObj.cc

#include "DistributedObj.h"
#include "CommParameters.h"

XC::DistributedObj::DistributedObj(void)
  : processID(0) {}

XC::DistributedObj::DistributedObj(const ChannelQueue &theC)
  : processID(0), theChannels(theC) {}

bool XC::DistributedObj::buscaCanal(const CommParameters &cp,int &sendID)
  {
    // check if already using this object
    bool found = false;
    const int numChannels= theChannels.size();
    for(int i=0; i<numChannels; i++)
      if(theChannels[i] == cp.getChannel())
        {
          sendID= i+1;
	  found = true;
        }
    return found;
  }

int XC::DistributedObj::setProcessID(int dTag) 
  {
    processID = dTag;
    return 0;
  }

int XC::DistributedObj::setChannels(const ChannelQueue &theC)
  {
    theChannels= theC;
    return 0;
  }
