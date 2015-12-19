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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/23 18:24:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/actor/Actor.h,v $
                                                                        
                                                                        
#ifndef Actor_h
#define Actor_h

// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for Actor.
// Actor is meant to be an abstract base class and thus no objects of it's type
// can be instantiated. 
//
// What: "@(#) Actor.h, revA"

#include <vector>

namespace XC {

class ObjectBroker;
class Message;
class Channel;
class ChannelAddress;
class MovableObject;
class Matrix;
class Vector;
class ID;
class FEM_ObjectBroker;

class ActorMethod
  {
  public:
    int tag;
    int (*theMethod)();
    inline ActorMethod(int tg, int (*f)())
      : tag(tg), theMethod(f) {}
  };

//! @ingroup ACTOR
//
//! @brief 
class Actor
  {
  private:	
    int numMethods, maxNumMethods;
    std::vector<ActorMethod *> actorMethods;
    ChannelAddress *theRemoteShadowsAddress;

    int commitTag;

    void libera(void);
    void alloc(const std::vector<ActorMethod *> &otro);
    int alloc_method(const ActorMethod &);

  protected:
    FEM_ObjectBroker *theBroker; 
    Channel *theChannel;
  public:
    Actor(Channel &theChannel, FEM_ObjectBroker &theBroker,int numActorMethods =0);
    Actor(const Actor &);
    Actor &operator=(const Actor &);
    virtual ~Actor(void);
    
    virtual int  run(void) = 0;

    virtual int  addMethod(int tag, int (*fp)());
    virtual int  getMethod();
    virtual int  processMethod(int tag);

    virtual int sendObject(MovableObject &theObject, ChannelAddress *theAddress =0);  
    virtual int recvObject(MovableObject &theObject, ChannelAddress *theAddress =0);

    virtual int sendMessage(const Message &theMessage, ChannelAddress *theAddress =0);   
    virtual int recvMessage(Message &theMessage, ChannelAddress *theAddress =0);  
    
    virtual int sendMatrix(const Matrix &theMatrix, ChannelAddress *theAddress =0);   
    virtual int recvMatrix(Matrix &theMatrix, ChannelAddress *theAddress =0);  
    
    virtual int sendVector(const Vector &theVector, ChannelAddress *theAddress =0);   
    virtual int recvVector(Vector &theVector, ChannelAddress *theAddress =0);  
    
    virtual int sendID(const ID &theID, ChannelAddress *theAddress =0);   
    virtual int recvID(ID &theID, ChannelAddress *theAddress =0);  

    Channel *getChannelPtr(void) const;
    FEM_ObjectBroker *getObjectBrokerPtr(void) const;    
    ChannelAddress *getShadowsAddressPtr(void) const;            

    virtual int barrierCheck(int result);
    void setCommitTag(int commitTag);
  };

} // end of XC namespace

#endif

