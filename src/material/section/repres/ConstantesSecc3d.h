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
//ConstantesSecc3d.h

#ifndef ConstantesSecc3d_h
#define ConstantesSecc3d_h

class EjesPrincInercia2d;

#include "ConstantesSecc2d.h"

namespace XC {

//! @ingroup MATSCCRepres
//
//! @brief Propiedades mecánicas de una sección (area, inercias,...)
//! en un problema tridimensional (seis grados de libertad por sección).
class ConstantesSecc3d: public ConstantesSecc2d
  {
  private:
    double iy, iyz, j;
    static Matrix ks4;
    static Matrix ks6;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    ConstantesSecc3d(double E, double A, double Iz, double Iy, double G, double J);
    ConstantesSecc3d(double EA, double EIz, double EIy, double GJ);
    ConstantesSecc3d(void);

    bool check_values(void);
    inline double &Iz(void)
      { return ConstantesSecc2d::I(); }
    inline const double &Iz(void) const
      { return ConstantesSecc2d::I(); }
    inline void setIz(const double &i)
      { ConstantesSecc2d::I()= i; }
    inline double &Iy(void)
      { return iy; }
    inline const double &Iy(void) const
      { return iy; }
    inline void setIy(const double &i)
      { iy= i; }
    inline double &Iyz(void)
      { return iyz; }
    inline const double &Iyz(void) const
      { return iyz; }
    inline void setIyz(const double &i)
      { iyz= i; }
    inline double &J(void)
      { return j; }
    inline const double &J(void) const
      { return j; }
    inline void setJ(const double &i)
      { j= i; }
    //! @brief Devuelve la rigidez a flexión en z.
    inline double EIz(void) const
      { return ConstantesSecc2d::EI(); }
    //! @brief Devuelve la rigidez a flexión en y.
    inline double EIy(void) const
      { return E()*iy; }
    //! @brief Devuelve la colaboración del producto
    //! de inercia a la rigidez a flexión.
    inline double EIyz(void) const
      { return E()*iyz; }
    //! @brief Devuelve la rigidez a torsión.
    inline double GJ(void) const
      { return G()*j; }

    double getTheta(void) const;
    double getI1(void) const;
    double getI2(void) const;
    EjesPrincInercia2d getEjesInercia(void) const;
    Vector2d getVDirEje1(void) const;
    Vector2d getVDirEjeFuerte(void) const;
    Vector2d getVDirEje2(void) const;
    Vector2d getVDirEjeDebil(void) const;

    const Matrix &getSectionTangent4x4(void) const;
    const Matrix &getInitialTangent4x4(void) const;
    const Matrix &getSectionFlexibility4x4(void) const;
    const Matrix &getInitialFlexibility4x4(void) const;
    const Matrix &getSectionTangent6x6(void) const;
    const Matrix &getInitialTangent6x6(void) const;
    const Matrix &getSectionFlexibility6x6(void) const;
    const Matrix &getInitialFlexibility6x6(void) const;

    void gira(const double &theta);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    int setParameter(const std::vector<std::string> &argv,Parameter &param,SectionForceDeformation *scc);
    int updateParameter(int parameterID, Information &info);

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print (std::ostream &s, int flag = 0) const;
  };
} // fin namespace XC

#endif
