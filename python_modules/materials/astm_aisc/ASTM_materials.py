# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

''' Structural steel as specified in ASTM standard.

Predefined ASTM steel types: A36, A529, A572, A53, A992, A500, A307
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import json
import scipy.interpolate
import xc_base
import geom
import materials
from materials import steel_base
from misc_utils import log_messages as lmsg
from materials import buckling_base
from materials.astm_aisc import AISC_limit_state_checking as aisc
from import_export import block_topology_entities as bte

class ASTMSteel(steel_base.BasicSteel):
    '''ASTM structural steel.

    :ivar name: steel identifier.
    :ivar fy: yield stress (defaults to 250e6 Pa)
    :ivar fu: ultimate tensile strength (defaults to 400e6 Pa)
    :ivar gammaM: partial factor (defaults to 1.0)
    '''
    def __init__(self, name= None, fy= 250e6, fu= 400e6, gammaM= 1.0):
        ''' Constructor.

        :param name: steel identifier.
        :param fy: yield stress (defaults to 250e6 Pa)
        :param fu: ultimate tensile strength (defaults to 400e6 Pa)
        :param gammaM: partial factor (defaults to 1.0)
        '''
        super(ASTMSteel,self).__init__(200e9,0.3,fy,fu,gammaM)
        if(name):
            self.name= name
        else:
            self.name= None

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(ASTMSteel,self).getDict()
        name= None
        if(self.name):
            name= self.name
        retval.update({'name': name})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.name= None
        if('name' in dct):
            name= dct['name']
            if(name): self.name= name

A36= ASTMSteel('A36', 250e6,400e6,1.0)
A529= ASTMSteel('A529', 290e6,414e6,1.0)
A572= ASTMSteel('A572', 345e6,450e6,1.0)
A53= ASTMSteel('A53', 240e6,414e6,1.0)
A992= ASTMSteel('A992', 345e6,450e6,1.0)
A500= ASTMSteel('A500', 315e6,400e6,1.0)
A307= ASTMSteel('A307', 245e6,390e6,1.0)

def getFilletWeldMinimumLeg(t):
    '''
    Return the minimum leg size for a fillet bead that welds a sheet 
    of thickness t according to table J2.4 of AISC 360.

    :param t: Sheet thickness.
    '''
    if(t<=6e-3):
        return 3e-3
    elif(t<=13e-3):
        return 5e-3
    elif(t<=19e-3):
        return 6e-3
    else:
        return 8e-3

def getFilletWeldMaximumLeg(t):
    '''
    Return the maximum leg size for a fillet bead that welds a sheet 
    of thickness t according to section J2.2b of AISC 360.

    :param t: Sheet thickness.
    '''
    if(t<=6e-3):
        return t
    else:
        return t-2e-3

def getFilletWeldMinimumLegSheets(t1, t2):
    '''
    Return the minimum leg size which can be used to weld two sheets
    according to table J2.4 of AISC 360.

    :param t1: Thickness of sheet 1.
    :param t2: Thickness of sheet 2.
    '''
    amin1= getFilletWeldMinimumLeg(t1)
    amin2= getFilletWeldMinimumLeg(t2)
    return max(amin1,amin2)

def getFilletWeldMaximumLegSheets(t1, t2):
    '''
    Return the maximum leg size which can be used to weld two sheets
    according to ection J2.2b of AISC 360.

    :param t1: Thickness of sheet 1.
    :param t2: Thickness of sheet 2.
    '''
    amax1= getFilletWeldMaximumLeg(t1)
    amax2= getFilletWeldMaximumLeg(t2)
    return min(amax1,amax2)

class BoltBase(object):
    ''' Base class for bolts.

    :ivar diameter: bolt diameter
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
           
    def getName(self):
        return 'M'+str(self.diameter*1e3)[0:2]

    def getArea(self):
        ''' Return the area of the anchor rod.
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

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.diameter= dct['diameter']
        self.pos3d= None
        if('pos3d' in dct):
            xyz= dct['pos3d']
            if(xyz):
                self.pos3d= geom.Pos3d(xyz[0], xyz[1], xyz[2])

    def getHole(self, refSys= geom.Ref3d3d()):
        ''' Return a circle in the hole position.'''
        pLocal= geom.Pos2d(self.pos3d.x, self.pos3d.y)
        holeDiameter= self.getNominalHoleDiameter()
        circle2d= geom.Circle2d(pLocal,holeDiameter/2.0)
        return geom.Circle3d(refSys, circle2d)
    
    def getHoleBlock(self, refSys= geom.Ref3d3d(), labels= []):
        ''' Return and octagon inscribed in the hole.'''
        hole= self.getHole(refSys)
        octagon= hole.getInscribedPolygon(8,0.0).getVertexList()
        octagon.append(octagon[0]) # close polygon
        retval= bte.BlockData()
        retval.blockFromPoints(octagon,labels)
        return retval
    
    def report(self, outputFile):
        ''' Reports bolt design values.'''
        outputFile.write('      diameter: '+str(self.diameter*1000)+' mm\n')


class BoltFastener(BoltBase):
    ''' ASTM bolt according to chapter J of AISC 360-16.

    :ivar group: bolt material strength group according to section
                 J3.1 of AISC 360-16.
    '''
    # See table J3.4 M of AISC 360-16.
    bf_diams= [16e-3, 20e-3, 22e-3, 24e-3, 27e-3, 30e-3, 36e-3]
    tabJ3_4M= [22e-3, 26e-3, 28e-3, 30e-3, 34e-3, 38e-3, 46e-3]
    fTabJ3_4M= scipy.interpolate.interp1d(bf_diams,tabJ3_4M)
    # See table J3.3 M of AISC 360-16.
    standardHoleDia= [18e-3, 22e-3, 24e-3, 27e-3, 30e-3, 33e-3]
    oversizeHoleDia= [20e-3, 24e-3, 28e-3, 30e-3, 35e-3, 38e-3]
    fStandardHoleDia= scipy.interpolate.interp1d(bf_diams[:-1], standardHoleDia)
    fOversizeHoleDia= scipy.interpolate.interp1d(bf_diams[:-1], oversizeHoleDia)
    def __init__(self, diameter, group= 'A'):
       ''' Constructor.

       :param diameter: bolt diameter.
       :param group: bolt material strength group according to section
                     J3.1 of AISC 360-16.
       '''
       super(BoltFastener,self).__init__(diameter)
       self.group= group
    
    def getMinDistanceBetweenCenters(self):
        ''' Return the minimum distance between centers of standard, 
            oversized or slotted holes according to section J3.3 of
            AISC 360-16.'''
        return (2+2.0/3.0)*self.diameter
    
    def getRecommendedDistanceBetweenCenters(self):
        ''' Return the minimum distance between centers of standard, 
            oversized or slotted holes according to section J3.3 of
            AISC 360-16.'''
        return 3.0*self.diameter

    def getMinimumEdgeDistanceJ3_4M(self):
        ''' Return the minimum edge Distance from center of standard 
            hole to edge of connected part according to toble
            J3.4M of AISC 360-16.'''
        if(self.diameter<=36e-3):
            return self.fTabJ3_4M(self.diameter)
        else:
            return 1.25*self.diameter

    def getNominalHoleDiameter(self, oversized= False):
        ''' Return the minimum distance between centers of standard, 
            oversized or slotted holes according to table J3.3M of
            AISC 360-16.

        :param oversized: true if hole is oversized.
        '''
        if(oversized):
            if(self.diameter>=36e-3):
                return self.diameter+8e-3
            else:
                return float(self.fOversizeHoleDia(self.diameter))
        else:
            if(self.diameter>=36e-3):
                return self.diameter+3e-3
            else:
                return float(self.fStandardHoleDia(self.diameter))

    def getNominalTensileStrength(self):
        ''' Return the nominal strength of the fastener according
            to table J3.2 of AISC 360-16.
        '''
        retval= self.getArea()
        if(self.group=='A307'):
            retval*= 310e6
        elif(self.group=='A'):
            retval*= 620e6
        elif(self.group=='B'):
            retval*= 780e6
        elif(self.group=='C'):
            retval*= 1040e6
        else:
            lmsg.error('Unknown strength group: \''+self.group+'.\n')
        return retval
            
    def getDesignTensileStrength(self):
        ''' Return the design tensile strength of the bolt according
            to section J3.6 of of AISC 360-16.
        '''
        return 0.75*self.getNominalTensileStrength()
    
    def getNominalShearStrength(self, threadsExcluded= False):
        ''' Return the nominal strength of the fastener according
            to table J3.2 of AISC 360-16.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        retval= self.getArea()
        if(self.group=='A307'):
            retval*= 186e6
        elif(self.group=='A'):
            if(threadsExcluded):
                retval*= 469e6
            else:
                retval*= 372e6
        elif(self.group=='B'):
            if(threadsExcluded):
                retval*= 579e6
            else:
                retval*= 469e6
        elif(self.group=='C'):
            if(threadsExcluded):
                retval*= 779e6
            else:
                retval*= 620e6
        else:
            lmsg.error('Unknown strength group: \''+self.group+'.\n')
        return retval

    def getDesignShearStrength(self, threadsExcluded= False):
        ''' Return the design shear strength of the bolt according
            to section J3.6 of of AISC 360-16.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        return 0.75*self.getNominalShearStrength(threadsExcluded)

    def __str__(self):
        return self.getName()
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(BoltFastener,self).getDict()
        retval.update({'group':self.group})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(BoltFastener,self).setFromDict(dct)
        self.group= dct['group']

    def report(self, outputFile):
        ''' Reports bolt design values.'''
        super(BoltFastener,self).report(outputFile)
        outputFile.write('      group: '+self.group+'\n')

M16= BoltFastener(16e-3)
M20= BoltFastener(20e-3)
M22= BoltFastener(22e-3)
M24= BoltFastener(24e-3)
M27= BoltFastener(27e-3)
M30= BoltFastener(30e-3)
M36= BoltFastener(36e-3)

class BoltArray(object):
    distances= [50e-3, 75e-3, 100e-3, 120e-3, 150e-3, 200e-3, 250e-3,.3,.4,.5,.6,.7,.8,.9]
    ''' Bolt array.

    :ivar bolt: bolt type.
    :ivar nRows: row number.
    :ivar nCols: column number.
    :ivar dist: distance between rows and columns
                 (defaults to three diameters).
    '''
    def __init__(self, bolt= M16, nRows= 1, nCols= 1, dist= None):
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
            tmp= 3.0*self.bolt.diameter
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
            octagon.append(octagon[0]) # close polygon
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
    def __init__(self, boltArray= BoltArray(), thickness= 10e-3, steelType= A36):
        ''' Constructor.

        :param boltArray: bolt array.
        :param thickness: plate thickness.
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
        
