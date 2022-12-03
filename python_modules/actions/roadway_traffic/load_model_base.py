# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2018,  LCPT AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmail.com"

import sys
import geom
import xc
from actions import loads
from model.geometry import geom_utils as gu
from misc_utils import log_messages as lmsg
from geotechnics import horizontal_surcharge as hs
from geotechnics import boussinesq

# Traffic loads.
#
#      |^^^^^^^^^^^^^^||____
#      |      Truck   |||””‘|””\__,_
#      | ____________ l||___|__|__||)
#      |(@)@)”””””””****|(@)(@)**|(@)
# ——————————————————————————————————————————

class WheelLoad(object):
    ''' Load of a wheel.
    
    :ivar position: position of the wheel
    :ivar load: load
    :ivar lx: length of the wheel contact area in transversal direction 
    :ivar ly: length of the wheel contact area in longitudinal direction 
    :ivar nodes: nodes under the wheel.
    '''
    def __init__(self, pos, ld, lx=0, ly=0, localCooSystem= None, nodes= None):
        ''' Constructor.

        :param pos: position of the wheel
        :param ld: load
        :param lx: length of the wheel contact area in transversal direction 
        :param ly: length of the wheel contact area in longitudinal direction 
        :param localCooSystem: local coordinate system whose I vector
                               is aligned with the lane axis and
                               its XY plane contains the road surface
                               at the wheel position.
        :param nodes: nodes under the wheel.
        '''
        self.position= pos
        self.load= ld
        self.lx=lx
        self.ly=ly
        self.localCooSystem= localCooSystem
        self.nodes= nodes

    def getLocalReferenceSystem(self):
        ''' Return a local reference system for the wheel with origin in
            the wheel position, x axis along the lane axis and y axis normal
            to the x axis on the road surface.'''
        retval= None
        if(self.localCooSystem):
            retval= geom.Ref2d3d(self.position, self.localCooSystem.getIVector(), self.localCooSystem.getJVector())
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; undefined local coordinate system.')
        return retval        

    def getLoadedContour(self, spreadingLayers= None):
        ''' Return the loaded contour of the wheel taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface of the bridge deck.

        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of 
                                the layers between the wheel contact 
                                area and the middle surface of the 
                                bridge deck.
        '''
        # Compute spread.
        spread= 0.0
        if(spreadingLayers):
            for sl in spreadingLayers:
                layerDepth= sl[0]
                layerSpreadToDepthRatio= sl[1]
                spread+= layerDepth*layerSpreadToDepthRatio
        # Compute local axis.
        reference= self.getLocalReferenceSystem()
        laneAxisDir= reference.getLocalCoordinates(self.localCooSystem.getIVector())
        jVector= laneAxisDir # Longitudinal direction.
        iVector= geom.Vector2d(-jVector.y, jVector.x) # Transverse direction.
        # Compute spread in each direction.
        transverseSpread= self.lx/2.0+spread
        longitudinalSpread= self.ly/2.0+spread
        # Compute contour points.
        contourPoints= list()
        origin= geom.Pos2d(0,0)
        contourPoints.append(origin+transverseSpread*iVector+longitudinalSpread*jVector) # +,+
        contourPoints.append(origin-transverseSpread*iVector+longitudinalSpread*jVector) # -,+
        contourPoints.append(origin-transverseSpread*iVector-longitudinalSpread*jVector) # -,-
        contourPoints.append(origin+transverseSpread*iVector-longitudinalSpread*jVector) # +,-
        return reference, geom.Polygon2d(contourPoints)
            
    def getDict(self):
        ''' Return a dictionary with the object values.'''
        nodeTags= list()
        if(self.nodes):
            for n in self.nodes:
                nodeTags.append(n.tag)
        return {'pos':self.position,'load':self.load,'lx':self.lx,'ly':self.ly, 'nodeTags': nodeTags}

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.position= dct['pos']
        self.load= dct['ld']
        self.lx= dct['lx']
        self.ly= dct['ly']
        self.nodeTags= None # Loaded nodes reading from dict not implemented yet.
        #self.nodeTags= dct['nodeTag']

    def __str__(self):
        return str(self.getDict())

    def pickDeckNodes(self, originSet, spreadingLayers= None):
        ''' Pick the deck nodes loaded by the wheel.

        :param originSet: set to pick the loaded nodes from.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of 
                                the layers between the wheel contact 
                                area and the middle surface of the 
                                bridge deck.
        '''
        if(self.localCooSystem):
            reference, loadedContour= self.getLoadedContour(spreadingLayers= spreadingLayers)
            tol= .01
            self.nodes= list()
            for n in originSet.nodes:
                nodePos2d= reference.getLocalPosition(n.getInitialPos3d)
                if(loadedContour.In(nodePos2d, tol)): # node in loaded contour.
                    self.nodes.append(n)
        else:
            n= originSet.getNearestNode(self.position)
            self.nodes= [n]

    def getDeckLoadedContourThroughEmbankment(self, embankment, deckMidplane, deckSpreadingRatio= 2/1.0):
        ''' Return the loaded contour of the wheel taking into account
            the dispersal through the different pavement, earth and
            concrete layers between the wheel contact area and the
            middle surface of the bridge deck.

        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckMidplane: mid-plane of the bridge deck.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see 
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        # Compute spreading Layers.
        ## Compute the vertical proyection of the wheel position on the deck
        ## mid-plane.
        vertLine= geom.Line3d(self.position, geom.Vector3d(0,0,-100.0))
        projPos= deckMidplane.getIntersection(vertLine)
        ## Ask the embankment about the layer thicknesses in this position.
        layerThicknesses= embankment.getLayerThicknesses(point= projPos)
        ## Construct the spreading layers list.
        spreadingLayers= list()
        fillingThickness= 0.0
        for thk, layer in zip(layerThicknesses, embankment.layers):
            spreadingLayers.append((thk, layer.loadSpreadingRatio))
            fillingThickness+= thk
        ## Compute the deck thickness.
        midplaneDepth= self.position.dist(projPos)
        halfDeckThickness= midplaneDepth-fillingThickness
        ## Append the deck spreading too.
        spreadingLayers.append((halfDeckThickness, deckSpreadingRatio))
        # Call the regular method.
        return self.getLoadedContour(spreadingLayers= spreadingLayers)
        
    def pickDeckNodesThroughEmbankment(self, originSet, embankment, deckSpreadingRatio= 2/1.0):
        ''' Pick the nodes loaded by the wheel.

        :param originSet: set to pick the loaded nodes from.
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see 
                                   clause 4.3.6 on Eurocode 1-2:2003).
        '''
        if(self.localCooSystem):
            # Compute the deck mid-plane as the regression plane
            # of the positions of its nodes.
            deckMidplane= originSet.nodes.getRegressionPlane(0.0)
            # Compute the loaded contour.
            reference, loadedContour= self.getDeckLoadedContourThroughEmbankment(embankment= embankment, deckMidplane= deckMidplane, deckSpreadingRatio= deckSpreadingRatio)
            tol= .01
            self.nodes= list()
            for n in originSet.nodes:
                nodePos2d= reference.getLocalPosition(n.getInitialPos3d)
                if(loadedContour.In(nodePos2d, tol)): # node in loaded contour.
                    self.nodes.append(n)
        else:
            n= originSet.getNearestNode(self.position)
            self.nodes= [n]

    def getLoadVector(self, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Return the load vector at the contact surface.


        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load (properly factored).
        '''
        # Compute load vector.
        retval= self.load*gravityDir
        if(brakingDir): # compute braking load
            retval+= self.load*xc.Vector(brakingDir)
        return retval
        
    def defNodalLoads(self, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Create the nodal loads corresponding to the contact pressure.

        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load (properly factored).
        '''
        retval= list()
        numLoadedNodes= 0
        if(self.nodes):
            numLoadedNodes= len(self.nodes)
        if(numLoadedNodes>0):
            # Compute load vector.
            vLoad= self.getLoadVector(gravityDir= gravityDir, brakingDir= brakingDir)
            loadVector= xc.Vector([vLoad[0],vLoad[1],vLoad[2],0.0,0.0,0.0])
            if(numLoadedNodes==1):
                retval.append(self.nodes[0].newLoad(loadVector))
            else:
                ptCoo= [self.position.x, self.position.y, self.position.z]
                slidingVectorLoad= loads.SlidingVectorLoad(name= 'wheelLoad', nodes= self.nodes, pntCoord= ptCoo, loadVector= loadVector)
                slidingVectorLoad.appendLoadToCurrentLoadPattern()
        return retval

    def getBackfillConcentratedLoad(self, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Return the concentrated loads on the backfill corresponding to
            the wheel loads.

        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load (properly factored).
        '''
        normalized= gravityDir.Normalized()
        if(abs(gravityDir[0])>1e-3) or (abs(gravityDir[1])>1e-3):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; gravity directions different from (0,0,-1) not supported yet.')
        vLoad= self.getLoadVector(gravityDir= gravityDir, brakingDir= brakingDir)
        boussinesqLoad= boussinesq.ConcentratedLoad(p= self.position, Q=vLoad[2])
        horizontalLoad= hs.HorizontalConcentratedLoadOnBackfill3D(pos= self.position, H= geom.Vector3d(vLoad[0],vLoad[1],0))
        return (horizontalLoad, boussinesqLoad)
       
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

    def getTotalLoad(self):
        ''' Return the total load of the tandem.'''
        return 2.0*self.axleLoad

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
    ''' Lane axis

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
            wl.localCooSystem= geom.CooSysRect3d3d(ref.getIVector(), ref.getJVector()) 
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
    
    def getLength(self):
        ''' Return the axis of the notional lane.'''
        startingEdge= self.getStartingEdge()
        finishEdge= self.getFinishEdge()
        return startingEdge.getCenterOfMass().dist(finishEdge.getCenterOfMass())

    def getVDir(self, lmbdArcLength= 0.5):
        ''' Return the direction vector of the lane axis.

        :param lmbdArcLength: parameter (0.0->start of the axis, 1.0->end of
                              the axis).
         '''
        return self.getAxis().getVDir(lmbdArcLength= lmbdArcLength)

    def getContiguousLaneSegment(self, length, name= None):
        ''' Return the contour of another piece of road lane that continues
        this one at its beginning (if length<0) or at its end (if length>0). 

        This solution will probably be deprecated when a more general type
        of notional lanes (not necessarily quadrilateral) have been developed.

        :param lengtht: real number that defines the length of the new lane
                        segment and its position with respecto to this one
                        (at its beginning if smaller than 0, or at its end 
                        if greater than 0).
        :param name: name for the new notional lane.
        '''
        # Compute mirror and mirrored edges.
        lengthFactor= length/self.getLength()
        ## Assume lengthFactor>0
        mirrorEdge= self.getFinishEdge() # Edge in the mirrored plane.
        mirroredEdge= self.getStartingEdge() # Edge to be mirrored.
        suffix= '_following'
        if(lengthFactor<0): # If not, then swap
            mirrorEdge, mirroredEdge= mirroredEdge, mirrorEdge
            suffix= '_preceding'
        #Compute mirror transformation.
        normalVector= self.contour.getKVector()
        ptA= mirrorEdge.getFromPoint()
        ptB= mirrorEdge.getToPoint()
        ptC= ptA+100.0*normalVector
        mirrorPlane= geom.Plane3d(ptA, ptB, ptC)
        mirrorTransformation= geom.Reflection3d(mirrorPlane)
        # Compute the mirrored lane contour.
        mirroredPtA= mirrorTransformation.getTransformed(mirroredEdge.getFromPoint())
        mirroredPtB= mirrorTransformation.getTransformed(mirroredEdge.getToPoint())
        absLengthFactor= abs(lengthFactor)
        vectorA= absLengthFactor*(mirroredPtA-ptB)
        vectorB= absLengthFactor*(mirroredPtB-ptA)
        if(lengthFactor<0.0):
            contourPoints= [ptA+vectorA, ptB+vectorB, ptB, ptA]
        else:
            contourPoints= [ptB, ptA, ptA+vectorA, ptB+vectorB]
        # Construct the new notional lane.
        if(name is None):
            name= self.name+suffix
        return NotionalLane(name= name, contour= geom.Polygon3d(contourPoints))
        

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

    def getContiguousNotionalLanes(self, length):
        ''' Return the contiguous notional lanes that continue the lanes of
        this one at its beginning (if length<0) or at its end (if length>0). 
        The length of the returned segment is length times the length of 
        this one.

        This solution will probably be deprecated when a more general type
        of notional lanes (not necessarily quadrilateral) have been developed.

        :param lenght: real number that defines the length of the new lane 
                       segments and its position with respecto to those of
                       this set (at its beginning if negative, or at its end
                       positive).
        '''
        newLanes= list()
        for lane in self.lanes:
            newLanes.append(lane.getContiguousLaneSegment(length= length))
        retval= NotionalLanes()
        retval.lanes= newLanes
        return retval
        
    def getWheelLoads(self, tandems, relativePositions):
        ''' Return a the wheel loads due to the tandems argument placed at
            the positions argument.

        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        '''
        retval= list()
        # Compute load positions in each lane.
        for nl,rpos,td in zip(self.lanes, relativePositions,tandems):
            tandemLoads= list()
            if(td is not None):
                laneAxis= nl.getAxis() # Lane axis.
                tandemLoads= laneAxis.getWheelLoads(loadModel= td, lmbdArcLength= rpos)
            retval.extend(tandemLoads)
        return retval
    
    def getDeckWheelLoads(self, tandems, relativePositions, originSet= None, spreadingLayers= None):
        ''' Return the wheel loads due to the tandems argument placed at
            the positions argument.

        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: pick the loaded nodes for each wheel load.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of 
                                the layers between the wheel contact 
                                area and the middle surface of the 
                                concrete slab.
        '''
        retval= self.getWheelLoads(tandems= tandems, relativePositions= relativePositions)
        if(originSet): # pick the loaded by each wheel
            for wheelLoad in retval:
                wheelLoad.pickDeckNodes(originSet= originSet, spreadingLayers= spreadingLayers)
        return retval
    
    def getBackfillConcentratedLoads(self, tandems, relativePositions, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Return the wheel loads on the backfill due to the tandems argument 
        placed at the positions argument.

        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load (properly factored).
        '''
        wheelLoads= self.getWheelLoads(tandems= tandems, relativePositions= relativePositions)
        retval= list()
        for wheelLoad in wheelLoads:
            retval.append(wheelLoad.getBackfillConcentratedLoad(gravityDir= gravityDir, brakingDir= brakingDir))
        return retval
    
    def defDeckUniformLoadsXCSets(self, modelSpace, originSet):
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

    def defDeckUniformLoads(self, laneUniformLoads= [9e3, 2.5e3, 2.5e3], gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
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

    def defDeckPunctualLoads(self, tandems, relativePositions, originSet= None, gravityDir= xc.Vector([0,0,-1]), brakingDir= None, spreadingLayers= None):
        ''' Define punctual loads under the wheels.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set with the nodes to pick from.
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of 
                                the layers between the wheel contact 
                                area and the middle surface of the 
                                bridge deck.
        '''
        wheelLoads= self.getDeckWheelLoads(tandems= tandems, relativePositions= relativePositions, originSet= originSet, spreadingLayers= spreadingLayers)
        retval= list()
        for wl in wheelLoads:
            retval.append(wl.defNodalLoads(gravityDir= gravityDir, brakingDir= brakingDir))
        return retval
    
    
    def defDeckLoads(self, tandems, relativePositions, laneUniformLoads, originSet= None, gravityDir= xc.Vector([0,0,-1]), brakingDir= None, spreadingLayers= None):
        ''' Define punctual and uniform loads.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of each tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param laneUniformLoads: load for each notional lane [1st, 2nd, 3rd,...].
        :param originSet: set containing the nodes to pick from.
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        :param spreadingLayers: list of tuples containing the depth
                                and the spread-to-depth ratio of 
                                the layers between the wheel contact 
                                area and the middle surface of the 
                                bridge deck.
        '''
        # punctual loads.
        self.defDeckPunctualLoads(tandems= tandems, relativePositions= relativePositions, originSet= originSet, gravityDir= gravityDir, brakingDir= brakingDir, spreadingLayers= spreadingLayers)
        # uniform load.
        self.defDeckUniformLoads(laneUniformLoads= laneUniformLoads, gravityDir= gravityDir, brakingDir= brakingDir)

    def getDeckWheelLoadsThroughEmbankment(self, tandems, relativePositions, embankment, deckSpreadingRatio= 2/1.0, originSet= None):
        ''' Return a dictionary containing the wheel loads due to the tandems
            argument in the positions argument.

        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see 
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        '''
        retval= self.getWheelLoads(tandems= tandems, relativePositions= relativePositions)
        if(originSet): # pick the loaded by each wheel
            for load in retval:
                load.pickDeckNodesThroughEmbankment(originSet= originSet, embankment= embankment, deckSpreadingRatio= deckSpreadingRatio)
        return retval
    
    def defDeckPunctualLoadsThroughEmbankment(self, tandems, relativePositions, embankment, deckSpreadingRatio= 2/1.0, originSet= None, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define punctual loads under the wheels.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param embankment: embankment object as defined in
                           earthworks.embankment.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see 
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        wheelLoads= self.getDeckWheelLoadsThroughEmbankment(tandems= tandems, relativePositions= relativePositions, embankment= embankment, deckSpreadingRatio= deckSpreadingRatio, originSet= originSet)
        retval= list()
        for wl in wheelLoads:
            retval.append(wl.defNodalLoads(gravityDir= gravityDir, brakingDir= brakingDir))
        return retval
    
    def defDeckLoadsThroughEmbankment(self, tandems, relativePositions, laneUniformLoads, embankment, deckSpreadingRatio= 2/1.0, originSet= None, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define punctual and uniform loads.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of each tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param laneUniformLoads: load for each notional lane [1st, 2nd, 3rd,...].
        :param embankment: embankment object as defined in 
                           earthworks.embankment.
        :param deckSpreadingRatio: spreading ratio of the load between the deck
                                   surface and the deck mid-plane (see 
                                   clause 4.3.6 on Eurocode 1-2:2003).
        :param originSet: set containing the nodes to pick from.
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        # punctual loads.
        self.defDeckPunctualLoadsThroughEmbankment(tandems= tandems, relativePositions= relativePositions, embankment= embankment, deckSpreadingRatio=deckSpreadingRatio, originSet= originSet, gravityDir= gravityDir, brakingDir= brakingDir)
        # uniform load.
        self.defDeckUniformLoads(laneUniformLoads= laneUniformLoads, gravityDir= gravityDir, brakingDir= brakingDir)

        
    def defBackfillPunctualLoads(self, tandems, relativePositions, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define punctual loads under the wheels.
        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of the tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in 
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        backfillLoads= self.getBackfillConcentratedLoads(tandems= tandems, relativePositions= relativePositions, gravityDir= gravityDir, brakingDir= brakingDir)
        phi= embankment.layers[0].soil.phi
        sz= len(backfillLoads)
        avgLoadedAreaRatio= 0.0
        for bfl in backfillLoads:
            horizontalLoad= bfl[0]
            avgLoadedAreaRatio+= horizontalLoad.appendLoadToCurrentLoadPattern(elements= originSet.elements, phi= phi, delta= delta)
            verticalLoad= bfl[1]
            verticalLoad.appendLoadToCurrentLoadPattern(elements= originSet.elements, eta= eta, delta= delta)
        if(sz):
            avgLoadedAreaRatio/= sz
        return avgLoadedAreaRatio
    
    def defBackfillUniformLoads(self, originSet, embankment, delta, eta= 1.0, laneUniformLoads= [9e3, 2.5e3, 2.5e3], gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define uniform loads on the lanes with the argument values:

        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in 
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    implementation remarks in boussinesq module).
        :param laneUniformLoads: load for each notional lane [1st, 2nd, 3rd,...].
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        loadedAreaRatio= 0.0
        for q, lane in zip(laneUniformLoads, self.lanes):
            if(q is not None):
                # Compute load vector on each lane.
                loadVector= q*gravityDir
                if(brakingDir): # Compute braking load
                    loadVector+= q*xc.Vector(brakingDir)
                if(loadVector.Norm()>1e-6): # Not zero.
                    # Compute Boussinesq loaded area
                    ## Get contour points.
                    laneContour= lane.contour.getVertexList()
                    p1= laneContour[0]; p2= laneContour[1]
                    p3= laneContour[2]; p4= laneContour[3]
                    ## Define Boussinesq loaded area.
                    boussinesqLoadedArea= boussinesq.QuadLoadedArea(p1= p1, p2= p2, p3= p3, p4= p4, q= loadVector[2], eSize= 0.5)
                    ## Compute loads on elements.
                    boussinesqLoadedArea.appendLoadToCurrentLoadPattern(elements= originSet.elements, eta= eta, delta= delta)
                    ## Define horizontally loaded area.
                    hLoadedArea= hs.HorizontalLoadedAreaOnBackfill3D(contour= [p1, p2, p3, p4], H= geom.Vector3d(loadVector[0], loadVector[1], 0.0))
                    ## Compute loads on elements.
                    phi= embankment.layers[0].soil.phi # effective friction angle of soil.
                    loadedAreaRatio= hLoadedArea.appendLoadToCurrentLoadPattern(elements= originSet.elements, phi= phi, delta= delta)
        return loadedAreaRatio
    
    def defBackfillLoads(self, tandems, relativePositions, laneUniformLoads, originSet, embankment, delta, eta= 1.0, gravityDir= xc.Vector([0,0,-1]), brakingDir= None):
        ''' Define punctual and uniform loads.

        :param tandems: tandems on each notional lane (tandem1 -> notional 
                        lane 1, tandem 2 -> notional lane 2 and so on).
        :param relativePositions: relative positions of each tandem center in
                                  the notional lane axis (0 -> beginning of
                                  the axis, 0.5-> middle of the axis, 1-> end
                                  of the axis).
        :param laneUniformLoads: load for each notional lane [1st, 2nd, 3rd,...].
        :param originSet: set containing the elements to pick from.
        :param embankment: embankment object as defined in 
                           earthworks.embankment.
        :param delta: friction angle between the soil and the element material.
        :param eta: Poisson's ratio (ATTENTION: defaults to 1.0: see 
                    implementation remarks in boussinesq module).
        :param gravityDir: direction of the gravity field.
        :param brakingDir: direction of the braking load.
        '''
        # punctual loads.
        self.defBackfillPunctualLoads(tandems= tandems, relativePositions= relativePositions, originSet= originSet, embankment= embankment, delta= delta, eta= eta, gravityDir= gravityDir, brakingDir= brakingDir)
        # uniform load.
        self.defBackfillUniformLoads(originSet= originSet, embankment= embankment, delta= delta, eta= eta, laneUniformLoads= laneUniformLoads, gravityDir= gravityDir, brakingDir= brakingDir)


    

