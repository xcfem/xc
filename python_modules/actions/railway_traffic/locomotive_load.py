# -*- coding: utf-8 -*-
''' Generic locomotive load model.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2023,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import sys
import math
import geom
import xc
from misc_utils import log_messages as lmsg
from actions.railway_traffic import wheel_load
from actions.railway_traffic import dynamic_factor_load as dfl

# Locomotive load.
#     ____
#     |DD|____T_
#     |_ |_____|<
#     @-@-@-oo\
#

class LocomotiveLoad(dfl.DynamicFactorLoad):
    ''' Locomotive load.

    :ivar nAxes: number of axes.
    :ivar axleLoad: axle load.
    :ivar xSpacing: tandem axle spacing.
    :ivar yXpacing: distance between wheels of the same axle.
    '''
    def __init__(self, nAxes= 4, axleLoad= 250e3, xSpacing= 1.6, ySpacing= 1.435, dynamicFactor= 1.0, classificationFactor= 1.21):
        ''' Constructor.

        :param nAxes: defaults to 4 (Eurocode 1, load model 1)
        :param axleLoad: axle load.
        :param xSpacing: tandem axle spacing (defaults to 1.6 m, Eurocode 1, load model 1).
        :param ySpacing: distance between wheels of the same axle (defaults to nternational standard gauge 1435 mm).
        :param dynamicFactor: dynamic factor.
        :param classificationFactor: classification factor (on lines carrying
                                     rail traffic which is heavier or lighter
                                     than normal rail traffic).
        '''
        super().__init__(dynamicFactor= dynamicFactor, classificationFactor= classificationFactor)
        self.nAxes= nAxes
        self.axleLoad= axleLoad
        self.xSpacing= xSpacing
        self.ySpacing= ySpacing

    def getAxleClassifiedLoad(self):
        ''' Return the value of the axle load affected by the classification
            factor.'''
        return self.axleLoad*self.classificationFactor

    def getTotalLoad(self):
        ''' Return the total load of the tandem.'''
        return self.nAxes*self.getAxleClassifiedLoad()*self.dynamicFactor

    def getTotalLength(self):
        ''' Return the length occupied by the locomotive.'''
        return self.nAxes*self.xSpacing

    def getNumWheels(self):
        ''' Return the number of wheels of the locomotive.'''
        return self.nAxes*2

    def getWheelLocalPositions(self):
        ''' Return a list with the positions of the wheels.'''
        dX= self.xSpacing
        dY= self.ySpacing/2.0
        axesDisp= (self.nAxes-1)*dX/2.0  # Axes positions relative to the center.
        axlesPos= list()
        for i in range(0,self.nAxes):
            axlesPos.append(i*dX-axesDisp)
        retval= list()
        for y in [-dY, dY]:
            for x in axlesPos:
                retval.append(geom.Pos2d(x,y))
        return retval

    def getWheelsGlobalPositions(self, ref):
        ''' Return the loads of the wheels of the tandem along with its
            positions.

        :param ref: reference system at the center of the locomotive.
        '''
        # Compute local positions.
        positions= self.getWheelLocalPositions()
        # Transform to global positions.
        retval= list()
        for pos2d in positions:
            retval.append(ref.getGlobalPosition(pos2d))
        return retval

    def getDisplacementsUnderWheels(self, ref, xcSet):
        ''' Return the positions and displacements of the nodes under each 
            wheel (the nearest one).

        :param ref: reference system at the center of the locomotive.
        :param xcSet: set to search the nodes on.
        '''
        retval= list()
        wheelsPositions= self.getWheelsGlobalPositions(ref= ref)
        setPlane= xcSet.nodes.getRegressionPlane(0.0) # 0.0 -> scale factor for the current position: 1.0 (curreentPosition= initialPosition+scaleFactor*nodeDisplacement).
        avgElementSize= xcSet.elements.getAverageSize(True)
        avgElementSideLength= math.sqrt(avgElementSize)
        tol= 0.7*avgElementSideLength# 0.25*self.xSpacing
        for i, wheelPos in enumerate(wheelsPositions):
            nearestNode= xcSet.getNearestNode(wheelPos)
            nearestNodePos= nearestNode.getInitialPos3d
            nearestNodeDisp= nearestNode.getDisp
            projWheelPos= setPlane.getProjection(wheelPos)
            dist2d= projWheelPos.dist(setPlane.getProjection(nearestNodePos))
            if(dist2d<tol):
                retval.append((nearestNodePos, nearestNodeDisp))
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                warningMsg= '; wheel in position: '+str(wheelPos)
                warningMsg+= ' too far ('+str(dist2d)
                warningMsg+= ') from the nearest node in the bridge deck: '
                warningMsg+= str(nearestNodePos)
                warningMsg+= ' this wheel will be ignored.'
                lmsg.warning(className+'.'+methodName+warningMsg)
        return retval

    def getTwist(self, ref, xcSet, length= 3.0, removeGeometricTwist= False, outputDict= None):
        ''' Computes the deck twist fromn the displacements of the nodes
            under the wheels.

        :param ref: reference system at the center of the locomotive.
        :param xcSet: set to search the nodes on.
        :param length: length to measure the twist over.
        :param removeGeometricTwist: remove the twist due to the mesh geometry.
        :param outputDict: Python dictionary to store the displacement results. 
        '''
        wheelsDisplacements= self.getDisplacementsUnderWheels(ref= ref, xcSet= xcSet)
        axisStep= math.ceil(length/self.xSpacing)
        factor= length/(self.xSpacing*axisStep)
        nAxes= min(int(len(wheelsDisplacements)/2), self.nAxes)
        numberOfComputedTwists= int(nAxes/axisStep)
        retval= list()
        if(numberOfComputedTwists>0):
            if(removeGeometricTwist):
                for i in range(0, numberOfComputedTwists):
                    p0= wheelsDisplacements[i][0]
                    p1= wheelsDisplacements[i+nAxes][0]
                    p2= wheelsDisplacements[i+axisStep][0]
                    plane= geom.Plane3d(p0, p1, p2)
                    # Make the third point coplanar.
                    p3= plane.getProjection(wheelsDisplacements[i+nAxes+axisStep][0])
                    wheelsDisplacements[i+nAxes+axisStep]= (p3, wheelsDisplacements[i+nAxes+axisStep][1])
            displacedPositions= list()
            for (nodePos, nodeDisp) in wheelsDisplacements:        
                displacedPositions.append(nodePos+geom.Vector3d(nodeDisp[0], nodeDisp[1], nodeDisp[2]))
            for i in range(0, numberOfComputedTwists):
                p0= displacedPositions[i]
                p1= displacedPositions[i+nAxes]
                p2= displacedPositions[i+axisStep]
                p3= displacedPositions[i+nAxes+axisStep]
                plane= geom.Plane3d(p0, p1, p2)
                twist= plane.dist(p3)*factor
                retval.append(twist)
                # Store computed displacements in the output dict.
                if(outputDict is not None):
                    twistResults= {'twist':twist, 'factor':factor}
                    for j, k in enumerate([i, i+nAxes, i+axisStep, i+nAxes+axisStep]):
                        wheelKey= 'wheel_position_'+str(j)
                        wheelPos= wheelsDisplacements[k][0]
                        twistResults[wheelKey]= (wheelPos.x, wheelPos.y, wheelPos.z)
                        dispKey= 'wheel_displacement_'+str(j)
                        wheelDisp= wheelsDisplacements[k][1]
                        twistResults[dispKey]= list(wheelDisp)
                    twistKey= 'twist_'+str(i)
                    outputDict[twistKey]= twistResults
                        
        return retval
    
    def getNosingLoadPositions(self):
        ''' Return a list with the positions for the nosing loads: center 
            of the rails under the locomotive.'''
        dY= self.ySpacing/2.0
        return [geom.Pos2d(0,-dY), geom.Pos2d(0,+dY)]

    def getWheelLoad(self):
        ''' Return half the axle load.'''
        return self.axleLoad/2.0
    
    def getClassifiedWheelLoad(self):
        ''' Return the load on each wheel affected by the classification factor.'''
        return self.getAxleClassifiedLoad()/2.0

    def getDynamicWheelLoad(self):
        ''' Return the load on each wheel affected by the dynamic factor.'''
        return self.getClassifiedWheelLoad()*self.dynamicFactor

    def getWheelLoads(self, ref= None, loadFactor= 1.0, static= False, loadDirectionVector= xc.Vector([0, 0, -1])):
        ''' Return the loads of the wheels of the tandem along with its
            positions.

        :param ref: reference system at the center of the locomotive.
        :param loadFactor: factor to apply to the loads.
        :param static: if true don't apply the dynamic factor.
        :param loadDirectionVector: load direction vector.
        '''
        if(static):
            wheelLoad= self.getClassifiedWheelLoad()*loadFactor
        else:
            wheelLoad= self.getDynamicWheelLoad()*loadFactor
        # Compute local positions.
        positions= self.getWheelLocalPositions()
        wheelLoads= list()
        for p in positions:
            wl= wheel_load.WheelLoad(pos= p, ld=wheelLoad, directionVector= loadDirectionVector)
            wheelLoads.append(wl)
        if(ref): # return the loads in global coordinates.
            retval= list()
            for wl in wheelLoads:
                pos2d= wl.position
                pos3d= ref.getGlobalPosition(pos2d)
                wl.position= pos3d
                wl.localCooSystem= geom.CooSysRect3d3d(ref.getIVector(), ref.getJVector())
                retval.append(wl)
        else: # return the loads in local coordinates.
            retval= wheelLoads 
        return retval

    def getCentrifugalWheelLoads(self, centrifugalLoads, centrifugalDirection, ref):
        ''' Define the wheel loads due to the locomotives argument placed at
            the positions argument.

        .param centrifugalLoads: centrifugal loads for each locomotive wheel.
        :param centrifugalDirection: direction of the centrifugal force.
        :param ref: reference system at the center of the locomotive.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                concrete slab.
        :param originSet: pick the loaded nodes for each wheel load.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param loadFactor: factor to apply to the loads.
        '''
        positions= self.getWheelLocalPositions()
        wheelLoads= list()
        for p, cl in zip(positions, centrifugalLoads):
            if(ref):
                pos3d= ref.getGlobalPosition(p)
                load3d= cl.x*centrifugalDirection+cl.y*ref.getKVector()
                wheelLoad= load3d.getModulus()
                directionVector= load3d.normalized()
                wl= wheel_load.WheelLoad(pos= pos3d, ld= wheelLoad, directionVector= directionVector)
                wl.localCooSystem= geom.CooSysRect3d3d(ref.getIVector(), ref.getJVector())
                wheelLoads.append(wl)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; need a locomotive reference system.')
                lmsg.error()
        return wheelLoads
    
    def defDeckWheelLoadsThroughLayers(self, ref, spreadingLayers= None, originSet= None, deckThickness= 0.0, deckSpreadingRatio= 1/1, gravityDir= xc.Vector([0,0,-1]), loadFactor= 1.0):
        ''' Define the wheel loads due to this locomotive placed at the
            positions obtained by applying the reference system argument.

        :param ref: reference system at the center of the locomotive.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                concrete slab.
        :param originSet: pick the loaded nodes for each wheel load.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param gravityDir: direction of the gravity field (unit vector).
        :param loadFactor: factor to apply to the loads.
        '''
        wheelLoads= self.getWheelLoads(ref= ref, loadFactor= loadFactor, loadDirectionVector= gravityDir)
        retval= list()
        if(originSet):  # pick the loaded by each wheel
            for wheelLoad in wheelLoads:
                retval.extend(wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        return retval
    
    def defDeckCentrifugalWheelLoadsThroughLayers(self, centrifugalLoads, centrifugalDirection, ref, spreadingLayers= None, originSet= None, deckThickness= 0.0, deckSpreadingRatio= 1/1):
        ''' Define the wheel loads due to the locomotives argument placed at
            the positions argument.

        :param centrifugalLoads: centrifugal loads for each locomotive wheel.
        :param centrifugalDirection: direction of the centrifugal force.
        :param ref: reference system at the center of the locomotive.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                concrete slab.
        :param originSet: pick the loaded nodes for each wheel load.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        positions= self.getWheelLocalPositions()
        wheelLoads= list()
        if(ref):
            refKVector= ref.getKVector()
            if(refKVector[2]<0): # Swap interior and exterior wheels.
                positions.reverse()
       
        for p, cl in zip(positions, centrifugalLoads):
            if(ref):
                pos3d= ref.getGlobalPosition(p)
                load3d= cl.x*centrifugalDirection+cl.y*ref.getKVector()
                wheelLoad= load3d.getModulus()
                directionVector= load3d.normalized()
                wl= wheel_load.WheelLoad(pos= pos3d, ld= wheelLoad, directionVector= directionVector)
                wl.localCooSystem= geom.CooSysRect3d3d(ref.getIVector(), ref.getJVector())
                wheelLoads.append(wl)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; need a locomotive reference system.')
                lmsg.error()

        retval= list()
        if(originSet):  # pick the loaded by each wheel
            for wheelLoad in wheelLoads:
                retval.extend(wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; need a set to pick the nodes from it.')
            lmsg.error()
        return retval
    
    def defDeckNosingLoadThroughLayers(self, nosingDirection, ref, spreadingLayers= None, originSet= None, deckThickness= 0.0, deckSpreadingRatio= 1/1):
        ''' Define the wheel loads due to the locomotives argument placed at
            the positions argument.

        :param nosingDirection: direction of the nosing force.
        :param ref: reference system at the center of the locomotive.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of
                                the layers between the wheel contact
                                area and the middle surface of the
                                concrete slab.
        :param originSet: pick the loaded nodes for each wheel load.
        :param deckThickness: thickness of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        nosingLoadValue= self.getNosingLoad()
        nosingLoads= list()
        dY= -self.ySpacing/2.0
        if(ref):
            refKVector= ref.getKVector()
            if(refKVector[2]<0): # Swap interior and exterior wheels.
                dY= -dY
        nosingLoadPos= geom.Pos2d(0,dY)
        if(ref):
            pos3d= ref.getGlobalPosition(nosingLoadPos)
            nl= wheel_load.WheelLoad(pos= pos3d, ld= nosingLoadValue, directionVector= nosingDirection)
            nl.localCooSystem= geom.CooSysRect3d3d(ref.getIVector(), ref.getJVector())
            nosingLoads.append(nl)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; need a locomotive reference system.')
            lmsg.error()

        retval= list()
        if(originSet): # pick the loaded by each wheel
            for nosingLoad in nosingLoads:
                retval.extend(nosingLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= deckThickness, deckSpreadingRatio= deckSpreadingRatio))
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; need a set to pick the nodes from it.')
            lmsg.error()
        return retval
