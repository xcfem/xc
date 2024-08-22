// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
#ifndef SurfaceLoadBase_h
#define SurfaceLoadBase_h

// Written: Chris McGann
// Created: 04.2009
// Modified: Chris McGann, 11.2010
//           Chris McGann, 02.2011 -> added elemental load

// Description: This file contains the class definition for SurfaceLoadBase. 

#include "domain/mesh/element/ElementBase.h"
#include "domain/load/ElementalLoad.h"
#include "utility/utils/misc_utils/colormod.h"

namespace XC {

template <int NNODES>
class SurfaceLoadBase: public ElementBase<NNODES>
  {
  protected:
    double my_pressure; //!< pressure applied to surface of element
    double mLoadFactor; //!< factor from load pattern
  protected:   
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    SurfaceLoadBase(int tag, int classTag, double pressure, double loadFactor);
    
    double getLoadFactor(void) const;
    void setLoadFactor(const double &);
    double getPressure(void) const;
    void setPressure(const double &);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);
    
    void zeroLoad(void);        
    int addInertiaLoadToUnbalance(const Vector &accel);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    
    void Print(std::ostream &, int flag =0);
  };
  
template <int NNODES>
//! @brief Constructor.
SurfaceLoadBase<NNODES>::SurfaceLoadBase(int tag, int classTag, double pressure, double loadFactor)
 : ElementBase<NNODES>(tag, classTag),     
   my_pressure(pressure),
   mLoadFactor(loadFactor)
  {}

template <int NNODES>
int SurfaceLoadBase<NNODES>::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->ElementBase<NNODES>::commitState()) != 0)
      {
        std::cerr << Color::red << this->getClassName() << "::" << __FUNCTION__
	          << "; failed in base class"
	          << Color::def << std::endl;
      }

    return retVal; 
  }

template <int NNODES>
int SurfaceLoadBase<NNODES>::revertToLastCommit(void)
  { return 0; }

template <int NNODES>
int SurfaceLoadBase<NNODES>::revertToStart(void)
  { return 0; }

template <int NNODES>
int SurfaceLoadBase<NNODES>::update(void)
  { return 0; }


template <int NNODES>
double SurfaceLoadBase<NNODES>::getLoadFactor(void) const
  { return this->mLoadFactor; }

template <int NNODES>
void SurfaceLoadBase<NNODES>::setLoadFactor(const double &d)
  { this->mLoadFactor= d; }

template <int NNODES>
//! @brief Set the pressure.
double SurfaceLoadBase<NNODES>::getPressure(void) const
  { return this->my_pressure; }

template <int NNODES>
//! @brief Get the pressure.
void SurfaceLoadBase<NNODES>::setPressure(const double &d)
  { this->my_pressure= d; }

template <int NNODES>
int SurfaceLoadBase<NNODES>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    int type(0);
    const Vector &data= theLoad->getData(type, loadFactor);

    if(type == LOAD_TAG_SurfaceLoad)
      {
	mLoadFactor = loadFactor;
        this->my_pressure+= data[0]; // Set pressure.
	return 0;
      }
    else
      {
	std::cerr << Color::red << this->getClassName() << "::" << __FUNCTION__
		  << "; ele with tag: " << this->getTag()
		  << " does not accept load type: " << type
		  << Color::def << std::endl;
	return -1;
      }
    return -1;
  }

template <int NNODES>
void XC::SurfaceLoadBase<NNODES>::zeroLoad(void)
  {
    this->my_pressure= 0.0;
  }

template <int NNODES>
int XC::SurfaceLoadBase<NNODES>::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }

template <int NNODES>
//! @brief Send object members through the communicator argument.
int SurfaceLoadBase<NNODES>::sendData(Communicator &comm)
  {
    int res= ElementBase<NNODES>::sendData(comm);
    res+=comm.sendDoubles(this->mLoadFactor, this->my_pressure, this->getDbTagData(),CommMetaData(7));
    return res;
  }

template <int NNODES>
//! @brief Receives object members through the communicator argument.
int SurfaceLoadBase<NNODES>::recvData(const Communicator &comm)
  {
    int res= ElementBase<NNODES>::recvData(comm);
    res+=comm.receiveDoubles(this->mLoadFactor, this->my_pressure, this->getDbTagData(),CommMetaData(7));
    return res;
  }

template <int NNODES>
void SurfaceLoadBase<NNODES>::Print(std::ostream &os, int flag)
  {
    os << this->getClassName() << ", element id:  "
       << this->getTag() << std::endl
       << "   Connected external nodes:  " << this->theNodes << std::endl; 
  }



} //end of XC namespace
#endif
