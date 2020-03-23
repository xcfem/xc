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
//NMPointCloudBase.h
//Internal forces point cloud container base

#ifndef NMPOINTCLOUDBASE_H
#define NMPOINTCLOUDBASE_H

namespace XC {


//! @ingroup MATSCCDiagInt
//
//! @brief Base class for (N,M) point sets
class NMPointCloudBase
  {
  protected:
    double threshold;
  public:
    NMPointCloudBase(const double &u= 0.0);
    inline void setThreshold(const double &u)
      { threshold= u; }
    inline const double &getThreshold(void)
      { return threshold; }
    inline virtual void clear(void)
      { threshold= 0.0; }
  };

} // end of XC namespace

#endif
