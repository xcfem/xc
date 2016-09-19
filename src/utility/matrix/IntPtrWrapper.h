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

#ifndef IntPtrWrapper_h
#define IntPtrWrapper_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <boost/python/list.hpp>

namespace XC {
class IntPtrWrapper: public EntCmd
  {
  private:
    static int ID_NOT_VALID_ENTRY;
    int sz;
    int *data;
    int fromFree;
    void libera(void);
    void check_sizes(void);
    IntPtrWrapper(const IntPtrWrapper &);
    IntPtrWrapper &operator=(const IntPtrWrapper  &V);
  public:
    // constructors and destructor
    IntPtrWrapper(void);
    IntPtrWrapper(int *data, int size, bool cleanIt= false);
    virtual ~IntPtrWrapper();
 
    // utility methods
    inline int Size(void) const
      { return sz; }
    void Zero(void);
    inline const int *getDataPtr(void) const
      { return data; }
    inline int *getDataPtr(void)
      { return data; }
    bool Nulo(void) const;
    const int &max(void) const;
    const int &min(void) const;

    bool checkRange(const int &) const;
    int &at(const int &); 
    const int &at(const int &) const; 
    int &operator()(const int &);
    const int &operator()(const int &) const;
    int &operator[](const int &i)    	    
      { return at(i); }
    const int &operator[](const int &i) const    	    
      { return at(i); }
    
    int setData(int *newData, int size, bool cleanIt = false);
    int getLocation(int value) const;
    int getLocationOrdered(int value) const; // for when insert was used to add elements
    int removeValue(int value);

    friend std::ostream &operator<<(std::ostream &, const IntPtrWrapper &);


    friend class UDP_Socket;
    friend class TCP_Socket;
    friend class TCP_SocketNoDelay;
    friend class MPI_Channel;
  };

std::ostream &operator<<(std::ostream &, const IntPtrWrapper &);

std::vector<int> id_to_std_vector(const IntPtrWrapper &);

inline bool IntPtrWrapper::Nulo(void) const
  { return (data==NULL); }

//! @brief check if argument is inside range [0,sz-1]
inline bool IntPtrWrapper::checkRange(const int &i) const
  {
    if((i < 0) || (i >= sz)) //Range checking.
      {
        std::cerr << "IntPtrWrapper::(loc) - loc "
		  << i << " outside range 0 - " <<  sz-1 << std::endl;
        return false;
      }
    else
      return true;
  }
 
inline int &IntPtrWrapper::at(const int &i) 
  {
    if(checkRange(i))
      return data[i];
    else
      return ID_NOT_VALID_ENTRY;
  }

inline const int &IntPtrWrapper::at(const int &i) const 
  {
    if(checkRange(i))
      return data[i];
    else
      return ID_NOT_VALID_ENTRY;
  }

 
inline int &IntPtrWrapper::operator()(const int &i) 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif
    return data[i];
  }

inline const int &IntPtrWrapper::operator()(const int &i) const 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(!checkRange(i))
      return ID_NOT_VALID_ENTRY;
#endif

    return data[i];
  }

} // end of XC namespace

#endif



