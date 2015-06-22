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
//ConstantesSecc2d.h

#ifndef ConstantesSecc2d_h
#define ConstantesSecc2d_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Information;
class Parameter;
class Matrix;
class Vector;
class ID;
class SectionForceDeformation;

//! @ingroup MATSCCRepres
//
//! @brief Propiedades mecánicas de una sección (area, inercias,...)
//! en un problema bidimensional (tres grados de libertad por sección).
class ConstantesSecc2d: public EntCmd, public MovableObject
  {
  private:
    double e, g, a, i, alpha;
    static Matrix ks2;
    static Matrix ks3;
  protected:
    virtual DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    ConstantesSecc2d(double E, double A, double I, double G= 0.0,double alpha= 0.0);
    ConstantesSecc2d(double EA, double EI);
    ConstantesSecc2d(void);

    bool check_values(void);
    inline double &E(void)
      { return e; }
    inline const double &E(void) const
      { return e; }
    inline void setE(const double &ee)
      { e= ee; }
    inline double &G(void)
      { return g; }
    inline const double &G(void) const
      { return g; }
    inline void setG(const double &gg)
      { g= gg; }
    inline double &A(void)
      { return a; }
    inline const double &A(void) const
      { return a; }
    inline void setA(const double &aa)
      { a= aa; }
    inline double &Alpha(void)
      { return alpha; }
    inline const double &Alpha(void) const
      { return alpha; }
    inline void setAlpha(const double &al)
      { alpha= al; }
    inline double &I(void)
      { return i; }
    inline const double &I(void) const
      { return i; }
    inline void setI(const double &ii)
      { i= ii; }
    //! @brief Devuelve la rigidez a tracción.
    inline double EA(void) const
      { return e*a; }
    //! @brief Devuelve la rigidez a flexión en z.
    inline double EI(void) const
      { return e*i; }
    //! @brief Devuelve la rigidez a cortante.
    inline double GAAlpha(void) const
      { return g*a*alpha; }

    const Matrix &getSectionTangent2x2(void) const;
    const Matrix &getInitialTangent2x2(void) const;
    const Matrix &getSectionFlexibility2x2(void) const;
    const Matrix &getInitialFlexibility2x2(void) const;
    const Matrix &getSectionTangent3x3(void) const;
    const Matrix &getInitialTangent3x3(void) const;
    const Matrix &getSectionFlexibility3x3(void) const;
    const Matrix &getInitialFlexibility3x3(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    int setParameter(const std::vector<std::string> &,Parameter &,SectionForceDeformation *);
    int updateParameter(int parameterID, Information &info);

    const Matrix& getInitialTangentSensitivity3x3(int gradIndex);

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print (std::ostream &s, int flag = 0) const;
  };

std::ostream &operator<<(std::ostream &os,const ConstantesSecc2d &c);

} // fin namespace XC

#endif
