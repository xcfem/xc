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
// YsVisual.h: interface
//
//////////////////////////////////////////////////////////////////////

#if !defined(YSVISUAL)
#define YSVISUAL

#include <utility/recorder/Recorder.h>

namespace XC {
class ColorMap;
class Renderer;
 class Element;

class YsVisual : public Recorder
  {
  private:
    int      displayMode;
    Element  *ysEle;
/*     Renderer *pView; */
/*     ColorMap *theMap; */
  public:
    virtual ~YsVisual();
    YsVisual(Element* theEle, const char *title, double scale,
	         int xLoc, int yLoc, int width, int height);
	

    int record(int cTag, double d);
    int playback(int cTag);
    int restart(void);
  };
} // fin namespace XC

#endif // !defined
