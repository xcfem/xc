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
// YieldSurface_BC2D.cpp: implementation of the YieldSurfaceBC class.
//
//////////////////////////////////////////////////////////////////////

#include "YieldSurface_BC2D.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include "utility/matrix/Matrix.h"
#define modifDebug  0
#define returnDebug 0
#define transDebug  0
#define driftDebug  0

#include <utility/recorder/response/MaterialResponse.h>

double XC::YieldSurface_BC2D::error(1.0e-6);
XC::Vector XC::YieldSurface_BC2D::v6(6);
XC::Vector XC::YieldSurface_BC2D::T2(2);
XC::Vector XC::YieldSurface_BC2D::F2(2);
XC::Vector XC::YieldSurface_BC2D::g2(2);
XC::Vector XC::YieldSurface_BC2D::v2(2);
XC::Vector XC::YieldSurface_BC2D::v4(4);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::YieldSurface_BC2D::YieldSurface_BC2D(int tag, int classTag, double xmax, double ymax, YS_Evolution &model)
  :YieldSurface_BC(tag, classTag, model, xmax, ymax)
  {
    status_hist = -1; //elastic

    fx_hist = 0;
    fy_hist = 0;

    offset     = 0.01;
    increment  = 0.022;

    state = 0;
  }

const XC::Vector &XC::YieldSurface_BC2D::getExtent(void)
  {
    v4(0) = xPos;
    v4(1) = xNeg;
    v4(2) = yPos;
    v4(3) = yNeg;

    return v4;
  }


//////////////////////////////////////////////////////////////////////
// Transformation
//////////////////////////////////////////////////////////////////////

// Use this function to initialize other stuff
void XC::YieldSurface_BC2D::setTransformation(int xDof, int yDof, int xFact, int yFact)
  {
    this->XC::YieldSurface_BC::setTransformation(xDof, yDof, xFact, yFact);

    this->setExtent();
    if(xPos == 0 && yPos == 0 && xNeg ==0 && yNeg == 0)
    {
             std::cerr << "WARNING - YieldSurface_BC2D - surface extent not set correctly\n";
    }

    if(xPos == 0 || xNeg == 0)
            std::cerr << "Error - YieldSurface_BC2D no X extent\n";

    ////////////////////////////////////////////////////////
    // Next set the 'a' and 'b' for the internal quad
    ////////////////////////////////////////////////////////

double x1, y1, x2, y2;

    // QUAD 1
    x1 = 0; y1 = yPos - offset ; x2 = xPos - offset; y2 = 0;
    a1 = (y1 - y2)/(x1 - x2);
    b1 = y1 - a1*x1;

    // QUAD 2
    x1 = 0; y1 = yPos - offset; x2 = xNeg + offset; y2 = 0;
    a2 = (y1 - y2)/(x1 - x2);
    b2 = y1 - a2*x1;

    // QUAD 3
    x1 = 0; y1 = yNeg + offset; x2 = xNeg + offset; y2 = 0;
    a3 = (y1 - y2)/(x1 - x2);
    b3 = y1 - a3*x1;

    // QUAD 4
    x1 = 0; y1 = yNeg + offset; x2 = xPos - offset; y2 = 0;
    a4 = (y1 - y2)/(x1 - x2);
    b4 = y1 - a4*x1;

  }


//////////////////////////////////////////////////////////////////////
// Overridden methods
//////////////////////////////////////////////////////////////////////

int XC::YieldSurface_BC2D::getState(int stateInfo)
  {
    if(stateInfo == this->StateLoading)
            return isLoading;
    else
            return -1;
  }

