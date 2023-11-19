# -*- coding: utf-8 -*-
''' Simple utilities to modelize layers of terrain.'''

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
from scipy.constants import g

# Terrain data structure:
#
#
# + p5    + p1
# \       \
#  \      \
#   \     \ p2
#    \    +--------------------+ p3
#     \       Soil 1             \  <- Terrain layer 1: 
#  p6 +-------------------+ p7   \    (soil1, [p1, p2, p3,p4])
#                         \       \
#                          \       \    <- Terrain layer 2:
#                           \       \     (soil2, [p5, p6, p7,p8])
#                            +       + 
#                             p8       p4

class Layer(object):
    ''' Base class for terrain and water table layers.

    :ivar pline: plane 3D polyline representing the layer top surface.
    '''
    def __init__(self, pline):
        ''' Constructor.
        
        :param pline: plane 3D polyline representing the layer top surface.
        '''
        self.pline= pline

    def getDepth(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the depth corresponding to the point argument.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        retval= None        
        plane= self.pline.getPlane()
        prj= plane.getProjection(point)
        vLine= geom.Line3d(prj, 100*verticalDir) # Vertical line.
        if(self.pline.intersects(vLine)):
            intPoints= self.pline.getIntersection(vLine)
            sz= len(intPoints) # number of intersection points.
            if(sz==0): # none
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; something went wrong. No intersection found.')
            else:
                if(sz>1): # multiple.
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.warning(className+'.'+methodName+'; multiple intersection with layer surface. Returning depth with respect the first one.')
                ip= intPoints[0] # intersection point.
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
        plane= self.pline.getPlane()
        retval= list()
        for p in pointList:
            prj= plane.getProjection(p)
            vLine= geom.Line3d(prj, 100*verticalDir) # Vertical line.
            if(self.pline.intersects(vLine)):
                intPoints= self.pline.getIntersection(vLine)
                sz= len(intPoints) # number of intersection points.
                if(sz==0): # none
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+'; something went wrong. No intersection found.')
                else:
                    ip= intPoints[0] # intersection point.
                    if(sz>1): # multiple.
                        dist= ip.dist(intPoints[1])
                        if(dist>1e-6): # is not the same point.
                            className= type(self).__name__
                            methodName= sys._getframe(0).f_code.co_name
                            lmsg.warning(className+'.'+methodName+'; multiple: ('+str(sz)+') intersection with layer surface. Returning depth with respect the first one.')
                    v= ip-prj # Vector from the intersection to the proj. pt.
                    dist= verticalDir.dot(v) # Dot product.
                    retval.append(dist)
        return retval


class WaterTable(Layer):
    ''' Abstraction of a water table.

    :ivar soil: soil model.
    '''
    def __init__(self, pline, waterDensity= 1e3):
        ''' Constructor.
        
        :param pline: plane 3D polyline representing the layer top surface.
        :param waterDensity: density of the water.
        '''
        super().__init__(pline= pline)
        self.waterDensity= waterDensity

    def getPressure(self, point, verticalDir= geom.Vector3d(0,0,1), capillaryAction= False):
        ''' Return the pressure corresponding to the point argument.

        :param point: point whose pressure will be computed.
        :param verticalDir: vector pointing «up».
        :param capillaryAction: if true compute also negative pressures.
        '''
        retval= 0.0
        depth= self.getDepth(point= point, verticalDir= verticalDir)
        if(capillaryAction):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; capillary action not implemented yet')
        if(depth>0.0):
            retval= depth*g*self.waterDensity
        return retval
    
