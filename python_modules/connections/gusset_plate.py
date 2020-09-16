# -*- coding: utf-8 -*-
''' Naive gusset plate model.'''

from __future__ import division
from __future__ import print_function

import xc_base
import geom
from import_export import block_topology_entities as bte

class GussetPlate(object):
    ''' Naive gusset plate model.

    :ivar gussetTip: intersection of the gusset plate with the
                      axis of its attached member.
    :ivar halfChamfer: half portion of the chamfer.
    :ivar direction: direction of the attached member.
    :ivar boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                arrangement.
    :ivar contour: list of 3D points defining the gusset contour.
    '''
    def __init__(self, boltedPlateTemplate, gussetTip, halfChamfer, direction):
        ''' Constructor.

        :param boltedPlateTemplate: bolted plate dimensions and bolt type and 
                                    arrangement.
        :param gussetTip: intersection of the gusset plate with the
                          axis of its attached member.
        :param halfChamfer: half portion of the chamfer.
        :param direction: direction of the attached member.
        '''
        self.gussetTip= gussetTip
        self.halfChamfer= halfChamfer
        self.direction= direction
        self.boltedPlateTemplate= boltedPlateTemplate

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
        p2= p1-legLength*(self.direction-slope*halfChamferVector).normalized() # Sloped leg.
        return p1, p2

    def getSlopedBottomLeg(self, slope, legLength):
        '''Return the points of a sloped top leg.

        :param legLength: length of the returned leg.
        '''
        p1= self.gussetTip-self.halfChamfer
        halfChamferVector= self.halfChamfer.normalized()
        p2= p1-legLength*(self.direction+slope*halfChamferVector).normalized() # Sloped leg.
        return p1, p2
    
    def getVerticalBottomLeg(self, origin):
        '''Return the points of a verticol bottom leg.

        :param origin: connection origin.
        '''
        p1= self.gussetTip-self.halfChamfer
        p2= geom.Pos3d(p1.x,p1.y,origin.z) # Vertical leg.
        return p1, p2
    
    def getToColumnBottomLeg(self, p0):
        '''Return the points of a verticol bottom leg.

        :param p0: intersection of the member axis with the column.
        '''
        p1= self.gussetTip-self.halfChamfer
        p2= p0-geom.Vector3d(0.0,0.0,p0.z) # vertical of the intersection with the column.
        return p1, p2

    def setContour(self, pointList):
        ''' Set the points that define the contour of the gusset plate.

        :param pointList: list of 3D points defining the gusset contour.
        '''
        self.contour= pointList

    def getBoltRefSys(self):
        ''' Return the reference system for the bolt array.'''
        boltCenter= self.gussetTip-self.direction*(self.boltedPlateTemplate.length/2.0)
        halfChamferVector= self.halfChamfer.normalized()
        return geom.Ref3d3d(boltCenter, self.direction, halfChamferVector)

    def getBoltPositions(self):
        ''' Return the positions of the bolts.'''
        boltRefSys= self.getBoltRefSys()
        return self.boltedPlateTemplate.boltArray.getPositions(boltRefSys)

    def getBlocks(self, lbls= None):
        ''' Return the blocks that define the gusset for the
            diagonal argument.

        :param lbls: labels to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        labels= ['gusset_plate']
        if(lbls):
            labels.extend(lbls)
        blk= retval.blockFromPoints(self.contour, labels= labels, thickness= self.boltedPlateTemplate.thickness, matId= self.boltedPlateTemplate.steelType.name)
        # Get the hole blocks for the new plate
        ownerId= 'hole_owr_f'+str(blk.id) # Hole owner.
        holeLabels= labels+['holes',ownerId]
        boltRefSys= self.getBoltRefSys()
        blk.holes= self.boltedPlateTemplate.boltArray.getHoleBlocks(boltRefSys,holeLabels)
        retval.extend(blk.holes)
        return retval