int XC::YieldSurface_BC2D::commitState(XC::Vector &force)
  {
    XC::YieldSurface_BC::commitState(force);
    status_hist = this->getTrialForceLocation(force);
//    std::cerr << "XC::YieldSurface_BC2D::commitState(..), status = " << status_hist << std::endl;

//        std::cerr << "XC::YieldSurface_BC2D::commitState " << getTag() <<" - force location: " << status_hist << std::endl;
//        std::cerr << " FORCE = " << force;
    if(status_hist > 0)
      {
    std::cerr << "WARNING - XC::YieldSurface_BC2D::commitState(..) [" << getTag()<<"]\n";
    std::cerr << "Can't commit with force outside the surface\n";
    std::cerr << "\a";
      }

    double driftOld = this->getDrift(fx_hist, fy_hist);
    double driftNew = this->getTrialDrift(force);
    isLoading = 0;
    if(status_hist >= 0)
    {
            isLoading = 1;
    }
    else
    {
            if(driftOld < driftNew) // drift will be negative
                    isLoading = 1;
    }
            
    //hModel->commitState(status_hist);
    hModel->commitState();

    toLocalSystem(force, fx_hist, fy_hist, true);
    hModel->toOriginalCoord(fx_hist, fy_hist);
    
    // std::cerr << "yPos = " << yPos << ", fy = " << fy_hist << std::endl;
    if(fy_hist/yPos > 0.85)
            hModel->setDeformable(true);
    else
            hModel->setDeformable(false); 

    gx_hist = 0;
    gy_hist = 0;

    if(status_hist==0)
    {
            // double fx = fx_hist;
            // double fy = fy_hist;

            // hModel->toOriginalCoord(fx, fy);
            getGradient(gx_hist, gy_hist, fx_hist, fy_hist);
    }

    return 0;
  }


int XC::YieldSurface_BC2D::update(int flag)
  {
    return hModel->update(flag);
  }


int XC::YieldSurface_BC2D::revertToLastCommit(void)
  {
    hModel->revertToLastCommit();
    return 0;
  }

// assuming in original coordinates, return value does not account
// for any isotropic factor
XC::Vector& XC::YieldSurface_BC2D::translationTo(XC::Vector &f_new, XC::Vector &f_dir)
  {
    double x1 = f_dir(0);
    double y1 = f_dir(1);

    double x2 = f_new(0);
    double y2 = f_new(1);

    // first find the point to interpolate to
    bool is_hardening = true;
    state = 1;
    
    double hi = getDrift(x2, y2);
            if(hi < 0)
            {
                    is_hardening = false;
                    state = -1;
            }
    if(fabs(hi) < 1e-12) state = 0;

//        std::cerr << "Drift New = " << hi <<endl;
//        std::cerr << "F new = " << f_new;
    
    hi = 5*fabs(hi); //approx half to interpolate - that didn't work for all cases
    // changed from factor of 2 to 5
    // radial/normal evolution -> no problem
    // for bounding surface, dir. of evolution with hardening may cause the
    // interpolation end point (xi, yi) to be out side

    double h = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
    double c = hi/h;
    double sign= -1.0;

    if(c > 1.0)
    {
            std::cerr << "oops - XC::YieldSurface_BC2D::translationTo - c > 1.0 \n";
            c = 1.0;
    }

    if(!is_hardening)
            sign = 1.0;

//        double xi = x2 + sign*c*fabs(x2 - x1);
//        double yi = y2 + sign*c*fabs(y2 - y1); 
    double xi = x2 + sign*c*(x2 - x1);
    double yi = y2 + sign*c*(y2 - y1);
        
    if(is_hardening) // Fi inside, Fnew outside
    {
        double t = interpolate(xi, yi, x2, y2);
        T2(0) = (1 - t)*(x2 - xi);
        T2(1) = (1 - t)*(y2 - yi);
    }
    else // Fnew inside, Fi outside
    {
        double t = interpolate(x2, y2, xi, yi);
        T2(0) = t*(x2 - xi);
        T2(1) = t*(y2 - yi);
    }

//        std::cerr << "F New = " << f_new;
//        std::cerr << "F dir = " << f_dir;
//        std::cerr << "Translation vector = " << v2;
    
    return T2;
  }

// magPlasticDefo is based on incremental force
// we always modify the surface only if the last conv force was on the
// surface, otherwise we'll just set to surface if it shoots through

