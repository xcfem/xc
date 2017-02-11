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
//NodePtrsWithIDs.h

#ifndef NodePtrsWithIDs_h
#define NodePtrsWithIDs_h

#include "NodePtrs.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Node;
class Domain;
class Channel;
class FEM_ObjectBroker;

//! \ingroup FEMisc
//
//! @brief Elemento con pointers to nodes
class NodePtrsWithIDs: public NodePtrs, public MovableObject
  {
  protected:
    ID connectedExternalNodes; //!< contains the tags of the end nodes.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);


    friend class Element;
    void set_ptr_nodos(Domain *domain);
  public:
    NodePtrsWithIDs(Element *owr,size_t numNodos);

    inline const size_t numNodos(void)
      { return NodePtrs::size(); }
    // public methods to obtain information about dof & connectivity    
    int getNumExternalNodes(void) const;
    NodePtrs &getNodePtrs(void);
    const NodePtrs &getNodePtrs(void) const;
    const ID &getExternalNodes(void) const;
    int getTagNode(const int &i) const;


    void set_id_nodos(int Nd1,int Nd2);
    void set_id_nodos(int nd1,int nd2,int nd3);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6,int nd7);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6,int nd7,int nd8);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6,int nd7,int nd8,int nd9);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6,int nd7,int nd8,int nd9,int nd10
                      ,int nd11,int nd12,int nd13,int nd14,int nd15,int nd16,int nd17,int nd18,int nd19,int nd20);
    void set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6,int nd7,
                                           int nd8,int nd9,int nd10,int nd11,int nd12,int nd13,
                                           int nd14,int nd15,int nd16,int nd17,int nd18,int nd19,
                                           int nd20, int nd21, int nd22,int nd23, int nd24, int nd25,
		      int nd26,int nd27);
    void set_id_nodos(const std::vector<int> &inodos);
    void set_id_nodos(const ID &inodos);
    void Print(std::ostream &os) const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    bool equalNumDOF(void);
    bool checkNumDOF(const size_t &, const size_t &);
    
  };

std::ostream &operator<<(std::ostream &os,const NodePtrsWithIDs &nodePtrs);

} //end of XC namespace
#endif
