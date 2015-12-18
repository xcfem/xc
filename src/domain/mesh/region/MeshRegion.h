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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:02 $
// $Source: domain/mesh/region/MeshRegion.h,v $
                                                                        
                                                                        
// Written: fmk 
//
// Description: This file contains the class definition for MeshRegion.
// A Region is a part of the domain which is defined by a set of
// Elements and Nodes (all the end nodes of the elements are in the region, 
// as are all elements whose end nodes are in the region)
//
// What: "@(#) Region.h, revA"

#ifndef MeshRegion_h
#define MeshRegion_h

#include "domain/component/ContinuaReprComponent.h"
#include "domain/mesh/element/RayleighDampingFactors.h"

namespace XC {
class Element;
class Node;
class ElementRecorder;
class NodeRecorder;
class ID;

class MeshRegion : public ContinuaReprComponent
  {
  private:
    RayleighDampingFactors rayFactors; //!< Rayleigh damping factors

    ID *theNodes;
    ID *theElements;

    int	currentGeoTag;
    int lastGeoSendTag;

    void libera(void);
    void copia(const MeshRegion &otra);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    MeshRegion(int tag);
    MeshRegion(int tag, int classTag);    
    MeshRegion(const MeshRegion &);
    MeshRegion &operator=(const MeshRegion &);
    virtual ~MeshRegion(void);
    virtual MeshRegion *getCopy(void) const;

    // methods dealing with setting up the region
    virtual int setNodes(const ID &theNodes);
    virtual int setElements(const ID &theEles);

    // methods getting the ID's of nodes & ele
    virtual const ID &getNodes(void);
    virtual const ID &getElements(void);

    // methods dealing with setting parameters in the region
    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF);

    // methods to send & recv data for database/parallel applications
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    virtual void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace


#endif

