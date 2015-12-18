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
//ShellBData.h,v $

#ifndef ShellBData_h
#define ShellBData_h

#include <cstdlib>
#include <vector>

namespace XC {

class Matrix;

//! \ingroup ElemPlanos
//
//! @brief Datos auxiliares para los elementos «shell».
class ShellBData
  {
  private: 

    //B-bar data
    std::vector<Matrix> GammaB1pointer;
    std::vector<Matrix> GammaD1pointer;
    std::vector<Matrix> GammaA2pointer;
    std::vector<Matrix> GammaC2pointer;

    //Bhat data
    std::vector<Matrix> Bhat;


    void shear_matrix_ptrs_setup(void);
    void bhat_setup(void);
  public:
    ShellBData(void);

    const Matrix &getGammaB1(const size_t &node) const;
    Matrix &getGammaB1(const size_t &node);
    const Matrix &getGammaD1(const size_t &node) const;
    Matrix &getGammaD1(const size_t &node);
    const Matrix &getGammaA2(const size_t &node) const;
    Matrix &getGammaA2(const size_t &node);
    const Matrix &getGammaC2(const size_t &node) const;
    Matrix &getGammaC2(const size_t &node);
    const Matrix &getBhat(const size_t &j) const;
    Matrix &getBhat(const size_t &j);

    void ZeroBHat(void);
    void DivideBHat(const double &);
    const Matrix &computeBshear(const size_t &,const double shp[3][4] ) const;
    const Matrix &computeBbarShear(const size_t &,const double &,const double &,const Matrix &) const;
    void computeB(const double shp[3][4],const double &,const double &,const Matrix &,const double &);
  }; 

} // end of XC namespace
#endif
