# -*- coding: utf-8 -*-
from __future__ import print_function
import scipy.interpolate
import math
from geom_utils import acad_script_utils as script
import os

class extrap1d(object):
  fn= None
  def check_x(self,x):
    sz= len(x)
    for i in range(1,sz):
      x0= x[i-1]
      x1= x[i]
      if(x1<=x0):
        print("problem at position: ", i, " in ", x)
  def __init__(self,x,y):
    self.check_x(x)
    self.fn= scipy.interpolate.interp1d(x, y)
   
  def __call__(self,x):
    retval= None
    if (x < self.fn.x[0]):
      Ax= self.fn.x[1]-self.fn.x[0]
      if Ax!=0.0:
        retval= self.fn.y[0]+(self.fn.y[1]-self.fn.y[0])/Ax*(x-self.fn.x[0])
      else:
        print("repeated vertices at the beginning; Ax= ", Ax)
    elif (x > self.fn.x[-1]):
      Ax= self.fn.x[-1]-self.fn.x[-2]
      if Ax!=0.0:
        retval= self.fn.y[-2]+(self.fn.y[-1]-self.fn.y[-2])/Ax*(x-self.fn.x[-2])
      else:
        print("repeated vertices at the end; Ax= ", Ax)
    else:
      retval= self.fn(x)
    return retval

class GuitarString:
  name= 'nil'
  xData= []
  yData= []
  fmt= '{:.3f}'
  def __init__(self,name,xData,yData):
    self.name= name
    self.xData= xData
    self.yData= yData
  def __len__(self):
    return min(len(self.xData),len(self.yData))
  def x(self,i):
    return self.xData[i]
  def y(self,i):
    return self.yData[i]
  def getInterpolation1d(self):
    return extrap1d(self.xData,self.yData)
  def getOffseted(self,name,offset):
    sz= len(self.xData)
    tmp=[]
    for i in range(0,sz):
      tmp.append(self.yData[i]+offset)
    return GuitarString(name,self.xData,tmp)
  def getDifference(self,name,other,x):
    fnA= self.getInterpolation1d()
    fnB= other.getInterpolation1d()
    tmp= []
    for xi in x:
      v= float(fnA(xi))-float(fnB(xi)) 
      tmp.append(self.fmt.format(v))
    return GuitarString(name,x,tmp)
  def getTexts(self,fScript,x):
    fn= self.getInterpolation1d()
    retval= []
    for xi in x:
      v= float(fn(xi)) 
      retval.append(self.fmt.format(v))
    return retval

def getGuitarStringFromPline3D(name,points):
  x= getAbscissasFromPline3d(points)
  y= getOrdinatesFromPline3d(points)
  return GuitarString(name,x,y)


