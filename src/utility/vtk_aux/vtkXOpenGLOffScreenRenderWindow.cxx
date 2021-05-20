//----------------------------------------------------------------------------
//  biblioteca vtk_aux; utilidades construidas sobre VTK (<http://www.vtk.org>)
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkXOpenGLOffScreenRenderWindow.cxx,v $
  Language:  C++
  Date:      $Date: 2003/03/26 14:04:59 $
  Version:   $Revision: 1.42 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkXOpenGLOffScreenRenderWindow.h"
#include "vtkOpenGLRenderer.h"
#include "vtkOpenGLProperty.h"
#include "vtkOpenGLTexture.h"
#include "vtkOpenGLCamera.h"
#include "vtkOpenGLLight.h"
#include "vtkOpenGLActor.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "GL/gl.h"
#include "GL/glx.h"
#include <sstream>

#include "vtkToolkits.h"

#include "vtkCommand.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

class vtkXOpenGLOffScreenRenderWindow;
class vtkRenderWindow;

class vtkXOpenGLOffScreenRenderWindowInternal
  {
    friend class vtkXOpenGLOffScreenRenderWindow;
  private:
    vtkXOpenGLOffScreenRenderWindowInternal(vtkRenderWindow*);
    GLXContext ContextId; 
  };

vtkXOpenGLOffScreenRenderWindowInternal::vtkXOpenGLOffScreenRenderWindowInternal(vtkRenderWindow * vtkNotUsed(rw))
  { this->ContextId = nullptr; }


vtkCxxRevisionMacro(vtkXOpenGLOffScreenRenderWindow, "$Revision: 0.01 $")
vtkStandardNewMacro(vtkXOpenGLOffScreenRenderWindow)


#define MAX_LIGHTS 8


XVisualInfo *vtkXOpenGLOffScreenRenderWindowTryForVisual(Display *DisplayId,
                                               int doublebuff, int stereo,
                                               int multisamples)
{
  int           index;
  static int    attributes[50];

  // setup the default stuff we ask for
  index = 0;
  attributes[index++] = GLX_RGBA;
  attributes[index++] = GLX_RED_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_GREEN_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_BLUE_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_DEPTH_SIZE;
  attributes[index++] = 1;
  if(doublebuff)
    {
    attributes[index++] = GLX_DOUBLEBUFFER;
    }
  if(stereo)
    {
    // also try for STEREO
    attributes[index++] = GLX_STEREO;
    }
  if(multisamples)
    {
#ifdef GLX_SAMPLE_BUFFERS_SGIS
    attributes[index++] = GLX_SAMPLE_BUFFERS_SGIS;
    attributes[index++] = 1;
    attributes[index++] = GLX_SAMPLES_SGIS;
    attributes[index++] = multisamples;
#endif
    }
    
  attributes[index++] = None;

  return glXChooseVisual(DisplayId, DefaultScreen(DisplayId), attributes );
}

XVisualInfo *vtkXOpenGLOffScreenRenderWindow::GetDesiredVisualInfo()
{
  XVisualInfo   *v = nullptr;
  int           multi;
  int           stereo = 0;
  
  // get the default display connection 
  if(!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay((char *)nullptr); 
    if(this->DisplayId == nullptr) 
      {
      vtkErrorMacro(<< "bad X server connection.\n");
      }
    this->OwnDisplay = 1;
    }

  // try every possibility stoping when we find one that works
  for(stereo = this->StereoCapableWindow; !v && stereo >= 0; stereo--)
    {
    for(multi = this->MultiSamples; !v && multi >= 0; multi--)
      {
      if(v) 
        {
        XFree(v);
        }
      v = vtkXOpenGLOffScreenRenderWindowTryForVisual(this->DisplayId,
                                            this->DoubleBuffer, 
                                            stereo, multi);
      if(v && this->StereoCapableWindow && !stereo)
        {
        // requested a stereo capable window but we could not get one
        this->StereoCapableWindow = 0;
        }
      }
    }
  for(stereo = this->StereoCapableWindow; !v && stereo >= 0; stereo--)
    {
    for(multi = this->MultiSamples; !v && multi >= 0; multi--)
      {
      if(v) 
        {
        XFree(v);
        }
      v = vtkXOpenGLOffScreenRenderWindowTryForVisual(this->DisplayId,
                                            !this->DoubleBuffer, 
                                            stereo, multi);
      if(v)
        {
        this->DoubleBuffer = !this->DoubleBuffer;
        }
      if(v && this->StereoCapableWindow && !stereo)
        {
        // requested a stereo capable window but we could not get one
        this->StereoCapableWindow = 0;
        }
      }
    }
  if(!v) 
    {
    vtkErrorMacro(<< "Could not find a decent visual\n");
    }
  return ( v );
}

