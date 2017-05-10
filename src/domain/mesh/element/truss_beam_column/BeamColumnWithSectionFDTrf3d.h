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
                                                                        
// $Revision: 1.11 $
// $Date: 2003/02/25 23:32:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beamWithHinges/BeamWithHinges3d.h,v $

#ifndef BeamColumnWithSectionFDTrf3d_h
#define BeamColumnWithSectionFDTrf3d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.h>

namespace XC {
class Node;

class CrdTransf;
class CrdTransf3d;

//! \ingroup OneDimensionalElem
//
//! @brief 3D beam colun element with SeccionBarraPrismatica material type.
class BeamColumnWithSectionFDTrf3d: public BeamColumnWithSectionFD
  {
    BeamColumnWithSectionFDTrf3d &operator=(const BeamColumnWithSectionFDTrf3d &);
  protected:
    CrdTransf3d *theCoordTransf;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

    void set_transf(const CrdTransf *trf);
  public:
    BeamColumnWithSectionFDTrf3d(int tag,int classTag,int numSec);
    BeamColumnWithSectionFDTrf3d(int tag,int classTag,int numSec,const Material *mat,const CrdTransf *trf);
    BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf3d &coordTransf);
    BeamColumnWithSectionFDTrf3d(const BeamColumnWithSectionFDTrf3d &);
    ~BeamColumnWithSectionFDTrf3d(void);
    void initialize_trf(void);
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    Vector getVDirStrongAxisLocalCoord(const size_t &i) const;
    Vector getVDirWeakAxisLocalCoord(const size_t &i) const;
    double getStrongAxisAngle(const size_t &i) const;
    double getWeakAxisAngle(const size_t &i) const;
    const Vector &getVDirStrongAxisGlobalCoord(const size_t &i, bool initialGeometry) const;
    const Vector &getVDirWeakAxisGlobalCoord(const size_t &i, bool initialGeometry) const;
  };
} // end of XC namespace

#endif
