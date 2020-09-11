# -*- coding: utf-8 -*-
''' Naive gusset plate model.'''

from __future__ import division
from __future__ import print_function

import xc_base
import geom
from import_export import block_topology_entities as bte

class BoltArray(object):
    distances= [50e-3, 75e-3, 100e-3, 120e-3, 150e-3, 200e-3, 250e-3,.3,.4,.5,.6,.7,.8,.9]
    ''' Bolt array.

    :ivar bolt: bolt type.
    :ivar nRows: row number.
    :ivar nCols: column number.
    :ivar dist: distance between rows and columns
                 (defaults to three diameters).
    '''
    def __init__(self, bolt= None, nRows= 1, nCols= 1, dist= None):
        ''' Constructor.

        :param bolt: bolt type.
        :param nRows: row number.
        :param nCols: column number.
        :param dist: distance between rows and columns
                     (defaults to three diameters).
        '''
        self.bolt= bolt
        self.nRows= nRows
        self.nCols= nCols
        if(dist):
            self.dist= dist
        else:
            diameter= 16e-3
            if(self.bolt):
                diameter= self.bolt.diameter
            tmp= 3.0*diameter
            for d in self.distances:
                if(d>= tmp):
                    self.dist= d
                    break

    def getNumBolts(self):
        ''' Return the number of bolts of the array.'''
        return self.nRows*self.nCols

    def checkDistanceBetweenCenters(self):
        ''' Check distance between centers return a
            number < 1 if the distance is ok.'''
        return self.bolt.getRecommendedDistanceBetweenCenters()/self.dist

    def getMinPlateWidth(self):
        ''' Return the minimum width of the bolted plate.'''
        minEdgeDist= self.bolt.getMinimumEdgeDistanceJ3_4M()
        return 2.0*minEdgeDist+self.dist*(self.nRows-1)

    def getMinPlateLength(self):
        ''' Return the minimum length of the bolted plate.'''
        minEdgeDist= self.bolt.getMinimumEdgeDistanceJ3_4M()
        return 2.0*minEdgeDist+self.dist*(self.nCols-1)

    def getNetWidth(self, plateWidth):
        ''' Return the net width due to the bolt holes.

        :param plateWidth: width of the bolted plate.
        '''
        return plateWidth-self.nRows*self.bolt.getNominalHoleDiameter()

    def getDesignShearStrength(self, doubleShear= False):
        ''' Return the shear strength of the bolt group.

        :param doubleShear: true if double shear action.
        '''
        retval= self.getNumBolts()*self.bolt.getDesignShearStrength()
        if(doubleShear):
            retval*=2.0
        return retval

    def getDesignShearEfficiency(self, Pd, doubleShear= False):
        ''' Return the shear efficiency of the bolt group.

        :param Pd: design value of the force to resist.
        :param doubleShear: true if double shear action.
        '''
        return Pd/self.getDesignShearStrength(doubleShear)

    def getStrengthEfficiency(self, Pd, doubleShear= False):
        ''' Return the value of efficiency with respect to
            the strength.

        :param Pd: design value of the force to resist.
        :param doubleShear: true if double shear action.
        '''
        return self.getDesignShearEfficiency(Pd, doubleShear)

    def __str__(self):
        return ' number of rows: '+str(self.nRows)+' number of columns: '+str(self.nCols)+' distance between centers: '+str(self.dist)+ ' bolts: '+str(self.bolt) 
    def getDict(self):
        ''' Put member values in a dictionary.'''
        return {'bolt':self.bolt.getDict(), 'nRows':self.nRows, 'nCols':self.nCols, 'dist':self.dist}    

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.bolt.setFromDict(dct['bolt'])
        self.nRows= dct['nRows']
        self.nCols= dct['nCols']
        self.dist= dct['dist']

    def getLocalPositions(self):
        ''' Return the local coordinates of the bolts.'''
        retval= list()
        x0= self.dist*(self.nCols-1)/2.0
        y0= self.dist*(self.nRows-1)/2.0
        center= geom.Pos2d(x0,y0)
        for i in range(0,self.nCols):
            for j in range(0,self.nRows):
                x= i*self.dist-center.x
                y= j*self.dist-center.y
                retval.append(geom.Pos2d(x, y))
        return retval
        
    def getHoleBlocks(self, refSys= geom.Ref3d3d(), labels= []):
        ''' Return octagons inscribed in the holes.'''
        localPos= self.getLocalPositions()
        holes= list()
        for pLocal in localPos:
            circle= geom.Circle2d(pLocal,self.bolt.diameter/2.0)
            octagon= circle.getInscribedPolygon(8,0.0).getVertexList()
            holes.append(octagon)
        retval= bte.BlockData()
        # Base points (A)
        for h in holes:
            holeVertices= list()
            for v in h:
                p3d= geom.Pos3d(v.x,v.y,0.0)
                holeVertices.append(refSys.getPosGlobal(p3d))
            retval.blockFromPoints(holeVertices,labels)
        return retval
                    
    def getPositions(self, refSys= geom.Ref3d3d()):
        ''' Return the global coordinates of the bolts.

        :param refSys: reference system.
        '''
        localPos= self.getLocalPositions()
        retval= list()
        for pLocal in localPos:
            p3d= geom.Pos3d(pLocal.x,pLocal.y, 0.0)
            retval.append(refSys.getPosGlobal(p3d))
        return retval
    
    def report(self, outputFile):
        ''' Reports connection design values.'''
        outputFile.write('    bolts:\n')
        outputFile.write('      number of bolts: '+str(self.getNumBolts())+' x '+self.bolt.getName()+'\n')
        outputFile.write('      spacing: '+str(self.dist*1000)+' mm\n')
        self.bolt.report(outputFile)
        

