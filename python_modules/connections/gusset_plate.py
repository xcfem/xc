# -*- coding: utf-8 -*-
''' Naive gusset plate model.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import xc_base
import geom
from import_export import block_topology_entities as bte
from connections import bolted_plate

class GussetPlate(object):
    ''' Naive gusset plate model.

    :ivar gussetTip: intersection of the gusset plate with the
                      axis of its attached member.
    :ivar halfChamfer: half portion of the chamfer.
    :ivar ijkVectors: unary vectors of the local reference system.
    :ivar boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                arrangement.
    :ivar contour: list of 3D points defining the gusset contour.
    '''
    def __init__(self, boltedPlateTemplate, gussetTip, halfChamfer, ijkVectors):
        ''' Constructor.

        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        :param gussetTip: intersection of the gusset plate with the
                          axis of its attached member.
        :param halfChamfer: half portion of the chamfer.
        :param ijkVectors: unary vectors of the local reference system.
        '''
        self.gussetTip= gussetTip
        self.halfChamfer= halfChamfer
        self.iVector= ijkVectors[0]
        self.jVector= ijkVectors[1]
        self.kVector= ijkVectors[2]
        self.boltedPlateTemplate= boltedPlateTemplate

    def getOrientation(self, origin):
        ''' Return the gusset plate orientation with respect to the
            origin.

        :param origin: connection origin.
        :return: 1 if the iVector is oriented fromwards the origin argument.
                -1 if the iVector is orientes towards the origin argument.
        '''
        retval= 1.0
        d0= origin.dist2(self.gussetTip)
        p1= self.gussetTip+10.0*self.iVector
        d1= origin.dist2(p1)
        if(d0>d1):
            retval= -1.0
        return retval
    
    def getThickness(self):
        ''' Return the plate thickess.'''
        return self.boltedPlateTemplate.thickness
    
    def getHorizontalTopLeg(self, origin):
        '''Return the points of an horizontal top leg.

        :param origin: connection origin.
        '''
        p1= self.gussetTip+self.halfChamfer
        p2= geom.Pos3d(origin.x,origin.y,p1.z) # Horizontal leg.
        return p1, p2
        
    def getSlopedTopLeg(self, slope, legLength):
        '''Return the points of a sloped top leg.

        :param legLength: length of the returned leg.
        '''
        p1= self.gussetTip+self.halfChamfer
        halfChamferVector= self.halfChamfer.normalized()
        p2= p1-legLength*(self.iVector-slope*halfChamferVector).normalized() # Sloped leg.
        return p1, p2

    def getSlopedBottomLeg(self, slope, legLength):
        '''Return the points of a sloped top leg.

        :param legLength: length of the returned leg.
        '''
        p1= self.gussetTip-self.halfChamfer
        halfChamferVector= self.halfChamfer.normalized()
        p2= p1-legLength*(self.iVector+slope*halfChamferVector).normalized() # Sloped leg.
        return p1, p2
    
    def getVerticalBottomLeg(self, origin):
        '''Return the points of a verticol bottom leg.

        :param origin: connection origin.
        '''
        p1= self.gussetTip-self.halfChamfer
        p2= geom.Pos3d(p1.x,p1.y,origin.z) # Vertical leg.
        return p1, p2
    
    def getToColumnBottomLeg(self, p0, cutKnifePoint= 1.0):
        '''Return the points of a verticol bottom leg.

        :param p0: intersection of the member axis with the column.
        :param cutKnifePoint: if <1 cuts the point of the knife at
                              the contact of the leg with the column
                              surface.
        '''
        p1= self.gussetTip-self.halfChamfer
        tmp= p0-geom.Vector3d(0.0,0.0,p0.z) # vertical of the intersection
                                            # with the column.
        if(cutKnifePoint==1.0):
            p2= tmp
            p3= None
        else:
            knifeEdge= geom.Segment3d(p1, tmp)
            p3= p0-geom.Vector3d(0.0,0.0,cutKnifePoint*p0.z)
            # Horizontal plane through p3
            planeP3= geom.Plane3d(p3, geom.Vector3d(1,0,0), geom.Vector3d(0,1,0))
            # Cut the point
            p2= planeP3.getIntersection(knifeEdge)
        return p1, p2, p3

    def setContour(self, pointList):
        ''' Set the points that define the contour of the gusset plate.

        :param pointList: list of 3D points defining the gusset contour.
        '''
        self.contour= pointList

    def getBoltRefSys(self):
        ''' Return the reference system for the bolt array.'''
        boltCenter= self.gussetTip-self.iVector*(self.boltedPlateTemplate.length/2.0)
        halfChamferVector= self.halfChamfer.normalized()
        return geom.Ref3d3d(boltCenter, self.iVector, halfChamferVector)

    def getBoltPositions(self):
        ''' Return the positions of the bolts.'''
        boltRefSys= self.getBoltRefSys()
        return self.boltedPlateTemplate.boltArray.getPositions(boltRefSys)

    def getWeldLinesIndexes(self, verticalWeldLegSize, horizontalWeldLegSize):
        ''' Return the indexes of the points in the contour
            that define the lines that must be welded to the structure.

        :param verticalWeldLegSize: leg size for the vertical welds.
        :param horizontalWeldLegSize: leg size for the horizontal welds.
        '''
        retval= list()
        retval.append((1, 2, verticalWeldLegSize)) # vertical weld
        retval.append((2, 3, horizontalWeldLegSize)) # horizontal weld.
        return retval

    def getHoleBlocks(self, ownerId, blockProperties= None):
        ''' Return the blocks representing the holes for the bolts.

        :param ownerId: identifier of the face with the holes.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        holeProperties= bpe.BlockProperties.copyFrom(blockProperties)
        holeProperties.appendAttribute('objType', 'hole')
        holeProperties.appendAttribute('ownerId', ownerId) # Hole owner id.
        boltRefSys= self.getBoltRefSys()
        return self.boltedPlateTemplate.boltArray.getHoleBlocks(boltRefSys,holeProperties)
    
    def getWeldBlocks(self, ownerId, kPointIds, verticalWeldLegSize, horizontalWeldLegSize, blockProperties= None):
        ''' Return the blocks representing the welds.

        :param ownerId: identifier of the face with the welds.
        :param kPointIds: identifiers of the points at weld ends.
        :param verticalWeldLegSize: leg size for the vertical welds.
        :param horizontalWeldLegSize: leg size for the horizontal welds.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        weldProperties= bpe.BlockProperties.copyFrom(blockProperties)
        weldProperties.appendAttribute('objType', 'weld')
        weldProperties.appendAttribute('ownerId', ownerId) # Weld owner id.
        weldLinesIndexes= self.getWeldLinesIndexes(verticalWeldLegSize, horizontalWeldLegSize)
        for l in weldLinesIndexes:
            pA= kPointIds[l[0]]
            pB= kPointIds[l[1]]
            weldLegSize= l[2]
            weldProperties.appendAttribute('legSize', weldLegSize)
            weldBlk= bte.BlockRecord(id= -1, typ= 'line', kPoints= [pA, pB], blockProperties= weldProperties, thk= None)
            retval.appendBlock(weldBlk)
        return retval
        
    def getGussetBlocks(self, verticalWeldLegSize, horizontalWeldLegSize, blockProperties= None):
        ''' Return the blocks that define the gusset.

        :param verticalWeldLegSize: leg size for the vertical welds.
        :param horizontalWeldLegSize: leg size for the horizontal welds.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        gussetPlateProperties= bpe.BlockProperties.copyFrom(blockProperties)
        gussetPlateProperties.appendAttribute('objType', 'gusset_plate')
        blk= retval.blockFromPoints(self.contour, blockProperties= gussetPlateProperties, thickness= self.boltedPlateTemplate.thickness, matId= self.boltedPlateTemplate.steelType.name)
        holeProperties= bpe.BlockProperties.copyFrom(blockProperties)
        ownerId= 'f'+str(blk.id)
        holeProperties.appendAttribute('ownerId', ownerId) # Hole owner id.
        blk.holes= self.getHoleBlocks(ownerId, blockProperties= holeProperties) # Get the hole blocks for the new plate
        retval.extend(blk.holes)
        kPointIds= blk.getKPointIds()
        blk.weldBlocks= self.getWeldBlocks(ownerId, kPointIds, verticalWeldLegSize, horizontalWeldLegSize, blockProperties) # Get the weld blocks for the new plate
        retval.extend(blk.weldBlocks)
        return retval

    def getBoltedPlateBlocks(self, boltedPlate, diagonal, origin, blockProperties, side= 1):
        ''' Return the blocks corresponding to the plate
            bolted to the gusset plate.

        :param boltedPlate: bolted plate that will be attached 
                            to this one.
        :param diagonal: element that provide the internal forces in the
                         bolted plate edge.
        :param blockProperties: labels and attributes to assign to the newly created blocks.
        :param side: if side==1 put the plate at the positive side of the
                     gusset plate reference system, if side==-1 put the
                     plate at the negative side.

        :return: return the distance between the plates midplanes
                 and the blocks corresponding to the bolted plate.
        '''
        # Move the reference system.
        boltedPlateRefSys= self.getBoltRefSys()
        distBetweenPlates= (self.getThickness()+boltedPlate.thickness)*0.5*side
        vMove= boltedPlateRefSys.getKVector()*distBetweenPlates
        boltedPlateRefSys.Org+= vMove
        # Get loads on the plate edge.
        loadTag= diagonal.eTag
        diagonalOrientation= diagonal.getOrientation(origin)
        loadDirI= diagonalOrientation*diagonal.iVector
        loadDirJ= diagonalOrientation*diagonal.jVector
        loadDirK= diagonalOrientation*diagonal.kVector
        plateProperties= bte.BlockProperties.copyFrom(blockProperties)
        plateProperties.appendAttribute('side', side)
        # Create blocks.
        return distBetweenPlates, boltedPlate.getBlocks(boltedPlateRefSys, plateProperties, loadTag, loadDirI, loadDirJ, loadDirK)

    def getBlocks(self, verticalWeldLegSize, horizontalWeldLegSize, boltedPlate, diagonal, origin, blockProperties):
        ''' Return the blocks corresponding to the gusset plate connection
            and the single or double plates bolted to it.


        :param verticalWeldLegSize: leg size for the vertical welds.
        :param horizontalWeldLegSize: leg size for the horizontal welds.
        :param boltedPlate: bolted plate that will be attached 
                            to this one.
        :param diagonal: element that provide the internal forces in the
                         bolted plate edge.
        :param blockProperties: labels and attributes to assign to the newly created blocks.

        :return: return the blocks corresponding to the connection.
        '''
        retval= bte.BlockData()
        gussetPlateBlocks= self.getGussetBlocks(verticalWeldLegSize, horizontalWeldLegSize, blockProperties)
        retval.extend(gussetPlateBlocks)
        distBetweenPlates1, attachedPlateBlocks1= self.getBoltedPlateBlocks(boltedPlate, diagonal, origin, blockProperties, side= 1) # bolted plate at positive side.
        retval.extend(attachedPlateBlocks1)
        boltBlocks1= bolted_plate.getBoltedPointBlocks(gussetPlateBlocks, attachedPlateBlocks1, abs(distBetweenPlates1)) # points linked by bolts.
        retval.extend(boltBlocks1)
        if(self.boltedPlateTemplate.doublePlate):
            distBetweenPlates2, attachedPlateBlocks2= self.getBoltedPlateBlocks(boltedPlate, diagonal, origin, blockProperties, side= -1) # bolted plate at negative side.
            retval.extend(attachedPlateBlocks2)
            boltBlocks2= bolted_plate.getBoltedPointBlocks(gussetPlateBlocks, attachedPlateBlocks2, abs(distBetweenPlates2)) # points linked by bolts.
            retval.extend(boltBlocks2)            
        return retval