// return value >  0 --> surface is expanding
// return value == 0 --> surface unchanged
// return value <  0 --> surface is shrinking
int XC::YieldSurface_BC2D::modifySurface(double magPlasticDefo, XC::Vector &Fsurface, XC::Matrix &G, int flag)
  {

// check 1
    if( this->getTrialForceLocation(Fsurface) !=0)
    {
            std::cerr << "Can't modify surface with Force Location = " << getTrialForceLocation(Fsurface) << std::endl;
            return 0;
    }


// check 2
    if(magPlasticDefo < 0)
    {
            std::cerr << "YieldSurface_BC2D::modifySurface(..) \n";
            std::cerr << "Warning -   magPlasticDefo < 0 " << magPlasticDefo << "\n";
            // magPlasticDefo = 0;
            return 0;
    }


    double fx_def, fy_def, gx, gy;

    toLocalSystem(Fsurface, fx_def, fy_def, true);
    // set to ys coordinates
    toLocalSystem(G, gx, gy, false, true);

    F2(0) = fx_def;
    F2(1) = fy_def;
    g2(0) = gx;
    g2(1) = gy;

    hModel->evolveSurface(this, magPlasticDefo, g2, F2, flag);
    
    /*double  isotropicFactor      = hModel->getTrialIsotropicFactor(0);
    double  isotropicFactor_hist = hModel->getCommitIsotropicFactor(0);

    int res;
    if( fabs (fabs(isotropicFactor) - fabs(isotropicFactor_hist)) < 1e-13) // remains same
    {
            res = 0;
    }
    else if(isotropicFactor > isotropicFactor_hist) // surface is expanding
    {
             res = 1;
    }
    else if(isotropicFactor < isotropicFactor_hist)
    {
            res = -1;
    }
    else
            std::cerr << "ModifySurface - this condition should not happen\n";
    */
    return state;

  }


void XC::YieldSurface_BC2D::getCommitGradient(XC::Matrix &G)
  {
double gx = gx_hist;
double gy = gy_hist;

    toElementSystem(G, gx, gy, false, true);
  }

void XC::YieldSurface_BC2D::getTrialGradient(XC::Matrix &G, XC::Vector &force)
  {
double gx, gy, fx, fy;



    toLocalSystem(force, fx, fy, true);
    hModel->toOriginalCoord(fx, fy);

//        double drift = getDrift(fx, fy);
//        std::cerr << "XC::YieldSurface_BC2D::getTrialGradient  trial_drift: " << this->getTrialDrift(force)
//                 << "  drift: " << drift << std::endl;
//        std::cerr << "----> calling getGradient " << std::endl;

    getGradient(gx, gy, fx, fy);
//        std::cerr << "<---- done calling getGradient " << std::endl;

    toElementSystem(G, gx, gy, false, true);
  }


double XC::YieldSurface_BC2D::getTrialDrift(XC::Vector &force)
  {
double fx, fy;
    toLocalSystem(force, fx, fy, true);
    hModel->toOriginalCoord(fx, fy);
double drift = getDrift(fx, fy);

    return drift;
  }


// used by the element
int XC::YieldSurface_BC2D::getTrialForceLocation(XC::Vector &force)
  {
double drift = this->getTrialDrift(force);
            return this->forceLocation(drift);
  }

int XC::YieldSurface_BC2D::getCommitForceLocation()
  {
    return status_hist;
  }

// used by the yield surface
// here we decide on what we define as outside, inside
// and on the surface
// -1 -> inside
//  0 -> within
// +1 -> outside
int  XC::YieldSurface_BC2D::forceLocation(double drift)
  {
double tolNeg = 0.00;
double tolPos = 1e-5;

int           status = -2;

    // first get rid of -1e-13, etc
    if(fabs(drift) < 1e-7)
            drift = 0;

    if(drift <  -tolNeg)
    {
            status = -1; //inside
    }
    else if(drift >= -tolNeg && drift <= tolPos) // on the surface
    {
             status = 0;
    }
    else if(drift > tolPos) // outside the surface
    {
             status = 1;
    }
    else
    {
             std::cerr << "XC::YieldSurface_BC2D::forceLocation(double drift) - this condition not possible\n";
             std::cerr << "\a";
    }

    return status;
  }

void XC::YieldSurface_BC2D::addPlasticStiffness(XC::Matrix &K)
  {
XC::Vector v2 = hModel->getEquiPlasticStiffness();

       v6.Zero();
double kpX =  v2(0);
double kpY =  v2(1);

//void toElementSystem(XC::Vector &eleVector, double &x, double &y);
      toElementSystem(v6, kpX, kpY, false, false);

      for(int i=0; i<6; i++)
      {
        K(i,i) += v6(i);
      }

  }


