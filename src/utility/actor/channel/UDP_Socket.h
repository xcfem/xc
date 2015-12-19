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
// $Date: 2003/10/15 00:31:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/UDP_Socket.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for UDP_Socket.
// UDP_Socket is a sub-class of channel. It is implemented with
// Berkeley datagram sockets using the UDP protocol. Messages delivery
// is thus unreliable.
//
// What: "@(#) UDP_Socket.h, revA"

#ifndef UDP_Socket_h
#define UDP_Socket_h

#include "utility/actor/channel/Socket.h"
#include "utility/actor/channel/Channel.h"
namespace XC {
class SocketAddress;

class UDP_Socket : public Channel
  {
  private:
    int sockfd;
    int connectType;
    union {
	  struct sockaddr    addr;
	  struct sockaddr_in addr_in;
    } my_Addr;
    union {
      struct sockaddr    addr;
      struct sockaddr_in addr_in;
    } last_Addr;

    socklen_t addrLength;
    char *shadow_inetAddr;
    unsigned int shadow_port;
    unsigned int myPort;    
    char add[40];    
  protected:
    unsigned int getPortNumber(void) const;
  public:
    UDP_Socket(void);
    UDP_Socket(unsigned int port);   
    UDP_Socket(unsigned int other_Port, char *other_InetAddr);  
    ~UDP_Socket(void);
    
    char *addToProgram(void);
    
    virtual int setUpConnection(void);

    int setNextAddress(const ChannelAddress &otherChannelAddress);

    int sendObj(int commitTag, MovableObject &, ChannelAddress *theAddress =0);
    int recvObj(int commitTag, MovableObject &, FEM_ObjectBroker &, ChannelAddress *theAddress =0);
		
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress =0);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress =0);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &, ChannelAddress *theAddress =0);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress =0);
    
    int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress =0);
    int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress =0);
    
    int sendID(int dbTag, int commitTag, const ID &, ChannelAddress *theAddress =0);
    int recvID(int dbTag, int commitTag, ID &, ChannelAddress *theAddress =0);    
  };
} // end of XC namespace

#endif 








 