class BoltedPlate(object):
    ''' Bolted plate.

    :ivar boltArray: bolt array.
    :ivar width: plate width.
    :ivar length: plate length.
    :ivar thickness: plate thickness.
    :ivar steelType: steel type.
    '''
    def __init__(self, boltArray= BoltArray(), thickness= 10e-3, steelType= None):
        ''' Constructor.

        :param boltArray: bolt array.
        :param thickness: plate thickness.
        :param steelType: steel type.
        '''
        self.boltArray= boltArray
        self.thickness= thickness
        self.steelType= steelType
        minWidth= self.boltArray.getMinPlateWidth()
        for d in self.boltArray.distances:
            if(d>= minWidth):
                self.width= d
                break
        minLength= self.boltArray.getMinPlateLength()
        for d in self.boltArray.distances:
            if(d>= minLength):
                self.length= d
                break
            
    def getNetWidth(self):
        ''' Return the net width due to the bolt holes. '''
        return self.boltArray.getNetWidth(self.width)        
            
    def getPlateDimensions(self):
        ''' Return the dimensions of the plate.'''
        return (self.width, self.length, self.thickness)

    def getMinThickness(self, Pd):
        ''' Return the minimum thickness of the plate
            to resist the force argument.

        :param Pd: design value of the force to resist.
        '''
        # Yielding in the gross section.
        minThicknessA= Pd/0.9/self.steelType.fy/self.width
        # Tension fracture in the net section.
        minThicknessB= Pd/0.75/self.steelType.fu/self.getNetWidth()
        return max(minThicknessA,minThicknessB)

    def checkThickness(self, Pd):
        ''' Check plate thickness; return a number < 1 if the 
            thickness is ok.

        :param Pd: design value of the force to resist.
        '''
        return self.getMinThickness(Pd)/self.thickness
    
    def getStrengthEfficiency(self, Pd, doubleShear= False):
        ''' Return the value of efficiency with respect
            to the strength of the bolted plate.

        :param Pd: design value of the force to resist.
        :param doubleShear: true if double shear action.
        '''
        retval= self.boltArray.getStrengthEfficiency(Pd, doubleShear)
        retval= max(retval, self.checkThickness(Pd))
        return retval

    def getEfficiency(self, internalForces, doubleShear= False):
        '''Return efficiency according to section 

        :param internalForces: internal forces.
        '''
        CF= 0.0
        if(internalForces.N<0): # compression
            CF= self.getStrengthEfficiency(-internalForces.N, doubleShear)
        else:
            CF= self.getStrengthEfficiency(internalForces.N, doubleShear)
        if(abs(internalForces.My)>1e-3):
            lmsg.error('bending not implemented yet.')
        if(abs(internalForces.Mz)>1e-3):
            lmsg.error('bending not implemented yet.')
        if(abs(internalForces.Vy)>1e-3):
            lmsg.error('shear not implemented yet.')
        if(abs(internalForces.Vz)>1e-3):
            lmsg.error('shear not implemented yet.')
        return CF

    def getFilletMinimumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return getFilletWeldMinimumLegSheets(self.thickness, otherThickness)
        
    def getFilletMaximumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return getFilletWeldMaximumLegSheets(self.thickness, otherThickness)

    def __str__(self):
        return 'width: '+ str(self.width) + ' length: '+ str(self.length) + ' thickness: '+ str(self.thickness) + ' bolts: ' + str(self.boltArray)
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        return {'boltArray':self.boltArray.getDict(), 'width':self.width, 'length':self.length, 'thickness':self.thickness, 'steelType':self.steelType.getDict()}

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.boltArray.setFromDict(dct['boltArray'])
        self.width= dct['width']
        self.length= dct['length']
        self.thickness= dct['thickness']
        self.steelType.setFromDict(dct['steelType'])

    def jsonRead(self, inputFileName):
        ''' Read object from JSON file.'''
        with open(inputFileName) as json_file:
            boltedPlateDict= json.load(json_file)
        self.setFromDict(boltedPlateDict)
        json_file.close()

    def report(self, outputFile):
        ''' Reports connection design values.'''
        outputFile.write('  gusset plates:\n')
        outputFile.write('    chamfer width: '+str(self.width*1000)+' mm\n')
        outputFile.write('    plate thickness: '+str(self.thickness*1000)+' mm\n')
        outputFile.write('    steel type: '+str(self.steelType.name)+'\n')
        self.boltArray.report(outputFile)
        
def readBoltedPlateFromJSONFile(inputFileName):
    ''' Read bolted plate object from a JSON file.'''
    retval= BoltedPlate()
    retval.jsonRead(inputFileName)
    return retval