double XC::YieldSurface_BC2D::getDrift(double x, double y)
  {

double sdrift = getSurfaceDrift(x, y);

// if sdrift > 0, trust it to be outside but if
// sdrift < 0, the point may be either inside
// or outside.  Reason is that surfaces close
// on themselves nicely, but may deform or
// open up while expanding.

//        if(sdrift > 0)
//                return sdrift;

// Now sdrift < 0, if it is inside the internal
// quad, sdrift is correct

double R_xy = sqrt(x*x + y*y);
//double x_in, y_in, R_in; //internal
double x0, y0, R0; // intersection

// find the point where the line joining 0,0 and x,y
// intersects the internal quad -> x_in, y_in (R_in)

    if(x!=0)
      {
        double x1 =0, y1 = 0;
        double x2 = x, y2 = y;
        double a_ext = (y1 - y2)/(x1 - x2);
        double b_ext =  y1 - a_ext*x1;
        double a_int= 0.0, b_int= 0.0;

        // y = a_ext*x +b_ext
        if(x>0 && y>=0)
          {
            a_int = a1;
            b_int = b1;
          }
        else if(x < 0 && y >= 0)
          {
            a_int = a2;
            b_int = b2;
          }
        else if(x<0 && y<=0)
          {
            a_int = a3;
            b_int = b3;
          }
        else if(x > 0 && y <= 0)
          {
            a_int = a4;
            b_int = b4;
          }
        else  // happened to be
          std::cerr << "XC::YieldSurface_BC2D::getDrift(..) - condition not possible, x = "
                    << x << ", y = " << y << std::endl;

        if(driftDebug)
          {
             std::cerr << "Equation Internal: a = " << a_int << ", b = " << b_int << "\n";
             std::cerr << "Equation External: a = " << a_ext << ", b = " << b_ext << "\n";
          }
        x0 = -(b_int - b_ext)/(a_int - a_ext);
        y0 = a_int*x0 + b_int;
      }
    else // x = 0
      {
        if(y >= 0)
          {
            x0 = 0;
            y0 = yPos - offset;
          }
        else
          {
            x0 = 0;
            y0 = yNeg + offset;
          }
      }

    R0 = sqrt(x0*x0 + y0*y0);

    if(driftDebug)
      {
        std::cerr << "R_xy = " << R_xy << " (x= " << x << ", y= " << y << "), R0 = " << R0;
        std::cerr << " (x0= " << x0 << ", y0= " << y0 << ")\n";
      }

    // If the R_xy < R0, then the point is inside the
    // internal quad, sdrift is correct
    if(R_xy < R0)
      {
        if(driftDebug)
          {
            std::cerr << " R_xy < R0, returning sdrift " << sdrift << "\n";
            std::cerr << "\a";
          }
        return sdrift;
      }
    // Now the point can be between the inner quad and the ys
    // or outside the surface

    if(R0 == 0)
      std::cerr << "ERROR: XC::YieldSurface_BC2D::getDrift(..) - R0 = 0 (yPos="<<yPos<<", yNeg="<<yNeg<<"\n";

    double delx = (x0/R0)*increment; // increment already defined
    double dely = (y0/R0)*increment;

    double xOld, yOld, xNew, yNew;
    double xi, yi, Ri; //incremental

    xOld = x0;
    yOld = y0;

    int count = 0;
    while(1)
      {
        Ri = sqrt(xOld*xOld + yOld*yOld);

        // check if point is between the inner quad and
        // the yield surface
        if(R_xy < Ri)
          {
                    if(driftDebug)
                    {
                            std::cerr << " R_xy < Ri, returning sdrift " << sdrift << "\n";
                            std::cerr << "\a";
                    }
                    return sdrift;
            }
            xNew = xOld + delx;
            yNew = yOld + dely;

            if(getSurfaceDrift(xNew, yNew) > 0) // we just crossed the surface
            {
                    double t = interpolateClose(xOld, yOld, xNew, yNew);
                    xi =  xOld + t*delx;
                    yi =  yOld + t*dely;

                    Ri = sqrt(xi*xi + yi*yi);

                    if(driftDebug)
                    {
                            std::cerr << " Set to surface at xi = " << xi << ", yi = " << yi << ", Ri = " << Ri << "\n";
                            std::cerr << " Returning drift " << R_xy - Ri << "\n";
                            std::cerr << "\a";
                    }
                    return (R_xy - Ri);
            }

            xOld = xNew;
            yOld = yNew;
            count++;
            if(count > 100)
            {
                    std::cerr << "ERROR: XC::YieldSurface_BC2D::getDrift(..) - not converging\n";
                    std::cerr << "\a";
            }

    }

    std::cerr << "XC::YieldSurface_BC2D::getDrift(..) - should not reach here\n";
    std::cerr << "\a";

    return sdrift;
  }




//////////////////////////////////////////////////////////////////////
// Other
//////////////////////////////////////////////////////////////////////

