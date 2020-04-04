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
                                                                        
// $Revision$
// $Date$
// $Source$
                                                                        

#include "Beam2dPartialUniformLoad.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"

XC::Vector XC::Beam2dPartialUniformLoad::data(4);

//! @brief Constructor.
XC::Beam2dPartialUniformLoad::Beam2dPartialUniformLoad(int tag, double wt, double wa, double aL, double bL, const ID &theElementTags)
  : BeamUniformLoad(tag, LOAD_TAG_Beam2dPartialUniformLoad,wt,wa,theElementTags),
   aOverL(aL), bOverL(bL), parameterID(0) {}

//! @brief Constructor.
XC::Beam2dPartialUniformLoad::Beam2dPartialUniformLoad(int tag)
  : BeamUniformLoad(tag, LOAD_TAG_Beam2dPartialUniformLoad),
   aOverL(0.0), bOverL(0.0), parameterID(0) {}

const XC::Vector &XC::Beam2dPartialUniformLoad::getData(int &type, const double &loadFactor) const
  {
    type = LOAD_TAG_Beam2dPartialUniformLoad;
    data(0) = Trans;
    data(1) = Axial;
    data(2) = aOverL;
    data(3) = bOverL;
    return data;
  }


//! @brief Return the dimension of the force vector.
size_t XC::Beam2dPartialUniformLoad::getForceVectorDimension(void) const
  { return 2; }

//! @brief Return the dimension of the vector moment.
size_t XC::Beam2dPartialUniformLoad::getMomentVectorDimension(void) const
  { return 1; }

//! @brief Send data through the channel being passed as parameter.
int XC::Beam2dPartialUniformLoad::sendData(Communicator &comm)
  {
    int res= BeamLoad::sendData(comm);
    res+= comm.sendDoubles(aOverL,bOverL,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::Beam2dPartialUniformLoad::recvData(const Communicator &comm)
  {
    int res= BeamLoad::recvData(comm);
    res+= comm.receiveDoubles(aOverL,bOverL,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Beam2dPartialUniformLoad::sendSelf(XC::Communicator &comm)
  {
    inicComm(7);
    int result= sendData(comm);
    const int dbTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Beam2dPartialUniformLoad::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received\n" ;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Print stuff.
void XC::Beam2dPartialUniformLoad::Print(std::ostream &s, int flag) const
  {
    s << getClassName() << " - tag " << this->getTag() << std::endl;
    s << "  Transverse: " << Trans << std::endl;
    s << "  Axial:      " << Axial << std::endl;
    s << "  Region:     " << aOverL << " to " << bOverL << std::endl;
    s << "  Element acted on: " << elemTags << std::endl;
  }

int XC::Beam2dPartialUniformLoad::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const int argc= argv.size();
    if (argc < 1)
      return -1;

    if((argv[0]=="wTrans") || (argv[0]=="wy"))
      return param.addObject(1, this);

    if((argv[0]=="wAxial") || (argv[0]=="wx"))
      return param.addObject(2, this);

    if((argv[0]=="aOverL") || (argv[0]=="a"))
      return param.addObject(3, this);

    if((argv[0]=="bOverL") || (argv[0]=="b"))
      return param.addObject(4, this);

    return -1;
  }

int XC::Beam2dPartialUniformLoad::updateParameter(int parameterID, Information &info)
  {
  switch (parameterID)
    {
    case 1:
      setTransComponent(info.theDouble);
      return 0;
    case 2:
      setAxialComponent(info.theDouble);
      return 0;
    case 3:
      aOverL = info.theDouble;
      return 0;
    case 4:
      bOverL = info.theDouble;
      return 0;
    default:
      return -1;
    }
  }

int XC::Beam2dPartialUniformLoad::activateParameter(int paramID)
  {
    parameterID = paramID;
    return 0;
  }

const XC::Vector &XC::Beam2dPartialUniformLoad::getSensitivityData(int gradNumber)
  {
    data.Zero();

    switch(parameterID)
      {
      case 1:
	data(0) = 1.0;
	break;
      case 2:
	data(1) = 1.0;
	break;
      case 3:
	data(2) = 1.0;
	break;
      case 4:
	data(3) = 1.0;
	break;
      default:
	break;
      }
    return data;
  }