vtkXOpenGLOffScreenRenderWindow::vtkXOpenGLOffScreenRenderWindow()
  {
    this->ParentId = (Window)nullptr;
    this->ScreenSize[0] = 0;
    this->ScreenSize[1] = 0;
    this->OwnDisplay = 0;
    this->CursorHidden = 0;
    this->ForceMakeCurrent = 0;
    this->UsingHardware = 0;
    this->DisplayId = (Display *)nullptr;
    this->WindowId = (Window)nullptr;
    this->NextWindowId = (Window)nullptr;
    this->ColorMap = (Colormap)0;
    this->OwnWindow = 0;
 
    this->Internal = new vtkXOpenGLOffScreenRenderWindowInternal(this);

    this->XCArrow =   0;
    this->XCSizeAll = 0;
    this->XCSizeNS =  0;
    this->XCSizeWE =  0;
    this->XCSizeNE =  0;
    this->XCSizeNW =  0;
    this->XCSizeSE =  0;
    this->XCSizeSW =  0;

    this->Capabilities= "";
  }

void vtkXOpenGLOffScreenRenderWindow::DestroyWindow(void)
  {
    // free the cursors
    if(this->DisplayId)
      {
        if(this->WindowId)
          { XUndefineCursor(this->DisplayId,this->WindowId); }
        if(this->XCArrow)
          { XFreeCursor(this->DisplayId,this->XCArrow); }
        if(this->XCSizeAll)
          { XFreeCursor(this->DisplayId,this->XCSizeAll); }
        if(this->XCSizeNS)
          { XFreeCursor(this->DisplayId,this->XCSizeNS); }
        if(this->XCSizeWE)
          { XFreeCursor(this->DisplayId,this->XCSizeWE); }
        if(this->XCSizeNE)
          { XFreeCursor(this->DisplayId,this->XCSizeNE); }
        if(this->XCSizeNW)
          { XFreeCursor(this->DisplayId,this->XCSizeNW); }
        if(this->XCSizeSE)
          { XFreeCursor(this->DisplayId,this->XCSizeSE); }
        if(this->XCSizeSW)
          { XFreeCursor(this->DisplayId,this->XCSizeSW); }
      }
    XCArrow =   0;
    XCSizeAll = 0;
    XCSizeNS =  0;
    XCSizeWE =  0;
    XCSizeNE =  0;
    XCSizeNW =  0;
    XCSizeSE =  0;
    XCSizeSW =  0;
  
    this->MakeCurrent();
  
    // tell each of the renderers that this render window/graphics context
    // is being removed (the RendererCollection is removed by vtkRenderWindow's
    // destructor)
    vtkRenderer* ren;
    this->Renderers->InitTraversal();
    for( ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject());
        ren != nullptr;
        ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject())  )
      {
        ren->SetRenderWindow(nullptr);
        ren->SetRenderWindow(this);
      }

    // make sure we have been initialized 
    if(this->Internal->ContextId)
      {

        /* first delete all the old lights */
        for(short cur_light = GL_LIGHT0; cur_light < GL_LIGHT0+MAX_LIGHTS; cur_light++)
          { glDisable((GLenum)cur_light); }

        /* now delete all textures */
        glDisable(GL_TEXTURE_2D);
        for(int i = 1; i < this->TextureResourceIds->GetNumberOfIds(); i++)
          {
            GLuint txId = static_cast<GLuint>(this->TextureResourceIds->GetId(i));
#ifdef GL_VERSION_1_1
            if(glIsTexture(txId))
              { glDeleteTextures(1, &txId); }
#else
            if(glIsList(txId))
              { glDeleteLists(txId,1); }
#endif
          }

        glFinish();

        glXDestroyGLXPixmap(this->DisplayId,this->WindowId);
        XFreePixmap(this->DisplayId,this->pixmap);


        glXDestroyContext( this->DisplayId, this->Internal->ContextId);
        this->Internal->ContextId = nullptr;
      }
    // then close the old window 
    if(this->OwnWindow && this->DisplayId && this->WindowId)
      {
        XDestroyWindow(this->DisplayId,this->WindowId);
        this->WindowId = static_cast<Window>(NULL);
      }
    if(this->DisplayId)
      { XSync(this->DisplayId,0); }

    // if we create the display, we'll delete it
    if(this->OwnDisplay && this->DisplayId)
      {
        XCloseDisplay(this->DisplayId);
        this->DisplayId = nullptr;
      }

    Capabilities= "";

    // make sure all other code knows we're not mapped anymore
    this->Mapped = 0;
  }

