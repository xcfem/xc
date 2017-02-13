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
//NodePtrsWithIDs.cc

#include "NodePtrsWithIDs.h"
#include "domain/mesh/node/Node.h"

//! @ brief Default constructor.
XC::NodePtrsWithIDs::NodePtrsWithIDs(Element *owr,size_t numNodos)
  : NodePtrs(owr,numNodos), MovableObject(0), connectedExternalNodes(numNodos)
  {
    // ensure the connectedExternalNode XC::ID is of correct size & set values
    if(size_t(connectedExternalNodes.Size()) != numNodos)
      {
        std::cerr << "FATAL NodePtrsWithIDs::NodePtrsWithIDs - " <<
          "failed to create an ID of size " << numNodos << "\n";
        exit(-1);
      }
  }

//! @brief Return the number of external nodes.
int XC::NodePtrsWithIDs::getNumExternalNodes(void) const
  { return connectedExternalNodes.Size(); }

//! @brief Returns a pointer to the vector de nodos.
const XC::NodePtrs &XC::NodePtrsWithIDs::getNodePtrs(void) const
  { return *this; }

//! @brief Returns a pointer to the vector de nodos.
XC::NodePtrs &XC::NodePtrsWithIDs::getNodePtrs(void)
  { return *this; }

//! @brief Reuturns external nodes identifiers.
const XC::ID &XC::NodePtrsWithIDs::getExternalNodes(void) const
  { return connectedExternalNodes; }

//! @brief Returns the tag del nodo cuyo índice se pasa como parámetro.
int XC::NodePtrsWithIDs::getTagNode(const int &i) const
  { return connectedExternalNodes(i); }

//! @brief Asigna los identificadores (tags) de los nodos.
void XC::NodePtrsWithIDs::set_id_nodos(const std::vector<int> &inodos)
  {
    const size_t numNodes= getNumExternalNodes();
    if(numNodes != inodos.size())
      std::cerr << "NodePtrsWithIDs::NodePtrsWithIDs - se esperaban " 
                << numNodes << " índices de nodo." << std::endl;
    for(register size_t i= 0;i<numNodes;i++)
      connectedExternalNodes(i) = inodos[i];
  }

//! @brief Asigna los identificadores (tags) de los nodos.
void XC::NodePtrsWithIDs::set_id_nodos(const ID &inodos)
  {
    const int numNodes= getNumExternalNodes();
    if(numNodes != inodos.Size())
      std::cerr << "NodePtrsWithIDs::NodePtrsWithIDs - se esperaban " 
                << numNodes << " índices de nodo." << std::endl;
    for(register int i= 0;i<numNodes;i++)
      connectedExternalNodes(i) = inodos(i);
  }

//! @brief Asigna los pointers to los nodos.
void XC::NodePtrsWithIDs::set_ptr_nodos(Domain *domain)
  {
    if(domain)
      NodePtrs::setPtrs(domain,connectedExternalNodes);
  }

//! @brief Asigna los índices de los nodos 1 y 2.
void XC::NodePtrsWithIDs::set_id_nodos(int Nd1,int Nd2)
  {
    assert(connectedExternalNodes.Size()>=2);
    connectedExternalNodes(0)= Nd1;
    connectedExternalNodes(1)= Nd2;
  }

//! @brief Asigna los índices de los nodos 1 a 3.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3)
  {
    set_id_nodos(nd1,nd2);
    assert(connectedExternalNodes.Size()>=4);
    connectedExternalNodes(2)= nd3;
  }

//! @brief Asigna los índices de los nodos 1 a 4.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3, int nd4)
  {
    set_id_nodos(nd1,nd2,nd3);
    assert(connectedExternalNodes.Size()>=4);
    connectedExternalNodes(3)= nd4;
  }

//! @brief Asigna los índices de los nodos 1 a 5.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3, int nd4, int nd5)
  {
    set_id_nodos(nd1,nd2,nd3,nd4);
    assert(connectedExternalNodes.Size()>=5);
    connectedExternalNodes(4)= nd5;
  }

//! @brief Asigna los índices de los nodos 1 a 6.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3, int nd4, int nd5, int nd6)
  {
    set_id_nodos(nd1,nd2,nd3,nd4,nd5);
    assert(connectedExternalNodes.Size()>=6);
    connectedExternalNodes(5)= nd6;
  }

//! @brief Asigna los índices de los nodos 1 a 7.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3, int nd4, int nd5, int nd6, int nd7)
  {
    set_id_nodos(nd1,nd2,nd3,nd4,nd5,nd6);
    assert(connectedExternalNodes.Size()>=7);
    connectedExternalNodes(6)= nd7;
  }

//! @brief Asigna los índices de los nodos 1 a 8.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3, int nd4,int nd5,int nd6,int nd7,int nd8)
  {
    set_id_nodos(nd1,nd2,nd3,nd4,nd5,nd6,nd7);
    assert(connectedExternalNodes.Size()>=8);
    connectedExternalNodes(7)= nd8;
  }

