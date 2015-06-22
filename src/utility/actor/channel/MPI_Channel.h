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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/15 00:26:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/MPI_Channel.h,v $
                                                                        
                                                                        
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for MPI_Channel.
// MPI_Channel is a sub-class of channel. It is implemented with Berkeley
// stream sockets using the TCP protocol. Messages delivery is garaunteed. 
// Communication is full-duplex between a pair of connected sockets.

#ifndef MPI_Channel_h
#define MPI_Channel_h

#include <utility/actor/channel/Channel.h>
#include <mpi/mpi.h>

namespace XC {
class MPI_Channel: public Channel
  {
  private:
    int otherTag;
    MPI_Comm otherComm;    
  public:
    MPI_Channel(int otherProcess);        

    char *addToProgram(void);
    
    virtual int setUpConnection(void);

    int setNextAddress(const ChannelAddress &otherChannelAddress);
    virtual ChannelAddress *getLastSendersAddress(void);

    int sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress =0);
    
    int recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theBroker,ChannelAddress *theAddress =0);

    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress =0);       
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress =0);    

    int sendMatrix(int dbTag, int commitTag, const Matrix &, ChannelAddress *theAddress =0);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress =0);
    
    int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress =0);
    int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress =0);
    
    int sendID(int dbTag, int commitTag, const ID &, ChannelAddress *theAddress =0);
    int recvID(int dbTag, int commitTag, ID &, ChannelAddress *theAddress =0);    
  };
} // fin namespace XC


#endif 






 