class AnchorBolt(BoltBase):
    ''' ASTM anchor bolt according to table 2.2 from the document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc.

    :ivar name: bolt identifier
    :ivar steel: steel material.
    :ivar diameter: bolt diameter.
    '''

    # See table 3.2 of the design guide:
    ab_diams= [0.015875, 0.01905, 0.022225, 0.0254, 0.028575, 0.03175, 0.0381, 0.04445, 0.0508, 0.05715, 0.0635, 0.06985, 0.0762, 0.08255, 0.0889, 0.09525, 0.1016]
    bearingArea=[0.00044451524, 0.00058451496, 0.0007870952, 0.00096774, 0.0011677396, 0.0014451584, 0.0020193508, 0.0026903172, 0.003451606, 0.0043161204, 0.0052709572, 0.006322568, 0.007354824, 0.008580628, 0.009870948, 0.0112903, 0.012838684]
    fBearingArea= scipy.interpolate.interp1d(ab_diams,bearingArea)

    # Hole diameters; see table 2.3 of the design guide:
    rDiams= [0.01905, 0.022225, 0.0254, 0.03175, 0.0381, 0.04445, 0.0508, 0.0635] # rod diameters.
    hDiams= [0.0333375, 0.0396875, 0.0460375, 0.0523875, 0.0587375, 0.06985, 0.08255, 0.08255] # hole diameters.
    fHoleDiameter= scipy.interpolate.interp1d(rDiams,hDiams)
    

    def __init__(self, name, steel, diameter, pos3d= None):
       ''' Constructor.

       :param name: bolt identifier
       :param steel: steel material.
       :param diameter: bolt diameter.
       :param pos3d: bolt position.
       '''
       super(AnchorBolt,self).__init__(diameter, pos3d)
       self.name= name
       self.steelType= steel

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(AnchorBolt, self).getDict()
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'name':self.name, 'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict()})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(AnchorBolt, self).setFromDict(dct)
        self.name= dct['name']
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])
        
    # Tension
    def getTensileStrength(self):
        ''' Return the tensile strength of the anchor rod.
        '''
        Ag= self.getArea()
        return self.steelType.fu*Ag
    
    def getNominalTensileStrength(self):
        ''' Return the tensile strength of the anchor rod.
        '''
        return 0.75*self.getTensileStrength()
    
    def getNominalShearStrength(self, typ= 'N'):
        ''' Return the shear strength of the anchor rod.
        '''
        factor= 0.4
        if(typ=='X'):
            factor= 0.5
        return factor*self.getTensileStrength()
    
    def getDesignTensileStrength(self, phi= 0.75):
        ''' Return the tensile strength of the anchor rod.

        :param phi: resistance factor for anchor steel  
                in tension (defaults to 0.75).
        '''
        return phi*self.getNominalTensileStrength()

    def getDesignShearStrength(self, typ= 'N', phi= 0.55):
        ''' Return the shear strength of the anchor rod.

        :param phi: resistance factor for anchor steel  
                in tension (defaults to 0.55).
        '''
        return phi*self.getNominalShearStrength(typ)
    
    def getBearingArea(self):
        ''' Return the bearing area of the anchor according
            to the table 3.2 of the design guide.'''
        return self.fBearingArea(self.diameter)
    
    def getNominalHoleDiameter(self):
        ''' Return the hole diameter for the anchor according
            to the table 2.3 of the design guide.'''
        return self.fHoleDiameter(self.diameter)
    
    def getNominalPulloutStrength(self, fc, psi4= 1.0):
        ''' Return the nominal pullout strength of the anchor 
        rod based on the ACI Appendix D provisions (Section D5.3)

        :param fc: concrete strength.
        :param psi4: 1.4 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        '''
        Abrg= self.getBearingArea() # the bearing area of the anchor
                                    # rod head or nut.
        return psi4*Abrg*8*fc
    
    def getDesignPulloutStrength(self, fc, psi4= 1.0, phi= 0.7):
        ''' Return the design pullout strength of the anchor 
        rod based on the ACI Appendix D provisions (Section D5.3)

        :param fc: concrete strength.
        :param psi4: 1.4 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking (f t – f r ) at service levels, 
                     otherwise 1.0
        '''
        return phi*self.getNominalPulloutStrength(fc,psi4)

    def getConcreteBreakoutConePolygon(self, h_ef):
        ''' Return the full breakout cone in tension as per ACI 318-02.

        :param h_ef: depth of embedment.
        '''
        retval= geom.Polygon2d()
        delta= 1.5*h_ef
        if(self.pos3d):
            origin= geom.Pos2d(self.pos3d.x, self.pos3d.y)
            retval.appendVertex(origin+geom.Vector2d(delta,delta))
            retval.appendVertex(origin+geom.Vector2d(-delta,delta))
            retval.appendVertex(origin+geom.Vector2d(-delta,-delta))
            retval.appendVertex(origin+geom.Vector2d(delta,-delta))
        else:
            lmsg.error('Anchor position not specified.')        
        return retval

    def report(self, outputFile):
        ''' Write the anchor specification.'''
        super(AnchorBolt,self).report(outputFile)
        outputFile.write('     steel type: '+self.steelType.name+'\n')
    
