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
//EsfBeamColumn3d.h


#ifndef EsfBeamColumn3d_h
#define EsfBeamColumn3d_h

#include "utility/matrix/Vector.h"

namespace XC {

class DbTagData;

class FEM_ObjectBroker;
class CommParameters;

//! \ingroup ElemBarra
//
//! @brief Esfuerzos en un elemento barra tridimensional.
class EsfBeamColumn3d: public Vector
  {
  public:
    EsfBeamColumn3d(void);
    explicit EsfBeamColumn3d(const Vector &);
    EsfBeamColumn3d(const EsfBeamColumn3d &otro);
    EsfBeamColumn3d &operator=(const EsfBeamColumn3d &otro);
    //! @brief Devuelve el axil.
    inline const double &N(void) const
      { return (*this)[0]; }
    //! @brief Devuelve el axil.
    inline double &N(void)
      { return (*this)[0]; }
    //! @brief Devuelve la fuerza axil que se ejerce sobre la barra en el extremo dorsal.
    inline double AN1(void) const
      { return -N(); }
    //! @brief Devuelve la fuerza axil que se ejerce sobre la barra en el extremo frontal.
    inline double AN2(void) const
      { return N(); }
    //! @brief Devuelve el momento z en el extremo dorsal.
    inline const double &Mz1(void) const
      { return (*this)[1]; }
    //! @brief Devuelve el momento z en el extremo dorsal.
    inline double &Mz1(void)
      { return (*this)[1]; }
    //! @brief Devuelve el momento z en el extremo frontal.
    inline const double &Mz2(void) const
      { return (*this)[2]; }
    //! @brief Devuelve el momento z en el extremo frontal.
    inline double &Mz2(void)
      { return (*this)[2]; }
    //! @brief Devuelve el cortante Vy.
    inline double Vy(const double &L) const
      { return (Mz1()+Mz2())/L; }
    //! @brief Devuelve el momento y en el extremo dorsal.
    inline const double &My1(void) const
      { return (*this)[3]; }
    //! @brief Devuelve el momento y en el extremo dorsal.
    inline double &My1(void)
      { return (*this)[3]; }
    //! @brief Devuelve el momento y en el extremo frontal.
    inline const double &My2(void) const
      { return (*this)[4]; }
    //! @brief Devuelve el momento y en el extremo frontal.
    inline double &My2(void)
      { return (*this)[4]; }
    //! @brief Devuelve el cortante Vy.
    inline double Vz(const double &L) const
      { return -((My1()+My2())/L); }
    //! @brief Devuelve el torsor.
    inline const double &T(void) const
      { return (*this)[5]; }
    //! @brief Devuelve el torsor.
    inline double &T(void)
      { return (*this)[5]; }
    //! @brief Devuelve el torsor en el extremo dorsal.
    inline double T1(void) const
      { return -T(); }
    //! @brief Devuelve el torsor en el extremo frontal.
    inline double T2(void) const
      { return T(); }
  };
int sendEsfBeamColumn3d(const EsfBeamColumn3d &, int posDbTag,DbTagData &dt, CommParameters &cp);
int receiveEsfBeamColumn3d(EsfBeamColumn3d &v, int posDbTag,DbTagData &dt,const CommParameters &cp);

} // end of XC namespace

#endif
