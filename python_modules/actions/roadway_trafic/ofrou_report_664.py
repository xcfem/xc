# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway trafic load models from OFROU report 664:
   "Charges de trafic actualisées pour les dalles de roulement en béton
   des ponts existants".'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2018, A_OO LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import xc_base
import geom

class WheelLoad(object):
  ''' Load of a wheel.
    
    :ivar position: position of the wheel
    :ivar load: load
  '''
  def __init__(self,pos,ld):
    self.position= pos
    self.load= ld

class LoadModel(object):
  ''' Roadway trafic load model OFROU report 664.
    
    :ivar wheelLoads: position and loads of each wheel
  '''

  def __init__(self,wLoads):
    self.wheelLoads= wLoads # Wheel positions and loads

  def getPositions(self):
    retval= list()
    for p in self.wheelLoads:
      retval.append(p.position)
    return retval
  
  def getLoads(self):
    retval= list()
    for l in self.wheelLoads:
      retval.append(l.load)
    return retval

  def getTotalLoad(self):
    retval= 0.0
    for l in self.wheelLoads:
      retval+= l.load
    return retval
    
  def getCentroid(self):
    '''Return the centroid of the loads.'''
    retvalPos= geom.Pos2d(0.0,0.0)
    totalLoad= 0.0
    for i in self.wheelLoads:
      retvalPos.x+= i.load*i.position.x
      retvalPos.y+= i.load*i.position.y
      totalLoad+= i.load
    if(totalLoad!=0.0):
      retvalPos.x/=totalLoad
      retvalPos.y/=totalLoad
    return retvalPos

  def getRelativePositions(self):
    '''Return the loads positions with respect to the loads centroid.'''
    centroidVector= self.getCentroid().getPositionVector()
    retval= list()
    for i in self.wheelLoads:
      pos= i.position-centroidVector
      retval.append(pos)
    return retval

  def getCenteredBoundary(self):
    retval= geom.BND2d()
    tmp= self.getRelativePositions()
    for p in tmp:
      retval.update(p)
    return retval
    
  def getRotatedPi(self):
    newLoads= list()
    for i in self.wheelLoads:
      newLoads.append(WheelLoad(geom.Pos2d(-i.position.x,-i.position.y),i.load))
    return LoadModel(newLoads)
      

CraneTruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(-3.17,1.0),216e3/2.0),
                                        WheelLoad(geom.Pos2d(-1.57,1.0),216e3/2.0),
                                        WheelLoad(geom.Pos2d(0.03,1.0),198e3/2.0),
                                        WheelLoad(geom.Pos2d(1.63,1.0),180e3/2.0),
                                        WheelLoad(geom.Pos2d(4.03,1.0),180e3/2.0),
                                        WheelLoad(geom.Pos2d(-3.17,-1.0),216e3/2.0),
                                        WheelLoad(geom.Pos2d(-1.57,-1.0),216e3/2.0),
                                        WheelLoad(geom.Pos2d(0.03,-1.0),198e3/2.0),
                                        WheelLoad(geom.Pos2d(1.63,-1.0),180e3/2.0),
                                        WheelLoad(geom.Pos2d(4.03,-1.0),180e3/2.0)])

Det11TruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(0.0,1.0),117e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,1.0),117e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,1.0),156e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),195e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),195e3/2.0),
                                        WheelLoad(geom.Pos2d(0.0,-1.0),117e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,-1.0),117e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,-1.0),156e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),195e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),195e3/2.0)])

Det12TruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(0.0,1.0),81e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,1.0),81e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,1.0),108e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),135e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),135e3/2.0),
                                        WheelLoad(geom.Pos2d(0.0,-1.0),81e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,-1.0),81e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,-1.0),108e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),135e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),135e3/2.0)])


Det21TruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(0.0,1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,1.0),120e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),150e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),150e3/2.0),
                                        WheelLoad(geom.Pos2d(0.0,-1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,-1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,-1.0),120e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),150e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),150e3/2.0)])

Det22TruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(0.0,1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),112.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),112.5e3/2.0),
                                        WheelLoad(geom.Pos2d(0.0,-1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,-1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,-1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),112.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),112.5e3/2.0)])
