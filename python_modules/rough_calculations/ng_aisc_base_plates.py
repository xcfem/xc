# -*- coding: utf-8 -*-
''' Base plate design according to document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc.
'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
import json
import copy
import xc_base
import geom
from postprocess.reports import common_formats as fmt
from misc_utils import log_messages as lmsg
from materials.astm_aisc import ASTM_materials
import materials
from import_export import block_topology_entities as bte
import ezdxf # See https://ezdxf.readthedocs.io
import csv
from colorama import Fore
from colorama import Style

# Base plates under concentric axial compressive loads.

def getConcreteStrength(A, fc, phi= 0.65):
    ''' Return the bearing strength of the concrete
        surface according to expression in section 
        3.1 of the guide for A1==A2.

    :param A: concrete bearing area.
    :param fc: minimum concrete compressive strength.
    :param phi: resistance factor for bearing on 
                concrete (defaults to 0.65).
    '''
    return phi*0.85*fc*A

def computeBasePlateArea(Pu, fc, phi= 0.65):
    ''' Return the required base plate area according
        to expression in section 3.1 of the guide
        for A1==A2.

    :param Pu: axial load (LRFD).
    :param fc: minimum concrete compressive strength.
    :param phi: resistance factor for bearing on 
                concrete (defaults to 0.65).
    '''
    return Pu/(phi*0.85*fc)

def getDelta(steelShape):
    ''' Return the Deltadimension of the base plate according
        to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    return (0.95*d-0.8*bf)/2.0
  
def getNDimension(steelShape, Areq):
    ''' Return the N dimension of the base plate according
        to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    :param Areq: base plate required area
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    delta= getDelta(steelShape)
    Nmin= max(math.sqrt(Areq),d)
    return Nmin+delta

def getBDimension(steelShape, N, Areq):
    ''' Return the B dimension of the base plate.

    :param steelShape: steel shape of the shaft.
    :param Areq: base plate required area
    '''
    B=Areq/N
    bf= steelShape.get('b') # Flange width.
    delta= getDelta(steelShape)
    B= max(B,bf+delta)
    return B
  
def computePlateThickness(steelShape, N, B, Pu, Pp, Fy, phi= 0.9):
    ''' Return the required thickness of the base plate 
        according to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    :param N: dimension parallel to the web of the shaft.
    :param B: dimension parallel to the flange of the shaft.
    :param Pu: axial load (LRFD).
    :param Pp: bearing strength of the concrete area (LRFD).
    :param Fy: base plate steel yield stress
    :param phi: resistance factor for base plate steel (defaults to 0.9).
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    m= (N-0.95*d)/2.0
    n= (B-0.8*bf)/2.0
    X= ((4.0*d*bf)/(d+bf)**2)*abs(Pu)/Pp
    if(X>1):
        lmsg.error('Pu ('+str(Pu/1e3)+' kN) too big with respect to Pp ('+str(Pp/1e3)+' kN) increase base plate area\n')
    lmbd= 2.0*math.sqrt(X)/(1+math.sqrt(1-X))
    lmbd= min(lmbd,1.0)
    lmbd_np= lmbd*math.sqrt(d*bf)/4.0
    l= max(m,n,lmbd_np)
    retval= l*math.sqrt(2.0*abs(Pu)/(phi*Fy*B*N))
    return retval

# Design for shear
  