class LongProfile:
  title= 'nil'
  dataLines= {}
  abscissaName= 'nil'
  ordinateNames= []
  rowOrder= []
  horizScale= 1/200.0
  vertScale= 1/20.0
  textScale= vertScale**2
  zHorizon= 940
  yMX= 0.0
  yMN= 1e9

  def __init__(self,x,nameY,y):
    sz= len(x)
    zeros= [0]*sz
    self.dataLines= {}
    self.ordinateNames= []
    self.rowOrder= []
    self.setAbscissaLine("distCum",x,x)
    px= self.getPartials()
    self.appendDummyLine("distPart",px)
    self.appendOrdinatesLine(nameY,x,y)

  def update(self,dl):
    self.dataLines.update({dl.name:dl})

  def appendDummyLine(self,name,data):
    x= self.dataLines[self.abscissaName].xData
    self.update(GuitarString(name,x,data))

  def appendDummyString(self,string):
    self.update(string)

  def setAbscissaLine(self,name,xData,yData):
    dl= GuitarString(name,xData,yData)
    self.update(dl)
    self.abscissaName= name

  def appendOrdinatesLine(self,name,xData,yData):
    self.appendOrdinatesString(GuitarString(name,xData,yData))

  def appendOrdinatesString(self,gString):
    self.update(gString)
    self.ordinateNames.append(gString.name)
    

  def getOrdinateInterpolation1d(self,name):
    return self.dataLines[name].getInterpolation1d()

  def getInterpolatedValues(self,name,x):
    fn= self.getOrdinateInterpolation1d(name)
    retval= []
    for xi in x:
      retval.append(fn(xi))
    return retval

  def getEnvelope(self,x):
    sz= len(x)
    yMax= [0]*sz
    for ordName in self.ordinateNames:
      y= self.getInterpolatedValues(ordName,x)
      for i in range(0,sz):
        yMax[i]= max(yMax[i],y[i])
        self.yMX= max(self.yMX,y[i])
        self.yMN= min(self.yMN,y[i])
    self.zHorizon= math.floor(self.yMN-(self.yMX-self.yMN))
    return yMax

  def getPartials(self):
    x= self.dataLines[self.abscissaName].xData
    retval= [0]
    sz= len(x)
    for i in range(1,sz):
      retval.append(x[i]-x[i-1])
    return retval

  def getDifference(self,newName,nameA,nameB):
    x= self.dataLines[self.abscissaName].xData
    lA= self.dataLines[nameA]
    lB= self.dataLines[nameB]
    return lA.getDifference(newName,lB,x)

  def getTrfPoint(self,x,y):
    newX= x*self.horizScale
    newY= (y-self.zHorizon)*self.vertScale
    return (newX,newY)

  def getTrfPoints(self,y):
    x= self.dataLines[self.abscissaName].xData
    retval= []
    sz= len(x)
    for i in range(0,sz):
      retval.append(self.getTrfPoint(x[i],y[i]))
    return retval

  def plotVertLines(self,fScript):
    script.makeLayer(fScript,"grid","_red")
    x= self.dataLines[self.abscissaName]
    y= self.getEnvelope(x.xData)
    pts= self.getTrfPoints(y)
    for p in pts:
      pA= [p[0],0.0]
      pB= p
      script.line2D(fScript,pA,pB)

  def plotOrdinates(self,fScript):
    x= self.dataLines[self.abscissaName]
    for ordName in self.ordinateNames:
      script.comment(fScript,ordName+" ordinates.")
      y= self.getInterpolatedValues(ordName,x.xData)
      pts= self.getTrfPoints(y)
      script.makeLayer(fScript,ordName,"_white")
      script.pline(fScript,pts)

  def plotTexts(self,fScript):
    script.makeLayer(fScript,"texts","_white")
    x= self.dataLines[self.abscissaName].xData
    h= (self.yMX-self.zHorizon)*self.vertScale
    h= math.ceil(h)*self.textScale
    y= -2*h
    if (len(self.rowOrder)==0):
      for key in self.dataLines:
         self.rowOrder.append(key)
    for key in self.rowOrder:
      script.comment(fScript,key+" numbers.")
      texts= self.dataLines[key].getTexts(fScript,x)
      i= 0
      for xi in x:
        pX= xi*self.horizScale
        pY= y
        script.text(fScript,pX,pY,"_right",h,0.0,texts[i])
        i+=1
      y-= 2*h
    #Horizon
    xLabelHorizon= h
    yLabelHorizon= h
    labelHorizon= "Horizon: {:.1f}".format(self.zHorizon) + " m"
    script.text(fScript,xLabelHorizon,yLabelHorizon,"_left",h,0.0,labelHorizon)
    #Labels for each line.
    script.comment(fScript,"labels.")
    xLabel= (x[0]-(x[1]-x[0]))*self.horizScale
    yLabel= -2*h
    for key in self.rowOrder:
      script.text(fScript,xLabel,yLabel,"_right",h,0.0,key)
      yLabel-= 2*h
    #Title.
    script.comment(fScript,"title.")
    xTitle= xLabel
    yTitle= yLabel-2*h
    script.text(fScript,xTitle,yTitle,"_left",2*h,0.0,self.title)
    #Scales.
    xScales= xTitle
    yScales= yTitle-2*h
    denomScaleHoriz= str(int(1.0/self.horizScale))
    denomScaleVert= str(int(1.0/self.vertScale))
    scaleLabel= '1:' + denomScaleHoriz + ' / 1:' + denomScaleVert
    script.text(fScript,xScales,yScales,"_left",h,0.0,scaleLabel)
    

  def acadScript(self,scriptFileName, dwgFileName):
    fScript= open(scriptFileName,"w")
    script.snap(fScript,0.0000001)
    self.plotVertLines(fScript)
    self.plotOrdinates(fScript)
    self.plotTexts(fScript)
    script.zoomExtents(fScript)
    script.save(fScript,dwgFileName)
    fScript.close()
    os.system("todos "+scriptFileName) 

def filterPointsFromPline3d(points,tol):
  p0= points[0]
  retval= [p0]
  for p in points:
    dist= getDistXY(p0,p)
    if(dist>tol):
      retval.append(p)
      p0= p
  return retval

def getDistXY(pA,pB):
  Ax2= (pB.x-pA.x)**2
  Ay2= (pB.y-pA.y)**2
  return math.sqrt(Ax2+Ay2)

def getDistXYZ(pA,pB):
  Ax2= (pB.x-pA.x)**2
  Ay2= (pB.y-pA.y)**2
  Az2= (pB.z-pA.z)**2
  return math.sqrt(Ax2+Ay2+Az2)

def getLongXYPline3d(points,dist):
  p0= points[0]
  retval= 0
  for p in points:
    retval+= getDistXY(p0,p)
    p0= p
  return distOrg

def getAbscissasFromPline3d(points):
  p0= points[0]
  distOrg= 0
  retval= []
  for p in points:
    dist= getDistXY(p0,p)
    distOrg+= dist
    retval.append(distOrg)
    p0= p
  return retval

def getOrdinatesFromPline3d(points):
  retval= []
  for p in points:
    retval.append(p.z)
  return retval

def getAbscissasFromPoints2d(points2d):
  retval= []
  for p in points2d:
    retval.append(p[0])
  return retval

def getOrdinatesFromPoints2d(points2d):
  retval= []
  for p in points2d:
    retval.append(p[1])
  return retval


def getPointsProfilAlongPline3d(points,spacement):
  x= getAbscissasFromPline3d(points)
  y= getOrdinatesFromPline3d(points)
  fn= scipy.interpolate.interp1d(x,y)
  xMax= x[-1]
  retval= []
  s= 0.0
  while (s<xMax):
    p= (s,fn(s))
    retval.append(p)
    s+= spacement
  p= (x[-1],y[-1])
  retval.append(p)
  return retval


def getProfileFromPline3D(points,name,tol=1e-4):
  pline= filterPointsFromPline3d(points,tol)
  x= getAbscissasFromPline3d(pline)
  y= getOrdinatesFromPline3d(pline)
  return LongProfile(x,name,y)

def getProfileFromPline3DEveryXmetres(points,name,space):
  points2d= getPointsProfilAlongPline3d(points,space)
  x= getAbscissasFromPoints2d(points2d)
  y= getOrdinatesFromPoints2d(points2d)
  return LongProfile(x,name,y)
