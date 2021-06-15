# -*- coding: utf-8 -*-
''' Bolt fasteners basic properties.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import xc_base
import geom
import importlib
import scipy.interpolate
from import_export import block_topology_entities as bte

class Nut(object):
    # Bolt nominal diameters (m)
    diameters= [1.6e-3, 2e-3, 2.5e-3, 3e-3, 3.5e-3, 4e-3, 5e-3, 6e-3, 8e-3, 10e-3, 12e-3, 14e-3, 16e-3, 18e-3, 20e-3, 22e-3, 24e-3, 27e-3, 30e-3, 33e-3, 36e-3, 39e-3, 42e-3, 45e-3, 48e-3, 52e-3, 56e-3, 60e-3, 64e-3]
    # Maximum width across flats (m)
    s_max= [3.2e-3, 4e-3, 5e-3, 5.5e-3, 6e-3, 7e-3, 8e-3, 10e-3, 13e-3, 16e-3, 18e-3, 21e-3, 24e-3, 27e-3, 30e-3, 34e-3, 36e-3, 41e-3, 46e-3, 50e-3, 55e-3, 60e-3, 65e-3, 70e-3, 75e-3, 80e-3, 85e-3, 90e-3, 95e-3]
    fs_max= scipy.interpolate.interp1d(diameters,s_max)
    # Minimum widht across flats (m)
    s_min= [3.02e-3, 3.82e-3, 4.82e-3, 5.32e-3, 5.82e-3, 6.78e-3, 7.78e-3, 9.78e-3, 12.73e-3, 15.73e-3, 17.73e-3, 20.67e-3, 23.67e-3, 26.16e-3, 29.16e-3, 33e-3, 35e-3, 40e-3, 45e-3, 49e-3, 53.8e-3, 58.8e-3, 63.1e-3, 68.1e-3, 73.1e-3, 78.1e-3, 82.8e-3, 87.8e-3, 92.8e-3]
    fs_min= scipy.interpolate.interp1d(diameters,s_min)
    # Width across corners (m)
    e= [3.41e-3, 4.32e-3, 5.45e-3, 6.01e-3, 6.58e-3, 7.66e-3, 8.79e-3, 11.05e-3, 14.38e-3, 17.77e-3, 20.03e-3, 23.36e-3, 26.75e-3, 29.56e-3, 32.95e-3, 37.29e-3, 39.55e-3, 45.2e-3, 50.85e-3, 55.37e-3, 60.79e-3, 66.44e-3, 71.3e-3, 76.95e-3, 82.6e-3, 88.25e-3, 93.56e-3, 99.21e-3, 104.86e-3]
    fe= scipy.interpolate.interp1d(diameters,e)
    # Maximum height of the nut (m).
    m_max= [1.3e-3, 1.6e-3, 2e-3, 2.4e-3, 2.8e-3, 3.2e-3, 4.7e-3, 5.2e-3, 6.8e-3, 8.4e-3, 10.8e-3, 12.8e-3, 14.8e-3, 15.8e-3, 18e-3, 19.4e-3, 21.5e-3, 23.8e-3, 25.6e-3, 28.7e-3, 31e-3, 33.4e-3, 34e-3, 36e-3, 38e-3, 42e-3, 45e-3, 48e-3, 51e-3]
    fm_max= scipy.interpolate.interp1d(diameters,m_max)
    # Minimum height of the nut (m).
    m_min= [1.05e-3, 1.35e-3, 1.75e-3, 2.15e-3, 2.55e-3, 2.9e-3, 4.4e-3, 4.9e-3, 6.44e-3, 8.04e-3, 10.37e-3, 12.1e-3, 14.1e-3, 15.1e-3, 16.9e-3, 18.1e-3, 20.2e-3, 22.5e-3, 24.3e-3, 27.4e-3, 29.4e-3, 31.8e-3, 32.4e-3, 34.4e-3, 36.4e-3, 40.4e-3, 43.4e-3, 46.4e-3, 49.1e-3]
    fm_min= scipy.interpolate.interp1d(diameters,m_min)
    ''' Nut dimensions.

    :ivar boltDiameter: nominal diameter of the bolt.
    :ivar m: height of the nut.
    '''
    def __init__(self, nominalBoltDiameter):
        ''' Constructor.
        :param boltDiameter: nominal diameter of the bolt.
        '''
        self.boltDiameter= nominalBoltDiameter

    def getMaxWidthAcrossFlats(self):
        ''' Return the maximum width across flats of the nut.'''
        return float(self.fs_max(self.boltDiameter))

    def getMinWidthAcrossFlats(self):
        ''' Return the minimum width across flats of the nut.'''
        return float(self.fs_min(self.boltDiameter))
    
    def getMaxHeight(self):
        ''' Return the maximum height.'''
        return float(self.fm_max(self.boltDiameter))

    def getMinHeight(self):
        ''' Return the minimum height.'''
        return float(self.fm_min(self.boltDiameter))

    def getWidthAcrossCorners(self):
        ''' Return the width across corners.'''
        return float(self.fe(self.boltDiameter))
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= {'boltDiameter':self.boltDiameter}
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.boltDiameter= dct['boltDiameter']
        
class BoltBase(object):
    ''' Base class for bolts.

    :ivar diameter: bolt diameter.
    :ivar pos3d: bolt position.
    '''
    def __init__(self, diameter, pos3d= None):
       ''' Constructor.

       :param diameter: bolt diameter.
       :param pos3d: bolt position.
       '''
       self.diameter= diameter
       if(pos3d):
           self.pos3d= pos3d
       else:
           self.pos3d= None

    def getNut(self):
        ''' Return a nut object that corresponds to this bolt.'''
        return Nut(self.diameter)
    
    def getMinDistanceBetweenCenters(self):
        ''' Return the minimum distance between centers.'''
        raise NotImplementedError()
    
    def getRecommendedDistanceBetweenCenters(self):
        ''' Return the recommended distance between centers.'''
        raise NotImplementedError()
           
    def getMinimumEdgeDistance(self):
        ''' Return the minimum edge distance from center of standard 
            hole to edge of connected part.'''
        raise NotImplementedError()
    
    def getNominalHoleDiameter(self):
        ''' Return the nominal hole diameter for the bolt.'''
        raise NotImplementedError()
    
    def getDesignHoleDiameter(self):
        ''' Return the hole diameter to use in net area calculations.'''
        raise NotImplementedError()
    
    def getName(self):
        return 'M'+str(self.diameter*1e3)[0:2]

    def getArea(self):
        ''' Return the area of the bolt.
        '''
        return math.pi*(self.diameter/2.0)**2
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= {'diameter':self.diameter}
        xyz= None
        if(self.pos3d):
            xyz= (self.pos3d.x, self.pos3d.y, self.pos3d.z)
        retval.update({'pos3d': xyz})
        return retval

    def setFromDict(self, dct):
        ''' Read member values from a dictionary.

        :param dct: dictionary to read the values from.
        '''
        self.diameter= dct['diameter']
        self.pos3d= None
        if('pos3d' in dct):
            xyz= dct['pos3d']
            if(xyz):
                self.pos3d= geom.Pos3d(xyz[0], xyz[1], xyz[2])

    def getHole(self, refSys= geom.Ref3d3d()):
        ''' Return a circle in the hole position.

        :param refSys: coordinate reference system.
        '''
        pLocal= geom.Pos2d(self.pos3d.x, self.pos3d.y)
        holeDiameter= self.getNominalHoleDiameter()
        circle2d= geom.Circle2d(pLocal,holeDiameter/2.0)
        return geom.Circle3d(refSys, circle2d)

    def getHoleAsPolygon(self, refSys= geom.Ref3d3d(), nSides= 8):
        ''' Return a polygon inscribed in the hole.

        :param refSys: coordinate reference system.
        :param blockProperties: number of sides for the polygon representing the hole contour.
       '''
        hole= self.getHole(refSys)
        retval= hole.getInscribedPolygon(8,0.0)
        return retval

    def getHoleCenterBlockProperties(self, blockProperties, ownerId):
        ''' Return the labels and attributes for the hole center.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        # Hole center.
        retval= bte.BlockProperties.copyFrom(blockProperties)
        retval.appendAttribute('objType', 'hole_center')
        retval.appendAttribute('diameter', self.diameter)
        retval.appendAttribute('boltMaterial', self.steelType.name)
        retval.appendAttribute('ownerId', 'f'+str(ownerId)) # Hole center owner.
        return retval
        
    def getBoltBlock(self, refSys= geom.Ref3d3d(), blockProperties= None):
        ''' Return the hole contour and hole center as block topology entities.

        :param refSys: coordinate reference system.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        # Hole vertices.
        holeProperties= bte.BlockProperties.copyFrom(blockProperties)
        holeProperties.appendAttribute('objType', 'hole')
        octagon= self.getHoleAsPolygon(refSys, nSides= 8).getVertexList()
        blk= retval.blockFromPoints(octagon, holeProperties)
        # Hole center.
        centerProperties= self.getHoleCenterBlockProperties(blockProperties, blk.id)
        center3d= refSys.getGlobalPosition(self.pos3d)
        pA= retval.appendPoint(-1, center3d.x, center3d.y, center3d.z, pointProperties= centerProperties)
        retval.topPoint= retval.points[pA]
        return retval
    
    def report(self, outputFile):
        ''' Reports bolt design values.'''
        outputFile.write('     diameter: '+str(self.diameter*1000)+' mm\n')

class AnchorBase(BoltBase):
    ''' Base class for anchor bolts.

    :ivar plateWasher: plate washer (if any).
    '''
    def __init__(self, diameter, pos3d= None, plateWasher= None):
       ''' Constructor.

       :param steel: steel material.
       :param diameter: bolt diameter.
       :param pos3d: bolt position.
       :param plateWasher: plate washer (if any).
       '''
       super(AnchorBase,self).__init__(diameter, pos3d)
       self.plateWasher= plateWasher
       
    def getAnchorPointBlockProperties(self, blockProperties):
        ''' Return the labels and attributes for the anchor point.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        # Hole center.
        retval= bte.BlockProperties.copyFrom(blockProperties)
        retval.appendAttribute('objType', 'anchor_point')
        retval.appendAttribute('diameter', self.diameter)
        retval.appendAttribute('boltMaterial', self.steelType.name)
        retval.appendAttribute('ownerId', None) # Has no owner.
        return retval
    
    def getAnchorBlock(self, refSys= geom.Ref3d3d(), blockProperties= None):
        ''' Return the hole contour and hole center as block topology entities.

        :param refSys: coordinate reference system.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        blk= super(AnchorBase, self).getBoltBlock(refSys, blockProperties)
        retval.extend(blk)
        # Anchor point.
        anchorPointProperties= self.getAnchorPointBlockProperties(blockProperties)
        pA= blk.topPoint
        center3d= geom.Pos3d(pA.coords[0], pA.coords[1], pA.coords[2])
        pB= retval.appendPoint(-1, center3d.x, center3d.y, center3d.z-1*self.diameter, pointProperties= anchorPointProperties)
        boltBlk= bte.BlockRecord(id= -1, typ= 'line', kPoints= [pA.id, pB])
        id= retval.appendBlock(boltBlk)
        return retval
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(AnchorBase, self).getDict()
        if(self.plateWasher):
            retval['plateWasher']= self.plateWasher.getDict()
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(AnchorBase, self).setFromDict(dct)
        if('plateWasher' in dct):
            self.plateWasher= spw.SquarePlateWasher()
            self.plateWasher.setFromDict(dct['plateWasher'])


def createHolesOnMemberBlocks(templateHoles, memberBlocks, boltProperties, materialsModuleName):
    ''' Projects the holes in the argument onto the surfaces
        of the member blocks to create the holes in those
        surfaces.

    :param templateHoles: holes that will be projected on the
                          member surfaces, typically from a bolted
                          plate that will be attached to the
                          member.
    :param memberBlocks: surfaces corresponding to the member plates:
                          flanges, web, etc. Those surfaces will receive
                          the new holes into them.
    :param blockProperties: labels and attributes to assign to the newly created blocks.
    :param materialsModuleName: name of the module that contains the material for the new bolts.
    '''
    retval= bte.BlockData()
    # Import the materials module
    matModule= importlib.import_module(materialsModuleName)
    for holes in templateHoles:
        for key in holes.points:
            p= holes.points[key]
            objType= p.getAttribute('objType')
            if(objType=='hole_center'):
                pos= geom.Pos3d(p.getX(), p.getY(), p.getZ())
                boltDiameter= p.getAttribute('diameter')
                boltMaterialId= p.getAttribute('boltMaterial')
                boltMaterial= getattr(matModule,boltMaterialId)
                nearestFace= memberBlocks.getNearest(pos)
                boltProperties.appendAttribute('ownerId','f'+str(nearestFace.id)) # Hole owner
                nearestFacePlane= nearestFace.getGeomObject(memberBlocks.points).getPlane()
                posInFlange= nearestFacePlane.getProjection(pos)
                refSys= geom.Ref3d3d(posInFlange, nearestFacePlane.getBase1(), nearestFacePlane.getBase2())
                bolt= matModule.BoltFastener(diameter= boltDiameter, steelType= boltMaterial, pos3d= geom.Pos3d(0,0,0))
                boltBlk= bolt.getBoltBlock(refSys, boltProperties)
                retval.extend(boltBlk)
    return retval