// free up memory & close the window
vtkXOpenGLOffScreenRenderWindow::~vtkXOpenGLOffScreenRenderWindow()
  {
    this->Finalize();
  
    vtkRenderer* ren;
    this->Renderers->InitTraversal();
    for ( ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject());
        ren != nullptr;
        ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject())  )
      { ren->SetRenderWindow(nullptr); }

    if(Internal)
      {
        delete Internal;
        Internal= nullptr;
      }
  }

// End the rendering process and display the image.
void vtkXOpenGLOffScreenRenderWindow::Frame(void)
{
  this->MakeCurrent();
}
 
//
// Set the variable that indicates that we want a stereo capable window
// be created. This method can only be called before a window is realized.
//
void vtkXOpenGLOffScreenRenderWindow::SetStereoCapableWindow(int capable)
{
  if(!this->WindowId)
    {
    vtkOpenGLRenderWindow::SetStereoCapableWindow(capable);
    }
  else
    {
    vtkWarningMacro(<< "Requesting a StereoCapableWindow must be performed "
                    << "before the window is realized, i.e. before a render.");
    }
}


void vtkXOpenGLOffScreenRenderWindow::CreateAWindow(void)
  {
    XVisualInfo  *v;
    int width = ((this->Size[0] > 0) ? this->Size[0] : 300);
    int height = ((this->Size[1] > 0) ? this->Size[1] : 300);
    
    // get the default display connection 
    if(!this->DisplayId)
      {
	this->DisplayId = XOpenDisplay((char *)nullptr); 
	if(this->DisplayId == nullptr) 
	  {
	    vtkErrorMacro(<<"bad X server connection.\n");
	  }
      }

    // get visual info and set up pixmap buffer
    int attributeList[] = { GLX_RGBA, GLX_RED_SIZE, 1,  
			    GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, 
			    GLX_DEPTH_SIZE, 1, None };
    //std::clog << "glxChooseVis DPY=" << this->DisplayId
    //          << " SCRN=" << DefaultScreen(this->DisplayId) << std::endl;
    v = glXChooseVisual(this->DisplayId,
			DefaultScreen(this->DisplayId),
			attributeList);
    //std::clog << "Visual selected.\n";

    this->Internal->ContextId = glXCreateContext(this->DisplayId,v,0,GL_FALSE);
    /* create offscreen pixmap to render to (same depth as RootWindow) */
    this->pixmap = XCreatePixmap(this->DisplayId,
				 RootWindow(this->DisplayId,v->screen),
				 width,
				 height,
				 v->depth);
    //std::clog << "Depth= " << v->depth << std::endl;
    this->WindowId= glXCreateGLXPixmap(this->DisplayId,v,this->pixmap); 

    if(v)
      XFree(v);

    this->MakeCurrent();
    vtkDebugMacro(<< " glMatrixMode ModelView\n");
    glMatrixMode( GL_MODELVIEW );
    vtkDebugMacro(<< " zbuffer enabled\n");
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    vtkDebugMacro(" texture stuff\n");
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    // initialize blending for transparency
    vtkDebugMacro(<< " blend func stuff\n");
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable(GL_BLEND);
    glEnable( GL_NORMALIZE );
    glAlphaFunc(GL_GREATER,0);
    this->Mapped = 0; // if its mapped, then it trys windowgetattributes which fails!
    this->swapBuffers = 0;
    this->DoubleBuffer = 0;
  }

// Initialize the window for rendering.
void vtkXOpenGLOffScreenRenderWindow::WindowInitialize(void)
  {   
    CreateAWindow();
    MakeCurrent();

    // tell our renderers about us
    vtkRenderer* ren;
    for(this->Renderers->InitTraversal(); (ren = this->Renderers->GetNextItem());)
      {
        ren->SetRenderWindow(0);
        ren->SetRenderWindow(this);
      }
  }

// Initialize the rendering window.
void vtkXOpenGLOffScreenRenderWindow::Initialize(void)
  {
    // make sure we havent already been initialized 
    if(this->Internal->ContextId)
      { return; }
    else // now initialize the window 
      this->WindowInitialize();
  }

// Change the window to fill the entire screen.
void vtkXOpenGLOffScreenRenderWindow::SetFullScreen(int arg)
  {
    // Just Disable it
    return;
  }

// Set the preferred window size to full screen.
void vtkXOpenGLOffScreenRenderWindow::PrefFullScreen()
{

  // limit the off screen size
  this->Size[0] = 1280;
  this->Size[1] = 1024;
 
  // don't show borders 
  this->Borders = 0;
}

// Resize the window.
void vtkXOpenGLOffScreenRenderWindow::WindowRemap()
{
  short cur_light;

  /* first delete all the old lights */
  for(cur_light = GL_LIGHT0; cur_light < GL_LIGHT0+MAX_LIGHTS; cur_light++)
    {
    glDisable((GLenum)cur_light);
    }
    {
    glXDestroyContext( this->DisplayId, this->Internal->ContextId);
    // then close the old window 
    if(this->OwnWindow)
      {
      XDestroyWindow(this->DisplayId,this->WindowId);
      }
    }
  
  
  // set the default windowid 
  this->WindowId = this->NextWindowId;
  this->NextWindowId = (Window)nullptr;

  // configure the window 
  this->WindowInitialize();
}

// Begin the rendering process.
void vtkXOpenGLOffScreenRenderWindow::Start(void)
{
  // if the renderer has not been initialized, do so now
  if(!this->Internal->ContextId)
    {
    this->Initialize();
    }

  // set the current window 
  this->MakeCurrent();
}


// Specify the size of the rendering window.
void vtkXOpenGLOffScreenRenderWindow::SetSize(int x,int y)
{
  if((this->Size[0] != x)||(this->Size[1] != y))
    {
    this->Modified();
    this->Size[0] = x;
    this->Size[1] = y;
    }

  // if we arent mappen then just set the ivars 
  if(!this->Mapped)
    {
      return;
    }

  XResizeWindow(this->DisplayId,this->WindowId,x,y);
  XSync(this->DisplayId,False);

}



int vtkXOpenGLOffScreenRenderWindow::GetDesiredDepth()
{
  XVisualInfo *v;
  int depth = 0;
  
  // get the default visual to use 
  v = this->GetDesiredVisualInfo();
  
  if(v)
    {
    depth = v->depth;  
    XFree(v);
    }

  return depth;
}

// Get a visual from the windowing system.
Visual *vtkXOpenGLOffScreenRenderWindow::GetDesiredVisual ()
{
  XVisualInfo *v;
  Visual *vis=0;
  
  // get the default visual to use 
  v = this->GetDesiredVisualInfo();

  if(v)
    {
    vis = v->visual;  
    XFree(v);
    }
  
  return vis;  
}


// Get a colormap from the windowing system.
Colormap vtkXOpenGLOffScreenRenderWindow::GetDesiredColormap ()
{
  XVisualInfo *v;
  
  if(this->ColorMap) return this->ColorMap;
  
  // get the default visual to use 
  v = this->GetDesiredVisualInfo();

  if(v)
    {
    this->ColorMap = XCreateColormap(this->DisplayId,
                                     RootWindow( this->DisplayId, v->screen),
                                     v->visual, AllocNone ); 
    XFree(v);
    }

  return this->ColorMap;  
}

void vtkXOpenGLOffScreenRenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "ContextId: " << this->Internal->ContextId << "\n";
// #ifdef VTK_OPENGL_HAS_OSMESA
//   os << indent << "OffScreenContextId: " << this->Internal->OffScreenContextId << "\n";
// #endif
  os << indent << "Color Map: " << this->ColorMap << "\n";
  os << indent << "Display Id: " << this->GetDisplayId() << "\n";
  os << indent << "Next Window Id: " << this->NextWindowId << "\n";
  os << indent << "Window Id: " << this->GetWindowId() << "\n";
}

// the following can be useful for debugging XErrors
// When uncommented (along with the lines in MakeCurrent) 
// it will cause a segfault upon an XError instead of 
// the normal XError handler
//  extern "C" {int vtkXError(Display *display, XErrorEvent *err)
//  {
//  // cause a segfault
//    *(float *)(0x01) = 1.0;
//    return 1;
//  }}

// Tells if this window is the current OpenGL context for the calling thread.
bool vtkXOpenGLOffScreenRenderWindow::IsCurrent()
  {
    bool result=false;
    if(this->Internal->ContextId)
     { result=this->Internal->ContextId==glXGetCurrentContext(); }
    return result;
  }

void vtkXOpenGLOffScreenRenderWindow::MakeCurrent()
{
  // when debugging XErrors uncomment the following lines
  //  if(this->DisplayId)
//      {
//        XSynchronize(this->DisplayId,1);
//      }
//     XSetErrorHandler(vtkXError);

    {
    if(this->Internal->ContextId && ((this->Internal->ContextId != glXGetCurrentContext()) || this->ForceMakeCurrent))
      {
      glXMakeCurrent(this->DisplayId,this->WindowId,this->Internal->ContextId);
      this->ForceMakeCurrent = 0;
      }
    }
}

void vtkXOpenGLOffScreenRenderWindow::SetForceMakeCurrent()
{
  this->ForceMakeCurrent = 1;
}

int vtkXOpenGLOffScreenRenderWindowFoundMatch;

extern "C"
{
  Bool vtkXOpenGLOffScreenRenderWindowPredProc(Display *vtkNotUsed(disp), 
                                      XEvent *event, 
                                      char *arg)
  {
    Window win = (Window)arg;
    
    if(((reinterpret_cast<XAnyEvent *>(event))->window == win) &&
        ((event->type == ButtonPress)))
      {
      vtkXOpenGLOffScreenRenderWindowFoundMatch = 1;
      }

    return 0; 
  }
}

void *vtkXOpenGLOffScreenRenderWindow::GetGenericContext()
{
  static GC gc = (GC) nullptr;
  if(!gc) gc = XCreateGC(this->DisplayId, this->WindowId, 0, 0);
  return (void *) gc;
}

int vtkXOpenGLOffScreenRenderWindow::GetEventPending()
{
  XEvent report;
  
  vtkXOpenGLOffScreenRenderWindowFoundMatch = 0;
  XCheckIfEvent(this->DisplayId, &report, vtkXOpenGLOffScreenRenderWindowPredProc, 
                (char *)this->WindowId);
  return vtkXOpenGLOffScreenRenderWindowFoundMatch;
}

// Get the size of the screen in pixels
int *vtkXOpenGLOffScreenRenderWindow::GetScreenSize()
{
  // get the default display connection 
  if(!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay((char *)nullptr); 
    if(this->DisplayId == nullptr) 
      {
      vtkErrorMacro(<< "bad X server connection.\n");
      }
    else
      {
      this->OwnDisplay = 1;
      }
    }

  this->ScreenSize[0] = 
    DisplayWidth(this->DisplayId, DefaultScreen(this->DisplayId));
  this->ScreenSize[1] = 
    DisplayHeight(this->DisplayId, DefaultScreen(this->DisplayId));

  return this->ScreenSize;
}

// Get this RenderWindow's X display id.
Display *vtkXOpenGLOffScreenRenderWindow::GetDisplayId()
{
  // get the default display connection 
  if(!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay((char *)nullptr); 
    if(this->DisplayId == nullptr) 
      {
      vtkErrorMacro(<< "bad X server connection.\n");
      }
    this->OwnDisplay = 1;
    }
  vtkDebugMacro(<< "Returning DisplayId of " << (void *)this->DisplayId << "\n"); 

  return this->DisplayId;
}

// Get this RenderWindow's parent X window id.
Window vtkXOpenGLOffScreenRenderWindow::GetParentId()
{
  vtkDebugMacro(<< "Returning ParentId of " << (void *)this->ParentId << "\n");
  return this->ParentId;
}

// Get this RenderWindow's X window id.
Window vtkXOpenGLOffScreenRenderWindow::GetWindowId()
{
  vtkDebugMacro(<< "Returning WindowId of " << (void *)this->WindowId << "\n");
  return this->WindowId;
}

// Sets the parent of the window that WILL BE created.
void vtkXOpenGLOffScreenRenderWindow::SetParentId(Window arg)
{
//   if(this->ParentId)
//     {
//     vtkErrorMacro("ParentId is already set.");
//     return;
//     }
  
  vtkDebugMacro(<< "Setting ParentId to " << (void *)arg << "\n"); 

  this->ParentId = arg;
}

// Set this RenderWindow's X window id to a pre-existing window.
void vtkXOpenGLOffScreenRenderWindow::SetWindowId(Window arg)
{
  vtkDebugMacro(<< "Setting WindowId to " << (void *)arg << "\n"); 

  this->WindowId = arg;

  if(this->CursorHidden)
    {
    this->CursorHidden = 0;
    this->HideCursor();
    } 
}

// Set this RenderWindow's X window id to a pre-existing window.
void vtkXOpenGLOffScreenRenderWindow::SetWindowInfo(char *info)
{
  int tmp;
  
  // get the default display connection 
  if(!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay((char *)nullptr); 
    if(this->DisplayId == nullptr) 
      {
      vtkErrorMacro(<< "bad X server connection.\n");
      }
    else
      {
      this->OwnDisplay = 1;
      }
    }

  sscanf(info,"%i",&tmp);
 
  this->SetWindowId(tmp);
}

// Set this RenderWindow's X window id to a pre-existing window.
void vtkXOpenGLOffScreenRenderWindow::SetNextWindowInfo(char *info)
{
   int tmp;
   sscanf(info,"%i",&tmp);
 
   this->SetNextWindowId((Window)tmp);
}

// Sets the X window id of the window that WILL BE created.
void vtkXOpenGLOffScreenRenderWindow::SetParentInfo(char *info)
{
  int tmp;
  
  // get the default display connection 
  if(!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay((char *)nullptr); 
    if(this->DisplayId == nullptr) 
      {
      vtkErrorMacro(<< "bad X server connection.\n");
      }
    else
      {
      this->OwnDisplay = 1;
      }
    }

  sscanf(info,"%i",&tmp);
 
  this->SetParentId(tmp);
}

void vtkXOpenGLOffScreenRenderWindow::SetWindowId(void *arg)
{
  this->SetWindowId((Window)arg);
}
void vtkXOpenGLOffScreenRenderWindow::SetParentId(void *arg)
{
  this->SetParentId((Window)arg);
}

