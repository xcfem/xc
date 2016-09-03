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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/08/29 07:17:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/machineBroker/MPI_MachineBroker.cpp,v $
                                                                        
                                                                        
// Written: fmk
// Revision: A


#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/actor/machineBroker/MPI_MachineBroker.h>
#include <utility/actor/channel/MPI_Channel.h>
#include <utility/matrix/ID.h>

#include <mpi/mpi.h>

void XC::MPI_MachineBroker::libera(void)
  {
    const size_t sz= theChannels.size();
    for(size_t i=0; i<sz; i++)
      {
        if(theChannels[i])
          {
            delete theChannels[i];
            theChannels[i]= nullptr;
          }
        else
	  std::cerr << "Puntero a canal nulo." << std::endl;
      }
    theChannels.clear();
    if(usedChannels)
      {
        delete usedChannels;
        usedChannels= nullptr;
      }
  }

void XC::MPI_MachineBroker::alloc(const std::size_t &sz)
  {
    libera();
    theChannels= std::vector<MPI_Channel *>(sz,nullptr);
    for(size_t i=0; i<sz; i++)
      theChannels[i]= new MPI_Channel(i);
    usedChannels= new ID(sz);
    usedChannels->Zero();
  }

XC::MPI_MachineBroker::MPI_MachineBroker(FEM_ObjectBroker *theBroker, int argc,char *argv[])
  :MachineBroker(theBroker)
  {
    MPI_Init(&argc, &argv);
    int r= rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &r);
    rank= r;
    int sz= theChannels.size();
    MPI_Comm_size(MPI_COMM_WORLD, &sz);
    alloc(sz);
  }


XC::MPI_MachineBroker::~MPI_MachineBroker(void)
  {
    libera();
    MPI_Finalize();
  }


int XC::MPI_MachineBroker::getPID(void)
  { return rank; }


int XC::MPI_MachineBroker::getNP(void)
  { return theChannels.size(); }



XC::Channel *XC::MPI_MachineBroker::getMyChannel(void)
  { return theChannels[0]; }



XC::Channel *XC::MPI_MachineBroker::getRemoteProcess(void)
  {
    if(rank != 0)
      {
        std::cerr << "XC::MPI_MachineBroker::getRemoteProcess() - child process cannot not yet allocate processes\n";
        return 0;
      }
    const size_t sz= theChannels.size();
    for(size_t i=0; i<sz; i++)
      if(i != rank)
        if((*usedChannels)(i) == 0)
          {
            (*usedChannels)(i) = 1;
            return theChannels[i];
          }
  
    // no processes available
    return 0;
  }



int XC::MPI_MachineBroker::freeProcess(Channel *theChannel)
  {
    const size_t sz= theChannels.size();
    for(size_t i=0; i<sz; i++)
      if(i != rank)
        if(theChannels[i] == theChannel)
          {
            (*usedChannels)(i)= 0;
            return 0;
          }
    // channel not found!
    return -1;
  }

