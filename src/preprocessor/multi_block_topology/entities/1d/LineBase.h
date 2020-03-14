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
//LineBase.h

#ifndef LINEBASE_H
#define LINEBASE_H

#include "Edge.h"


namespace XC {

//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Base class for 1D entities.
class LineBase: public Edge
  {
  protected:
    Pnt *p1; //!< Back end.
    Pnt *p2; //!< Front end.

    //! @brief Return pointer to back end.
    inline Pnt *P1(void)
      { return p1; }
    //! @brief Return pointer to front end.
    virtual Pnt *P2(void)
      { return p2; }
    virtual bool check_points(void) const;
  public:
    LineBase(Preprocessor *m,const size_t &ndiv= 4);
    LineBase(const std::string &nombre,Preprocessor *m,const size_t &ndiv= 4);
    virtual bool operator==(const LineBase &) const;
    const Pnt *P1(void) const;
    const Pnt *P2(void) const;

    virtual double getLambda(const Pos3d &) const= 0;

    virtual const Pnt *getVertex(const size_t &i) const;
    virtual void SetVertice(const size_t &,Pnt *);
    void SetVertice(const size_t &,const size_t &);
    void SetVertices(Pnt *,Pnt *);


  };

} //end of XC namespace
#endif
