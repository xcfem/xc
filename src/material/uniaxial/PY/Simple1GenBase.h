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
//Simple1GenBase.h

#include <string>
#include <vector>


namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class Simple1GenBase
  {
  protected:	
    // Variables used for reading input files:
    int NumNodes, NumPileEle, NumLayer, NumLoad, NumSp, NumMat;
    double b, depth, stress, ru;
    int *NodeNum;  // Arrays for Nodes File
    double *Nodey, *Nodex;
    int *PileEleNum, *PileNode1, *PileNode2;		// Arrays for Pile Elements File
    double *gamma_t, *gamma_b, *z_t, *z_b, *c_t, *c_b, // Arrays for Soil Properties File
    	*zLoad_t, *zLoad_b, *load_val_t, *load_val_b, *zSp_t, *zSp_b, *sp_val_t,
    	*sp_val_b, *zMp_t, *zMp_b, *mp_val_t, *mp_val_b, *ru_t, *ru_b;
    std::vector<std::string> MatType;
    std::string PatternInfo;

    void libera(void);
    // Member functions for reading input files:
    void GetNodes(const std::string &file);
    void GetPileElements(const std::string &file);
    int NumRows(const std::string &file, const std::string &begin);
    double linterp(double x1, double x2, double y1, double y2, double x3);


    // Member functions for generating output:

  public:
    // Public member functions accessed from TclModelBuilder.cpp
    Simple1GenBase(void);
    ~Simple1GenBase(void);
  };
} // fin namespace XC