void XC::YieldSurface_BC2D::customizeInterpolate(double &xi, double &yi, double &xj, double &yj)
  {
double yCheck = yNeg;

    if(yj > 0)
            yCheck = yPos;

    if(fabs(yj) > fabs(yCheck)) // switch to radial - whatever the case
    {
            xi = 0;
            yi = 0;
    }

  }

double        XC::YieldSurface_BC2D::interpolate(double xi, double yi, double xj, double yj)
  {
    this->customizeInterpolate(xi, yi, xj, yj);

//check
double di = getDrift(xi, yi);
double dj = getDrift(xj, yj);
    
 if(di > 0 && fabs(di) < 1e-7)
   return 0;
 if(dj < 0 && fabs(dj) < 1e-7)
   return 1;

 if( di > 0)
   {
             std::cerr << "ERROR - XC::YieldSurface_BC2D::interpolate(xi, yi, xj, yj)\n";
            std::cerr << "point 1 is outside\n";
            std::cerr << xi << "," << yi << "  " << xj << "," << yj << " : "<< di<<"\n";
            std::cerr << "\a";
            return 0;
    }
    else if(dj <0 )
    {
             std::cerr << "ERROR - XC::YieldSurface_BC2D::interpolate(xi, yi, xj, yj)\n";
            std::cerr << "point 2 is inside\n";
            std::cerr << xi << "," << yi << "  " << xj << "," << yj << " : "<< dj<<"\n";
            hModel->Print(std::cerr);
            std::cerr << "\a";
            return 0;
    }

double tr, tu, tl, dtu, dtl, dtr=100;
double dy = yj - yi;
double dx = xj - xi;
int count = 0;
    tu = 1; tl =0;

    //double d = getDrift(xi, yi, false);
    //if(d>0) std::cerr << "WARNING - XC::Orbison2D::interpolate, Drift inside > 0 (" << d << ")\n";

    while(fabs(dtr) > 1.0e-7)
    {
            count++;
            if(count > 1000)
            {
                    std::cerr << "YieldSurface_BC2D::Interpolate()-> Error: Unable to converge\n";
                    std::cerr << "xi, yi: " << xi << ","<< yi << "\t xj, yj: " << xj << "," << yj << "\n";
                    std::cerr << "Drift Point j = " << dj << "\n";
                    hModel->Print(std::cerr);
                    std::cerr << "\a";
                    return 1;
            }

            dtl = getDrift(xi + tl*dx, yi + tl*dy);
            dtu = getDrift(xi + tu*dx, yi + tu*dy);

            tr        =        tu - (  dtu*(tl - tu)/(dtl - dtu)  );
            dtr = getDrift(xi + tr*dx, yi + tr*dy);

            if(dtr >= 0)
            {
                    if(dtu >= 0)
                            tu = tr;
                    else
                            tl = tr;
            }
            else
            {
                    if(dtu < 0)
                            tu = tr;
                    else
                            tl = tr;
            }

    }// while
    //std::cerr << "std::cerr for iterpolation = " << count << "\n"; 5 ~ 15
    return tr;
  }

double        XC::YieldSurface_BC2D::interpolateClose(double xi, double yi, double xj, double yj)
  {

    //check
    const double di = getSurfaceDrift(xi, yi);
    const double dj = getSurfaceDrift(xj, yj);
    if(di>0)
      {
             std::cerr << "ERROR - XC::YieldSurface_BC2D::interpolateClose(xi, yi, xj, yj)\n";
            std::cerr << "point 1 is outside\n";
            std::cerr << xi << "," << yi << "  " << xj << "," << yj << " : "<< di<<"\n";
            std::cerr << "\a";
            return 0;
      }
    else if(dj<0)
      {
             std::cerr << "ERROR - XC::YieldSurface_BC2D::interpolateClose(xi, yi, xj, yj)\n";
            std::cerr << "point 2 is inside\n";
            std::cerr << xi << "," << yi << "  " << xj << "," << yj << " : "<< dj<<"\n";
            hModel->Print(std::cerr);
            std::cerr << "\a";
            return 0;
      }

    double tr, dtu, dtl, dtr=100;
    const double dy = yj - yi;
    const double dx = xj - xi;
    int count = 0;
    double tu= 1; double tl= 0;

    //double d = getDrift(xi, yi, false);
    //if(d>0) std::cerr << "WARNING - XC::Orbison2D::interpolate, Drift inside > 0 (" << d << ")\n";

    while(fabs(dtr) > 1.0e-7)
      {
            count++;
            if(count > 1000)
              {
                std::cerr << "YieldSurface_BC2D::InterpolateClose()-> Error: Unable to converge\n";
                std::cerr << "xi, yi: " << xi << ","<< yi << "\t xj, yj: " << xj << "," << yj << "\n";
                hModel->Print(std::cerr);
                std::cerr << "\a";
                return 1;
              }

            dtl = getSurfaceDrift(xi + tl*dx, yi + tl*dy);
            dtu = getSurfaceDrift(xi + tu*dx, yi + tu*dy);

            tr        =        tu - (  dtu*(tl - tu)/(dtl - dtu)  );
            dtr = getSurfaceDrift(xi + tr*dx, yi + tr*dy);

            if(dtr >= 0)
            {
                    if(dtu >= 0)
                            tu = tr;
                    else
                            tl = tr;
            }
            else
            {
                    if(dtu < 0)
                            tu = tr;
                    else
                            tl = tr;
            }

    }// while
    //std::cerr << "std::cerr for iterpolation = " << count << "\n"; 5 ~ 15
    return tr;
  }