const char* vtkXOpenGLOffScreenRenderWindow::ReportCapabilities()
  {
    MakeCurrent();

    if(!this->DisplayId) 
      { return "display id not set"; }

    int scrnum = DefaultScreen(this->DisplayId);
    const char *serverVendor = glXQueryServerString(this->DisplayId, scrnum, GLX_VENDOR);
    const char *serverVersion = glXQueryServerString(this->DisplayId, scrnum, GLX_VERSION);
    const char *serverExtensions = glXQueryServerString(this->DisplayId, scrnum, GLX_EXTENSIONS);
    const char *clientVendor = glXGetClientString(this->DisplayId, GLX_VENDOR);
    const char *clientVersion = glXGetClientString(this->DisplayId, GLX_VERSION);
    const char *clientExtensions = glXGetClientString(this->DisplayId, GLX_EXTENSIONS);
    const char *glxExtensions = glXQueryExtensionsString(this->DisplayId, scrnum);
    const char *glVendor = (const char *) glGetString(GL_VENDOR);
    const char *glRenderer = (const char *) glGetString(GL_RENDERER);
    const char *glVersion = (const char *) glGetString(GL_VERSION);
    const char *glExtensions = (const char *) glGetString(GL_EXTENSIONS);

    std::ostringstream strm;
    strm << "server glx vendor string:  " << serverVendor << endl;
    strm << "server glx version string:  " << serverVersion << endl;
    strm << "server glx extensions:  " << serverExtensions << endl;
    strm << "client glx vendor string:  " << clientVendor << endl;
    strm << "client glx version string:  " << clientVersion << endl;
    strm << "client glx extensions:  " << clientExtensions << endl;
    strm << "glx extensions:  " << glxExtensions << endl;
    strm << "OpenGL vendor string:  " << glVendor << endl;
    strm << "OpenGL renderer string:  " << glRenderer << endl;
    strm << "OpenGL version string:  " << glVersion << endl;
    strm << "OpenGL extensions:  " << glExtensions << endl;
    strm << "X Extensions:  ";

    int n = 0;
    char **extlist = XListExtensions(this->DisplayId, &n);

    for(int i = 0; i < n; i++) 
      {
        if(i != n-1)
          { strm << extlist[i] << ", "; }
        else
          { strm << extlist[i] << endl; }
      }
    strm << ends;
    Capabilities= strm.str();
    return Capabilities.c_str();
  }

int vtkXOpenGLOffScreenRenderWindow::SupportsOpenGL()
  {
    MakeCurrent();
    if(!this->DisplayId) 
      { return 0; }

  int value = 0;
  XVisualInfo *v = this->GetDesiredVisualInfo();
  if(v)
    {
      glXGetConfig(this->DisplayId, v, GLX_USE_GL, &value);
    }

  return value;
}


int vtkXOpenGLOffScreenRenderWindow::IsDirect()
{
  MakeCurrent();
  if(!this->DisplayId || !this->Internal->ContextId)
    {
      return 0;
    }
  this->UsingHardware = glXIsDirect(this->DisplayId, 
                                    this->Internal->ContextId) ? 1:0;
  return this->UsingHardware;
}


void vtkXOpenGLOffScreenRenderWindow::SetWindowName(const char * cname)
{
  char *name = new char[ strlen(cname)+1 ];
  strcpy(name, cname);
  XTextProperty win_name_text_prop;

  vtkOpenGLRenderWindow::SetWindowName( name );

  if(this->Mapped)
    {
    if( XStringListToTextProperty( &name, 1, &win_name_text_prop ) == 0 )
      {
      XFree (win_name_text_prop.value);
      vtkWarningMacro(<< "Can't rename window"); 
      delete [] name;
      return;
      }
    
    XSetWMName( this->DisplayId, this->WindowId, &win_name_text_prop );
    XSetWMIconName( this->DisplayId, this->WindowId, &win_name_text_prop );
    XFree (win_name_text_prop.value);
    }
  delete [] name;
}


// Specify the X window id to use if a WindowRemap is done.
void vtkXOpenGLOffScreenRenderWindow::SetNextWindowId(Window arg)
{
  vtkDebugMacro(<< "Setting NextWindowId to " << (void *)arg << "\n"); 

  this->NextWindowId = arg;
}

void vtkXOpenGLOffScreenRenderWindow::SetNextWindowId(void *arg)
{
   this->SetNextWindowId((Window)arg);
}


// Set the X display id for this RenderWindow to use to a pre-existing 
// X display id.
void vtkXOpenGLOffScreenRenderWindow::SetDisplayId(Display  *arg)
{
  vtkDebugMacro(<< "Setting DisplayId to " << (void *)arg << "\n"); 

  this->DisplayId = arg;
  this->OwnDisplay = 0;

}
void vtkXOpenGLOffScreenRenderWindow::SetDisplayId(void *arg)
{
  this->SetDisplayId((Display *)arg);
  this->OwnDisplay = 0;
}

void vtkXOpenGLOffScreenRenderWindow::Render()
{
  XWindowAttributes attribs;

  // To avoid the expensive XGetWindowAttributes call,
  // compute size at the start of a render and use
  // the ivar other times.
  if(this->Mapped)
    {
    //  Find the current window size 
    XGetWindowAttributes(this->DisplayId, 
                                    this->WindowId, &attribs);

    this->Size[0] = attribs.width;
    this->Size[1] = attribs.height;
    }

  // Now do the superclass stuff
  this->vtkOpenGLRenderWindow::Render();
}

