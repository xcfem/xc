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
//NodeDispVectors
                                                                        
                                                                        
#ifndef NodeDispVectors_h
#define NodeDispVectors_h

#include "NodeVectors.h"

namespace XC {

//! \ingroup Nod
//
//! @brief Vectores para almacenar los valores de prueba
//! y los valores consumados («commited») del desplazamiento del nodo.
class NodeDispVectors: public NodeVectors
  {
  private:
    int createDisp(const size_t &);

    Vector *incrDisp;
    Vector *incrDeltaDisp;
  protected:
    void libera(void);
  public:
    // constructors
    NodeDispVectors(void);
    NodeDispVectors(const NodeDispVectors &otro);
    NodeDispVectors &operator=(const NodeDispVectors &otro);
    virtual ~NodeDispVectors(void);

    // public methods for obtaining committed and trial
    // response quantities of the node
    virtual const Vector &getIncrDisp(const size_t &) const;
    virtual const Vector &getIncrDeltaDisp(const size_t &) const;

    // public methods for updating the trial response quantities
    virtual int setTrialDispComponent(const size_t &nDOF,const double &value,const size_t &dof);
    virtual int setTrialDisp(const size_t &nDOF,const Vector &);
    virtual int incrTrialDisp(const size_t &nDOF,const Vector &);

    // public methods dealing with the committed state of the node
    virtual int commitState(const size_t &nDOF);
    virtual int revertToLastCommit(const size_t &nDOF);

    virtual void Print(std::ostream &s, int flag = 0);
  };

} // end of XC namespace

#endif

