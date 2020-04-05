// -*-c++-*-
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
//BeamColumnWithSectionFDTrf2d.h

#ifndef BeamColumnWithSectionFDTrf2d_h
#define BeamColumnWithSectionFDTrf2d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.h>

namespace XC {
class Node;

class CrdTransf;
class CrdTransf2d;

//! @ingroup BeamColumnElemGrp
//
//! @brief 2D beam element with SectionForceDeformation type material.
class BeamColumnWithSectionFDTrf2d: public BeamColumnWithSectionFD
  {
    BeamColumnWithSectionFDTrf2d &operator=(const BeamColumnWithSectionFDTrf2d &);
  protected:  
    CrdTransf2d *theCoordTransf;

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

    void set_transf(const CrdTransf *trf);

  public:
    BeamColumnWithSectionFDTrf2d(int tag,int classTag,int numSec);
    BeamColumnWithSectionFDTrf2d(int tag,int classTag,int numSec,const Material *mat,const CrdTransf *trf);
    BeamColumnWithSectionFDTrf2d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf2d &coordTransf);
    BeamColumnWithSectionFDTrf2d(const BeamColumnWithSectionFDTrf2d &);
    ~BeamColumnWithSectionFDTrf2d(void);
    void initialize_trf(void);
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

  };
} // end of XC namespace

#endif
