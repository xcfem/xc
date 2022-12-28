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
from geotechnics import terrain_layers
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

class EmbankmentLayer(terrain_layers.TerrainLayer):
    ''' Abstraction of an embankment layer.

    :ivar soil: soil model.
    :ivar pline: plane 3D polyline representing the layer top surface.
    :ivar loadSpreadingRatio: ratio of load spreading for this layer.
    '''
    def __init__(self, pline, soil= None, loadSpreadingRatio= 1/1):
        ''' Constructor.
        
        :param soil: soil model.
        :param pline: plane 3D polyline representing the layer top surface.
        :param loadSpreadingRatio: ratio of load spreading for this layer.
        '''
        super().__init__(pline= pline, soil= soil)
        self.loadSpreadingRatio= loadSpreadingRatio

                
# See ascii-art describing data structure at the top of this file.
class EmbankmentCrossSection(terrain_layers.TerrainStrata):
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
        super().__init__(terrainLayers= None)
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


