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
//DqUniaxialMaterial.h
//deque de punteros (se emplear en la clase Set).


#ifndef DQUNIAXIALMATERIAL_H
#define DQUNIAXIALMATERIAL_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include <deque>
#include <material/uniaxial/UniaxialMaterial.h>


namespace XC {

//! @ingroup MatUnx
//
//! @brief Contenedor de punteros a UniaxialMaterial.
class DqUniaxialMaterial: public EntCmd, public MovableObject, public std::deque<UniaxialMaterial *>
  {
    void libera(void);
  public:
    typedef std::deque<UniaxialMaterial *> lst_ptr;
    typedef lst_ptr::const_iterator const_iterator;
    typedef lst_ptr::iterator iterator;
    typedef lst_ptr::reference reference;
    typedef lst_ptr::const_reference const_reference;
    typedef lst_ptr::size_type size_type;
  protected:
    void copia_lista(const DqUniaxialMaterial &,SectionForceDeformation *s= NULL);
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);


  public:
    DqUniaxialMaterial(EntCmd *owner= NULL,const size_t &sz= 0);
    DqUniaxialMaterial(EntCmd *owner,const UniaxialMaterial &);
    DqUniaxialMaterial(EntCmd *owner,const UniaxialMaterial &um,const size_t &sz);
    DqUniaxialMaterial(EntCmd *owner,const UniaxialMaterial *um,const size_t &sz);
    DqUniaxialMaterial(const DqUniaxialMaterial &otro);
    DqUniaxialMaterial(const DqUniaxialMaterial &otro,SectionForceDeformation *s);
    DqUniaxialMaterial &operator=(const DqUniaxialMaterial &otro);
    ~DqUniaxialMaterial(void);
    void push_back(const UniaxialMaterial *,SectionForceDeformation *s= NULL);
    void push_front(const UniaxialMaterial *,SectionForceDeformation *s= NULL);
    inline iterator begin(void)
      { return lst_ptr::begin(); }
    const_iterator begin(void) const
      { return lst_ptr::begin(); }
    iterator end(void)
      { return lst_ptr::end(); }
    const_iterator end(void) const
      { return lst_ptr::end(); }
    void clear(void);
    void clearAll(void);
    inline size_type size(void) const
      { return lst_ptr::size(); }
    void resize(const size_t &n);

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);

    int setInitialStrain(const Vector &def,const size_t &offset);
    int setTrialStrain(const Vector &def,const size_t &offset);
    int setTrialStrain(const double &strain,const double &strainRate);
    void getStrain(Vector &,const size_t &offset) const;
    void getInitialStrain(Vector &,const size_t &offset) const;
    void getTangent(Matrix &,const size_t &offset) const;
    void getInitialTangent(Matrix &,const size_t &offset) const;
    void getFlexibility(Matrix &f,const size_t &offset) const;
    void getInitialFlexibility(Matrix &f,const size_t &offset) const;
    void getStress(Vector &,const size_t &offset) const;

    inline reference operator[](const size_t i)
      { return lst_ptr::operator[](i); }
    inline const_reference operator[](const size_t i) const
      { return lst_ptr::operator[](i); }

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;

  };

} //end of XC namespace
#endif

