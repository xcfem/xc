# -*- coding: utf-8 -*-
from __future__ import division


__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2018,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import xc_base
import geom
import xc
from actions import loads
from model.geometry import geom_utils as gu

class WheelLoad(object):
    ''' Load of a wheel.
    
    :ivar position: position of the wheel
    :ivar load: load
    :ivar lx: lenght wheel in transversal direction 
    :ivar ly: lenght wheel in longitudinal direction 
    '''
    def __init__(self,pos,ld,lx=0,ly=0):
       self.position= pos
       self.load= ld
       self.lx=lx
       self.ly=ly

class LoadModel(object):
    ''' Roadway trafic load model

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

class VehicleDistrLoad(object):
    ''' Distribute the point loads defined in the object lModel over the shell 
    elements under the wheels affected by them.

    :ivar name: name identifying the load
    :ivar xcSet: set that contains the shell elements
    :ivar lModel: instance of the class LoadModel with the definition of
                  vehicle of the load model.
    :ivar xCent: global coord. X where to place the centroid of the vehicle
    :ivar yCent: global coord. Y where  to place the centroid of the vehicle
    :ivar hDistr: height considered to distribute each point load with
                  slope slopeDistr 
    :ivar slopeDistr: slope (H/V) through hDistr to distribute the load of 
                  a wheel
     '''
    def __init__(self,name,xcSet,loadModel, xCentr,yCentr,hDistr,slopeDistr):
        self.name=name
        self.xcSet=xcSet
        self.loadModel= loadModel
        self.xCentr=xCentr
        self.yCentr=yCentr
        self.hDistr=hDistr
        self.slopeDistr=slopeDistr

    def genLstLoadDef(self):
        '''generates a list with the definition of all the wheel loads 
        '''
        deltaL=2*self.slopeDistr*self.hDistr
        ldWheels=list()
        cont=0
        for w in self.loadModel.wheelLoads:
            nm=self.name+str(cont)
            lVect=xc.Vector([0,0,-w.load,0,0,0])
            xCwheel=self.xCentr+w.position.x
            yCwheel=self.yCentr+w.position.y
            basePrism=gu.rect2DPolygon(xCent=xCwheel,yCent=yCwheel,Lx=w.lx+deltaL,Ly=w.ly+deltaL)
            ldWheels.append(loads.PointLoadOverShellElems(nm,self.xcSet,lVect,basePrism,'Z','Global'))
            cont+=1
        return ldWheels
            

    def appendLoadToCurrentLoadPattern(self):
        ''' Append load to the current load pattern.'''
        ldW=self.genLstLoadDef()
        for l in ldW:
            l.appendLoadToCurrentLoadPattern()
        
    
    
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

