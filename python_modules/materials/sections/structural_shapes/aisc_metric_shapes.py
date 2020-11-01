# -*- coding: utf-8 -*-
''' AISC's structural steel shapes (metric units).'''

from __future__ import print_function
from __future__ import division

# Section axis:

#    AISC            XC

#       ^ Y           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> X        | -> Z
#       |             |
#     -----         -----

# The axis used in AISC catalogue are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

import math
from materials.sections.structural_shapes import aisc_shapes_dictionaries as shapes
from materials.sections.structural_shapes import aisc_shapes_labels as labels
from materials.sections import structural_steel
from misc_utils import log_messages as lmsg
import xc_base
import geom
from import_export import block_topology_entities as bte

# Shear areas.

# In y axis: Ay = depth of the section* web thickness
# In z axis: Az = 2/3 * combined area of the flanges. 

# *************************************************************************
# AISC W profiles.
# *************************************************************************
for item in shapes.W:
    shape= shapes.W[item]
    shape['Avy']= shape['h']*shape['tw'] # depth of the section* web thickness
    shape['Avz']= 2/3.0*(2*shape['b']*shape['tf']) # 2/3 * combined area of the flanges. 
    shape['alpha']= shape['Avy']/shape['A']
    shape['G']= shape['E']/(2*(1+shape['nu']))
    shape['hi']= shape['h']-2*shape['tf']
    shape['AreaQz']= 2*shape['b']*shape['tf'] 
    shape['AreaQy']= shape['A']-shape['AreaQz']
W= shapes.W

def getShapePlasticAxialLoad(shape):
    ''' Return the plastic axial load of the shape.

    :param shape: structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    return shape.steelType.fy*shape.get('A')

def getShapePlasticMoment(shape, majorAxis= True):
    ''' Return the plastic moment of the shape.

    :param shape: structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    Fy= shape.steelType.fy
    if(majorAxis):
        retval= Fy*shape.get('Wzpl')
    else:
        retval= min(Fy*shape.get('Wypl'), 1.6*Fy*shape.get('Wyel')) # equation F6-1
    return retval

def getShapeBendingCompactWebAndFlangeRatio(shape, majorAxis= True):
    ''' If web and flanges are compact according to table 4.1b of 
        AISC-360-16 return a value less than one.

    :param majorAxis: true if flexure about the major axis.
    '''
    return max(shape.bendingCompactWebRatio(majorAxis),shape.bendingCompactFlangeRatio(majorAxis))
    
def getUIShapeRts(shape, majorAxis= True):
    ''' Return the value of rts according to equation F2-7
        of AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error(__name__+': r_{ts} coefficient not implemented for minor axis.')
    Cw= shape.get('Cw') # Warping constant.
    Iy= shape.get('Iy') # Moment of inertia about minor axis.
    Sz= shape.get('Wzel') # Elastic section modulus about major axis.
    return math.sqrt(math.sqrt(Iy*Cw)/Sz)

def getUIShapeLambdaPFlangeBending(shape):
    '''Return he limiting slenderness for a compact flange, 
       defined in Table B4.1b of AISC-360-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    return 0.38*math.sqrt(E/Fy) # Case 10 or case 13

def getUIShapeLambdaRFlangeBending(shape):
    '''Return he limiting slenderness for a noncompact flange, 
       defined in Table B4.1b of AISC-360-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    return 1.0*math.sqrt(E/Fy) # Case 10 or case 13

def getUIShapeLambdaRFlangeCompression(shape):
    '''Return he limiting slenderness for a nonslender flange, 
       defined in Table B4.1a of AISC-360-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    return 0.56*math.sqrt(E/Fy) # Case 1

def getUIShapeLambdaPWebBending(shape):
    '''Return he limiting slenderness for a compact web, 
       defined in Table B4.1b of AISC-360-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    return 3.76*math.sqrt(E/Fy) # Case 10 or case 13


def getUIShapeLp(shape, majorAxis= True):
    '''Return the limiting laterally unbraced length for the limit state
       of yielding according to equation F2-5 of AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error(__name__+': L_p not implemented for minor axis.')
    ry= shape.get('iy') # Radius of gyration about minor axis.
    E= shape.get('E') # Elastic modulus.
    Fy= shape.steelType.fy # specified minimum yield stress
    return 1.76*ry*math.sqrt(E/Fy)

def getUIShapeLr(shape, majorAxis= True):
    '''Return the limiting unbraced length for the limit state 
       of inelastic lateral-torsional buckling according to
       equation F2-6 of  AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error(__name__+': L_r not implemented for minor axis.')
    rts= shape.getRts()
    E= shape.get('E') # Elastic modulus.
    Fy= shape.steelType.fy # specified minimum yield stress
    Sz= shape.get('Wzel') # Elastic section modulus about major axis.
    h0= shape.get('ho') # Distance between the flange centroids
    J= shape.get('It') # Torsional moment of inertia
    c= shape.getCCoefficient()
    JcQ= J*c/Sz/h0
    sqrt1= math.sqrt((JcQ)**2+6.76*(0.7*Fy/E)**2)
    sqrt2= math.sqrt((JcQ)+sqrt1)
    return 1.95*rts*E/0.7/Fy*sqrt2

def getUIShapeCriticalStressF(shape, lateralUnbracedLength, Cb, majorAxis= True):
    '''Return the critical stress for the limit state 
       of lateral-torsional buckling according to
       equation F2-4 of  AISC-360-16.

    :param shape: I or channel structural shape.
    :param lateralUnbracedLength: length between points that are either 
                                  braced against lateral displacement of
                                  the compression flange or braced against 
                                  twist of the cross section.
    :param Cb: lateral-torsional buckling modification factor.
    :param majorAxis: true if flexure about the major axis.
    '''
    E= shape.get('E') # Elastic modulus.
    if(not majorAxis):
        b_tf= 2.0*shape.get('bSlendernessRatio') # see lambda expression in F6. 
        return 0.69*E/b_tf**2 # equation F6-4
    else:
        J= shape.get('It') # Torsional moment of inertia
        Sz= shape.get('Wzel') # Elastic section modulus about major axis.
        h0= shape.get('ho') # Distance between the flange centroids
        rts= shape.getRts()
        Lb_rts2= (lateralUnbracedLength/rts)**2
        J= shape.get('It') # Torsional moment of inertia
        c= shape.getCCoefficient()
        sqroot= math.sqrt(1+0.078*J*c/Sz/h0*Lb_rts2)
        return Cb*math.pi**2*E/Lb_rts2*sqroot

def getUIShapeNominalFlexuralStrength(shape, lateralUnbracedLength, Cb, majorAxis= True):
    ''' Return the nominal flexural strength of the member
        according to equations F2-1 to F2-3 or F3-1 to F3-2 of AISC-360-16.

    :param shape: I or channel structural shape.
    :param lateralUnbracedLength: length between points that are either 
                                  braced against lateral displacement of
                                  the compression flange or braced against 
                                  twist of the cross section.
    :param Cb: lateral-torsional buckling modification factor.
    :param majorAxis: true if flexure about the major axis.
    '''
    Mn= 0.0
    Mp= shape.getPlasticMoment(majorAxis) # plastic moment.
    Fy= shape.steelType.fy # specified minimum yield stress
    if(not majorAxis): # section F6
        compactFlanges= shape.bendingCompactFlangeRatio(majorAxis)
        if(compactFlanges<=1.0): # flanges are compact.
            Mn= Mp # equation F6-1
        else: 
            slenderFlanges= shape.bendingSlenderFlangeRatio(majorAxis)
            Sy= shape.get('Wyel') # Elastic section modulus about minor axis.
            if(slenderFlanges<=1.0): # flanges are noncompact -> equation F6-2 applies.
                lmbd= 2.0*shape.get('bSlendernessRatio') # see lambda expression in F6. 
                lmbd_pf= shape.getLambdaPFlangeBending()
                lmbd_rf= shape.getLambdaRFlangeBending()
                Mn= Mp-(Mp-0.7*Fy*Sy)*((lmbd-lmbd_pf)/(lmbd_rf-lmbd_pf)) # equation F6-2
            else: # slender flanges.

                Fcr= shape.getCriticalStressF(None, None, majorAxis)
                Mn= Fcr*Sy # equation F6-3
    else:
        compactRatio= shape.bendingCompactWebAndFlangeRatio(majorAxis)
        Sz= shape.get('Wzel') # Elastic section modulus about major axis.
        if(compactRatio<=1.0): # flange and web are compact.
            Lb= lateralUnbracedLength
            Lp= shape.getLp(majorAxis)
            if(Lb<Lp): # equation F2-1 applies
                Mn= Mp
            else:
                Lr= shape.getLr(majorAxis)
                if(Lb<=Lr): # equation F2-2 applies
                    Mn= Cb*(Mp-(Mp-0.7*Fy*Sz)*((Lb-Lp)/(Lr-Lp)))
                    Mn= min(Mn,Mp)
                else: # equation F2-3 applies
                    Fcr= shape.getCriticalStressF(Lb, Cb, majorAxis)
                    Mn= min(Fcr*Sz, Mp)
        else: # flange or web or both are not compact.
            compactWeb= shape.bendingCompactWebRatio(majorAxis)
            if(compactWeb<=1.0): # web is compact.
                slenderFlanges= shape.bendingSlenderFlangeRatio(majorAxis)
                lmbd= shape.get('bSlendernessRatio')
                if(slenderFlanges<=1.0): # flanges are noncompact -> equation F3-1 applies.
                    lmbd_pf= shape.getLambdaPFlangeBending()
                    lmbd_rf= shape.getLambdaRFlangeBending()
                    Mn= Mp-(Mp-0.7*Fy*Sz)*((lmbd-lmbd_pf)/(lmbd_rf-lmbd_pf)) # equation F3-1
                else: # flanges are slender -> equation F3-2 applies.
                    kc= max(min(4.0/math.sqrt(shape.get('hSlendernessRatio')),0.76),0.35)
                    Mn= 0.9*shape.get('E')*kc*Sz/lmbd**2 # equation F3-2
            else: # web is not compact. To implement from sections F4 and F5
                lmsg.error(__name__+': nominal flexural strength for noncompact web sections not implemented yet.')
    return Mn

def getUIAw(shape, majorAxis= True):
    ''' Return area for shear strength calculation.'''
    if(majorAxis):
        return shape.h()*shape.get('tw')
    else:
        return 2.0*shape.get('tf')*shape.get('b') # two resisting elements.

def getUIWebPlateShearBucklingCoefficient(shape, a= 1e6):
    '''Return the web plate shear buckling coefficient
       according to expression G2-5 of AISC-360-16.

    :param a: clear distance between transverse stiffeners
    '''
    h= shape.h()
    if(a/h>3.0):
        return 5.34
    else:
        denom= (a/h)**2
        return 5.0(1.0+1.0/denom)
    
def getG22WebShearBucklingStrengthCoefficient(shape, kv, majorAxis= True):
    ''' Return the web shear stress coefficient Cv2 according
        to section G2.2 of AISC-360-16.

    :param kv: web plate shear buckling coefficient.
    :param majorAxis: true if flexure about the major axis.
    '''
    Cv2= 1.0
    h_t= shape.get('hSlendernessRatio')
    if(not majorAxis):
        h_t= shape.get('bSlendernessRatio')
    E= shape.get('E')
    Fy= shape.steelType.fy
    sqrtkvE_Fy= math.sqrt(kv*E/Fy)
    h_t_threshold= 1.10*sqrtkvE_Fy
    if(h_t<=h_t_threshold):
        Cv2= 1.0 # equation G2-9
    else:
        h_t_threshold2= 1.37*sqrtkvE_Fy
        if(h_t<=h_t_threshold2):
            Cv2= h_t_threshold2/h_t # equation G2-10
        else:
            Cv2= 1.51*kv*E/h_t**2/Fy  # equation G2-11
    return Cv2
    
def getUINominalShearStrengthWithoutTensionFieldAction(shape, a= 1e6, majorAxis= True):
    ''' Return the nominal shear strength according to equation
        G2-1 of AISC-360-16.

    :param a: clear distance between transverse stiffeners.
    '''
    if(majorAxis):
        Cv1= shape.getWebShearStrengthCoefficient(a)
        return 0.6*shape.steelType.fy*shape.getAw()*Cv1 # equation G2-1
    else:
        Cv2= shape.getWebShearBucklingStrengthCoefficient(kv= 1.2, majorAxis= majorAxis)
        Fy= shape.steelType.fy
        return 0.6*Fy*shape.getAw(majorAxis)*Cv2 # eauation G6-1

def getShapeTorsionalElasticBucklingStress(shape, effectiveLengthX):
    ''' Return the torsional or flexural-torsional elastic buckling stress
        of the member according to equations E4-2, E4-3 and E4-4 of 
        AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
    '''
    retval= 0.0
    symmetry= shape.getSymmetry()
    Lc= shape.getEffectiveLengthX()
    if(symmetry=='double'):
        retval= shape.getTorsionalElasticBucklingStress(Lc)
    elif(symmetry=='simple'):
        Fex= shape.getTorsionalElasticBucklingStress(Lc) # E4-7
        Fey= getFlexuralElasticBucklingStressY() #E4-6
        H= shape.getFlexuralConstant() # E4-8
        retval= (Fey+Fex)/2/H*(1-math.sqrt(1-(4.0*Fey*Fex*H)/(Fey+Fex)**2))
    else: # no simmetry: E4-4
        lmsg.error('Torsional elastic buckling stress for unsymmetric members not implemented yet.')

    return retval


# AISC 341-16 Seismic Provisions for Structural Steel Buildings
def getUIShapeLambdaMDFlange(shape):
    '''Return the limiting width-to-thickness ratio for the unstiffened flange
       of a moderate ductile member according to table D1.1 of AISC 341-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    Ry= shape.steelType.Ry
    return 0.40*math.sqrt(E/Fy/Ry) # Case 1

def getUIShapeLambdaHDFlange(shape):
    '''Return the limiting width-to-thickness ratio for the unstiffened flange
       of a highly ductile member according to table D1.1 of AISC 341-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    Ry= shape.steelType.Ry
    return 0.32*math.sqrt(E/Fy/Ry) # Case 1

def getUIShapeLambdaMDWeb(shape, factor= 1.57):
    '''Return the limiting width-to-thickness ratio for the web
       of a moderate ductile member according to table D1.1 of AISC 341-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    Ry= shape.steelType.Ry
    return factor*math.sqrt(E/Fy/Ry) # Case 5

