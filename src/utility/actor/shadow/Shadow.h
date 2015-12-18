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
// $Date: 2005/11/23 18:25:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/shadow/Shadow.h,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for Shadow.
// Shadow is meant to be an abstract base class and thus no objects of it's type
// can be instantiated. 
//
// What: "@(#) Shadow.h, revA"

#ifndef Shadow_h
#define Shadow_h



namespace XC {
class MachineBroker;
class Message;
class Channel;
class ChannelAddress;
class MovableObject;
class Matrix;
class Vector;
class ID;

class FEM_ObjectBroker;

class Shadow
  {
  private:
    ChannelAddress *theRemoteActorsAddress;    
    int commitTag;
  protected:
    Channel *theChannel;    
    FEM_ObjectBroker *theBroker;
  public:
    Shadow(Channel &theChannel, FEM_ObjectBroker &);

    Shadow(Channel &theChannel, FEM_ObjectBroker &, ChannelAddress &theAddress);
    
    Shadow(int actorType, FEM_ObjectBroker &, MachineBroker &, int compDemand);

    virtual int sendObject(MovableObject &theObject);  
    virtual int recvObject(MovableObject &theObject);      
    virtual int sendMessage(const Message &theMessage);  
    virtual int recvMessage(Message &theMessage);  
    virtual int sendMatrix(const Matrix &theMatrix);  
    virtual int recvMatrix(Matrix &theMatrix);      
    virtual int sendVector(const Vector &theVector);  
    virtual int recvVector(Vector &theVector);      
    virtual int sendID(const ID &theID);  
    virtual int recvID(ID &theID);      
    void setCommitTag(int commitTag);

    Channel 		  *getChannelPtr(void) const;
    FEM_ObjectBroker 	  *getObjectBrokerPtr(void) const;        
    ChannelAddress        *getActorAddressPtr(void) const;
  };
} // end of XC namespace

#endif
