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
//EQBasePattern.h

#ifndef EQBasePattern_h
#define EQBasePattern_h

#include "domain/load/pattern/LoadPattern.h"
#include "domain/load/groundMotion/DqGroundMotions.h"

namespace XC {
class GroundMotion;
class Vector;

//! @ingroup LPatterns
//
//! @brief Pauta de cargas correspondiente a un terremoto.
class EQBasePattern: public LoadPattern
  {
  protected:
    DqGroundMotions theMotions;

    int addMotion(GroundMotion &theMotion);
    void alloc(const size_t &sz);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    EQBasePattern(int tag, int classTag);

    inline size_t getNumMotions(void) const
      { return theMotions.getNumGroundMotions(); }

    void clear(void)
      { theMotions.clear(); }

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif
