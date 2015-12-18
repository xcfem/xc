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
//beam2d.h

#ifndef beam2d_h
#define beam2d_h

#include <domain/mesh/element/truss_beam_column/ProtoBeam2d.h>

namespace XC {
class CrdTransf2d;

//! \ingroup ElemBarra
//
//! @brief Elemento barra para problemas bidimensionales.
class beam2d : public ProtoBeam2d
  {
  protected:
    // protected attributes - a copy for each object of the class        
    mutable double L;
    mutable double sn;
    mutable double cs;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    beam2d(int tag, int class_tag);
    beam2d(int tag, int class_tag, double A, double E, double I, int Nd1, int Nd2);    
  };
} // end of XC namespace

#endif

