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
                                                                        
// $Revision: 1.10 $
// $Date: 2005/11/23 22:37:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/matrix/ID.h,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ID.
// ID is a concrete class implementing the integer array abstraction.
// ID objects are Vectors of integers which only need a few
// operators defined on them.
//
// What: "@(#) ID.h, revA"


#ifndef ID_h
#define ID_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <vector>
#include <boost/python/list.hpp>

namespace XC {
class ID: public EntCmd, public std::vector<int>
  {
  public:
    typedef std::vector<int> v_int;
  private:
    static int ID_NOT_VALID_ENTRY;
  public:
    // constructors and destructor
    ID(void);
    explicit ID(const int &);
    ID(const int *data, int size);
    ID(const int &,const int &);
    explicit ID(const v_int &);
    ID(const boost::python::list &);
    explicit ID(const std::set<int> &);
    template <class InputIterator>
    inline ID(InputIterator first, InputIterator last)
      : EntCmd(), std::vector<int>(first,last) {}
    inline virtual ~ID(){}
 
    // utility methods
    inline int Size(void) const
      { return size(); }
    void Zero(void);
    inline const int *getDataPtr(void) const
      { return &(*this)[0]; }
    inline int *getDataPtr(void)
      { return &(*this)[0]; }
    inline bool Nulo(void) const
      { return empty(); }
    int resize(const int &newSize);
    const int &max(void) const;
    const int &min(void) const;

    bool checkRange(const int &) const;
    int &operator()(const int &);
    const int &operator()(const int &) const;
    int &operator[](const int &i);
    const int &operator[](const int &i) const
      { return this->at(i); }
  

    int getLocation(const int &) const;
    int getLocationOrdered(const int &) const; // for when insert was used to add elements
    int removeValue(const int &);

    friend std::ostream &operator<<(std::ostream &, const ID &);
    //    friend istream &operator>>(istream &s, ID &V);    


    friend class UDP_Socket;
    friend class TCP_Socket;
    friend class TCP_SocketNoDelay;
    friend class MPI_Channel;
  };

std::ostream &operator<<(std::ostream &, const ID &);

//! @brief check if argument is inside range [0,sz-1]
inline bool ID::checkRange(const int &i) const
  {
    const int sz= Size();
    if((i < 0) || (i >= sz)) //Range checking.
      {
        std::cerr << "ID::(loc) - loc "
		  << i << " outside range 0 - " <<  sz-1 << std::endl;
        return false;
      }
    else
      return true;
  }
 
inline int &ID::operator()(const int &i) 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif
    return (*this)[i];
  }

inline const int &ID::operator()(const int &i) const 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif

    return (*this)[i];
  }

} // end of XC namespace

#endif