void vtkXOpenGLOffScreenRenderWindow::Finalize()
  {
    // clean and destroy window
    DestroyWindow();
  }



//----------------------------------------------------------------------------
void vtkXOpenGLOffScreenRenderWindow::HideCursor()
{
  static char blankBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  static XColor black = { 0, 0, 0, 0, 0, 0 };
 
  if(!this->DisplayId || !this->WindowId)
    {
    this->CursorHidden = 1;
    }
  else if(!this->CursorHidden)
    {
    Pixmap blankPixmap = XCreateBitmapFromData(this->DisplayId,
                                               this->WindowId,
                                               blankBits, 16, 16);
    
    Cursor blankCursor = XCreatePixmapCursor(this->DisplayId, blankPixmap,
                                             blankPixmap, &black, &black,
                                             7, 7);
    
    XDefineCursor(this->DisplayId, this->WindowId, blankCursor);
    
    XFreePixmap(this->DisplayId, blankPixmap);
    
    this->CursorHidden = 1;
    }
}

//----------------------------------------------------------------------------
void vtkXOpenGLOffScreenRenderWindow::ShowCursor()
{
  if(!this->DisplayId || !this->WindowId)
    {
    this->CursorHidden = 0;
    }
  else if(this->CursorHidden)
    {
    XUndefineCursor(this->DisplayId, this->WindowId);
    this->CursorHidden = 0;
    }
}                                  



// This probably has been moved to superclass.
void *vtkXOpenGLOffScreenRenderWindow::GetGenericWindowId()
{
  return (void *)this->WindowId;
}

void vtkXOpenGLOffScreenRenderWindow::SetCurrentCursor(int shape)
{
  if( this->InvokeEvent(vtkCommand::CursorChangedEvent,&shape) )
    {
    return;
    }
  this->Superclass::SetCurrentCursor(shape);
  if(!this->DisplayId || !this->WindowId)
    {
    return;
    }
  
  if(shape == VTK_CURSOR_DEFAULT)
    {
    XUndefineCursor(this->DisplayId,this->WindowId);
    return;
    }
      
  switch (shape)
    {
    case VTK_CURSOR_ARROW:
      if(!this->XCArrow)
        {
        this->XCArrow = XCreateFontCursor(this->DisplayId, XC_top_left_arrow);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCArrow);
      break;
    case VTK_CURSOR_SIZEALL:
      if(!this->XCSizeAll)
        {
        this->XCSizeAll = XCreateFontCursor(this->DisplayId, XC_fleur);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeAll);
      break;
    case VTK_CURSOR_SIZENS:
      if(!this->XCSizeNS)
        {
        this->XCSizeNS = XCreateFontCursor(this->DisplayId,  
                                           XC_sb_v_double_arrow);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNS);
      break;
    case VTK_CURSOR_SIZEWE:
      if(!this->XCSizeWE)
        {
        this->XCSizeWE = XCreateFontCursor(this->DisplayId, 
                                           XC_sb_h_double_arrow);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeWE);
      break;
    case VTK_CURSOR_SIZENE:
      if(!this->XCSizeNE)
        {
        this->XCSizeNE = XCreateFontCursor(this->DisplayId, 
                                           XC_top_right_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNE);
      break;
    case VTK_CURSOR_SIZENW:
      if(!this->XCSizeNW)
        {
        this->XCSizeNW = XCreateFontCursor(this->DisplayId, 
                                           XC_top_left_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNW);
      break;
    case VTK_CURSOR_SIZESE:
      if(!this->XCSizeSE)
        {
        this->XCSizeSE = XCreateFontCursor(this->DisplayId, 
                                           XC_bottom_right_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeSE);
      break;
    case VTK_CURSOR_SIZESW:
      if(!this->XCSizeSW)
        {
        this->XCSizeSW = XCreateFontCursor(this->DisplayId, 
                                           XC_bottom_left_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeSW);
      break;
    }
}
