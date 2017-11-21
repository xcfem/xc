//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
//     theMap = new PlainMap();
//     pView =  new OpenGLRenderer(title, xLoc, yLoc, width, height, *theMap);
// #endif

// #ifdef _X11
//     if(!pView) //_GLX and _X11 both defined
//       {
//         theMap = new PlainMap();
//         pView =  new X11Renderer(title, xLoc, yLoc, width, height, *theMap);
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
