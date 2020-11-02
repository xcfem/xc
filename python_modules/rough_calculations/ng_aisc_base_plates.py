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

import math
import xc_base
import geom
from postprocess.reports import common_formats as fmt
from misc_utils import log_messages as lmsg
from materials.astm_aisc import ASTM_materials
import materials
from import_export import block_topology_entities as bte

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
    X= ((4.0*d*bf)/(d+bf)**2)*Pu/Pp
    if(X>1):
        lmsg.error('Pu ('+str(Pu/1e3)+' kN) too big with respect to Pp ('+str(Pp/1e3)+' kN) increase base plate area\n')
    lmbd= 2.0*math.sqrt(X)/(1+math.sqrt(1-X))
    lmbd= min(lmbd,1.0)
    lmbd_np= lmbd*math.sqrt(d*bf)/4.0
    l= max(m,n,lmbd_np)
    retval= l*math.sqrt(2.0*Pu/(phi*Fy*B*N))
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
    def __init__(self, N, B, t, steelShape, anchorGroup, fc, steel= ASTM_materials.A36, origin= geom.Pos3d()):
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
        steelShapeClassName= dct['steelShapeClassName']+'()'
        self.steelShape= eval(steelShapeClassName)
        self.steelShape.setFromDict(dct['steelShape'])
        self.anchorGroup= ASTM_materials.AnchorGroup(steel= None, diameter= 0.0, positions= [])
        self.anchorGroup.setFromDict(dct['anchorGroup'])
        steelClassName= dct['steelClassName']+'()'
        self.steel= eval(steelClassName)
        self.steel.setFromDict(dct['steel'])
        xyz= dct['origin']
        self.origin= geom.Pos3d(xyz[0],xyz[1],xyz[2])
        self.fc= dct['fc']
        self.nShearBolts= dct['nShearBolts']

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

    def getConcreteStrength(self):
        ''' Return the strenght of the concrete surface.'''
        return getConcreteStrength(self.getArea(), self.fc)

    def getConcreteStrengthEfficiency(self, Pu):
        ''' Return the efficiency of the concrete strength.'''
        if(Pu>0): # concrete compressed.
            return Pu/self.getConcreteStrength()
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
        if(Pu>0): # base plate compressed
            Pp= self.getConcreteStrength()
            return computePlateThickness(self.steelShape, self.N, self.B, Pu, Pp, self.steel.fy)
        else: # rods in tension
            T_rod= -Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
            tw= self.steelShape.get('tw') # Web thickness
            anchorLeverArm= self.getAnchorLeverArm()
            M= T_rod*anchorLeverArm-tw/2.0
            b_eff= (anchorLeverArm-tw/2.0)*2
            return math.sqrt(M*4.0/(b_eff*0.9*self.steel.fy))

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
        retval= phi*frictionCoef*Pu
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
        if(Pu>0): # base plate compressed
            shearStrength+= self.computeFrictionStrength(Pu)
        return Vu/shearStrength
    
    def getPulloutEfficiency(self, Pu):
        ''' Return the pullout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            anchorConcretePulloutStrength= self.anchorGroup.anchors[0].getDesignPulloutStrength(self.fc)
            T_rod= -Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
            retval= T_rod/anchorConcretePulloutStrength
        return retval
        
    def getBreakoutEfficiency(self, h_ef, Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param h_ef: depth of embedment.
        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            anchorConcreteBreakoutStrength= self.anchorGroup.getConcreteBreakoutStrength(h_ef, self.fc)
            retval= -Pu/anchorConcreteBreakoutStrength
        return retval
        
    def getWebStress(self, Pu):
        ''' Return the tension stress in the shaft web.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            T_rod= -Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
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
        if(Pu<0): # tensile load
            webStress= self.getWebStress(Pu)
            retval= webStress/(0.9*self.steelShape.steelType.fy)
        return retval

    def getRodTensileStrengthEfficiency(self,Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            Rn= self.anchorGroup.anchors[0].getDesignTensileStrength()
            T_rod= -Pu/self.anchorGroup.getNumberOfBolts() # Tensile load per anchor
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
    
    def report(self, outputFile):
        ''' Writes base plate specification.'''
        outputFile.write('  Base plate: \n')
        outputFile.write('    length: '+str(self.N*1000)+ ' mm\n')
        outputFile.write('    length offset: '+str(self.offsetN*1000)+ ' mm\n')
        outputFile.write('    width: '+str(self.B*1000)+ ' mm\n')
        outputFile.write('    width offset: '+str(self.offsetB*1000)+ ' mm\n')
        outputFile.write('    thickness: '+ str(self.t*1000)+ ' mm\n')
        outputFile.write('    base plate - column welds:\n')
        outputFile.write('      with the flange(s): 2 x '+str(math.floor(self.getFlangeWeldLegSize(0.3)*1000))+' mm (fillet weld leg size)\n')
        outputFile.write('      with the web: 2 x '+str(math.floor(self.getWebWeldLegSize()*1000))+' mm (fillet weld leg size)\n')
        #outputFile.write('   area: '+ self.getArea()+ ' m2')
        self.anchorGroup.report(outputFile)