class TerrainLayer(Layer):
    ''' Abstraction of a terrain layer.

    :ivar soil: soil model.
    '''
    def __init__(self, pline, soil= None):
        ''' Constructor.
        
        :param soil: soil model.
        :param pline: plane 3D polyline representing the layer top surface.
        '''
        super().__init__(pline= pline)
        self.soil= soil
            
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
class TerrainStrata(object):
    ''' Class representing the an terrain cross-section

    :ivar layers: list of TerrainLayer objects.
    :ivar commonPlane: all the 3D points are supposed to be contained in
                       the same plane. That plane is stored in this variable.
    '''
    def __init__(self, terrainLayers= None):
        ''' Constructor.

        :param terrainLayers: list of (soil, layerSurfacePoints) tuples:
                                 soil: layer soil model.
                                 layerSurfacePoints: sequence of 3D points 
                                                     representing the layer
                                                     surface.
        '''
        self.layers= list()
        self.commonPlane= None # All the 3D points must be on the same plane
                          # (the plane of the cross-section).
        if(terrainLayers):
            for (soil, layerSurfacePoints) in terrainLayers:
                self.appendLayer(soil, layerSurfacePoints)
        self.waterTable= None

    def appendLayer(self, soil, layerSurfacePoints):
        ''' Append an terrain layer.

        :param soil: layer soil model.
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
        self.layers.append(TerrainLayer(pline= plane_pline_3d, soil= soil))
        return self.layers[-1]

    def appendWaterTable(self, waterSurfacePoints, waterDensity= 1e3):
        ''' Append the water table surface defined by the argument points.

        :param waterSurfacePoints: sequence of 3D points representing the water
                                   table surface.
        :param waterDensity: density of the water.
        '''
        plane_pline_3d= geom.PlanePolyline3d(waterSurfacePoints)
        plane= plane_pline_3d.getPlane()
        if(self.commonPlane): # Check that are the same plane (or almost).
            angle= self.commonPlane.getAngle(plane)
            tol= 1e-3
            if(abs(angle)>tol and abs(angle-math.pi)>tol):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; waterr surfaces don\'t share a common plane with previous layers. Angle between planes: '+"{:4.2f}".format(math.degrees(angle))+' degrees')
        else: # Not set yet.
            self.commonPlane= plane
        self.waterTable= WaterTable(pline= plane_pline_3d, waterDensity= waterDensity)

    def getLayerAtPoint(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the layer that corresponds to the point argument.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        retval= None
        if(self.layers):
            rLayers= list(reversed(self.layers))
            for layer in rLayers:
                depth= layer.getDepth(point= point, verticalDir= verticalDir)
                if(depth>=0.0):
                    retval= layer
                    break
        return retval
    
    def getDepths(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the depths corresponding to the point argument for
            each terrain layer.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        retval= list()
        if(self.layers):
            for layer in self.layers:
                depth= layer.getDepth(point= point, verticalDir= verticalDir)
                if(depth is None):
                    depth= 0.0
                if(depth>=0.0):
                    retval.append(depth)
        return retval
    
    def getLayerThicknesses(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the thickness of each layer corresponding to a vertical
            borehole reaching the point argument.

        :param point: point whose depth will be computed.
        :param verticalDir: vector pointing «up».
        '''
        retval= list()
        depths= self.getDepths(point= point, verticalDir= verticalDir)
        if(depths):
            rDepths= list(reversed(depths)) # From bottom to top.
            rDepths= [0.0 if x is None else x for x in rDepths] # Replace None by 0.
            previousDepth= rDepths[0] # Deepest one.
            retval.append(previousDepth)
            for d in rDepths[1:]:
                layerThickness= max(d-previousDepth,0.0) # Compute layer thickness.
                retval.append(layerThickness) 
                previousDepth= d
            retval= list(reversed(retval)) # Results from top to bottom.
        return retval
    
    def getWeightVerticalStress(self, point, verticalDir= geom.Vector3d(0,0,1)):
        ''' Return the vertical stress due to the soil weight on each of the 
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
        if(self.waterTable):
            waterPressure= self.waterTable.getPressure(point= point, verticalDir= verticalDir)
            retval-= waterPressure
        return retval
    
    def getLateralPressuresOnPoints(self, points, unitVectorDirs, k):
        ''' Return the lateral pressure for the points being passed as 
            parameter due to the vertical stress introduced by the
            embankment.

        :param elements: elements whose pressure on will be computed.
        :param unitVectorDirs: pressure direction vectors (must be unit vectors)
                               corresponding to the points.
        :param k: coefficient of lateral earth pressure, k, defined as the 
                  ratio of the horizontal effective stress, to the vertical 
                  effective stress.
        '''
        retval= list()
        weightPressures= list()
        for point in points:
            weightPressure= self.getWeightVerticalStress(point= point)
            weightPressures.append(weightPressure)
        lateralPressures= list()
        if(isinstance(k, list)):
            for ki, weightPressure in zip(k,weightPressures):
                lateralPressure= ki*weightPressure
                lateralPressures.append(lateralPressure)
        else:
            for weightPressure in weightPressures:
                lateralPressure= k*weightPressure
                lateralPressures.append(lateralPressure)
        if(unitVectorDirs is not None):
            for pressure, vDir in zip(lateralPressures, unitVectorDirs):
                retval.append(-pressure*vDir)
        else:
            retval= lateralPressures
        return retval
    
    def getEarthPressuresOnPointGrid(self, pointGrid, unitVectorDirs, k):
        ''' Return the pressure for the elements being passed as parameter.

        :param pointGrid: array of 3D points (see geom.PosArray3d) whose
                          quadrilaterals will stand the pressure.
        :param unitVectorDirs: pressure direction vectors (must be unit vectors)
                               corresponding to the centroids of the grid.
        :param k: coefficient of lateral earth pressure, k, defined as the 
                  ratio of the horizontal effective stress, to the vertical 
                  effective stress.
        '''
        points= list()
        centroids= pointGrid.getQuadCentroids()
        for centroid in centroids:
            points.append(centroid)
        return self.getLateralPressuresOnPoints(points, unitVectorDirs, k)
    
    def getEarthPressuresOnElements(self, elements, unitVectorDirs, k):
        ''' Return the pressure for the elements being passed as parameter.

        :param elements: elements whose pressure on will be computed.
        :param unitVectorDirs: pressure direction vectors (must be unit vectors)
                               corresponding to the points.
        :param k: coefficient of lateral earth pressure, k, defined as the 
                  ratio of the horizontal effective stress, to the vertical 
                  effective stress.
        '''
        points= list()
        for e in elements:
            centroid= e.getPosCentroid(True)
            points.append(centroid)
        return self.getLateralPressuresOnPoints(points, unitVectorDirs, k)
