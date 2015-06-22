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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/23 18:27:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/Channel.h,v $
                                                                        
                                                                        
#ifndef Channel_h
#define Channel_h

// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for Channel.
// Channel is an abstract base class which defines the channel interface.
// A channel is a point of communication in a program, a mailbox to/from
// which data enters/leaves a program.
//
// What: "@(#) Channel.h, revA"


#include "xc_utils/src/nucleo/EntCmd.h"
#include <set>

namespace XC {
class ChannelAddress;
class Message;
class MovableObject;
class Matrix;
class Vector;
class ID;
class FEM_ObjectBroker;

class Channel: public EntCmd
  {
  private:
    static int numChannel;
    int tag; //! Tag del canal;
    std::set<int> usedDbTags;//!< dbTags usados.
  protected:
    int sendMovable(int commitTag, MovableObject &);
    int receiveMovable(int commitTag, MovableObject &, FEM_ObjectBroker &);
  public:
    Channel(void);
    inline virtual ~Channel(void) {}

    // methods to set up the channel in an actor space
    virtual char *addToProgram(void) =0;
    virtual int setUpConnection(void) =0;
    virtual int setNextAddress(const ChannelAddress &theAddress) =0;
    virtual ChannelAddress *getLastSendersAddress(void) =0;

    virtual bool isDatastore(void) const;
    virtual int getDbTag(void) const;
    bool checkDbTag(const int &dbTag);
    const ID &getUsedTags(void) const;
    void clearDbTags(void);
    int getTag(void) const;
    
    // methods to send/receive messages and objects on channels.
    virtual int sendObj(int commitTag, MovableObject &, ChannelAddress *theAddress= NULL) =0;
    virtual int recvObj(int commitTag, MovableObject &, FEM_ObjectBroker &, ChannelAddress *theAddress= NULL) =0;
    template <class inputIterator>
    int sendObjs(int commitTag,const inputIterator &first,const inputIterator &last,ChannelAddress *theAddress= NULL);
    template <class inputIterator>
    int recvObjs(int commitTag,const inputIterator &first,const inputIterator &last, FEM_ObjectBroker &, ChannelAddress *theAddress= NULL);

    virtual int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= NULL) =0;  
    virtual int recvMsg(int dbTag, int commitTag, Message &,ChannelAddress *theAddress= NULL) =0;  
    
    virtual int sendMatrix(int dbTag, int commitTag, const Matrix &,ChannelAddress *theAddress= NULL) =0;  
    virtual int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress= NULL) =0;  
 
    virtual int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress= NULL) =0;
    virtual int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress= NULL) =0;

    virtual int sendID(int dbTag, int commitTag,const ID &, ChannelAddress *theAddress= NULL) =0;  
    virtual int recvID(int dbTag, int commitTag,ID &, ChannelAddress *theAddress= NULL) =0;      
  };

//! @brief Envía una serie de objetos.
template <class inputIterator>
int Channel::sendObjs(int commitTag,const inputIterator &first,const inputIterator &last,ChannelAddress *theAddress)
  {
    int retval= 0;
    for(inputIterator i= first;i!=last;i++)
      {
        retval= sendObj(commitTag,*i,theAddress);
        if(retval!=0)
          break;
      }
    return retval;
  }

//! @brief Recibe una serie de objetos.
template <class inputIterator>
int Channel::recvObjs(int commitTag,const inputIterator &first,const inputIterator &last, FEM_ObjectBroker &ob, ChannelAddress *theAddress)
  {
    int retval= 0;
    for(inputIterator i= first;i!=last;i++)
      {
        retval= recvObj(commitTag,*i,ob,theAddress);
        if(retval!=0)
          break;
      }
    return retval;
  }

} // fin namespace XC

#endif
