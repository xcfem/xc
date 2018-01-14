# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway trafic load models from OFROU (Switzerland federal highway
   authority) report 664:
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
    :ivar vehicleBoundary: polygon without uniform load around the vehicle.
  '''

  def __init__(self,wLoads, vBoundary= None):
    self.wheelLoads= wLoads # Wheel positions and loads
    self.vehicleBoundary= vBoundary
    
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

  def getLoadRelativePositions(self):
    '''Return the loads positions with respect to the loads centroid.'''
    centroidVector= self.getCentroid().getPositionVector()
    retval= list()
    for i in self.wheelLoads:
      pos= i.position-centroidVector
      retval.append(pos)
    return retval

  def normalize(self):
    '''Sets the positions with respect to the loads centroid.'''
    dispModulus= self.getCentroid().getPositionVector().getModulo()
    if(dispModulus>1e-4):
      normalizedPositions= self.getLoadRelativePositions()
      if(self.vehicleBoundary):
        normalizedVehicleBoundary= self.getVehicleBoundaryRelativePositions()
        self.vehicleBoundary= normalizedVehicleBoundary
      for (old,new) in zip(self.wheelLoads,normalizedPositions):
        old.position= new
    

  def getVehicleBoundaryRelativePositions(self):
    '''Return the vehicle boundary positions with respect to
       the loads centroid.'''
    centroidVector= self.getCentroid().getPositionVector()
    retval= list()
    for p in self.vehicleBoundary:
      retval.append(p-centroidVector)
    return retval
  
  def getCenteredLoadBoundary(self):
    '''Return the boundary of the wheel loads with respect to
       the load centroid.'''
    retval= geom.BND2d()
    tmp= self.getLoadRelativePositions()
    for p in tmp:
      retval.update(p)
    return retval

  def getCenteredVehicleBoundary(self):
    '''Return the boundary of the vehicle with respect to
       the load centroid.'''
    retval= geom.Poligono2d()
    tmp= self.getVehicleBoundaryRelativePositions()
    for p in tmp:
      retval.agregaVertice(p)
    return retval

  def getRotatedPi(self):
    '''Return the load model rotated 180 degrees (pi radians).'''
    newLoads= list()
    for i in self.wheelLoads:
      newLoads.append(WheelLoad(geom.Pos2d(-i.position.x,-i.position.y),i.load))
    newVehicleBoundary= None
    if(self.vehicleBoundary):
      newVehicleBoundary= list()
      for p in self.vehicleBoundary:
        newVehicleBoundary.append(geom.Pos2d(-p.x,-p.y))
    return LoadModel(newLoads,newVehicleBoundary)

class VehicleLoad(object):
  '''Position of a load model in the structure.

    :ivar loadModel: load model that corresponds to the vehicle.
    :ivar ref2d3d: position and orientation of the vehicle in the structure.
  '''
  def __init__(self,lModel, rfSys):
    '''Constructor:

       :param lModel: load model that corresponds to the vehicle.
       :param rfSys: position and orientation of the vehicle in the structure.
    '''
    self.loadModel= lModel
    self.loadModel.normalize()
    self.refSys= rfSys
    
  def getCentroid(self):
    '''Return the centroid of the loads.'''
    return self.refSys.getOrg()

  def getGlobalPositions(self,localPositions):
    '''Return the corresponding global positions.'''
    retval= list()
    for p in localPositions:
      p3D= self.refSys.getPosGlobal(p)
      retval.append(p3D)
    return retval
  
  def getLoadPositions(self):
    '''Return the positions of the vehicle loads.'''
    positions= self.loadModel.getPositions()
    return self.getGlobalPositions(positions)
  
  def getVehicleBoundaryPositions(self):
    '''Return the vehicle boundary positions.'''
    return self.getGlobalPositions(self.loadModel.vehicleBoundary)
  
  def getLoadBoundary(self):
    '''Return the boundary of the vehicle loads.'''
    retval= geom.BND3d()
    tmp= self.getLoadPositions()
    for p in tmp:
      retval.update(p)
    return retval

  def getVehicleBoundary(self):
    '''Return the boundary of the vehicle.'''
    retval= geom.Poligono2d()
    tmp= self.getVehicleBoundaryPositions()
    for p in tmp:
      retval.agregaVertice(geom.Pos2d(p.x,p.y))
    return retval

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

Det2front= 0.75+1.50
Det2points= [geom.Pos2d(-Det2front,-1.5),geom.Pos2d(10.0-Det2front,-1.5),geom.Pos2d(10.0-Det2front,1.5),geom.Pos2d(-Det2front,1.5)]
Det21TruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(0.0,1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,1.0),120e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),150e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),150e3/2.0),
                                        WheelLoad(geom.Pos2d(0.0,-1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,-1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,-1.0),120e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),150e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),150e3/2.0)], vBoundary= Det2points)

Det22TruckLoadModel= LoadModel(wLoads= [WheelLoad(geom.Pos2d(0.0,1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),112.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),112.5e3/2.0),
                                        WheelLoad(geom.Pos2d(0.0,-1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80,-1.0),67.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60,-1.0),90e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),112.5e3/2.0),
                                        WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),112.5e3/2.0)], vBoundary= Det2points)
