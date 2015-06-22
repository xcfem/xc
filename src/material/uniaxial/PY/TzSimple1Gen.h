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
//$Revision: 1.4 $
//$Date: 2004/06/30 00:27:40 $
//$Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PY/TzSimple1Gen.h,v $

#include "Simple1GenBase.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class TzSimple1Gen: public Simple1GenBase
  {
  // Variables used for reading input files:
  int NumTzEle, NPile, NumMt, NumMtLoadSp;
  double p, zground, TULT, Z50, ca, delta, Sa;
  int *TzEleNum, *TzNode1, *TzNode2, *TzMat, *TzDir;	// Arrays for Py Elements File
  int *tzType;
  double *p_t, *p_b, *ca_t, *ca_b, *delta_t, *delta_b,
    *zMt_t, *zMt_b, *mt_val_t, *mt_val_b, tribcoord[2], *Sa_b, *Sa_t,
    *tult_t, *tult_b, *z50_t, *z50_b;  
  
   // Member functions for reading input files:
   void GetTzElements(const std::string &file);
   void GetSoilProperties(const std::string &file);
   double GetTult(const std::string &type);
   double GetZ50(const std::string &type);
   double GetMt(double *vx, double *vy, double x, int length);
   void GetTributaryCoordsTz(int nodenum1);
   void GetTributaryCoordsPile(int nodenum1);
  
    void libera(void);
  // Member functions for generating output:
  void GetTzSimple1(const std::string &file1,const std::string &file2,const std::string &file3,const std::string &file4, const std::string &file5);
  void GetPattern(const std::string &file6);
  
  // Member functions for calculating tult:
  double GetVStress(double z);
  
 public:
  
  void WriteTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5);
  void WriteTzSimple1(const std::string &file1, const std::string &file2, const std::string &file3, const std::string &file4, const std::string &file5, const std::string &file6);
  
  TzSimple1Gen();
  ~TzSimple1Gen();
};
} // fin namespace XC
