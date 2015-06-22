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
//DqGroundMotions.h

#ifndef DqGroundMotions_h
#define DqGroundMotions_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <deque>
#include "utility/actor/actor/MovableObject.h"


namespace XC {

  class GroundMotion;

//! @ingroup GMotion
//
//! @brief Contenedor de definiciones de sismo.
  class DqGroundMotions : public EntCmd, public MovableObject
  {
  protected:
    typedef std::deque<GroundMotion *> dq_ptr_GroundMotions;
    typedef dq_ptr_GroundMotions::const_reference const_reference;
    typedef dq_ptr_GroundMotions::reference reference;
    typedef dq_ptr_GroundMotions::iterator iterator;
    typedef dq_ptr_GroundMotions::const_iterator const_iterator;

    dq_ptr_GroundMotions gMotions; //!< Cola de punteros a GroundMotion.


    void ejecuta_bloque_for_each(CmdStatus &status,const std::string &bloque);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
    
  public:
    DqGroundMotions(const size_t &num= 0);
    DqGroundMotions(const DqGroundMotions &otro);
    DqGroundMotions &operator=(const DqGroundMotions &otro);
    ~DqGroundMotions(void);

    inline bool empty(void) const
      { return gMotions.empty(); }
    inline size_t getNumGroundMotions(void) const
      { return gMotions.size(); }

    inline iterator begin(void)
      { return gMotions.begin(); }
    inline const_iterator begin(void) const
      { return gMotions.begin(); }
    inline iterator end(void)
      { return gMotions.end(); }
    inline const_iterator end(void) const
      { return gMotions.end(); }

    inline reference operator[](const size_t &n)
      { return gMotions[n]; }
    inline const_reference operator[](const size_t &n) const
      { return gMotions[n]; }

    void addMotion(GroundMotion &f);

    void clear(void);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s,const int &flag);
  };
} // fin namespace XC

#endif