class RectangularBasePlate(object):
    ''' Rectangular base plate.

    :ivar N: dimension parallel to the web of the shaft.
    :ivar offsetN: displacement of the plate in the direction of the
                   web (defaults to 0).
    :ivar B: dimension parallel to the flange of the shaft.
    :ivar offsetB: displacement of the plate in the direction of the
                   flange (defaults to 0).
    :ivar t: thickness.
    :ivar steelShape: shape of the steel column supported
                       by the plate.
    :ivar anchorGroup: anchor group.
    :ivar fc: minimum concrete compressive strength.
    :ivar steel: steel material.
    :ivar origin: center of the base plate.
    :ivar nShearBolts: number of bolts that take shear force
                       (defaults to 2).
    '''
    def __init__(self, N= 0.0, B= 0.0, t= 0.0, steelShape= None, anchorGroup= None, fc= 0.0, steel= None, origin= geom.Pos3d()):
        ''' Constructor.

        :param N: dimension parallel to the web of the shaft.
        :param B: dimension parallel to the flange of the shaft.
        :param t: thickness.
        :param steelShape: shape of the steel column supported
                           by the plate.
        :param anchorGroup: anchor group.
        :param fc: minimum concrete compressive strength.
        :param steel: steel material.
        :param origin: center of the base plate.
        '''
        self.N= N
        self.offsetN= 0.0 
        self.B= B
        self.offsetB= 0.0
        self.t= t
        self.steelShape= steelShape
        self.anchorGroup= anchorGroup
        self.steel= steel
        self.origin= origin
        self.fc= fc
        self.nShearBolts= 2  # By default only 2 anchors work
                             # for shear (see 3.5.3 in the guide)

    def getDict(self):
        ''' Put member values in a dictionary.'''
        steelShapeClassName= str(self.steelShape.__class__)[8:-2]
        steelClassName= str(self.steel.__class__)[8:-2]
        retval= {'N':self.N, 'offsetN': self.offsetN, 'B':self.B, 'offsetB': self.offsetB, 't':self.t, 'steelShapeClassName': steelShapeClassName, 'steelShape':self.steelShape.getDict(), 'anchorGroup':self.anchorGroup.getDict(),'steelClassName':steelClassName, 'steel':self.steel.getDict()}
        xyz= (self.origin.x, self.origin.y, self.origin.z)
        retval.update({'origin': xyz, 'fc':self.fc, 'nShearBolts':self.nShearBolts})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.N= dct['N']
        self.offsetN= dct['offsetN']
        self.B= dct['B']
        self.offsetB= dct['offsetB']
        self.t= dct['t']
        steelClassName= dct['steelClassName']+'()'
        self.steel= eval(steelClassName)
        self.steel.setFromDict(dct['steel'])
        if dct['steelShapeClassName'] == "materials.astm_aisc.ASTM_materials.IShape":
            shape = dct['steelShape']['shape']
            bf = shape['b']
            tf = shape['tf']
            tw = shape['tw']
            hw = shape['hi']
            name = shape['name']
            steelShapeClassName = f"{dct['steelShapeClassName']}({bf}, {tf}, {tw}, {hw}, steel=self.steel, name='{name}')"
        else:
            steelShapeClassName= dct['steelShapeClassName']+'()'
        self.steelShape= eval(steelShapeClassName)
        self.steelShape.setFromDict(dct['steelShape'])
        self.anchorGroup= ASTM_materials.AnchorGroup(steel= None, diameter= 0.0, positions= [])
        self.anchorGroup.setFromDict(dct['anchorGroup'])
        xyz= dct['origin']
        self.origin= geom.Pos3d(xyz[0],xyz[1],xyz[2])
        self.fc= dct['fc']
        self.nShearBolts= dct['nShearBolts']

    def getCopy(self):
        ''' Return a deep copy of this object.'''
        retval= RectangularBasePlate()
        retval.setFromDict(self.getDict())
        return retval

    def getLocalRefSys(self):
        ''' Return the local reference system.'''
        return geom.Ref3d3d(self.origin)
    
    def getArea(self):
        ''' Return the area of the base plate.'''
        return self.B*self.N

    def getMidPlane(self):
        ''' Return the mid-plane of the base plate.'''
        return geom.Plane3d(self.origin, geom.Vector3d(1.0,0.0,0.0), geom.Vector3d(0.0,1.0,0.0))
        
    def getContour(self):
        ''' Return the base plate contour. '''
        retval= geom.Polygon2d()
        deltaX= self.B/2.0
        deltaY= self.N/2.0
        origin2d= geom.Pos2d(self.origin.x, self.origin.y)
        retval.appendVertex(origin2d+geom.Vector2d(deltaX+self.offsetB,deltaY+self.offsetN))
        retval.appendVertex(origin2d+geom.Vector2d(-deltaX+self.offsetB,deltaY+self.offsetN))
        retval.appendVertex(origin2d+geom.Vector2d(-deltaX+self.offsetB,-deltaY+self.offsetN))
        retval.appendVertex(origin2d+geom.Vector2d(deltaX+self.offsetB,-deltaY+self.offsetN))
        return retval

    def getBlocks(self, blockProperties= None):
        ''' Return the block decomposition of the base plate.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        contour= self.getContour().getVertexList()
        plateProperties= bte.BlockProperties.copyFrom(blockProperties)
        plateProperties.appendAttribute('objType', 'baseplate')
        points= list()
        for p2d in contour:
            points.append(geom.Pos3d(p2d.x, p2d.y, 0.0))
        blk= retval.blockFromPoints(points, plateProperties, thickness= self.t, matId= self.steel.name)
        ownerId= 'f'+str(blk.id) # Hole owner.
        boltGroup= 'joint_'+plateProperties.getAttribute('jointId') # Joint id.
        boltGroup+= '_'+plateProperties.getAttribute('objType')
        holeProperties= bte.BlockProperties.copyFrom(plateProperties)
        holeProperties.appendAttribute('boltGroup', boltGroup)        
        blk.holes= self.anchorGroup.getHoleBlocks(self.getLocalRefSys(), blockProperties= holeProperties, ownerId= ownerId)
        retval.extend(blk.holes)
        return retval

    def centerAnchors(self, columnShape):
        ''' Center anchors with respect to the column steel shape.'''
        nAnchors= self.anchorGroup.getNumberOfBolts()
        if(nAnchors<=4):
            flangeThickness= columnShape.get('tf')
            delta= (columnShape.get('h')-flangeThickness)/4.0
            positions= list()
            positions.append(geom.Vector3d(delta,delta,0.0))
            positions.append(geom.Vector3d(-delta,delta,0.0))
            positions.append(geom.Vector3d(-delta,-delta,0.0))
            positions.append(geom.Vector3d(delta,-delta,0.0))
            self.anchorGroup.setPositions(positions)
        else:
            lmsg.warning('Number of anchors is not 4. Cannot center them.')

    def getConcreteStrength(self):
        ''' Return the strenght of the concrete surface.'''
        return getConcreteStrength(self.getArea(), self.fc)

    def getConcreteStrengthEfficiency(self, Pu):
        ''' Return the efficiency of the concrete strength.'''
        if(Pu<0): # concrete compressed.
            return -Pu/self.getConcreteStrength()
        else:
            return 0.0

    def getAnchorLeverArm(self):
        ''' Return the lever arm of the anchors with
            respect to the shaft web.'''
        retval= 0.0
        for anchor in self.anchorGroup.anchors:
            retval+= abs(anchor.pos3d.x)
        retval/= self.anchorGroup.getNumberOfBolts()
        return retval

    def getRequiredThickness(self, Pu):
        ''' Return the required thickness.'''
        if(Pu<0): # base plate compressed
            Pp= self.getConcreteStrength()
            return computePlateThickness(self.steelShape, self.N, self.B, Pu, Pp, self.steel.fy)
        else: # rods in tension
            T_rod= Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
            
            tw= self.steelShape.get('tw') # Web thickness
            anchorLeverArm= self.getAnchorLeverArm()
            M= T_rod*anchorLeverArm-tw/2.0
            b_eff= (anchorLeverArm-tw/2.0)*2
            return math.sqrt(abs(M)*4.0/(b_eff*0.9*self.steel.fy))

    def getThicknessEfficiency(self, Pu):
        ''' Return the thickness efficiency.'''
        return self.getRequiredThickness(Pu)/self.t

    def computeFrictionStrength(self,Pu, frictionCoef= 0.55, phi= 0.75):
        ''' Return the shear resistance of the base plate.

        :param fc: minimum concrete compressive strength.
        :param Pu: axial load (LRFD).
        :param frictionCoef: friction coefficient μ is 0.55 for steel on 
                             grout, and 0.7 for steel on concrete.
        :param phi: resistance factor for concrete on shear (defaults to 0.75).
        '''
        retval= 0.0
        if(Pu<0): # base plate compressed
            retval= -phi*frictionCoef*Pu
            retval= min(retval, 0.2*self.fc*self.getArea())
        return retval
    
    def getShearEfficiency(self, Pu, Vu):
        ''' Return the shear efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        :param Vu: shear load (LRFD).
        '''
        anchorShearStrength= self.anchorGroup.anchors[0].getDesignShearStrength()
        shearStrength= self.nShearBolts*anchorShearStrength
        if(Pu<0): # base plate compressed
            shearStrength+= self.computeFrictionStrength(Pu)
        return Vu/shearStrength
    
    def getPulloutEfficiency(self, Pu):
        ''' Return the pullout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu>0): # tensile load
            T_rod= Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
            retval= self.anchorGroup.anchors[0].getPulloutEfficiency(T_rod, self.fc)
        return retval
        
    def getBreakoutEfficiency(self, h_ef, Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param h_ef: depth of embedment.
        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu>0): # tensile load
            anchorConcreteBreakoutStrength= self.anchorGroup.getDesignConcreteBreakoutStrength(h_ef, self.fc)
            retval= Pu/anchorConcreteBreakoutStrength
        return retval
        
    def getWebStress(self, Pu):
        ''' Return the tension stress in the shaft web.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu>0): # tensile load
            T_rod= Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
            tw= self.steelShape.get('tw') # Web thickness
            anchorLeverArm= self.getAnchorLeverArm()
            M= T_rod*anchorLeverArm-tw/2.0
            b_eff= (anchorLeverArm-tw/2.0)*2
            forcePerInch= T_rod/b_eff
            retval= 2.0*forcePerInch/tw
        return retval
    
    def getWebStressEfficiency(self, Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu>0): # tensile load
            webStress= self.getWebStress(Pu)
            retval= webStress/(0.9*self.steelShape.steelType.fy)
        return retval

    def getRodTensileStrengthEfficiency(self,Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu>0): # tensile load
            Rn= self.anchorGroup.anchors[0].getDesignTensileStrength()
            T_rod= Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
            retval= T_rod/Rn
        return retval
    
    def getFilletMinimumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return ASTM_materials.getFilletWeldMinimumLegSheets(self.t, otherThickness)
        
    def getFilletMaximumLeg(self, otherThickness):
        '''
        Return the minimum leg size for a fillet bead 
        according to table J2.4 of AISC 360.

        :param otherThickness: thickness of the other part to weld.
        '''
        return ASTM_materials.getFilletWeldMaximumLegSheets(self.t, otherThickness)
    
    def getFlangeLegMinSize(self):
        ''' Return the minimum leg size of the gusset plate
            with the flange.
        '''
        flangeThickness= self.steelShape.get('tf')
        return self.getFilletMinimumLeg(flangeThickness)
    
    def getFlangeLegMaxSize(self):
        ''' Return the maximum leg size of the gusset plate
            with the flange.
        '''
        flangeThickness= self.steelShape.get('tf')
        return self.getFilletMaximumLeg(flangeThickness)
    
    def getFlangeWeldLegSize(self, factor= 0.75):
        ''' Return the leg size of the welds that connect the gusset plate
            to the flange.
        '''
        minFlangeThickness= self.getFlangeLegMinSize()
        maxFlangeThickness= self.getFlangeLegMaxSize()
        return minFlangeThickness+factor*(maxFlangeThickness-minFlangeThickness)

    def getWebLegMinSize(self):
        ''' Return the minimum leg size of the gusset plate
            with the web.
        '''
        webThickness= self.steelShape.get('tw')
        return self.getFilletMinimumLeg(webThickness)
    
    def getWebLegMaxSize(self):
        ''' Return the maximum leg size of the gusset plate
            with the web.
        '''
        webThickness= self.steelShape.get('tw')
        return self.getFilletMaximumLeg(webThickness)
    
    def getWebWeldLegSize(self, factor= 0.6):
        ''' Return the leg size of the weld that connects the gusset plate
            with the web.
        '''
        minWebThickness= self.getWebLegMinSize()
        maxWebThickness= self.getWebLegMaxSize()
        return minWebThickness+factor*(maxWebThickness-minWebThickness)
    
    def getNominalHoleDiameter(self):
        ''' Return the hole diameter.'''
        retval= 0.0
        if(len(self.anchorGroup.anchors)>0):
            retval= self.anchorGroup.anchors[0].getNominalHoleDiameter(rounded= True)
        return retval

    def report(self, outputFile):
        ''' Writes base plate specification.

        :param outputFile: output file.
        '''
        outputFile.write('  Base plate: \n')
        outputFile.write('    length: '+str(self.N*1000)+ ' mm\n')
        outputFile.write('    length offset: '+str(self.offsetN*1000)+ ' mm\n')
        outputFile.write('    width: '+str(self.B*1000)+ ' mm\n')
        outputFile.write('    width offset: '+str(self.offsetB*1000)+ ' mm\n')
        outputFile.write('    thickness: '+ str(self.t*1000)+ ' mm\n')
        outputFile.write('    base plate - column welds:\n')
        outputFile.write('      with the flange(s): 2 x '+str(math.floor(self.getFlangeWeldLegSize(0.3)*1000))+' mm (fillet weld leg size)\n')
        outputFile.write('      with the web: 2 x '+str(math.floor(self.getWebWeldLegSize()*1000))+' mm (fillet weld leg size)\n')
        #outputFile.write('fc= ', fc/1e6,' MPa')
        #outputFile.write('   area: '+ self.getArea()+ ' m2')
        holeDiameter= self.getNominalHoleDiameter()
        outputFile.write('     hole diameter: '+ str(holeDiameter*1e3)+ ' mm\n')
        self.anchorGroup.report(outputFile)
        
    def writeDXFShapeContour(self, modelSpace, layerName):
        ''' Writes the shape contour in the model
            space argument.

        :param modelSpace: ezdxf model space to write into.
        :param layerName: DXF layer name.
        '''
        plg= self.steelShape.getContour()
        posVec3d= self.origin.getPositionVector()
        posVec2d= geom.Vector2d(posVec3d.x, posVec3d.y)
        plg.move(posVec2d)
        vtx= plg.getVertices()
        vtx.append(vtx[0])
        v0= vtx[0]
        for v in vtx[1:]:
            modelSpace.add_line((v0.x, v0.y), (v.x, v.y), dxfattribs={'layer': layerName})
            v0= v

    def writeDXFBasePlateContour(self, modelSpace, layerName):
        ''' Write the base plate contour in the model
            space argument

        :param modelSpace: ezdxf model space to write into.
        :param layerName: DXF layer name.
        '''
        deltaX= self.B/2.0
        deltaY= self.N/2.0
        positions= self.getContour().getVertexList()
        modelSpace.add_line((positions[0].x, positions[0].y), (positions[1].x, positions[1].y), dxfattribs={'layer': layerName})
        modelSpace.add_line((positions[1].x, positions[1].y), (positions[2].x, positions[2].y), dxfattribs={'layer': layerName})
        modelSpace.add_line((positions[2].x, positions[2].y), (positions[3].x, positions[3].y), dxfattribs={'layer': layerName})
        modelSpace.add_line((positions[3].x, positions[3].y), (positions[0].x, positions[0].y), dxfattribs={'layer': layerName})
        
    def writeDXFAnchorRodHoles(self, modelSpace, layerName):
        ''' Draw anchor rod holes in the model
            space argument

        :param modelSpace: ezdxf model space to write into.
        :param layerName: DXF layer name.
        '''
        boltDiameter= self.anchorGroup.anchors[0].diameter
        self.holeDiameter= self.getNominalHoleDiameter()
        origin2d= geom.Pos2d(self.origin.x, self.origin.y)
        for anchor in self.anchorGroup.anchors:
            center= origin2d+geom.Vector2d(anchor.pos3d.x, anchor.pos3d.y)
            modelSpace.add_circle((center.x, center.y), self.holeDiameter/2.0, dxfattribs={'layer': layerName})

    def writeDXF(self, modelSpace, steelShapeLayerName, basePlateLayerName, anchorHolesLayerName):
        ''' Draw the base plate in the model
            space argument

        :param modelSpace: ezdxf model space to write into.
        :param steelShapeLayerName: DXF layer name for steel shape contour.
        :param basePlateLayerName: DXF layer name for the base plate contour.
        :param anchorHolesLayerName: DXF layer name for the anchor rod holes.
        '''
        # Draw steel shape
        self.writeDXFShapeContour(modelSpace, steelShapeLayerName)
        # Draw base plate
        self.writeDXFBasePlateContour(modelSpace, basePlateLayerName)
        # Draw anchor holes
        self.writeDXFAnchorRodHoles(modelSpace, anchorHolesLayerName)

class BasePlateGroup(object):
    ''' Group of similar base plates.

    :ivar name: name of the base plates group.
    :ivar basePlates: base plate container (dictionary).
    '''
            
    def __init__(self, name: str):
        ''' Constructor.

        :param name: name of the base plate group.
        '''
        self.name= name
        self.basePlates= dict()

    def populateFromPointMap(self, templateBasePlate, pointMap):
        '''
        Populates the base plate dictionary with copies
        of the base plate argument.

        :param templateBasePlate: template base plate.
        :param pointMap: indentifiers and coordinates of the 
                       base plate centers.
        '''
        for key in pointMap:
            p= pointMap[key]
            self.basePlates[key]= templateBasePlate.getCopy()
            self.basePlates[key].origin= p

    def populate(self,constructorDict):
        '''
        Populates the base plate dictionary with the 
        object created by the constructors int the dictionary argument.

        :param constructorDict: constructors for each member.
        '''
        for key in constructorDict:
            constructor= constructorDict[key]
            print(constructor)
            self.basePlates[key]= eval(constructor)

    def allRodsWorkInShear(self):
        ''' All anchor rods contribute to shear strength.
        '''
        for key in self.basePlates:
            bp= self.basePlates[key]
            bp.nShearBolts= bp.anchorGroup.getNumberOfBolts() # Use welded washers

    def setRotatedAnchors(self, offset, anchorGroupRight, allRodsWorkInShear= True):
        ''' Set the anchor groups on the right.

        :param offset: displacement of the plate in the direction of the
                       flange. 
        :param allRodsWorkInShear: if true all anchor rods contribute
                                   to shear strength.
        '''
        for key in self.basePlates:
            bp= self.basePlates[key]
            bp.nShearBolts= bp.anchorGroup.getNumberOfBolts() # Use welded washers
            if(bp.origin.x>0.5):
                bp.offsetB= -offset
                if(anchorGroupRight):
                    bp.anchorGroup= anchorGroupRight
            else:
                bp.offsetB= offset

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        constructors= dict()
        objects= dict()
        for key in self.basePlates:
            print('key: ', key)
            basePlateClassName= str(self.basePlates[key].__class__)[8:-2].split('.')[-1]
            classModule= self.basePlates[key].__module__
            constructors[key]= basePlateClassName+'()'
            objects[key]= self.basePlates[key].getDict()
        retval['constructors']= constructors # Store constructors.
        retval['objects']= objects # Store objects.
        if(hasattr(self,'h_ef')):
           retval.update({'h_ef':self.h_ef})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        if('h_ef' in dct):
           self.h_ef= dct['h_ef']
        dct.pop('h_ef', None) # Remove already used key
        constructors= dct['constructors'] # Class dictionary
        self.populate(constructors) # Create object types.
        objects= dct['objects'] # Read objects.
        for key in objects:
            self.basePlates[key].setFromDict(objects[key])

    def jsonRead(self, inputFileName):
        ''' Read object from JSON file.

        :param inputFileName: name of the input file.
        '''
        with open(inputFileName) as json_file:
            basePlateGroupDict= json.load(json_file)
        self.setFromDict(basePlateGroupDict)
        json_file.close()
        
    def jsonWrite(self, outputFileName):
        ''' Write object to JSON file.

        :param outputFileName: name of the output file.
        '''
        with open(outputFileName, 'w') as outfile:
            json.dump(self.getDict(), outfile)
        outfile.close()
        
    def getBlocks(self, blockProperties):
        ''' Return the block decomposition of the base plates.

        :param blockProperties: labels and attributes to assign to the newly created blocks.
        '''
        retval= bte.BlockData()
        for key in self.basePlates:
            basePlate= self.basePlates[key]
            retval.extend(basePlate.getBlocks(blockProperties))
        return retval
        
    def writeDXF(self, modelSpace, steelShapeLayerName, basePlateLayerName, anchorHolesLayerName):
        ''' Draw the base plate in the model
            space argument

        :param modelSpace: ezdxf model space to write into.
        :param steelShapeLayerName: DXF layer name for steel shape contour.
        :param basePlateLayerName: DXF layer name for the base plate contour.
        :param anchorHolesLayerName: DXF layer name for the anchor rod holes.
        '''
        for key in self.basePlates:
            basePlate= self.basePlates[key]
            basePlate.writeDXF(modelSpace, steelShapeLayerName, basePlateLayerName, anchorHolesLayerName)

    def writeDXFFile(self):
        ''' Draw the base plate group in a DXF file.
        '''
        doc= ezdxf.new('R2010')
        doc.header['$MEASUREMENT'] = 1 # Metric
        doc.header['$LUNITS'] = 2 # Decimal units.
        doc.header['$INSUNITS'] = 6 # # Default drawing units.
        steelShapesLayerName= 'xc_'+self.name+'_steel_shapes'
        doc.layers.new(name= steelShapesLayerName, dxfattribs={'color': 4})
        basePlatesLayerName= 'xc_'+self.name+'_base_plates'
        doc.layers.new(name= basePlatesLayerName, dxfattribs={'color': 4})
        anchorHolesLayerName= 'xc_'+self.name+'_anchor_bolts'
        doc.layers.new(name= anchorHolesLayerName, dxfattribs={'color': 2})
        msp = doc.modelspace()  # add new entities to the modelspace
        self.writeDXF(msp, steelShapesLayerName, basePlatesLayerName, anchorHolesLayerName)
        fileName= self.name+'_base_plates.dxf'
        doc.saveas(fileName)
        
    def getNumberOfBolts(self):
        ''' Return the total number of bolts.'''
        retval= 0
        for key in self.basePlates:
            basePlate= self.basePlates[key]
            retval+= basePlate.anchorGroup.getNumberOfBolts()
        return retval
        
    def report(self, outputFile):
        ''' Writes base plate specification.

        :param outputFile: output file.
        '''        
        numberOfBolts= 0
        numberOfPlates= len(self.basePlates)
        outputFile.write(str(numberOfPlates)+' x ')
        firstKey= list(self.basePlates.keys())[0]
        basePlate= self.basePlates[firstKey]
        basePlate.report(outputFile)
        numberOfBolts= self.getNumberOfBolts()
        outputFile.write('total number of anchors: '+str(numberOfBolts)+'\n')
        outputFile.write('depth of embedment: '+ str(self.h_ef)+ ' m\n')
        #outputFile.write('number of base plates: '+str(len(self.basePlates)))

def readBasePlateGroupFromJSONFile(inputFileName):
    ''' Read base plate group object from a JSON file.

    :python inputFileName: name of the input file.
    '''
    retval= BasePlateGroup(name= '')
    retval.jsonRead(inputFileName)
    return retval

class CapacityFactors(object):
    ''' Capacity factors computed for a base plate group.

    :ivar basePlateGroup: group of base plates to check.
    :ivar spacingCF: anchor spacing capacity factor (result of checking 
                     distance between anchor rods [spacingCF<1.0 => OK].
    :ivar tCF: base plate thickness capacity factor (<1.0 => OK).
    :ivar rodCF: anchor rod capacity factor (<1.0 => OK).
    :ivar webStressCF: shaft web stress capacity factor (<1.0 => OK).
    :ivar concreteStrengthCF: concrete strength capacity factor (<1.0 => OK).
    :ivar pullOutCF: concrete pull out capacity factor (<1.0 => OK).
    :ivar breakOutCF: concrete break out capacity factor (<1.0 => OK).
    :ivar shearCF: anchor rods shear capacity factor (<1.0 => OK).
    '''
    def __init__(self, basePlateGroup):
        ''' Constructor.

        :param basePlateGroup: group of base plates to check.
        '''
        self.basePlateGroup= basePlateGroup
        self.spacingCF= 0.0
        self.tCF= 0.0
        self.rodCF= 0.0
        self.webStressCF= 0.0
        self.concreteStrengthCF= 0.0
        self.pulloutCF= 0.0
        self.breakoutCF= 0.0
        self.shearCF= 0.0
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        retval['basePlateGroup']= self.basePlateGroup.getDict()
        retval.update({'spacingCF':self.spacingCF, 'tCF': self.tCF, 'rodCF':self.rodCF, 'webStressCF': self.webStressCF, 'concreteStrengthCF':self.concreteStrengthCF, 'pulloutCF': self.pulloutCF, 'breakoutCF':self.breakoutCF, 'shearCF':self.shearCF})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        self.basePlateGroup.setFromDict(dct['basePlateGroup'])
        self.spacingCF= dct['spacingCF']
        self.tCF= dct['tCF']
        self.rodCF= dct['rodCF']
        self.webStressCF= dct['webStressCF']
        self.concreteStrengthCF= dct['concreteStrengthCF']
        self.pulloutCF= dct['pulloutCF']
        self.breakoutCF= dct['breakoutCF']
        self.shearCF= dct['shearCF']
                
    def jsonRead(self, inputFileName):
        ''' Read object from JSON file.

        :param inputFileName: name of the input file.
        '''
        with open(inputFileName) as json_file:
            dct= json.load(json_file)
        self.setFromDict(dct)
        json_file.close()

    def jsonWrite(self, outputFileName):
        ''' Write object to JSON file.

        :param outputFileName: name of the output file.
        '''
        with open(outputFileName, 'w') as outfile:
            json.dump(self.getDict(), outfile)
        outfile.close()
        
    def compute(self, reactions, h_ef):
        ''' Compute capacity factors.

        :param reactions: reaction values acting on baseplates.
        :param h_ef: anchor rods embedment.
        '''
        for pointId in reactions:
            # Base plate corresponding to the point identifier.
            basePlate= self.basePlateGroup.basePlates[pointId]
            basePlate.h_ef= h_ef
            # Base plate reactions
            basePlateReactions= reactions[pointId]
            for comb in basePlateReactions:
                values= basePlateReactions[comb]
                Pu= values['Rz'] # vertical load
                Rx= values['Rx']; Ry= values['Ry']
                Vu= math.sqrt(Rx**2+Ry**2) # shear
                self.shearCF= max(self.shearCF,basePlate.getShearEfficiency(Pu, Vu))
                self.concreteStrengthCF= max(self.concreteStrengthCF,basePlate.getConcreteStrengthEfficiency(Pu))
                self.tCF= max(self.tCF,basePlate.getThicknessEfficiency(Pu))
                self.pulloutCF= max(self.pulloutCF,basePlate.getPulloutEfficiency(Pu))
                self.breakoutCF= max(self.breakoutCF,basePlate.getBreakoutEfficiency(h_ef,Pu))
                self.webStressCF= max(self.webStressCF,basePlate.getWebStressEfficiency(Pu))
                self.rodCF= max(self.rodCF, basePlate.getRodTensileStrengthEfficiency(Pu))
                self.shearTensileInteractionCF= math.sqrt(self.rodCF**2+self.shearCF**2)
                self.spacingCF=  max(self.spacingCF,basePlate.anchorGroup.getMinimumSpacing(self.shearTensileInteractionCF)/basePlate.anchorGroup.getSpacing())
        self.maxCF= max([self.spacingCF, self.tCF, self.rodCF, self.webStressCF, self.concreteStrengthCF, self.pulloutCF, self.breakoutCF, self.shearCF, self.shearTensileInteractionCF])

        if(self.maxCF<= 1.0):
            self.basePlateGroup.h_ef= h_ef

    def report(self, outputFile):
        ''' Writes base plate specification.

        :param outputFile: output file.
        '''        
        #outputFile.write('fc= ', fc/1e6,' MPa')
        self.basePlateGroup.report(outputFile)
        outputFile.write('concreteStrengthCF= '+str(self.concreteStrengthCF)+'\n')
        outputFile.write('thickness efficiency tCF= '+str(self.tCF)+'\n')
        outputFile.write('rodCF= '+str(self.rodCF)+'\n')
        outputFile.write('spacingCF= '+str(self.spacingCF)+'\n')
        outputFile.write('webStressCF= '+str(self.webStressCF)+'\n')
        outputFile.write('pulloutCF= '+str(self.pulloutCF)+'\n')
        outputFile.write('breakoutCF= '+str(self.breakoutCF)+'\n')
        outputFile.write('shearCF= '+str(self.shearCF)+'\n')
        outputFile.write('shearTensileInteractionCF= '+str(self.shearTensileInteractionCF)+'\n')
        if(self.maxCF<= 1.0):
            outputFile.write(Fore.GREEN+'OK'+Style.RESET_ALL)
        elif(self.breakoutCF>1.0 and (max([self.tCF, self.rodCF, self.webStressCF, self.concreteStrengthCF, self.pulloutCF,self.shearCF])<=1.0)):
            outputFile.write(Fore.GREEN+'OK'+Style.RESET_ALL+', but supplementary reinforcement needed (breakoutCF>1).')            
        else:
            outputFile.write(Fore.RED+'KO'+Style.RESET_ALL)
        outputFile.write('\nh_ef= '+str(self.basePlateGroup.h_ef)+ 'm\n')

    def output(self, outputFileName, reportFile= sys.stdout):
        ''' Generate output: report+dxf file.

        :param outputFileName: name of the file for the DXF and JSON output.
        :param reportFile: output file for the report.
        '''
        self.report(reportFile)
        #print('h_ef= ', self.h_ef, 'm')
        self.basePlateGroup.writeDXFFile(outputFileName)
        basePlatesOutputFileName= './'+outputFileName+'_base_plates.json'
        self.jsonWrite(basePlatesOutputFileName)
