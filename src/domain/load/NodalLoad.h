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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/04 00:48:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/NodalLoad.h,v $
                                                                        
                                                                        
#ifndef NodalLoad_h
#define NodalLoad_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class interface for NodalLoad.
// NodalLoad is a class for applying nodal loads to the model.

#include "domain/load/Load.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Node;
class Vector;

//! \ingroup Loads
//
//! @defgroup NodeLoads Cargas sobre nodos.
//
//! @ingroup NodeLoads
//
//! @brief Carga en un nodo.
class NodalLoad : public Load
  {
  private:
    int  myNode; //!< tag of node on which load acts.
    mutable Node *myNodePtr; //!< pointer to node object on which load acts
    Vector load;       //!< the reference load
    bool  konstant;     //!< true if load is load factor independent
    // AddingSensitivity:BEGIN /////////////////////////////////////
    int parameterID;
    static Vector gradientVector;
    // AddingSensitivity:END ///////////////////////////////////////

    Node *get_node_ptr(void);
    const Node *get_node_ptr(void) const;

  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    NodalLoad(int tag, int theClassTag=  LOAD_TAG_NodalLoad);
    NodalLoad(int tag, int node, int classTag);
    NodalLoad(int tag, int node, const Vector &load, bool isLoadConstant = false);

    virtual void setDomain(Domain *newDomain);
    inline const Node *getNode(void) const
      { return get_node_ptr(); }
    virtual int getNodeTag(void) const;
    virtual void applyLoad(double loadFactor);
    
    const Vector &getForce(void) const;
    const Vector &getMoment(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    virtual void Print(std::ostream &s, int flag =0);   
    
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int            setParameter(const std::vector<std::string> &argv, Parameter &param);
    int            updateParameter(int parameterID, Information &info);
    int            activateParameter(int parameterID);
    const Vector & getExternalForceSensitivity(int gradNumber);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif

