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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/TCP_SocketNoDelay.h,v $
                                                                        
                                                                        
// File: ~/actor/TCP_SocketNoDelay.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for TCP_SocketNoDelay.
// TCP_SocketNoDelay is a sub-class of channel. It is implemented with Berkeley
// stream sockets using the TCP protocol. Messages delivery is garaunteed. 
// Communication is full-duplex between a pair of connected sockets.

#ifndef TCP_SocketNoDelay_h
#define TCP_SocketNoDelay_h

#include "../channel/Socket.h"
#include "../channel/Channel.h"
#include "../address/SocketAddress.h"

namespace XC {
class TCP_SocketNoDelay : public Channel
  {
  private:
    int sockfd;
    struct sockaddr_in  my_Addr, other_Addr;
    int addrLength;
    unsigned int myPort;
    char add[40];
  public:
    TCP_SocketNoDelay();        
    TCP_SocketNoDelay(unsigned int);    
    TCP_SocketNoDelay(unsigned int other_Port, char *other_InetAddr); 
    ~TCP_SocketNoDelay();

    virtual int setUpShadow(void);
    virtual int setUpActor(void);

    int setNextAddress(const ChannelAddress &otherChannelAddress);
    virtual ChannelAddress *getLastSendersAddress(void){ return 0;};

    int sendObj(MovableObject &theObject, FEM_ObjectBroker &, ChannelAddress *theAddress =0);
    int recvObj(MovableObject &theObject, FEM_ObjectBroker &, ChannelAddress *theAddress =0);
		
    int recvMsg(Message &, ChannelAddress *theAddress =0);       
    int sendMsg(const Message &, ChannelAddress *theAddress =0);    

    int sendMatrix(const Matrix &theMatrix, ChannelAddress *theAddress =0);
    int recvMatrix(Matrix &theMatrix, ChannelAddress *theAddress =0);
    
    int sendVector(const Vector &theVector, ChannelAddress *theAddress =0);
    int recvVector(Vector &theVector, ChannelAddress *theAddress =0);
    
    int sendID(const ID &theID, ChannelAddress *theAddress =0);
    int recvID(ID &theID, ChannelAddress *theAddress =0);    
    
    unsigned int getPortNumber(void) const;
    char *addToProgram(void);
  };
} // fin namespace XC


#endif 






 

