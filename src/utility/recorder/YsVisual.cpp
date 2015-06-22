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
// YsVisual.cpp: implementation
//
//////////////////////////////////////////////////////////////////////
#include "utility/recorder/YsVisual.h"
#include <domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.h>
#include <utility/recorder/response/Response.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::YsVisual::YsVisual(Element *theEle, const char *title, double scale,
                   int xLoc, int yLoc, int width, int height)
  :Recorder(RECORDER_TAGS_YsVisual),
   displayMode(0)
  {
    const char *argv[1] = {"ysVisual"};
    Information eleInfo(1);
    Response *theResponse = 0;

    ysEle = theEle;

    theResponse = theEle->setResponse(argv, 1, eleInfo);

    if(theResponse)
      displayMode = theResponse->getResponse();
    else
      std::cerr << "YsVisual only works on elements derived from XC::InelasticYS2DGNL\n";

// #ifdef _GLX
//     theMap = new XC::PlainMap();
//     pView =  new OpenGLRenderer(title, xLoc, yLoc, width, height, *theMap);
// #endif

// #ifdef _X11
//     if(!pView) //_GLX and _X11 both defined
//       {
//         theMap = new XC::PlainMap();
//         pView =  new XC::X11Renderer(title, xLoc, yLoc, width, height, *theMap);
//       }
// #endif
//     if(pView)
//       {
//         pView->setVRP(0.0, 0.0, 0.0);
//         pView->setVPN(0.0, 0.0, 1.0);
//         pView->setVUP(0.0, 1.0, 0.0);
//         pView->setFillMode("wire");             // wire mode
//         pView->setPlaneDist(1.0, -1.0);
//         pView->setPRP(0.0, 0.0, 10.0);
//         pView->setPortWindow(-1, 1, -1, 1);  // use the whole window

//         pView->setViewWindow(-scale, scale, -scale, scale);

//         pView->clearImage();
//         pView->doneImage();
//       }
//     else
      std::cerr << "WARNING: XC::YsVisual::createView - Renderer not available\n";
  }

XC::YsVisual::~YsVisual(void)
  {
//     if(theMap) delete theMap;
//     if(pView) delete pView;
  }

int  XC::YsVisual::record(int cTag, double d)
  {
    int res = 0;

//     if(pView)
//       {
//         pView->clearImage();
//         pView->startImage();
//         ysEle->displaySelf(*pView, displayMode, 1);
//         pView->doneImage();
//       }
//     else
      res = -1;
    return res;
  }

int XC::YsVisual::playback(int cTag)
  { return 0; }

int XC::YsVisual::restart(void)
  { return 0; }