class AnchorGroup(object):
    ''' Anchor group.

    :ivar anchors: anchor list.
    '''
    
    def __init__(self, steel, diameter, positions):
        ''' Creates an anchor group in the positions argument.

        :param steel: steel material.
        :param diameter: bolt diameter.
        :param positions: bolt positions.
        '''
        self.anchors= list()
        count= 0
        for p in positions:
            self.anchors.append(AnchorBolt(name= str(count), steel= steel, diameter= diameter, pos3d= p))
            count+= 1

    def setPositions(self, positions):
        ''' Set the anchors positions.

        :param positions: list of positions.
        '''
        for anchor, pos in zip(self.anchors, positions):
            anchor.pos3d= pos
            
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        for anchor in self.anchors:
            retval[anchor.name]= anchor.getDict()
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.anchors= list()
        for key in dct:
            bolt= AnchorBolt(name= '', steel= None, diameter= 0.0, pos3d= None)
            bolt.setFromDict(dct[key])
            self.anchors.append(bolt)
            
    def getNumberOfBolts(self):
        ''' Return the number of anchors in the group.'''
        return len(self.anchors)
    
    def getConcreteBreakoutConePolygon(self, h_ef):        
        ''' Return breakout cone in tension for the group
            as the union of individual cones.

        :param h_ef: depth of embedment.
        '''
        polygons= list()
        for anchor in self.anchors:
            polygons.append(anchor.getConcreteBreakoutConePolygon(h_ef))
        retval= polygons[0]
        for plg in polygons[1:]:
            retval.unePolygon2d(plg)
        return retval

    def getConcreteBreakoutStrength(self, h_ef, fc, psi3= 1.25, phi= 0.7):
        ''' Return the concrete breakout strength for the anchor
            group as per ACI 318-02, Appendix D.

        :param h_ef: depth of embedment.
        :param fc: concrete strength.
        :param psi3: 1.25 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking at service levels, otherwise 1.0
        :param phi: resistance factor for concrete breakout  
                    (defaults to 0.70).
        '''
        AN= self.getConcreteBreakoutConePolygon(h_ef).getArea()
        ANo= self.anchors[0].getConcreteBreakoutConePolygon(h_ef).getArea()
        fc_psi= fc*145.038e-6
        retval= phi*psi3*math.sqrt(fc_psi)*AN/ANo
        h_ef_in= h_ef/0.0254
        if(h_ef_in<11): 
            retval*= 24*math.pow(h_ef_in,1.5)
        else:
            retval*= 16*math.pow(h_ef_in,5.0/3.0)
        retval*=4.4482216 # pounds to Newtons
        return retval
    
    def getBlocks(self, refSys, labels):
        ''' Return the block decomposition of the base plate.'''
        retval= bte.BlockData()
        for anchor in self.anchors:
            retval.extend(anchor.getHoleBlock(refSys, labels))
        return retval

    def report(self, outputFile):
        ''' Writes anchor group specification.'''
        outputFile.write('   anchors: \n')
        outputFile.write('     number of anchors: '+ str(len(self.anchors))+ ' x '+self.anchors[0].getName()+'\n')
        self.anchors[0].report(outputFile)
        

