# -*- coding: utf-8 -*-
''' Simple utilities to obtain earth pressures due to embankments.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "anaOrtegaOrt@gmail.com"

import sys
import math
import geom
from misc_utils import log_messages as lmsg

# Embankment data structure:
#
#      p2 +--------------------+ p3
#        /       Soil 1         \  <- Embankment layer 1: 
#       /    p6 +--------+ p7    \    (soil1, [p1, p2, p3,p4])
#      /       /          \       \
#     /       /            \       \    <- Embankment layer 2:
#    /       /   Soil 2     \       \     (soil2, [p5, p6, p7,p8])
#   +       +                +       + 
# p1      p5                  p8       p4

class EmbankmentLayer(object):
    ''' Abstraction of an embankment layer.

    :ivar soil: soil model.
    :ivar pline: plane 3D polyline representing the layer surface.
    '''
    def __init__(self, pline, soil= None, loadSpreadingRatio= 1/1):
        ''' Constructor.
        
        :param soil: soil model.
        :param pline: plane 3D polyline representing the layer surface.
        '''
        self.pline= pline
        self.soil= soil
        self.loadSpreadingRatio= loadSpreadingRatio

    def getXMin(self):
        ''' Return the minimum x coordinate of the profile.'''
        #return self.pline.getXMin
        return self.pline.getFromPoint().x

    def getXMax(self):
        ''' Return the maximum x coordinate of the profile.'''
        #return self.pline.getXMax
        return self.pline.getToPoint().x

    def getDepth(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the depth corresponding to the point argument.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        # Compute boundaries.
        xMin= self.getXMin()
        xMax= self.getXMax()
        retval= None # Not defined unless between xMin and xMax
        prj= self.pline.getPlane().getProjection(point)
        x= prj.x
        if(x<=xMax and x>=xMin): # Check boundaries.
            vLine= geom.Line3d(prj, 100*verticalDir) # Vertical line.
            intPoints= self.pline.getIntersection(vLine)
            for ip in intPoints:
                v= ip-prj # Vector from the intersection to the proj. pt.
                dist= verticalDir.dot(v) # Dot product.
                retval= dist
        return retval
        
    def getDepths(self, pointList, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the depths corresponding to the points in the list
            argument.

        :param pointList: points whose depths will be computed.
        :param verticalDir: vector pointing «up».
        '''
        # Compute boundaries.
        xMin= self.getXMin()
        xMax= self.getXMax()
        retval= list()
        for p in pointList:
            prj= self.pline.getPlane().getProjection(p)
            x= prj.x
            if(x<=xMax and x>=xMin): # Check boundaries.
                vLine= geom.Line3d(prj, 100*verticalDir) # Vertical line.
                intPoints= self.pline.getIntersection(vLine)
                for ip in intPoints:
                    v= ip-prj # Vector from the intersection to the proj. pt.
                    dist= verticalDir.dot(v) # Dot product.
                    retval.append(dist)
        return retval
    
    def getWeightVerticalStresses(self, pointList, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the vertical stresses due to the soil weight on each of the 
            points in the list argument.

        :param pointList: points whose vertical stresses will be computed.
        :param verticalDir: vector pointing «up».
        '''
        # Compute depths.
        depths= self.getDepths(pointList= pointList, verticalDir= verticalDir)
        retval= list()
        for depth in depths:
            if(depth>0.0): # under the soil surface.
                vStress= self.soil.gammaSoil()*depth
            else: # over the soil surface (in the air).
                vStress= 0.0
            retval.append(vStress)
        return retval
                
# See ascii-art describing data structure at the top of this file.
class EmbankmentCrossSection(object):
    ''' Class representing the an embankment cross-section

    :ivar layers: list of EmbankmentLayer objects.
    :ivar commonPlane: all the 3D points are supposed to be contained in
                       the same plane. That plane is stored in this variable.
    '''
    def __init__(self, embankmentLayers= None):
        ''' Constructor.

        :param embankmentLayers: list of (soil, loadSpreadingRatio, 
                                 layerSurfacePoints) tuples:
                                 soil: layer soil model.
                                 loadSpreadingRatio: V:H slope of the truncated
                                                     pyramid shape for the 
                                                     load distribution.
                                 layerSurfacePoints: sequence of 3D points 
                                                     representing the layer
                                                     surface.
        '''
        self.layers= list()
        self.commonPlane= None # All the 3D points must be on the same plane
                          # (the plane of the cross-section).
        if(embankmentLayers):
            for (soil, loadSpreadingRatio, layerSurfacePoints) in embankmentLayers:
                self.appendLayer(soil, loadSpreadingRatio, layerSurfacePoints)

    def appendLayer(self, soil, loadSpreadingRatio, layerSurfacePoints):
        ''' Append an embankment layer.

        :param soil: layer soil model.
        :param loadSpreadingRatio: V:H slope of the truncated pyramid shape for
                                   the load distribution.
        :param layerSurfacePoints: sequence of 3D points representing the layer
                                   surface.
        '''
        plane_pline_3d= geom.PlanePolyline3d(layerSurfacePoints)
        plane= plane_pline_3d.getPlane()
        if(self.commonPlane): # Check that are the same plane (or almost).
            angle= self.commonPlane.getAngle(plane)
            tol= 1e-3
            if(abs(angle)>tol and abs(angle-math.pi)>tol):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; layer surfaces don\'t share a common plane. Angle between planes: '+"{:4.2f}".format(math.degrees(angle))+' degrees')
        else: # Not set yet.
            self.commonPlane= plane
        self.layers.append(EmbankmentLayer(pline= plane_pline_3d, soil= soil, loadSpreadingRatio= loadSpreadingRatio))
        return self.layers[-1]

    def getDepths(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the depths corresponding to the point argument for
            each embankment layer.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        retval= list()
        for layer in self.layers:
            retval.append(layer.getDepth(point= point, verticalDir= verticalDir))
        return retval

    def getLayerThicknesses(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the thickness of each layer corresponding to a vertical
            borehole reaching the point argument.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        retval= list()
        rDepths= list(reversed(self.getDepths(point= point, verticalDir= verticalDir))) # From bottom to top.
        rDepths= [0.0 if x is None else x for x in rDepths] # Replace None by 0.
        previousDepth= rDepths[0] # Deepest one.
        retval.append(previousDepth)
        for d in rDepths[1:]:
            layerThickness= max(d-previousDepth,0.0) # Compute layer thickness.
            retval.append(layerThickness) 
            previousDepth= d
        return list(reversed(retval)) # Results from top to bottom.

    def getWeightVerticalStresses(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the vertical stresses due to the soil weight on each of the 
            points in the list argument.

        :param point: point whose vertical stress will be computed.
        :param verticalDir: vector pointing «up».
        '''
        # Compute thicknesses.
        thicknesses= self.getLayerThicknesses(point= point, verticalDir= verticalDir)
        retval= 0.0
        for layer, thickness in zip(self.layers, thicknesses):
            if(thickness>0.0): # thickness for this layer.
                retval+= layer.soil.gamma()*thickness
        return retval

    def getEarthPressuresAtRest(self, elements, unitVectorDirs):
        ''' Return the pressure for the elements being passed as parameter.

        :param elements: elements whose pressure on will be computed.
        :param unitVectorDirs: pressure direction vectors (must be unit vectors)
                               corresponding to the points.
        '''
        retval= list()
        if(len(self.layers)>1):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; not implemented for multiple soil layers. Using the Jacky coefficient of the first layer only.')
        k0= self.layers[0].soil.K0Jaky()# Jaky's earth pressure at rest
                                        # coefficient.
        for e, vDir in zip(elements, unitVectorDirs):
            centroid= e.getPosCentroid(True)
            weightPressure= self.getWeightVerticalStresses(point= centroid)
            lateralPressure= -k0*weightPressure*vDir
            retval.append(lateralPressure)
        return retval
