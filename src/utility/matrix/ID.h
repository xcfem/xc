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
#include <boost/python/list.hpp>

namespace XC {
class ID: public EntCmd
  {
  private:
    static int ID_NOT_VALID_ENTRY;
    int sz;
    int *data;
    int arraySize;
    int fromFree;
    void libera(void);
    void check_sizes(void);
    void alloc(const size_t &);
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors and destructor
    ID(void);
    explicit ID(int);
    ID(int size, int arraySize);
    ID(const int *data, int size);
    ID(int *data, int size, bool cleanIt = false);
    explicit ID(const std::vector<int> &);
    ID(const boost::python::list &);
    explicit ID(const std::set<int> &);
    ID(const ID &);
    virtual ~ID();
 
    // utility methods
    int Size(void) const;
    void Zero(void);
    const int *getDataPtr(void) const;
    int *getDataPtr(void);
    bool Nulo(void) const;
    int resize(int newSize);
    const int &max(void) const;
    const int &min(void) const;
    
    // overloaded operators
    inline int &operator()(const int &);
    inline const int &operator()(const int &) const;
    int &operator[](int);    	    
    
    ID &operator=(const ID  &V);

    int setData(int *newData, int size, bool cleanIt = false);
    int getLocation(int value) const;
    int getLocationOrdered(int value) const; // for when insert was used to add elements
    int removeValue(int value);

    friend std::ostream &operator<<(std::ostream &, const ID &);
    //    friend istream &operator>>(istream &s, ID &V);    
    virtual any_const_ptr GetProp(const std::string &cod) const;

    friend class UDP_Socket;
    friend class TCP_Socket;
    friend class TCP_SocketNoDelay;
    friend class MPI_Channel;
  };

std::ostream &operator<<(std::ostream &, const ID &);

std::vector<int> id_to_std_vector(const ID &);

inline int ID::Size(void)  const
   {return sz;}

inline const int *ID::getDataPtr(void) const
  { return data; }
inline int *ID::getDataPtr(void)
  { return data; }

inline bool ID::Nulo(void) const
  { return (data==NULL); }

inline int &ID::operator()(const int &x) 
  {
#ifdef _G3DEBUG
  // check if it is inside range [0,sz-1]
  if (x < 0 || x >= sz) {
    std::cerr << "ID::(loc) - loc " << x << " outside range 0 - " <<  sz-1 << endln;
    return ID_NOT_VALID_ENTRY;
  }
#endif

  
  return data[x];
}

inline const int &ID::operator()(const int &x) const 
  {
#ifdef _G3DEBUG
  // check if it is inside range [0,sz-1]
  if (x < 0 || x >= sz) {
    std::cerr << "ID::(loc) - loc " << x << " outside range 0 - " <<  sz-1 << endln;
    return ID_NOT_VALID_ENTRY;
  }
#endif

  return data[x];
  }
} // end of XC namespace

#endif