double XC::YieldSurface_BC2D::setToSurface(XC::Vector &force, int algoType, int color)
  {
    double x2, y2;
    double xi, yi, xj, yj;
    double dx, dy, t;
    double x, y;
    if(returnDebug)
      {
    std::cerr << "YieldSurface_BC2D::setToSurface(XC::Vector &force, int algoType)\n";
    std::cerr << "Element system force = " << force << "\n";
      }

    // if force point is already on surface,
    // no need to proceed further
    if(getTrialForceLocation(force) == 0)
      return 0;

    toLocalSystem(force, x2, y2, true);
    xj = x2;
    yj = y2;

    hModel->toOriginalCoord(xj, yj);

    if(color != 0)
      {
        std::cerr << "Modificado LCPT oct 2006." << std::endl;
//             theView->clearImage();
//             this->displaySelf(*theView, 1, 1);
//             theView->startImage();
//             this->displayForcePoint(false, xj, yj, color);
      }


    if(returnDebug)
      {
    std::cerr << "Local system force - " << "fx = " << x2 << ",\tfy = " << y2 << "\n";
    std::cerr << "toOriginalCoord    - " << "fx = " << xj << ",\tfy = " << yj << "\n";
      }

    switch(algoType)
      {
    case 0: //dFReturn:
                    {
                            xi = fx_hist;
                            yi = fy_hist;
                            // hModel->toOriginalCoord(xi, yi); - stored as that

                            break;
                    }

                    case 1: //RadialReturn:
                    {
                            xi = 0;
                            yi = 0;

                            break;

                    }

                    case 2: //ConstantXReturn:
                    {
                            xi = xj;
                            yi = 0;                                         //if point is outside the ys

                            if(getDrift(xj, yj) < 0)         //point is inside the ys
                            {
                              //std::cerr << "ConstantXReturn - Point is inside: " << getDrift(xj, yj) << "\n";
                                    if(yj < 0)                                //point is below x-axis
                                            yj = yj - 1;
                                    else
                                            yj = yj + 1;
                            }
                            break;
                    }


                    case 3: //ConstantYReturn:
                    {
                            xi = 0;                                         //if point is outside the ys
                            yi = yj;

                            if(getDrift(xj, yj) < 0)         //point is inside the ys
                            {
                                //std::cerr << " ConstantYReturn - Point Inside\n"; //
                                    if(xj < 0)                                //point is left of y-axis
                                            xj = xj - 1;
                                    else
                                            xj = xj + 1;
                            }
                            break;
                    }


                    default:
                    {
                            std::cerr << "YieldSurface_BC2D: Method not implemented yet\n";
                            xi = 0; yi = 0; //revert to radial return
                            break;
                    }
            }//switch

            dx = xj - xi;
            dy = yj - yi;

            t =  interpolate(xi, yi, xj, yj);
            x =  xi + t*dx;
            y =  yi + t*dy;

            if(color != 0)
              {
                std::cerr << "Modificado LCPT oct 2006." << std::endl;
//                         this->displayForcePoint(false, x, y, color);
//                         theView->doneImage();
//                         std::cerr << "\a";
              }

            hModel->toDeformedCoord(x, y);

            toElementSystem(force, x, y, true);

        return t;
  }