def getUIShapeLambdaHDWeb(shape, factor= 1.57):
    '''Return the limiting width-to-thickness ratio for the web
       of a higly ductile member according to table D1.1 of AISC 341-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    Ry= shape.steelType.Ry
    return factor*math.sqrt(E/Fy/Ry) # Case 5

class WShape(structural_steel.IShape):
    '''W shape

    :ivar steel: steel material.
    :ivar name: shape name (i.e. W40X431).
    '''
    def __init__(self,steel= None, name= ''):
        ''' Constructor.
        '''
        super(WShape,self).__init__(steel,name,W)

    def h(self):
        ''' Return overall depth of member (d in AISC tables).'''
        return self.get('h')
      
    def d(self):
        ''' Return internal web height: clear distance between flanges
        less the fillet at each flange (h in AISC tables).'''
        return self.get('d')

    def getFlangeThickness(self):
        ''' Return the thickness of the flange.'''
        return self.get('tf')

    def getFlangeWidth(self):
        ''' Return the width of the flange.'''
        return self.get('b')
    
    def getWebThickness(self):
        ''' Return the thickness of the web.'''
        return self.get('tw')
    
    def getMetricName(self):
        '''Return the metric label from the US customary one.'''
        return getMetricLabel(self.name)
        
    def getContour(self):
        ''' Return the section contour.'''
        retval= geom.Polygon2d()
        halfB= self.getFlangeWidth()/2.0
        halfH= self.h()/2.0
        tf= self.getFlangeThickness()
        tw= self.getWebThickness()
        retval.appendVertex(geom.Pos2d(-halfB,-halfH))
        retval.appendVertex(geom.Pos2d(halfB,-halfH))
        retval.appendVertex(geom.Pos2d(halfB,-halfH+tf))
        retval.appendVertex(geom.Pos2d(tw/2.0,-halfH+tf))
        retval.appendVertex(geom.Pos2d(tw/2.0,halfH-tf))
        retval.appendVertex(geom.Pos2d(halfB,halfH-tf))
        retval.appendVertex(geom.Pos2d(halfB,halfH))
        retval.appendVertex(geom.Pos2d(-halfB,halfH))
        retval.appendVertex(geom.Pos2d(-halfB,halfH-tf))
        retval.appendVertex(geom.Pos2d(-tw/2.0,halfH-tf))
        retval.appendVertex(geom.Pos2d(-tw/2.0,-halfH+tf))
        retval.appendVertex(geom.Pos2d(-halfB,-halfH+tf))
        retval.appendVertex(geom.Pos2d(-halfB,-halfH))
        return retval
    
    def getMidLines(self):
        ''' Return the section mid-lines.'''
        halfB= self.getFlangeWidth()/2.0
        halfH= self.h()/2.0
        tf2= self.getFlangeThickness()/2.0
        bottom= -halfH+tf2
        top= halfH-tf2
        s0= geom.Segment2d(geom.Pos2d(-halfB,bottom), geom.Pos2d(0.0,bottom)) #Bottom flange.
        s1= geom.Segment2d(geom.Pos2d(0.0,bottom), geom.Pos2d(halfB,bottom))
        s2= geom.Segment2d(geom.Pos2d(0.0,bottom), geom.Pos2d(0.0,top)) # Web
        s3= geom.Segment2d(geom.Pos2d(-halfB,top), geom.Pos2d(0.0,top)) # Top flange.
        s4= geom.Segment2d(geom.Pos2d(0.0,top), geom.Pos2d(halfB,top))
        return [s0,s1,s2,s3,s4]

    def getWebMidPlane(self, org, extrusionVDir):
        ''' Return the mid plane of the web.

        :param org: origin point.
        :param extrusionVDir: extrusion direction vector.
        '''
        halfH= self.h()/2.0
        p0= geom.Pos3d(org.x,org.y-halfH, org.z)
        p1= geom.Pos3d(org.x,org.y+halfH, org.z)
        p2= p0+extrusionVDir
        return geom.Plane3d(p0,p1,p2)

    def getBottomFlangeMidPlane(self, org, extrusionVDir):
        ''' Return the mid plane of the bottom flange.

        :param org: origin point.
        :param extrusionVDir: extrusion direction vector.
        '''
        halfB= self.getFlangeWidth()/2.0
        halfH= self.h()/2.0
        bottom= -halfH+self.getFlangeThickness()/2.0
        p0= geom.Pos3d(org.x-halfB,org.y+bottom, org.z)
        p1= geom.Pos3d(org.x+halfB,org.y+bottom, org.z)
        p2= p0+extrusionVDir
        return geom.Plane3d(p0,p1,p2)

    def getTopFlangeMidPlane(self, org, extrusionVDir):
        ''' Return the mid plane of the bottom flange.

        :param org: origin point.
        :param extrusionVDir: extrusion direction vector.
        '''
        halfB= self.getFlangeWidth()/2.0
        halfH= self.h()/2.0
        top= halfH-self.getFlangeThickness()/2.0
        p0= geom.Pos3d(org.x-halfB,org.y+top, org.z)
        p1= geom.Pos3d(org.x+halfB,org.y+top, org.z)
        p2= p0+extrusionVDir
        return geom.Plane3d(p0,p1,p2)

    def getMidPoints(self):
        ''' Return the point at the middle of the web
            and the flange in local coordinates.
        '''
        halfB= self.getFlangeWidth()/2.0
        halfH= self.h()/2.0
        tf= self.getFlangeThickness()
        tf2= tf/2.0 # Half flange thickness.
        # Base points (Down)
        bottom= -halfH+tf2
        bottomFlange= [geom.Pos2d(-halfB,bottom), geom.Pos2d(0.0,bottom), geom.Pos2d(halfB,bottom)]
        top= halfH-tf2
        topFlange= [geom.Pos2d(-halfB, top), geom.Pos2d(0.0, top), geom.Pos2d(halfB, top)]
        return bottomFlange, topFlange

    def getMidPointsGlobalPos(self, refSys):
        ''' Return the position expressed in global coordinates
            of the pointsat the middle of the shape.

        :param refSys: reference systems which computes the global coordinates.
        '''
        localBottomFlange, localTopFlange= self.getMidPoints()
        bottomFlange= list()
        for p in localBottomFlange:
            bottomFlange.append(refSys.getPosGlobal(p))
        topFlange= list()
        for p in localTopFlange:
            topFlange.append(refSys.getPosGlobal(p))
        return bottomFlange, topFlange

    def getMidPlanesPoints(self, org, extrusionVDir):
        ''' Return the points at the middle of the
            plates.

        :param org: origin point.
        :param extrusionVDir: extrusion direction vector.
        '''
        # Reference system
        refPlane= geom.Plane3d(org, extrusionVDir)
        ref= geom.Ref2d3d(org, refPlane.getBase1(), refPlane.getBase2())
        # Base points (Down)
        localBottomFlange, localTopFlange= self.getMidPoints()
        bottomFlangeDown= list()
        for p in localBottomFlange:
            bottomFlangeDown.append(ref.getPosGlobal(p))
        topFlangeDown= list()
        for p in localTopFlange:
            topFlangeDown.append(ref.getPosGlobal(p))
        # Extruded points (Up)
        bottomFlangeUp= list()
        for p in bottomFlangeDown:
            bottomFlangeUp.append(p+extrusionVDir)
        topFlangeUp= list()
        for p in topFlangeDown:
            topFlangeUp.append(p+extrusionVDir)
        return {'bottomFlangeDown':bottomFlangeDown,'topFlangeDown':topFlangeDown,'bottomFlangeUp':bottomFlangeUp,'topFlangeUp':topFlangeUp}
                
    def getBlockData(self, org, extrusionVDir, blockProperties= None):
        ''' Return the kpoints and faces.

        :param org: origin point.
        :param extrusionVDir: extrusion direction vector.
        :param blockProperties: labels and attributes for the created blocks.
        '''
        shapeProperties= bte.BlockProperties.copyFrom(blockProperties)
        shapeProperties.appendAttribute('shape',self.name)
        midPoints= self.getMidPlanesPoints(org, extrusionVDir)
        retval= bte.BlockData()
        # Base points (A)
        bottomFlangeA= midPoints['bottomFlangeDown']
        topFlangeA= midPoints['topFlangeDown']
        bottomFlangeB= midPoints['bottomFlangeUp']
        topFlangeB= midPoints['topFlangeUp']
        self.bottomFlangeAId= list() # Dirty solution I know (LCPT)
        for p in bottomFlangeA:
            self.bottomFlangeAId.append(retval.appendPoint(-1,p.x,p.y,p.z,shapeProperties))
        self.topFlangeAId= list()
        for p in topFlangeA:
            self.topFlangeAId.append(retval.appendPoint(-1,p.x,p.y,p.z,shapeProperties))
        # Extruded points (B)
        self.bottomFlangeBId= list()
        for p in bottomFlangeB:
            self.bottomFlangeBId.append(retval.appendPoint(-1,p.x,p.y,p.z,shapeProperties))
        self.topFlangeBId= list()
        for p in topFlangeB:
            self.topFlangeBId.append(retval.appendPoint(-1,p.x,p.y,p.z,shapeProperties))
        # Faces
        tf= self.getFlangeThickness()
        ## Bottom flange.
        bottomFlange1Properties= bte.BlockProperties.copyFrom(shapeProperties)
        bottomFlange1Properties.appendAttribute('part','bottom_flange')
        bottomFlange1Properties.appendAttribute('sub_part',1)
        bottomFlange1= bte.BlockRecord(-1, 'face', [self.bottomFlangeAId[0],self.bottomFlangeAId[1],self.bottomFlangeBId[1],self.bottomFlangeBId[0]],bottomFlange1Properties, thk= tf, matId= self.steelType.name)
        retval.appendBlock(bottomFlange1)
        bottomFlange2Properties= bte.BlockProperties.copyFrom(shapeProperties)
        bottomFlange2Properties.appendAttribute('part','bottom_flange')
        bottomFlange2Properties.appendAttribute('sub_part',2)
        bottomFlange2= bte.BlockRecord(-1, 'face', [self.bottomFlangeAId[1],self.bottomFlangeAId[2],self.bottomFlangeBId[2],self.bottomFlangeBId[1]], bottomFlange2Properties, thk= tf, matId= self.steelType.name)
        retval.appendBlock(bottomFlange2)
        ## Top flange.
        topFlange1Properties= bte.BlockProperties.copyFrom(shapeProperties)
        topFlange1Properties.appendAttribute('part','top_flange')
        topFlange1Properties.appendAttribute('sub_part',1)
        topFlange1= bte.BlockRecord(-1, 'face', [self.topFlangeAId[0],self.topFlangeAId[1],self.topFlangeBId[1],self.topFlangeBId[0]], topFlange1Properties, thk= tf, matId= self.steelType.name)
        retval.appendBlock(topFlange1)
        topFlange2Properties= bte.BlockProperties.copyFrom(shapeProperties)
        topFlange2Properties.appendAttribute('part','top_flange')
        topFlange2Properties.appendAttribute('sub_part',2)
        topFlange2= bte.BlockRecord(-1, 'face', [self.topFlangeAId[1],self.topFlangeAId[2],self.topFlangeBId[2],self.topFlangeBId[1]], topFlange2Properties, thk= tf, matId= self.steelType.name)
        retval.appendBlock(topFlange2)
        ## Web
        webProperties= bte.BlockProperties.copyFrom(blockProperties)
        webProperties.appendAttribute('part','web')
        web= bte.BlockRecord(-1, 'face', [self.bottomFlangeAId[1],self.topFlangeAId[1],self.topFlangeBId[1],self.bottomFlangeBId[1]], webProperties, thk= self.getWebThickness(), matId= self.steelType.name)
        retval.faceBlocks= [bottomFlange1, bottomFlange2, topFlange1, topFlange2, web] # Dirty solution, I know (LCPT).
        retval.appendBlock(web)

        return retval
        
    def getWeldBlockData(self, flangeWeldLegSize, webWeldLegSize, blockProperties= None):
        ''' Return the lines corresponding to weld beads.

        :param flangeWeldLegSize: leg size for the weld to the flange.
        :param webWeldLegSize: leg size for the weld to the web.
        :param blockProperties: labels and attributes for the created blocks.
        '''
        retval= bte.BlockData()

        # Lines
        bottomFlange1WeldProperties= bte.BlockProperties.copyFrom(blockProperties)
        bottomFlange1WeldProperties.appendAttribute('part','bottom_flange')
        bottomFlange1WeldProperties.appendAttribute('sub_part',1)
        bottomFlange1WeldProperties.appendAttribute('legSize',flangeWeldLegSize)
        bottomFlange1= bte.BlockRecord(-1, 'line', [self.bottomFlangeAId[0],self.bottomFlangeAId[1]], blockProperties= bottomFlange1WeldProperties, thk= None, matId= self.steelType.name)
        retval.appendBlock(bottomFlange1)
        bottomFlange2WeldProperties= bte.BlockProperties.copyFrom(blockProperties)
        bottomFlange2WeldProperties.appendAttribute('part','bottom_flange')
        bottomFlange2WeldProperties.appendAttribute('sub_part',2)
        bottomFlange2WeldProperties.appendAttribute('legSize',flangeWeldLegSize)
        bottomFlange2= bte.BlockRecord(-1, 'line', [self.bottomFlangeAId[1],self.bottomFlangeAId[2]], blockProperties= bottomFlange2WeldProperties, thk= None, matId= self.steelType.name)
        retval.appendBlock(bottomFlange2)
        topFlange1WeldProperties= bte.BlockProperties.copyFrom(blockProperties)
        topFlange1WeldProperties.appendAttribute('part','top_flange')
        topFlange1WeldProperties.appendAttribute('sub_part',1)
        topFlange1WeldProperties.appendAttribute('legSize',flangeWeldLegSize)
        topFlange1= bte.BlockRecord(-1, 'line', [self.topFlangeAId[0],self.topFlangeAId[1]], blockProperties= topFlange1WeldProperties, thk= None, matId= self.steelType.name)
        retval.appendBlock(topFlange1)
        topFlange2WeldProperties= bte.BlockProperties.copyFrom(blockProperties)
        topFlange2WeldProperties.appendAttribute('part','top_flange')
        topFlange2WeldProperties.appendAttribute('sub_part',2)
        topFlange2WeldProperties.appendAttribute('legSize',flangeWeldLegSize)
        topFlange2= bte.BlockRecord(-1, 'line', [self.topFlangeAId[1],self.topFlangeAId[2]], blockProperties= topFlange2WeldProperties, thk= None, matId= self.steelType.name)
        retval.appendBlock(topFlange2)
        webWeldProperties= bte.BlockProperties.copyFrom(blockProperties)
        webWeldProperties.appendAttribute('part','web')
        webWeldProperties.appendAttribute('legSize',webWeldLegSize)
        web= bte.BlockRecord(-1, 'line', [self.bottomFlangeAId[1],self.topFlangeAId[1]], blockProperties= webWeldProperties, thk= None, matId= self.steelType.name)
        retval.weldBlocks= [bottomFlange1, bottomFlange2, topFlange1, topFlange2, web] # Dirty solution, I know (LCPT).
        retval.appendBlock(web)
        return retval        

    def getLambdaPFlangeBending(self):
        '''Return he limiting slenderness for a compact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaPFlangeBending(self)

    def getLambdaRFlangeBending(self):
        '''Return he limiting slenderness for a noncompact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaRFlangeBending(self)

    def bendingCompactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_p= self.getLambdaPFlangeBending()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then flanges are compact.
    
    def bendingSlenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_r= self.getLambdaRFlangeBending()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then flanges are noncompact.
    
    def getLambdaPWebBending(self):
        ''' Return the limiting Width-to-Thickness Ratio 
        between compact and noncompact webs according to 
        table B4.1b of AISC-360-16 case 15.
        '''
        return getUIShapeLambdaPWebBending(self)
    
    def getLambdaRWebBending(self):
        ''' Return the limiting Width-to-Thickness Ratio 
        between noncompact and slender webs according to 
        table B4.1b of AISC-360-16 case 15.
        '''
        return 5.70*math.sqrt(self.steelType.E/self.steelType.fy) # Case 19

    def bendingCompactWebRatio(self, majorAxis= True):
        ''' If web is compact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        lambda_p= self.getLambdaPWebBending()
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is compact.

    def bendingSlenderWebRatio(self, majorAxis= True):
        ''' If web is noncompact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one, otherwise the web
            is slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        lambda_r= self.getLambdaRWebBending() # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then web is noncompact.
    
    def bendingCompactWebAndFlangeRatio(self, shape, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeBendingCompactWebAndFlangeRatio(self, majorAxis)

    def getLambdaRFlangeCompression(self):
        ''' Return the limiting Width-to-Thickness Ratio 
        between nonslender and slender flanges according to 
        table B4.1a of AISC-360-16 case 1.
        '''
        return getUIShapeLambdaRFlangeCompression(self)
    
    def compressionSlendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= self.getLambdaRFlangeCompression()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # OK if < 1.0

    def getAw(self, majorAxis= True):
        ''' Return area for shear strength calculation.'''
        return getUIAw(self, majorAxis)
            
    def getWebPlateShearBucklingCoefficient(self, a= 1e6):
        '''Return the web plate shear buckling coefficient
           according to expression G2-5 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return getUIWebPlateShearBucklingCoefficient(self,a)
    
    def getWebShearStrengthCoefficient(self, a= 1e6):
        ''' Return the web shear stress coefficient Cv1 according
            to equations G2-2, G2-3 and G2-4 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        Cv1= 1.0
        h_tw= self.get('hSlendernessRatio')
        E_Fy= self.get('E')/self.steelType.fy
        h_tw_threshold= 2.24*math.sqrt(E_Fy)
        if(h_tw<=h_tw_threshold):
            Cv1= 1.0 # equation G2-2
        else:
            kv= self.getWebPlateShearBucklingCoefficient(a)
            h_tw_threshold2= 1.10*math.sqrt(kv*E_Fy)
            if(h_tw<=h_tw_threshold2):
                Cv1= 1.0 # equation G2-3
            else:
                Cv1= h_tw_threshold2/h_tw  # equation G2-4
        return Cv1
    
    def getWebShearBucklingStrengthCoefficient(self, kv, majorAxis= True):
        ''' Return the web shear stress coefficient Cv2 according
            to section G2.2 of AISC-360-16.

        :param kv: web plate shear buckling coefficient.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getG22WebShearBucklingStrengthCoefficient(self,kv,majorAxis)
    
    def getNominalShearStrengthWithoutTensionFieldAction(self, a= 1e6, majorAxis= True):
        ''' Return the nominal shear strength according to equation
            G2-1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return getUINominalShearStrengthWithoutTensionFieldAction(self,a, majorAxis)

    def getShearResistanceFactor(self):
        ''' Return the resistance factor for shear according to
            sections G1 and G2.1(a) of AISC-360-16.
        '''
        retval= 0.9 # see section G1.
        h_tw= self.get('hSlendernessRatio')
        E_Fy= self.get('E')/self.steelType.fy
        h_tw_threshold= 2.24*math.sqrt(E_Fy)
        if(h_tw<=h_tw_threshold):
            retval= 1.0 # see section G2.1(a)
        return retval

    def getDesignShearStrengthWithoutTensionFieldAction(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''

        rf= 0.9
        if(majorAxis):
            rf= self.getShearResistanceFactor()
        return rf*self.getNominalShearStrengthWithoutTensionFieldAction(a, majorAxis)

    def getDesignShearStrength(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return self.getDesignShearStrengthWithoutTensionFieldAction(a,majorAxis)

    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('Cw')
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Iy+Iz)
    
    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)

    def getCCoefficient(self, majorAxis= True):
        ''' Return the c coefficient according to equation
            F2-8a of AISC-360-16.'''
        return 1.0

    def getRts(self, majorAxis= True):
        ''' Return the value of rts according to equation F2-7
            of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeRts(self, majorAxis)

    def getLp(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length for the 
            limit state of yielding according to
            equation F2-5 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLp(self, majorAxis)
    
    def getLr(self, majorAxis= True):
        ''' Return the limiting unbraced length for the limit state 
            of inelastic lateral-torsional buckling according to
            equation F2-6 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLr(self, majorAxis)
    
    def getCriticalStressF(self, lateralUnbracedLength, Cb, majorAxis= True):
        '''Return the critical stress for the limit state 
           of lateral-torsional buckling according to
           equation F2-4 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        '''
        return getUIShapeCriticalStressF(self, lateralUnbracedLength, Cb, majorAxis)
    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations F2-1 to F2-3 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(lateralUnbracedLength, Cb, majorAxis)

    def getLambdaMDFlange(self):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           flange of a moderate ductile member according to table D1.1 
           of AISC 341-16.'''
        return getUIShapeLambdaMDFlange(self)
    
    def getLambdaHDFlange(self):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           flange of a highly ductile member according to table D1.1 
           of AISC 341-16.'''
        return getUIShapeLambdaHDFlange(self)
    
    def getLambdaMDWeb(self, N, M, momentFrameType= 0):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           web of a moderately ductile member according to table D1.1 
           of AISC 341-16.

        :param N: axial load.
        :param M: bending moment.
        :param momentFrameType: type of the moment frame (0: ordinary moment frame (OMF), 1: intermediate moment frame (IMF) 3: special moment frame).
        '''
        factor= 1.57
        e= 0.0 # eccentricity.
        if(N!=0.0):
            e= abs(M/N)
        else:
            e= abs(M)*1e6
        if(e>self.h()/2.0): # Bending -> beam.            
            # Ratio of required strength to available axial yield strength.
            Ca= N/self.getDesignTensileStrength() # table D1.1
            if(Ca<=0.114):
                factor= 3.96*(1-3.04*Ca)
            else:
                factor= max(1.29*(2.12-Ca),1.57)
            if(momentFrameType== 1): #IMF
                factor= min(factor, 3.96)
            elif(momentFrameType== 2): # SMF
                factor= min(factor, 2.57)
        return getUIShapeLambdaMDWeb(self, factor)
    
    def getLambdaHDWeb(self, N, M, momentFrameType= 0):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           web of a highly ductile member according to table D1.1 
           of AISC 341-16.

        :param N: axial load. 
        :param M: bending moment.
        :param momentFrameType: type of the moment frame (0: ordinary moment frame (OMF), 1: intermediate moment frame (IMF) 2: special moment frame).
        '''
        factor= 1.57
        e= 0.0 # eccentricity.
        if(N!=0.0):
            e= abs(M/N)
        else:
            e= abs(M)*1e6
        if(e>self.h()/2.0): # Bending -> beam.            
            # Ratio of required strength to available axial yield strength.
            Ca= N/self.getDesignTensileStrength() # table D1.1
            if(Ca<=0.114):
                factor= 2.57*(1-1.04*Ca)
            else:
                factor= max(0.88*(2.68-Ca),1.57)
            if(momentFrameType== 1): #IMF
                factor= min(factor, 3.96)
            elif(momentFrameType== 2): # SMF
                factor= min(factor, 2.57)
        return getUIShapeLambdaHDWeb(self, factor)

    def flangeLocalBucklingCheck(self, highlyDuctile= True):
        ''' Checks local buckling according to width-to-thickness ratios
            of members according to table D1.1 of AISC 341-16.

        :param higlyDuctile: if true the member is considered as highly
                             ductile.
        '''
        slendernessRatio= self.get('bSlendernessRatio')
        retval= slendernessRatio
        if(highlyDuctile):
            retval/= self.getLambdaHDFlange()
        else: # moderate ductile
            retval/= self.getLambdaMDFlange()            
        return retval

    def webLocalBucklingCheck(self, N, M, momentFrameType= 0, highlyDuctile= True):
        ''' Checks local buckling according to width-to-thickness ratios
            of members according to table D1.1 of AISC 341-16.

        :param N: axial load. 
        :param M: bending moment.
        :param momentFrameType: type of the moment frame (0: ordinary moment frame (OMF), 1: intermediate moment frame (IMF) 2: special moment frame).
        :param higlyDuctile: if true the member is considered as highly
                             ductile.
        '''
        slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio
        if(highlyDuctile):
            retval/= self.getLambdaHDWeb(N, M, momentFrameType)
        else: # moderately ductile
            retval/= self.getLambdaMDWeb(N, M, momentFrameType)            
        return retval
 
# *************************************************************************
# AISC C profiles.
# *************************************************************************

for item in shapes.C:
    shape= shapes.C[item]
    shape['G']= shape['E']/(2*(1+shape['nu']))
    shape['hi']= shape['h']-2*shape['tf']
    shape['Avy']= shape['h']*shape['tw'] # depth of the section* web thickness
    shape['Avz']= 2/3.0*(2*shape['b']*shape['tf']) # 2/3 * combined area of the flanges. 
    shape['AreaQz']= 2*shape['b']*shape['tf']
    shape['AreaQy']= shape['A']-shape['AreaQz']
C= shapes.C

class CShape(structural_steel.UShape):
    '''C shape.

    :ivar steel: steel material.
    :ivar name: shape name (i.e. C15X50).
    '''
    def __init__(self,steel,name):
        ''' Constructor.

        '''
        super(CShape,self).__init__(steel,name,C)

    def getMetricName(self):
        '''Return the metric label from the US customary one.'''
        return getMetricLabel(self.name)
        
    def getLambdaPFlangeBending(self):
        '''Return he limiting slenderness for a compact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaPFlangeBending(self)

    def getLambdaRFlangeBending(self):
        '''Return he limiting slenderness for a noncompact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaRFlangeBending(self)

    def getLambdaPWebBending(self):
        '''Return he limiting slenderness for a compact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaPWebBending(self)
    
    def bendingCompactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_p= self.getLambdaPFlangeBending()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then flanges are compact.
    
    def bendingSlenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_r= self.getLambdaRFlangeBending()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then flanges are noncompact.
    
    def bendingCompactWebRatio(self, majorAxis= True):
        ''' If web is compact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        lambda_p= self.getLambdaPWebBending()
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is compact.

    def bendingSlenderWebRatio(self, majorAxis= True):
        ''' If web is noncompact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one, otherwise the web
            is slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 5.70*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is noncompact.
    
    def bendingCompactWebAndFlangeRatio(self, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeBendingCompactWebAndFlangeRatio(self,majorAxis)
    
    def getAw(self, majorAxis= True):
        ''' Return area for shear strength calculation.'''
        return getUIAw(self, majorAxis)
    
    def getWebPlateShearBucklingCoefficient(shape, a= 1e6):
        '''Return the web plate shear buckling coefficient
           according to expression G2-5 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return getUIWebPlateShearBucklingCoefficient(shape,a)
    
    def getWebShearStrengthCoefficient(self, a= 1e6):
        ''' Return the web shear stress coefficient Cv1 according
            to equations G2-3 and G2-4 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        Cv1= 1.0
        h_tw= self.get('hSlendernessRatio')
        E_Fy= self.get('E')/self.steelType.fy
        kv= self.getWebPlateShearBucklingCoefficient(a)
        h_tw_threshold2= 1.10*math.sqrt(kv*E_Fy)
        if(h_tw<=h_tw_threshold2):
            Cv1= 1.0 # equation G2-3
        else:
            Cv1= h_tw_threshold2/h_tw  # equation G2-4
        return Cv1
    
    def getWebShearBucklingStrengthCoefficient(self, kv, majorAxis= True):
        ''' Return the web shear stress coefficient Cv2 according
            to section G2.2 of AISC-360-16.

        :param kv: web plate shear buckling coefficient.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getG22WebShearBucklingStrengthCoefficient(self,kv,majorAxis)

    def getNominalShearStrengthWithoutTensionFieldAction(self, a= 1e6 , majorAxis= True):
        ''' Return the nominal shear strength according to equation
            G2-1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getUINominalShearStrengthWithoutTensionFieldAction(self,a, majorAxis)
    
    def getDesignShearStrengthWithoutTensionFieldAction(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        :param majorAxis: true if flexure about the major axis.
        '''

        return 0.9*self.getNominalShearStrengthWithoutTensionFieldAction(a, majorAxis)
    
    def getDesignShearStrength(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return self.getDesignShearStrengthWithoutTensionFieldAction(a,majorAxis)

    def getLambdaRFlangeCompression(self):
        ''' Return the limiting Width-to-Thickness Ratio 
        between nonslender and slender flanges according to 
        table B4.1a of AISC-360-16 case 1.
        '''
        return getUIShapeLambdaRFlangeCompression(self)

    def compressionSlendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1a of AISC-360-16.'''
        lambda_r= self.getLambdaRFlangeCompression()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # OK if < 1.0
    
    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-7 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        Ag= self.get('A')
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('Cw')
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        z0= self.get('x')-self.get('e0')
        # Polar radius of gyration about the shear center.
        r02= z0**2+(Iy+Iz)/Ag # E4-9
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Ag*r02) # E4-7
    
    def getFlexuralConstant(self):
        ''' Return the flexural constant of the section according
            to equation E4-8 of AISC-360-16..'''
        Ag= self.get('A')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        z0= self.get('x')-self.get('e0')
        # Polar radius of gyration about the shear center.
        r02= z0**2+(Iy+Iz)/Ag # E4-9
        return 1-z0**2/r02
    
    def getCCoefficient(self, majorAxis= True):
        ''' Return the c coefficient according to equation
            F2-8b of AISC-360-16.'''
        if(not majorAxis):
            lmsg.error(__name__+': c coefficient not implemented for minor axis.')
        h0= self.get('ho') # Distance between the flange centroids
        Cw= self.get('Cw') # Warping constant.
        Iy= self.get('Iy') # Moment of inertia about minor axis.
        return h0/2.0*math.sqrt(Iy/Cw)

    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)

    def getRts(self, majorAxis= True):
        ''' Return the squared value of rts according to equation F2-7
            of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeRts(self, majorAxis)

    def getLp(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length for the 
            limit state of yielding according to
            equation F2-5 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLp(self, majorAxis)
    
    def getLr(self, majorAxis= True):
        ''' Return the limiting unbraced length for the limit state 
            of inelastic lateral-torsional buckling according to
            equation F2-6 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLr(self, majorAxis)

    def getCriticalStressF(self, lateralUnbracedLength, Cb, majorAxis= True):
        '''Return the critical stress for the limit state 
           of lateral-torsional buckling according to
           equation F2-4 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        '''
        return getUIShapeCriticalStressF(self, lateralUnbracedLength, Cb, majorAxis)

    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations F2-1 to F2-3 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(lateralUnbracedLength, Cb, majorAxis)
    
    def getLambdaMDFlange(self):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           flange of a moderate ductile member according to table D1.1 
           of AISC 341-16.'''
        return getUIShapeLambdaMDFlange(self)
    
    def getLambdaHDFlange(self):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           flange of a highly ductile member according to table D1.1 
           of AISC 341-16.'''
        return getUIShapeLambdaHDFlange(self)
    
    def getLambdaMDWeb(self):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           web of a moderate ductile member according to table D1.1 
           of AISC 341-16.'''
        return getUIShapeLambdaMDWeb(self)
    
    def getLambdaHDWeb(self):
        '''Return the limiting width-to-thickness ratio for the unstiffened 
           web of a highly ductile member according to table D1.1 
           of AISC 341-16.'''
        return getUIShapeLambdaHDWeb(self)

# *************************************************************************
# AISC Hollow Structural Sections.
# *************************************************************************

for item in shapes.HSS:
    shape= shapes.HSS[item]
    shape['alpha']= 5/12.0
    shape['G']= shape['E']/(2*(1+shape['nu']))
    if('h_flat' in shape): # rectangular
        shape['Avy']= 2*0.7*shape['h_flat']*shape['t']
        shape['Avz']= 2*0.7*shape['b_flat']*shape['t']
    else: # circular
        tmp= math.pi*(shape['OD']-shape['t'])/2.0*shape['t']
        shape['Avy']= tmp
        shape['Avz']= tmp
    shape['AreaQz']= shape['Avz']
    shape['AreaQy']= shape['Avy']
    
HSS= shapes.HSS

class HSSShape(structural_steel.QHShape):
    ''' Hollow structural section.

    :ivar steel: steel material.
    :ivar name: shape name (i.e. HSS2X2X_250).
    '''
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        super(HSSShape,self).__init__(steel,name,HSS)

    def getMetricName(self):
        '''Return the metric label from the US customary one.'''
        return getMetricLabel(self.name)

    def getLambdaRCompression(self):
        ''' Return the Limiting Width-to-Thickness Ratio
        between nonslender and slender elements
        according to case 6 of table B4.1a of AISC-360-16.
        '''
        return 1.4*math.sqrt(self.steelType.E/self.steelType.fy)

    def getBClassification(self):
        ''' Return the classification for local buckling of the
            "b" wall of the section according to table B4.1a
            of AISC 360-16.
        '''
        retval= 'nonslender'
        bSlendernessRatio= self.get('bSlendernessRatio')
        lambda_r= self.getLambdaRCompression()
        if(bSlendernessRatio>lambda_r):
            retval= 'slender'
        return retval
    
    def getHClassification(self):
        ''' Return the classification for local buckling of the
            "h" wall of the section according to table B4.1a
            of AISC 360-16.
        '''
        retval= 'nonslender'
        hSlendernessRatio= self.get('hSlendernessRatio')
        lambda_r= self.getLambdaRCompression()
        if(hSlendernessRatio>lambda_r):
            retval= 'slender'
        return retval
    
    def getClassification(self):
        ''' Return the classification for local buckling of the
            section according to table B4.1a of AISC 360-16.
        '''
        retval= 'nonslender'
        if((self.getHClassification()=='slender') or (self.getBClassification()=='slender')):
            retval= 'slender'
        return retval
    
    def getLambdaPFlangeBending(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 1.12*math.sqrt(self.steelType.E/self.steelType.fy) # Case 17
    
    def getLambdaRFlangeBending(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 1.40*math.sqrt(self.steelType.E/self.steelType.fy) # Case 17
    
    def getBFlexureClassification(self):
        ''' Return the classification for local buckling of the
            "b" wall of the section according to table B4.1b
            of AISC 360-16.
        '''
        retval= 'compact'
        bSlendernessRatio= self.get('bSlendernessRatio')
        lambda_P= self.getLambdaPFlangeBending()
        lambda_R= self.getLambdaRFlangeBending()
        if(bSlendernessRatio>lambda_P):
            retval= 'noncompact'
        elif(bSlendernessRatio>lambda_R):
            retval= 'slender'
        return retval
    
    def getLambdaPWebBending(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 2.42*math.sqrt(self.steelType.E/self.steelType.fy) # Case 19
    
    def getLambdaRWebBending(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 5.70*math.sqrt(self.steelType.E/self.steelType.fy) # Case 19
    
    def getHFlexureClassification(self):
        ''' Return the classification for local buckling of the
            "h" wall of the section according to table B4.1b
            of AISC 360-16.
        '''
        retval= 'compact'
        hSlendernessRatio= self.get('hSlendernessRatio')
        lambda_P= self.getLambdaPWebBending()
        lambda_R= self.getLambdaRWebBending()
        if(hSlendernessRatio>lambda_P):
            retval= 'noncompact'
        elif(hSlendernessRatio>lambda_R):
            retval= 'slender'
        return retval

    # Bending
    def bendingCompactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to case
            17 (HSS sections subject to flexure) of
            table 4.1b of AISC-360-16 return a value 
            less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        lambda_p= self.getLambdaPFlangeBending()
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_p
        return retval # if <1 then flanges are compact.
    
    def bendingSlenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        lambda_r= self.getLambdaRFlangeBending()
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_r
        return retval # if <1 then flanges are compact.
    
    def bendingCompactWebRatio(self, majorAxis= True):
        ''' If webs are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        lambda_p= self.getLambdaPWebBending()
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_p
        return retval # if <1 then webs are compact.
    
    def bendingSlenderWebRatio(self, majorAxis= True):
        ''' If web is noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            it's slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        lambda_r= self.getLambdaRWebBending()
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_r
        return retval # if <1 then webs are noncompact.
    
    def bendingCompactWebAndFlangeRatio(self, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeBendingCompactWebAndFlangeRatio(self,majorAxis)
    
    def getAw(self, majorAxis= True):
        ''' Return area for shear strength calculation.'''
        t= self.get('t')
        if(majorAxis): # see equation G4-1
            h= self.get('h_flat')
            return 2.0*h*t
        else: # see equation G6-1
            b= self.get('b_flat')
            return b*t

    
    def getWebShearBucklingStrengthCoefficient(self, kv, majorAxis= True):
        ''' Return the web shear stress coefficient Cv2 according
            to section G2.2 of AISC-360-16.

        :param kv: web plate shear buckling coefficient.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getG22WebShearBucklingStrengthCoefficient(self,kv,majorAxis)
    
    def getNominalShearStrength(self, Lv= 1e6, majorAxis= True):
        ''' Return the nominal shear strength according to equations
            G4-1 and G6-1 of AISC-360-16.

        :param Lv: distance from maximum to zero shear force.
        :param majorAxis: true if flexure about the major axis.
        '''
        Fy= self.steelType.fy
        if(majorAxis):
            Cv2= self.getWebShearBucklingStrengthCoefficient(kv= 5, majorAxis= majorAxis)
            return 0.6*Fy*self.getAw(majorAxis)*Cv2 # equation G4-1
        else:
            Cv2= self.getWebShearBucklingStrengthCoefficient(kv= 1.2, majorAxis= majorAxis)
            return 0.6*Fy*self.getAw(majorAxis)*Cv2 # eauation G6-1


    def getDesignShearStrength(self, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''

        return 0.9*self.getNominalShearStrength(majorAxis)
    
    def compressionSlendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1a of AISC-360-16.'''
        lambda_r= self.getLambdaRCompression()
        slendernessRatio= max(self.get('hSlendernessRatio'),self.get('bSlendernessRatio'))
        return slendernessRatio/lambda_r # OK if < 1.0
    
    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('C') #HSS torsional constant.
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Iy+Iz)
    
    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)

    def getReducedEffectiveH(self):
        '''Return the reduced effective width corresponding
           to the "h" walls of the shape.'''
        rt= math.sqrt(self.steelType.E/self.steelType.fy) #Simplification
        h_t= self.get('hSlendernessRatio')
        t= self.get('t')
        retval= 1.92*t*rt*(1.0-0.38/h_t*rt)
        retval= min(retval,self.get('h'))
        return retval
    
    def getReducedEffectiveB(self):
        '''Return the reduced effective width corresponding
           to the "b" walls of the shape.'''
        rt= math.sqrt(self.steelType.E/self.steelType.fy) #Simplification
        b_t= self.get('bSlendernessRatio')
        t= self.get('t')
        retval= 1.92*t*rt*(1.0-0.38/b_t*rt)
        retval= min(retval,self.get('b'))
        return retval
    
    def getEffectiveArea(self):
        '''Return the effective area.'''
        retval= self.get('A')
        classif= self.getClassification()
        if(classif == 'slender'):
            t= self.get('t')
            h_ineff= self.get('h_flat')-self.getReducedEffectiveH()
            retval-= 2.0*h_ineff*t
            b_ineff= self.get('b_flat')-self.getReducedEffectiveB()
            retval-= 2.0*b_ineff*t
        return retval

    def getEffectiveWidth(self, majorAxis= True):
        ''' Return the effective width of the flange for
            rectangular HSS sections according 
            to equation F7-4 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        E= self.get('E')
        Fy= self.steelType.fy
        root= math.sqrt(E/Fy)
        tf= self.get('t')
        b= self.get('b')
        if(not majorAxis):
            b= self.get('h')
        return min(1.92*tf*root*(1.0-0.38*root/(b/tf)),b)

    def getEffectiveInertia(self, majorAxis= True):
        ''' Return the effective moment of inertia for
            rectangular HSS sections due to 
            the "removed" area from the flange.

        :param majorAxis: true if flexure about the major axis.'''
        self.beff= self.getEffectiveWidth(majorAxis) # effective width
        b= self.get('b')
        if(not majorAxis):
            b= self.get('h')
        b_removed= b-self.beff # ineffective width of compression flange
        tf= self.get('t')/2.0
        A_removed= b_removed*tf # ineffective area of compression flange
        A= self.get('A')
        self.y_eff= (A*b/2.0-A_removed*(b-tf/2.0))/(A-A_removed) # new neutral axis measured from bottom of section.
        dna= (b/2.0)-self.y_eff # difference between old an new neutral axis.
        I_neg= 1/12.0*b_removed*tf**3 # moment of inertia of removed piece about ist own neutral axis.
        d_neg= b-self.y_eff-tf/2.0 # distance for new neutral axis to centroid of removed piece.
        I= self.get('Iz')
        if(not majorAxis):
            I= self.get('Iy')        
        return I+A*dna**2-I_neg-A_removed*d_neg**2
    
    def getEffectiveSectionModulus(self, majorAxis= True):
        ''' Return the effective section modulus determined 
            with the effective width of the compression angle
            calculated with equation F7-4 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        self.Ieff= self.getEffectiveInertia(majorAxis)
        b= self.get('b')
        if(not majorAxis):
            b= self.get('h')
        return self.Ieff/(b-self.y_eff)
                   
    def getFlangeLocalBucklingLimit(self, majorAxis= True):
        ''' Return the limit for nominal flexural strength for square of
            rectangular HSS sections due to flange buckling according 
            to section F7.2 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        Mp= self.getPlasticMoment(majorAxis)
        Mn= 0.0 
        compactFlanges= self.bendingCompactFlangeRatio(majorAxis)
        if(compactFlanges<=1.0):
            Mn= Mp # equation F7-1
        else:
            Fy= self.steelType.fy
            slenderFlanges= self.bendingSlenderFlangeRatio(majorAxis)
            if(slenderFlanges<1.0): # flanges are noncompact.
                E= self.get('E')
                S= self.get('Wzel') # Elastic section modulus about major axis.
                lmbd= self.get('bSlendernessRatio') # major axis slenderness ratio.
                if(not majorAxis):
                    S= self.get('Wyel') # Elastic section modulus about minor axis.
                    lmbd= self.get('hSlendernessRatio') # minor axis slenderness ratio.
                Mn= min(Mp-(Mp-Fy*S)*(3.57*lmbd*math.sqrt(Fy/E)-4.0),Mp) # equation F7-2
            else:
                Se= self.getEffectiveSectionModulus(majorAxis)
                Mn= Se*Fy # equation F7-3
        return Mn
    
    def getWebLocalBucklingLimit(self, majorAxis= True):
        ''' Return the limit for nominal flexural strength for square of
            rectangular HSS sections due to web buckling according 
            to section F7.3 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        Mp= self.getPlasticMoment(majorAxis)
        Mn= 0.0
        compactWebs= self.bendingCompactWebRatio(majorAxis)
        if(compactWebs<=1.0):
            Mn= Mp # equation F7-1
        else:
            Fy= self.steelType.fy
            slenderWebs= self.bendingSlenderWebRatio(majorAxis)
            if(slenderWebs<1.0): # webs are noncompact.
                E= self.get('E')
                S= self.get('Wzel') # Elastic section modulus about major axis.
                lmbd= self.get('bSlendernessRatio') # major axis slenderness ratio.
                if(not majorAxis):
                    S= self.get('Wyel') # Elastic section modulus about minor axis.
                    lmbd= self.get('hSlendernessRatio') # minor axis slenderness ratio.
                Mn= min(Mp-(Mp-Fy*S)*(0.305*lmbd*math.sqrt(Fy/E)-0.738),Mp) # equation F7-6
            else:
                lmsg.error(__name__+": something went wrong: there are no HSS with slender webs.")
                Mn= 0.0
        return Mn
    
    def getLimitingLaterallyUnbracedLengthForYielding(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length 
            for the limit state of yielding according
            to expression F7-12 of AISC 360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        ry= self.get('iy')
        if(not majorAxis):
            ry= self.get('iz')
        retval= 0.13*self.steelType.E*ry
        retval*= math.sqrt(self.get('It')*self.get('A'))
        Mp= self.getPlasticMoment(majorAxis)
        retval/=Mp
        return retval
    
    def getLimitingLaterallyUnbracedLengthForInelasticBuckling(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length 
            for the limit state of inelastic lateral torsional
            buckling according to expression F7-13 of AISC 360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        ry= self.get('iy')
        if(not majorAxis):
            ry= self.get('iz')
        retval= 2.0*self.steelType.E*ry
        retval*= math.sqrt(self.get('It')*self.get('A'))
        M= 0.7*self.steelType.fy*self.get('Wyel')
        retval/=M
        return retval
    
    def getLateralTorsionalBucklingLimit(self, Lb, Cb, majorAxis= True):
        ''' Return the maximum flexural strength
        due to lateral-torsional buckling according to
        expressions F7-10 to F7-11 of AISC 360-16

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor Cb
                   for non uniform moment diagrams when both ends of the 
                   segment are braced according to expression 
                   F1-1 of AISC 360-16.
        '''
        Mp= self.getPlasticMoment(majorAxis)
        Lp= self.getLimitingLaterallyUnbracedLengthForYielding(majorAxis)
        retval= 0.0
        if(Lb<=Lp): #No lateral-torsional buckling.
            retval= Mp
        else: #Lb>Lp
            Lr= self.getLimitingLaterallyUnbracedLengthForInelasticBuckling(majorAxis)
            if(Lb<Lr): # equation F7-10
                Sx= self.get('Wyel')
                if(not majorAxis):
                    Sx= self.get('Wzel')
                retval= Mp-(Mp-0.7*self.steelType.fy*Sx)*(Lb-Lp)/(Lr-Lp) 
                retval*= Cb
                retval= min(Mp,retval)
            else: #Lb>Lp  equation F7-11
                retval= 2.0*self.steelType.E*Cb
                retval*= math.sqrt(self.get('It')*self.get('A'))
                ry= self.get('iy')
                if(not majorAxis):
                    ry= self.get('iz')
                retval/= (Lb/ry)
        return retval

    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations F7-1 to F7-13 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        Mp= self.getPlasticMoment(majorAxis)
        Mn= 0.0

        compactSection= self.bendingCompactWebAndFlangeRatio(majorAxis)
        if(compactSection<=1.0):
            Mn= Mp # equation F7-1
        else:
            Mn= Mp
            Mnf= self.getFlangeLocalBucklingLimit(majorAxis)
            Mn= min(Mn,Mnf)
            Mnw= self.getWebLocalBucklingLimit(majorAxis)                
            Mn= min(Mn,Mnw)
            Mlt= self.getLateralTorsionalBucklingLimit(lateralUnbracedLength, Cb, majorAxis)
            Mn= min(Mn,Mlt)
        return Mn
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(lateralUnbracedLength, Cb, majorAxis)
    
    def getLambdaMD(self):
        '''Return the limiting width-to-thickness ratio for the walls 
           of a moderate ductile member according to table D1.1 
           of AISC 341-16.'''
        E= shape.get('E')
        Fy= shape.steelType.fy
        Ry= shape.steelType.Ry
        return 0.76*math.sqrt(E/Fy/Ry) # Case 4
    
    def getLambdaHD(self):
        '''Return the limiting width-to-thickness ratio for the walls 
           of a highly ductile member according to table D1.1 
           of AISC 341-16.'''
        E= shape.get('E')
        Fy= shape.steelType.fy
        Ry= shape.steelType.Ry
        return 0.65*math.sqrt(E/Fy/Ry) # Case 4
    
for item in shapes.CHSS:
    shape= shapes.CHSS[item]
    shape['alpha']= 5/12.0
    shape['G']= shape['E']/(2*(1+shape['nu']))
    tmp= math.pi*(shape['OD']-shape['t'])/2.0*shape['t']
    shape['Avy']= tmp
    shape['Avz']= tmp
    shape['AreaQz']= shape['Avz']
    shape['AreaQy']= shape['Avy']

CHSS= shapes.CHSS

class CHSSShape(structural_steel.CHShape):
    ''' Circular hollow structural section.

    :ivar steel: steel material.
    :ivar name: shape name (i.e. HSS16.000X0.375).
    '''
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        super(CHSSShape,self).__init__(steel,name,CHSS)

    def getMetricName(self):
        '''Return the metric label from the US customary one.'''
        return getMetricLabel(self.name)

    def getLambdaRCompression(self):
        ''' Return the Limiting Width-to-Thickness Ratio
        between nonslender and slender elements 
        according to case 9 in table B4.1a of AISC-360-16.
        '''
        return 0.11*(self.steelType.E/self.steelType.fy)

    def getLambdaPBending(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to case 20 in table B4.1b of AISC 360-16.
        '''
        return 0.07*(self.steelType.E/self.steelType.fy) # Case 20 (no square root here)
    
    def getLambdaRBending(self):
        ''' Return the limiting Width-to-Thickness Ratio (noncompact/slender)
            according to case 20 in table B4.1b of AISC 360-16.
        '''
        return 0.31*(self.steelType.E/self.steelType.fy) # Case 20 (no square root here)

    # Bending
    def bendingCompactRatio(self):
        ''' If section is compact according to case
            20 (round HSS sections subject to flexure) of
            table B4.1b of AISC-360-16 return a value 
            less than one.
        '''
        retval= 10.0
        slendernessRatio= self.get('slendernessRatio')
        lambda_p= self.getLambdaPBending() 
        retval= slendernessRatio/lambda_p
        return retval # if <1 then section is compact.
    
    def bendingSlenderRatio(self):
        ''' If section is noncompact according to table B4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        '''
        retval= 10.0
        slendernessRatio= self.get('slendernessRatio')
        lambda_r= self.getLambdaRBending()
        retval= slendernessRatio/lambda_r
        return retval # if <1 then flanges are compact.
    
    def getAw(self):
        ''' Return area for shear strength calculation.'''
        return self.get('A')
    
    def getNominalShearStrength(self, Lv= None):
        ''' Return the nominal shear strength according to equations
            G4-1 and G5-1, G5-2a and G5-2b of AISC-360-16.

        :param Lv: distance from maximum to zero shear force.
        :param majorAxis: true if flexure about the major axis.
        '''
        Fy= self.steelType.fy

        E= self.get('E')
        D= self.get('OD')
        slendernessRatio= self.get('slendernessRatio')
        Fcr= 0.0
        if(Lv):
           Fcr= 1.6*E/math.sqrt(Lv/D)/math.pow(slendernessRatio,5.0/4.0) # equation G5-2a
        Fcr= max(Fcr, 0.78*E/math.pow(slendernessRatio,3.0/2.0)) # equation G-2b
        Fcr= min(Fcr,0.6*Fy)
        return Fcr*self.getAw()/2.0

    def getDesignShearStrength(self, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param majorAxis: dummy argument needed for compatibility with
                          the other shapes.
        '''
        return 0.9*self.getNominalShearStrength()
    
    def compressionSlendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to case 9 of table B4.1a of AISC-360-16.'''
        lambda_r= self.getLambdaRCompression()
        slendernessRatio= self.get('slendernessRatio')
        return slendernessRatio/lambda_r # nonslender if < 1.0
    
    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('C') #HSS torsional constant.
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Iy+Iz)
    
    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self):
        ''' Return the plastic moment of the section.

        '''
        return getShapePlasticMoment(self)

    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations F7-1 to F7-13 and F8-1 to F8-4 
            of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        Mn= 0.0
        lmsg.error(__name__+'; nominal flexural strength for circular HSS sections not implemented yet.')
        return Mn
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(lateralUnbracedLength, Cb, majorAxis)

    def getLambdaMD(self):
        '''Return the limiting width-to-thickness ratio for the wall 
           of a moderate ductile member according to table D1.1 
           of AISC 341-16.'''
        E= shape.get('E')
        Fy= shape.steelType.fy
        Ry= shape.steelType.Ry
        return 0.062*(E/Fy/Ry) # Case 10
    
    def getLambdaHD(self):
        '''Return the limiting width-to-thickness ratio for the wall 
           of a highly ductile member according to table D1.1 
           of AISC 341-16.'''
        E= shape.get('E')
        Fy= shape.steelType.fy
        Ry= shape.steelType.Ry
        return 0.053*math.sqrt(E/Fy/Ry) # Case 10
    
# Label conversion metric->US customary | US customary -> metric.
def getUSLabel(metricLabel):
    '''Return the US customary label from the metric one.'''
    return labels.MetricLabel[metricLabel]

def getMetricLabel(USLabel):
    '''Return the metric label from the US customary one.'''
    return labels.USLabel[USLabel]




