# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2018,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import geom
import xc
from actions import loads
from model.geometry import geom_utils as gu
from misc_utils import log_messages as lmsg

class WheelLoad(object):
    ''' Load of a wheel.
    
    :ivar position: position of the wheel
    :ivar load: load
    :ivar lx: length wheel in transversal direction 
    :ivar ly: length wheel in longitudinal direction 
    :ivar node: node under the wheel.
    '''
    def __init__(self,pos,ld,lx=0,ly=0,node= None):
        self.position= pos
        self.load= ld
        self.lx=lx
        self.ly=ly
        self.node= node

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        return {'pos':self.position,'load':self.load,'lx':self.lx,'ly':self.ly, 'nodeTag': self.node.tag}

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.position= dct['pos']
        self.load= dct['ld']
        self.lx= dct['lx']
        self.ly= dct['ly']
        #self.nodeTag= dct['nodeTag']

    def __str__(self):
        return str(self.getDict())

    def pickNode(self, originSet):
        ''' Pick the nearest node to the wheel load position.

        :param originSet: in not None pick the nearest node for each wheel load.
        '''
        n= originSet.getNearestNode(self.position)
        self.node= n

    def defNodalLoad(self, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Create a new nodal load.

        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        vLoad= self.load*gravityDir
        if(brakingDir): # compute braking load
            vLoad+= self.load*xc.Vector(brakingDir)
        loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
        return self.node.newLoad(loadVector)
    
       
class TandemLoad(object):
    ''' Tandem load.

    :ivar axleLoad: axle load.
    :ivar xSpacing: distance between wheels of the same axle.
    :ivar ySpacing: tandem axle spacing.
    :ivar lx: width of the contact surface of the wheel.
    :ivar ly: length of the contact surface of the wheel.
    '''
    def __init__(self, axleLoad, xSpacing= 2.0, ySpacing= 1.2, lx= 0.4, ly= 0.4):
        ''' Constructor.

        :param axleLoad: axle load.
        :param xSpacing: distance between wheels of the same axle.
        :param ySpacing: tandem axle spacing.
        :param lx: width of the contact surface of the wheel.
        :param ly: length of the contact surface of the wheel.
        '''
        self.axleLoad= axleLoad
        self.xSpacing= xSpacing
        self.ySpacing= ySpacing
        self.lx= lx
        self.ly= ly

    def getWheelPositions(self, swapAxes= False):
        ''' Return a list with the positions of the wheels.

        :param swapAxes: if true swap X and Y axis.
        '''
        dX= self.xSpacing/2.0
        dY= self.ySpacing/2.0
        if(swapAxes):
            dX, dY= dY, dX
        return [geom.Pos2d(dX,-dY),geom.Pos2d(-dX,-dY),geom.Pos2d(dX,dY),geom.Pos2d(-dX,dY)]
        
    def getWheelLoads(self, loadFactor= 1.0, swapAxes= False):
        ''' Return the loads of the wheels of the tandem along with its 
            positions.

        :param loadFactor: factor to apply to the loads.
        :param swapAxes: if true swap X and Y axis.
        '''
        positions= self.getWheelPositions(swapAxes= swapAxes)
        wheelLoad= self.axleLoad/2.0*loadFactor
        retval= list()
        for p in positions:
            wl= WheelLoad(pos= p,ld=wheelLoad,lx=self.lx,ly=self.ly)
            retval.append(wl)
        return retval

class LoadModel(object):
    ''' Roadway traffic load model

    :ivar wheelLoads: position and loads of each wheel
    :ivar vehicleBoundary: polygon without uniform load around the vehicle.
    '''

    def __init__(self,wLoads, vBoundary= None):
        ''' Constructor.

        :param wheelLoads: position and loads of each wheel
        :param vehicleBoundary: polygon without uniform load around the vehicle.
        '''
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
        dispModulus= self.getCentroid().getPositionVector().getModulus()
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
        retval= geom.Polygon2d()
        tmp= self.getVehicleBoundaryRelativePositions()
        for p in tmp:
            retval.appendVertex(p)
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
        self.ldsWheels=self.genLstLoadDef()

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
#        ldW=self.genLstLoadDef()
#        for l in ldW:
        for l in self.ldsWheels:
            l.appendLoadToCurrentLoadPattern()
        
    def __mul__(self,factor):
        '''Apply the factor to the load and append it to the current load pattern'''
        for l in self.ldsWheels:
            l.__mul__(factor)
        
    def __rmul__(self,factor):
        '''Apply the factor to the load and append it to the current load pattern'''
        for l in self.ldsWheels:
            l.__mul__(factor)
    
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
            p3D= self.refSys.getGlobalPosition(p)
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
        retval= geom.Polygon2d()
        tmp= self.getVehicleBoundaryPositions()
        for p in tmp:
            retval.appendVertex(geom.Pos2d(p.x,p.y))
        return retval

class LaneAxis(object):
    ''' Notional lane

    :ivar pline: 3D polyline defining the axis of a lane.
    '''
    def __init__(self, pline):
        ''' Constructor.

        :param pline: 3D polyline defining the axis of a lane.
        '''
        self.pline= pline        
        
    def getReferenceAt(self, lmbdArcLength):
        ''' Return a 3D reference system with origin at the point 
            O+lmbdArcLength*L where O is the first point of the reference 
            axis and lmbdArcLength is a value between 0 and 1. If 
            lmbdArcLength=0 the returned point is the origin of the notional
            lane axis, and if lmbdArcLength= 1 it returns its end. The axis
            of the reference system are (or will be) defined as follows:

            - x: tangent to the notional lane axis oriented towards its end.
            - y: normal to the notional lane axis oriented towards its center.
            - z: defined by the cross-product x^y.

        :param lmbdArcLength: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
        '''
        if((lmbdArcLength>1.0) or (lmbdArcLength<0)):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; lbmdArcLength '+str(lmbdArcLength) + ' out of range (0,1)')
            
        l= lmbdArcLength*self.pline.getLength()
        O= self.pline.getPointAtLength(l)
        iVector= self.pline.getIVectorAtLength(l)
        jVector= self.pline.getJVectorAtLength(l)
        return geom.Ref2d3d(O, iVector, jVector)

    def getWheelLoads(self, loadModel, lmbdArcLength, loadFactor= 1.0):
        ''' Return the wheel loads of load model argument in the position
            specified by the lmbdArcLength parameter.

        :param loadModel: object that has a getWheelLoad method that returns
                          the 2D positions of the wheels and the loads to
                          apply on them (see TandemLoad class).
        :param lmbdArcLength: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
        :param loadFactor: factor to apply to the loads.
        '''
        wheelLoads= loadModel.getWheelLoads(loadFactor= loadFactor, swapAxes= True)
        ref= self.getReferenceAt(lmbdArcLength= lmbdArcLength)
        for wl in wheelLoads:
            pos2d= wl.position
            pos3d= ref.getGlobalPosition(pos2d)
            wl.position= pos3d
        return wheelLoads

    def getVDir(self, lmbdArcLength= 0.5):
        ''' Return the direction vector of the lane axis.

        :param lmbdArcLength: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
        '''
        return self.pline.getIVectorAtLength(lmbdArcLength*self.pline.getLength())
    
# Rudimentary implementation of the notional lane concept.
#
#     Notional lane
#   P1                                                 P2
#     +-----------------------------------------------+
#     |                                               |
#     |                                               |
#     |                                               |
#     +-----------------------------------------------+
#    P0                                                P3

class NotionalLane(object):
    ''' Notional lane

    :ivar contour: 3D polygon defining the contour of the notional lane.
    '''
    def __init__(self, name, contour):
        ''' Constructor.

        :param name: name of the notional lane.
        :param contour: 3D polygon defining the contour of the notional lane.
        '''
        self.name= name
        self.contour= contour

    def getArea(self):
        ''' Return the area of the notional lane contour.'''
        return self.contour.getArea()
    
    def getCentroid(self):
        ''' Return the centroid of the notional lane contour.'''
        return self.contour.getCenterOfMass()

    def getStartingEdge(self):
        ''' Return the edge at the "start" of the notional lane.'''
        return self.contour.getEdge(0)

    def getFinishEdge(self):
        ''' Return the edge at the "finish" of the notional lane.'''
        return self.contour.getEdge(2)

    def getAxis(self):
        ''' Return the axis of the notional lane.'''
        startingEdge= self.getStartingEdge()
        finishEdge= self.getFinishEdge()
        return LaneAxis(pline= geom.Polyline3d([startingEdge.getCenterOfMass(), finishEdge.getCenterOfMass()]))

    def getVDir(self, lmbdArcLength= 0.5):
        ''' Return the direction vector of the lane axis.

        :param lmbdArcLength: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
         '''
        return self.getAxis().getVDir(lmbdArcLength= lmbdArcLength)

class NotionalLanes(object):
    ''' Notional lanes container base class (abstract class).

    :ivar laneSets: sets of elements belonging to each notional lane.
    '''
    
    def getAreas(self):
        ''' Return the areas of the notional lanes.'''
        retval= list()
        for lane in self.lanes:
            retval.append(lane.getArea())
        return retval

    def getWheelLoads(self, tandems, relativePositions, originSet= None):
        ''' Return a dictionary containing the wheel loads due to the tandems
            argument in the positions argument.

        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePosition: relative positions of the tandem center in
                                 the notional lane axis (0 -> beginning of
                                 the axis, 0.5-> middle of the axis, 1-> end
                                 of the axis).
        :param originSet: in not None pick the nearest node for each wheel load.
        '''
        retval= list()
        # Compute load positions in each lane.
        for nl,rpos,td in zip(self.lanes, relativePositions,tandems):
            tandemLoads= list()
            if(td is not None):
                axis= nl.getAxis()
                tandemLoads= axis.getWheelLoads(loadModel= td, lmbdArcLength= rpos)
            retval.extend(tandemLoads)
        if(originSet): # pick the nodes under each wheel
            for load in retval:
                load.pickNode(originSet= originSet)
        return retval
    
    def defUniformLoadsXCSets(self, modelSpace, originSet):
        ''' Creates the XC sets with the elements that fall inside
            each of the notional lanes of the argument.

        :param modelSpace: model space used to define the FE problem.
        :param originSet: set with the elements to pick from.
        '''
        self.laneSets= list()
        for nl in self.lanes:
            setName= nl.name+'Set'
            xcSet= modelSpace.defSet(setName)
            modelSpace.pickElementsInZone(zone= nl.contour, resultSet= xcSet, originSet= originSet)
            xcSet.fillDownwards()
            self.laneSets.append(xcSet)
        return self.laneSets

    def defUniformLoads(self, laneUniformLoads= [9e3, 2.5e3, 2.5e3], gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define uniform loads on the lanes with the argument values:

        :param laneUniformLoads: load for each notional lane [1st, 2nd, 3rd,...].
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        retval= list()
        for q, xcSet in zip(laneUniformLoads, self.laneSets):
            if(q is not None):
                loadVector= q*gravityDir
                if(brakingDir): # Compute braking load
                    loadVector+= q*xc.Vector(brakingDir)
                for e in xcSet.elements:
                    retval.append(e.vector3dUniformLoadGlobal(loadVector))
        return retval

    def defPunctualLoads(self, tandems, relativePositions, originSet= None, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define punctual loads under the wheels.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePosition: relative positions of the tandem center in
                                 the notional lane axis (0 -> beginning of
                                 the axis, 0.5-> middle of the axis, 1-> end
                                 of the axis).
        :param originSet: in not None pick the nearest node for each wheel load.
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        wheelLoads= self.getWheelLoads(tandems= tandems, relativePositions= relativePositions, originSet= originSet)
        retval= list()
        for wl in wheelLoads:
            retval.append(wl.defNodalLoad(gravityDir= gravityDir, brakingDir= brakingDir))
        return retval
    
    def defLoads(self, tandems, relativePositions, laneUniformLoads, originSet= None, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define punctual and uniform loads.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePosition: relative positions of each tandem center in
                                 the notional lane axis (0 -> beginning of
                                 the axis, 0.5-> middle of the axis, 1-> end
                                 of the axis).
        :param laneUniformLoads: load for each notional lane [1st, 2nd, 3rd,...].
        :param originSet: in not None pick the nearest node for each wheel load.
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        # punctual loads.
        self.defPunctualLoads(tandems= tandems, relativePositions= relativePositions, originSet= originSet, gravityDir= gravityDir, brakingDir= brakingDir)
        # uniform load.
        self.defUniformLoads(laneUniformLoads= laneUniformLoads, gravityDir= gravityDir, brakingDir= brakingDir)
       


    