//! @brief Asigna los índices de los nodos 1 a 9.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1, int nd2, int nd3, int nd4,int nd5,int nd6,int nd7,int nd8,int nd9)
  {
    set_id_nodos(nd1,nd2,nd3,nd4,nd5,nd6,nd7,nd8);
    assert(connectedExternalNodes.Size()>=9);
    connectedExternalNodes(8)= nd9;
  }

//! @brief Asigna los índices de los nodos 1 a 20.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1,  int nd2,  int nd3,  int nd4,
                   int nd5,  int nd6,  int nd7,  int nd8,
                   int nd9,  int nd10, int nd11, int nd12,
                   int nd13, int nd14, int nd15, int nd16,
					   int nd17, int nd18, int nd19, int nd20)
  {
    set_id_nodos(nd1,nd2,nd3,nd4,nd5,nd6,nd7,nd8,nd9);
    assert(connectedExternalNodes.Size()>=20);
    connectedExternalNodes(9)= nd10;
    connectedExternalNodes(10)= nd11;
    connectedExternalNodes(11)= nd12;
    connectedExternalNodes(12)= nd13;
    connectedExternalNodes(13)= nd14;
    connectedExternalNodes(14)= nd15;
    connectedExternalNodes(15)= nd16;
    connectedExternalNodes(16)= nd17;
    connectedExternalNodes(17)= nd18;
    connectedExternalNodes(18)= nd19;
    connectedExternalNodes(19)= nd20;
  }

//! @brief Asigna los índices de los nodos 1 a 27.
void XC::NodePtrsWithIDs::set_id_nodos(int nd1,int nd2,int nd3,int nd4,int nd5,int nd6,int nd7,
                                           int nd8,int nd9,int nd10,int nd11,int nd12,int nd13,
                                           int nd14,int nd15,int nd16,int nd17,int nd18,int nd19,
                                           int nd20, int nd21, int nd22,int nd23, int nd24, int nd25,
                                           int nd26,int nd27)
  {
    set_id_nodos(nd1,nd2,nd3,nd4,nd5,nd6,nd7,nd8,nd9,nd10,nd11,nd12,nd13,nd14,nd15,nd16,nd17,nd18,nd19,nd20);
    assert(connectedExternalNodes.Size()>=27);
    connectedExternalNodes(20)= nd21;
    connectedExternalNodes(21)= nd22;
    connectedExternalNodes(22)= nd23;
    connectedExternalNodes(23)= nd24;
    connectedExternalNodes(24)= nd25;
    connectedExternalNodes(25)= nd26;
    connectedExternalNodes(26)= nd27; 
  }

//! @brief True if all nodes has the same DOF number.
bool XC::NodePtrsWithIDs::equalNumDOF(void)
  {
    const size_t numNodes= NodePtrs::size();
    bool retval= true;
    if(numNodes>0)
      {
        const int nDOF= (*this)[0]->getNumberDOF();
        for(register size_t i= 1;i<numNodes;i++)
          if((*this)[0]->getNumberDOF()!=nDOF)
            {
              retval= false;
              break;
            }
      }
    return retval;
  }

//! @brief Check DOF number on nodes.
bool XC::NodePtrsWithIDs::checkNumDOF(const size_t &numDOF, const size_t &elemTag)
  {
    bool retval= true;
    if(size()>0)
      {
        const bool equalNumDof= equalNumDOF();

        const size_t dofNd1= (*this)[0]->getNumberDOF();
        if((dofNd1!=numDOF) || !equalNumDof)
          {
            std::cerr << "ERROR, element (tag: " << elemTag 
                      << "), has a different number of DOFs in its nodes." << std::endl;
            retval= false;
          }
      }
    return retval;
  }

//! @brief Muestra información sobre el elemento.
void XC::NodePtrsWithIDs::Print(std::ostream &os) const
  { 
    os << "Connected Nodes: " << connectedExternalNodes;
    const size_t numNodes= NodePtrs::size();
    for(register size_t i= 0;i<numNodes;i++)
      os << " tag: " << (*this)[i]->getTag()
         << " num gdls: " << (*this)[0]->getNumberDOF() << std::endl;
  }

std::ostream &XC::operator<<(std::ostream &os,const XC::NodePtrsWithIDs &nodePtrs)
  {
    nodePtrs.Print(os);
    return os;
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodePtrsWithIDs::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Envia los miembros del objeto through the channel being passed as parameter.
int XC::NodePtrsWithIDs::sendData(CommParameters &cp)
  { return cp.sendID(connectedExternalNodes,getDbTagData(),CommMetaData(0));  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::NodePtrsWithIDs::recvData(const CommParameters &cp)
  { return cp.receiveID(connectedExternalNodes,getDbTagData(),CommMetaData(0)); }

int XC::NodePtrsWithIDs::sendSelf(CommParameters &cp)
  {
    inicComm(1); 
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NodePtrsWithIDs::send - ch failed to send data.\n";
    return res;
  }

int XC::NodePtrsWithIDs::recvSelf(const CommParameters &cp)
  {
    // first we get the data about the state of the mesh for this cTag
    inicComm(1);
    int res= cp.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << "NodePtrsWithIDs::recv - ch failed to recv the initial ID\n";
    else
      res+= recvData(cp);
    return res;
  }