class ASTMShape(object):
    """Steel shape with ASTM/AISC verification routines."""
    def __init__(self, name= ''):
       '''Constructor.

       :param name: name of the shape.
       '''
       self.name= name
       
    def getDict(self):
        ''' Put member values in a dictionary.'''
        return {'name':self.name}

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.name= dct['name']

    # Tension
    def getDesignTensileStrength(self, Ae= None):
        ''' Return the tensile strength of the section according
            to section D2 fo AISC-360-16.

        :param Ae: effective net area according to section D3.
        '''
        Ag= self.get('A')
        retval= 0.9*self.steelType.fy*Ag
        if(not Ae):
            Ae= Ag
        retval= min(retval,0.75*self.steelType.fu*Ae)
        return retval

    # Compression
    def getFlexuralSlendernessRatio(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural buckling slenderness ratio of the member.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        '''
        sc= self.slendernessCheck()
        if(sc>1.01):
            lmsg.warning('Member section has slender members. Results are not valid.')
        retval= effectiveLengthZ/self.get('iz')
        return max(retval,effectiveLengthY/self.get('iy'))
    
    def getFlexuralCriticalSlendernessRatio(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the critical value of the flexural buckling 
            slenderness ratio of the member.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
         '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        Fy= self.steelType.fy
        return sr/math.pi*math.sqrt(Fy/E)
    
    def getFlexuralElasticBucklingStress(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E3-4 of AISC-360-16.
 
        :param effectiveLengthY: effective length of member (minor axis).
        :parem effectiveLengthZ: effective length of member (major axis).
        '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        return math.pi**2*E/sr**2
    
    def getFlexuralElasticBucklingStressOnAxis(self, effectiveLengthY, effectiveLengthZ, majorAxis= True):
        ''' Return the flexural elastic buckling stress of the member according
            to equations E4-5 or E4-6 of AISC-360-16.
  
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param majorAxis: true if flexure about the major axis.
       '''
        if(majorAxis):
            sr= effectiveLengthZ/self.shape.get('iz') # Slenderness ratio for eq. E4-5
        else:
            sr= effectiveLengthY/self.shape.get('iy') # Slenderness ratio for eq. E4-6
        E= self.get('E')
        return math.pi**2*E/sr**2
    
    def getCriticalStressE(self, effectiveLengthY, effectiveLengthZ, sectionClassif, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        :param Fe: flexural or torsional elastic buckling stress.
        '''
        retval= 0.0
        if(sectionClassif<aisc.SectionClassif.slender):
            sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
            E= self.get('E')
            Fy= self.steelType.fy
            Fratio= Fy/Fe
            thresholdA= 4.71*math.sqrt(E/Fy)
            if((sr<=thresholdA) or (Fratio<=2.25)):
                retval= math.pow(0.658,Fratio)*Fy # (E3-2)
            else:
                retval= 0.877*Fe
        else:
            lmsg.error('Critical stress of slender members not implemented yet.')
        return retval
    
    def getFlexuralCriticalStress(self, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the flexural critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        Fe= self.getFlexuralElasticBucklingStress(effectiveLengthY, effectiveLengthZ)
        return self.getCriticalStressE(effectiveLengthY, effectiveLengthZ, sectionClassif,Fe)
    
    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return getShapeTorsionalElasticBucklingStress(self, effectiveLengthX)
    
    def getTorsionalCriticalStress(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the torsional critical stress of the member according
            to equations E4-2, E4-3 and E4-4 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        Fe= self.getTorsionalElasticBucklingStress(effectiveLengthX)
        return self.getCriticalStressE(effectiveLengthY= effectiveLengthY, effectiveLengthZ= effectiveLengthZ, sectionClassif= sectionClassif, Fe= Fe)

    def getNominalCompressiveStrength(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the nominal compressive strength of the member
            according to equation E3-1 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        Ag= self.shape.get('A') # Gross area of member
        retval= 0.0
        if(effectiveLengthX<= max(effectiveLengthY,effectiveLengthZ)):
            retval= self.getFlexuralCriticalStress(effectiveLengthY, effectiveLengthZ, sectionClassif)*Ag
        else:
            retval= self.getTorsionalCriticalStress(effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif)*Ag
        return retval
    
    def getDesignCompressiveStrength(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the design compressive strength of the member
            according to section E1 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLengthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        return 0.9*self.getNominalCompressiveStrength(effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif)
    
    def getReferenceCompressiveStrength(self, sectionClassif):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.

        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        return self.getDesignCompressiveStrength(effectiveLengthX= 0.1, effectiveLengthY= 0.1, effectiveLengthZ= 0.1,sectionClassif= sectionClassif)
    # Bending 
    def getReferenceFlexuralStrength(self):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.'''
        return self.getDesignFlexuralStrength(lateralUnbracedLength= 0.1, Cb= 1.0, majorAxis= True)

    # Shear
    def getYShearEfficiency(self, sectionClass, Vy):
        '''Return major axis shear efficiency according to AISC-360-16.

           :param sectionClass: dummy argument used for compatibility.
        '''
        return abs(Vy)/self.getDesignShearStrength(majorAxis= True)
    
    def getZShearEfficiency(self, sectionClass, Vz):
        '''Return major axis shear efficiency according to AISC-360-16.

           :param sectionClass: dummy argument used for compatibility.
        '''
        return Vz/self.getDesignShearStrength(majorAxis= False)
        

    # Combined internal forces
    def getBiaxialBendingEfficiency(self,sectionClassif,Nd,Myd,Mzd,Vyd= 0.0, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

           :param sectionClassif: section classification compact, noncompact, slender or too slender.
           :param chiN: axial load reduction reduction factor (default= 1.0).
           :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        ratioN= 0.0
        if(Nd<0): # compression
            NcRd= chiN*self.getReferenceCompressiveStrength(sectionClassif) # available axial strength.
            ratioN=  abs(Nd)/NcRd
        else:
            NcRd= self.getDesignTensileStrength() # available axial strength.
            ratioN= Nd/NcRd
        McRdy= self.getDesignFlexuralStrength(None, None, majorAxis= False) # available flexural strength minor axis.
        McRdz= self.getReferenceFlexuralStrength() # reference flexural strength major axis.
        # MvRdz= self.getMvRdz(sectionClass,Vyd)
        MvRdz= McRdz
        MbRdz= chiLT*MvRdz # available flexural strength major axis.
        ratioMz= abs(Mzd)/MbRdz
        ratioMy= abs(Myd)/McRdy
        if(ratioN>=0.2):
            CF= ratioN+8.0/9.0*(ratioMz+ratioMy) # equation H1-1a
        else:
            CF= ratioN/2.0+(ratioMz+ratioMy) # equation H1-1b
        return (CF,NcRd,McRdy,McRdz,MvRdz,MbRdz)

    def setupULSControlVars(self,elems,sectionClass= 1, chiN=1.0, chiLT=1.0):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to be satisfied.'''
        super(ASTMShape,self).setupULSControlVars(elems)
        for e in elems:
            e.setProp('sectionClass',sectionClass) #Cross section class.
            e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
            e.setProp('chiN',chiN) # Axial strength reduction factor.
            e.setProp('crossSection',self)
            
from materials.sections.structural_shapes import aisc_metric_shapes

class WShape(ASTMShape,aisc_metric_shapes.WShape):
    """W shape with ASTM/AISC verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name (i.e. W40X431)
    """
    def __init__(self,steel= None, name= ''):
        ''' Constructor.
        '''
        ASTMShape.__init__(self,name)
        aisc_metric_shapes.WShape.__init__(self,steel,name)
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= ASTMShape.getDict(self)
        retval.update(aisc_metric_shapes.WShape.getDict(self))
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        ASTMShape.setFromDict(self, dct)
        aisc_metric_shapes.WShape.setFromDict(self, dct)

class CShape(ASTMShape,aisc_metric_shapes.CShape):
    """C shape with ASTM 3 verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name  (i.e. C380X74).
    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CShape.__init__(self,steel,name)

class HSSShape(ASTMShape,aisc_metric_shapes.HSSShape):
    """Rectangular HSS shape with ASTM/AISC verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name (i.e. HSS2X2X_250).
    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.HSSShape.__init__(self,steel,name)

class CHSSShape(ASTMShape,aisc_metric_shapes.CHSSShape):
    """Circular HSS shape with ASTM/AISC verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name (i.e. HSS16.000X0.375).
    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CHSSShape.__init__(self,steel,name)

 
class BendingState(object):
    ''' Bending moments along the member.

        :ivar Mmax: absolute value of maximum moment in the unbraced 
                     segment.
        :ivar Ma: absolute value of moment at quarter point of the 
                   unbraced segment.
        :ivar Mb: absolute value of moment at centerline of the 
                   unbraced segment.
        :ivar Mc: absolute value of moment at three-quarter point of 
                   the unbraced segment.
    '''
    def __init__(self, Mmax, Ma, Mb, Mc):
        ''' Constructor.'''
        self.Mmax= Mmax
        self.Ma= Ma
        self.Mb= Mb
        self.Mc= Mc
        
    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor Cb
        for non uniform moment diagrams when both ends of the segment
        are braced according to expression F1-1 of AISC 360-16.
        '''
        return 12.5*self.Mmax/(2.5*self.Mmax+3*self.Ma+4*self.Mb+3*self.Mc)

class MemberConnection(buckling_base.MemberConnection):
    '''Member length and connections

       :ivar L: member length.
       :ivar Lb: Length between points that are either braced 
                 against lateral displacement of compression 
                 flange or braced against twist of the cross section.
       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,L,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        super(MemberConnection, self).__init__(rotI,transI,rotJ,transJ)
        self.L= L
        self.Lb= L
        
    def getLateralTorsionalBucklingModificationFactor(self,bendingState):
        ''' Return the lateral-torsional buckling modification factor Cb
            for non uniform moment diagrams when both ends of the segment
            are braced according to expression F1-1 of AISC 360-16.

            :param Mmax: absolute value of maximum moment in the unbraced 
                         segment.
            :param Ma: absolute value of moment at quarter point of the 
                       unbraced segment.
            :param Mb: absolute value of moment at centerline of the 
                       unbraced segment.
            :param Mc: absolute value of moment at three-quarter point of 
                       the unbraced segment.
        '''
        if(self.transJ=='free' and self.rotJ=='free'): # cantilever
            return 1.0
        else:
            return bendingState.getLateralTorsionalBucklingModificationFactor()

class ConnectedMember(object):
    """C shape with ASTM 3 verification routines."""
    def __init__(self,shape,connection):
        ''' Constructor.

        :ivar shape: structural shape.
        :ivar connection: member length and connection information.
        '''
        self.shape= shape
        self.connection= connection
        
    def getEffectiveLength(self):
        '''Return the member effective length according to
           section E2 of AISC 360-16.'''
        K= self.connection.getEffectiveBucklingLengthCoefficientRecommended()
        return  K*self.connection.L #Effective length of member.
    
    def getSlendernessRatio(self):
        '''Return the slenderness ratio of the member.'''
        Lc= self.getEffectiveLength()
        r= min(self.shape.get('iy'),self.shape.get('iz'))
        return Lc/r
    
    def getElasticBucklingStress(self):
        '''Return the elastic buckling stress of the member.'''
        return math.pi**2*self.shape.steelType.E/(self.getSlendernessRatio())**2
    
    def getCriticalStress(self):
        '''Return the critical stress as definded in
           section E7 of AISC 360-16.
        '''
        treshold= 4.71*math.sqrt(self.shape.steelType.E/self.shape.steelType.fy)
        r= self.getSlendernessRatio()
        Fe= self.getElasticBucklingStress()
        fy_fe= self.shape.steelType.fy/Fe
        if((r<=treshold) or (fy_fe<=2.25)):
            return math.pow(0.658,fy_fe)*self.shape.steelType.fy
        else:
            return 0.877*Fe
        
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength according to
            section E7 of AISC 360-16.
        '''
        return self.shape.getEffectiveArea()*self.getCriticalStress()
    
    def getZLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getLateralTorsionalBucklingLimit(Lb= self.connection.Lb, Cb= cb, majorAxis= True)
    
    def getZNominalflexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.


            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength= self.connection.Lb, Cb= cb, majorAxis= True)
    def getYLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getLateralTorsionalBucklingLimit(Lb= self.connection.Lb, Cb= cb, majorAxis= False)
    
    def getYNominalflexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength= self.connection.Lb, Cb= cb, majorAxis= False)
    
    def getCapacityFactor(self,Nd,Myd,Mzd,gammaC,bendingStateY,bendingStateZ):
        ''' Return the capacity factor according to section
            H1 of AISC 360-16.

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.
        :param Nd: design value for the axial load (negative for compression).
        :param Mzd: design value for the bending moment around z axis.
        :param Myd: design value for the bending moment around z axis.
        :param bendingStateY: y bending moments along the member.
        :param bendingStateZ: z bending moments along the member.
        '''
        if(Nd<=0.0):
            Pn= self.getNominalCompressiveStrength()/gammaC
            Mnz= self.getZNominalflexuralStrength(bendingStateZ)/gammaC
            Mny= self.getYNominalflexuralStrength(bendingStateY)/gammaC
            ratioN= abs(Nd)/Pn
            Msum= (abs(Mzd)/Mnz+abs(Myd)/Mny)
            if(ratioN>=0.2):
                return ratioN+8/9.0*Msum # (H1-1a)
            else:
                return ratioN/2.0+Msum # (H1-1b)

        else:
            lmsg.error('Capacity factor not implemented for tension.')
