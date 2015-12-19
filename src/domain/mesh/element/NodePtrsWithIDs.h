//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! \ingroup Elem
//
//! @brief Elemento con punteros a nodos
class NodePtrsWithIDs: public NodePtrs, public MovableObject
  {
  protected:
    ID connectedExternalNodes; //!< contains the tags of the end nodes.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);

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
