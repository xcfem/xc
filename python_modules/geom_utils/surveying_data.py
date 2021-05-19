# -*- coding: utf-8 -*-

from __future__ import print_function
import geom
from geom_utils import XY_parametrized_3Dpoly as param
from geom_utils import acad_script_utils as script
import math
import os

#Points piquetage.
def dist2XYZ(pA,pB):
  Ax2= (pB[0]-pA[0])**2
  Ay2= (pB[1]-pA[1])**2
  Az2= (pB[2]-pA[2])**2
  return Ax2+Ay2+Az2

def dist(p,points):
  dMin= 1.0e12
  for key in points:
    pt= points[key]
    dMin= min(dMin,dist2XYZ(p,pt))
  return dMin

def distXYToPline(p,pline):
  p2d= geom.Pos2d(p[0],p[1])
  p0= pline[0]
  p02d= geom.Pos2d(p0[0],p0[1])
  d= p2d.dist(p02d)
  sz= len(pline)
  for i in range(1,sz):
    p1= pline[i]
    p12d= geom.Pos2d(p1[0],p1[1])
    s= geom.Segment2d(p02d,p12d)
    ds= s.dist(p2d)
    d= min(d,ds)
    p0= p1
    p02d= geom.Pos2d(p0[0],p0[1])
  return d

def getNumberedPoints(points,firstNumber,step= 1):
  retval={}
  count= fistNumber
  for p in points:
    retval[count]= p
    count+= step
  return retval

def getPoints(plines,spacement,step= 1):
  pointsPiquetage={}
  for key in plines:
    pmtz= param.XYParametrized3DPoly(plines[key])
    pts= pmtz.getPointsEveryXmetres(spacement)
    count= key
    for p in pts:
      dt= dist(p,pointsPiquetage)
      if(dt>1.0):
        pointsPiquetage[count]= p
        count+=step
  return pointsPiquetage

def getPointsAt(plines,factors,step= 1):
  pointsPiquetage={}
  for key in plines:
    pmtz= param.XYParametrized3DPoly(plines[key])
    pts= pmtz.getPointsAt(factors)
    count= key
    for p in pts:
      dt= dist(p,pointsPiquetage)
      if(dt>1.0):
        pointsPiquetage[count]= p
        count+=step
  return pointsPiquetage

def writeAcadScripts(pointsPiquetage,pointsFileName,textFileName):
  xMax= 0.0
  yMax= 0.0
  xMin= 1.0e15
  yMin= 1.0e15
  for key in pointsPiquetage:
    p= pointsPiquetage[key]
    xMax= max(p[0],xMax)
    yMax= max(p[1],yMax)
    xMin= min(p[0],xMin)
    yMin= min(p[1],yMin)

  Ax= xMax-xMin
  Ay= yMax-yMin
  diag= math.sqrt(Ax*Ax+Ay*Ay)
  hText= math.ceil(diag/100.0)
  print(xMax)
  print(xMin)
  print(diag)
  print(hText)
  fScriptDrawing= open(pointsFileName,"w")
  fScriptTableau= open(textFileName,"w")
  script.snap(fScriptTableau,1.0e-8)

  script.makeLayer(fScriptDrawing,"points_piquetage","_white")
  i= 1
  for key in pointsPiquetage:
    number= key
    p= pointsPiquetage[key]
    x= p[0]
    y= p[1]
    z= p[2]
    msg= 'P'+'{}'.format(number)
    script.point(fScriptDrawing,x,y,z)
    script.text(fScriptDrawing,x,y,'_left',hText,0.0,msg)
    xLabel= 0
    yLabel= -1.5*i*hText
    script.text(fScriptTableau,xLabel,yLabel,'_right',hText,0.0,msg)
    xLabel= 10.0*hText
    msg= '{:.3f}'.format(x)
    script.text(fScriptTableau,xLabel,yLabel,'_right',hText,0.0,msg)
    xLabel= 20.0*hText
    msg= '{:.3f}'.format(y)
    script.text(fScriptTableau,xLabel,yLabel,'_right',hText,0.0,msg)
    xLabel= 30.0*hText
    msg= '{:.3f}'.format(z)
    script.text(fScriptTableau,xLabel,yLabel,'_right',hText,0.0,msg)
    i+= 1
  fScriptTableau.close()
  fScriptDrawing.close()
  os.system("todos "+pointsFileName) 
  os.system("todos "+textFileName) 
