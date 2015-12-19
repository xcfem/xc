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
                                                                        
#ifndef EnvelopeData_h
#define EnvelopeData_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Vector;
class Matrix;

class EnvelopeData: public MovableObject
  {
  private:	
    Matrix *data;
    Vector *currentData;
    bool first;
    friend class EnvelopeElementRecorder;
    friend class EnvelopeNodeRecorder;
    void alloc(const size_t &);
    void free(void);
  protected:
    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);
  public:
    EnvelopeData(void);
    ~EnvelopeData(void);

    inline const Matrix *getData(void) const
      { return data; }
    inline Matrix *getData(void)
      { return data; }
    inline const Vector *getCurrentData(void) const
      { return currentData; }
    inline Vector *getCurrentData(void)
      { return currentData; }
    inline const bool &First(void) const
      { return first; }
    inline bool &First(void)
      { return first; }

    int restart(void); 

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif
