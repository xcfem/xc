# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

#import xc_base
import geom
import uuid
import xc
import math
from materials import typical_materials as tm
from misc_utils import data_struct_utils as dsu
from materials.sections import section_properties as sectpr
from misc_utils import log_messages as lmsg
from materials.astm_aisc import ASTM_materials
from model import predefined_spaces

'''References:
[1] Steel connection analysis, by Paolo Rugarli
[2] Specification for Structural Steel Buildings,July 7, 2016
    American Institute of Steel Construction
'''
__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"


def cross(xcV1,xcV2):
    '''cross product of two 3D-vectors created from two lists
    of 3 elements
    '''
    v1=geom.Vector3d(xcV1[0],xcV1[1],xcV1[2])
    v2=geom.Vector3d(xcV2[0],xcV2[1],xcV2[2])
    vcross=v1.cross(v2)
    xcVcross=xc.Vector([vcross.x,vcross.y,vcross.z])
    return xcVcross

class Bolt(object):
    '''Class intended to define a bolt by its diameter and material and 
    generate sets of bolts of this type and their radii elements (ref. [1]).
    These radii elements represent the bearing area between the bolt shaft and 
    the plate connected.

    :ivar diam: diameter of the bolt in meters
    :ivar mat: steel material
    '''
    def __init__(self,diam,mat):
        self.diam=diam
        self.R=round(self.diam/2,5)
        self.mat=mat
        self.sectMat=None
        
    def getBearStiff(self,tPlate):
        '''Return the bearing stiffness (k_be) due to the deformability of 
        the plate in the region near the contact area between bolt shaft and 
        plate connected (according to reference [1], page 245

        :param tPlate: plate thickness in meters.
        '''
        D_M16=16e-3 #16 mm
        k_be=22.5*tPlate/D_M16*self.diam*self.mat.fu
        return k_be

    def createBoltSectMat(self,prep):
        '''Creates the section-material to model bolt
        '''
        gsect=sectpr.CircularSection('gsect',self.diam)
        mName=str(uuid.uuid1())
        self.sectMat=tm.BeamMaterialData(mName+'_mat',gsect,self.mat)
        self.sectMat.setupElasticShear3DSection(prep)

    def createBolt(self,pntLst,setName):
        '''Creates the bolt elements and appends them to a set which 
        name is given as parameter.

        :param prep: preprocessor
        :param pntLst: list of ordered points that define the bolt 
        :param setName: name of the set to which append the bolt elements
                        (set must be created previously)

        '''
        prep=pntLst[0].getPreprocessor
        nodes=prep.getNodeHandler
        elements=prep.getElementHandler
        # if not prep.getSets.exists(setName):
        #     retSet=prep.getSets.defSet(setName)
        # else:
        #     retSet=prep.getSets.getSet(setName)
        retSet=prep.getSets.getSet(setName)
        if not self.sectMat:
            self.createBoltSectMat(prep)
#        nodLst=[nodes.newNodeXYZ(p.getPos.x,p.getPos.y,p.getPos.z) for p in pntLst]
        for p in pntLst: p.genMesh(xc.meshDir.I)
        nodLst=[p.getNode() for p in pntLst]
        linname= str(uuid.uuid1())
        lin=prep.getTransfCooHandler.newLinearCrdTransf3d(linname)
        lin.xzVector=predefined_spaces.getSuitableXZVector(nodLst[0],nodLst[1])
        elements.defaultTransformation= lin.name
        elements.defaultMaterial=self.sectMat.name
        for i in range(len(nodLst)-1):
            e=elements.newElement("ElasticBeam3d",xc.ID([nodLst[i].tag,nodLst[i+1].tag]))
            retSet.elements.append(e)
        retSet.fillDownwards()
#        return retSet

    def getRadiusSectMat(self,prep,tPlate):
        '''Return the material-section to generate the radii elements.
        A square section is generated so that its stiffness is 
        equivalent to 0.25 times the bearing stiffness (k_be) (page 466 ref. [1])

        :param tPlate: thickness of the plate.
        '''
        k_be=self.getBearStiff(tPlate)
        A=0.25*k_be/self.mat.E
        l=math.sqrt(A)
        gsectR=sectpr.RectangularSection('gsectR',l,l)
        mRname=str(uuid.uuid1())
        radMat=tm.BeamMaterialData(mRname+'_Rmat',gsectR,self.mat)
        radMat.setupElasticShear3DSection(prep)
        return radMat

    def getMatsRelease(self,prep,tPlate):
        '''Return the six materials of the zero-length elements that 
        define the releases in the radii bearing elements.
        The in-plane stiffness is set as 0.25*k_be, the out-of-plane
        stiffness is set as 25*k_be. The bending stiffness is near null
        '''
        KX=0.25*self.getBearStiff(tPlate)
        KY=KX/10
        KZ=10*KX
        KrotX=KX*10
        KrotY=KX*10
        KrotZ=KX/10
        matKX=tm.defElasticMaterial(prep,'matKX',KX)
        matKY=tm.defElasticMaterial(prep,'matKY',KY)
        matKZ=tm.defElasticMaterial(prep,'matKZ',KZ)
        matKrotX=tm.defElasticMaterial(prep,'matKrotX',KrotX)
        matKrotY=tm.defElasticMaterial(prep,'matKropY',KrotY)
        matKrotZ=tm.defElasticMaterial(prep,'matKrotZ',KrotZ)
        return [matKX,matKY,matKZ,matKrotX,matKrotY,matKrotZ]
         
    def generateRadii(self,tPlate,pntBolt,pntHoleLst,setRadii=None):
        '''Return the eight bearing elements (radii) to model the bolt hole 

        :param tPlate: plate thickness
        :param pntBolt: point of the bolt
        :param pntHoleLst: list of points of the hole
        :param radiiSet: set where to append the radii 
                         (defaults to None, radii are created but not
                          appended to any set)
        '''
        prep=pntBolt.getPreprocessor
        nodes=prep.getNodeHandler
        elements=prep.getElementHandler
        pb=pntBolt.getPos
#        boltNode=boltSet.getNodes.getNearestNode(pb)
        boltNode=pntBolt.getNode()
#        holeNodes=[plateSet.getNodes.getNearestNode(p.getPos) for p in pntHoleLst]
        holeNodes=[p.getNode() for p in pntHoleLst]
        radMat=self.getRadiusSectMat(prep,tPlate)
        releaseMats=self.getMatsRelease(prep,tPlate)
        releaseMatsNames=[mat.name for mat in releaseMats]
        # first bearing element only end release
        hnod=holeNodes[0]  #hole node
        ph=hnod.getInitialPos3d
        vx=xc.Vector([ph.x-pb.x,ph.y-pb.y,ph.z-pb.z])
        vx=vx.Normalized()
        phaux=holeNodes[1].getInitialPos3d
        vaux=xc.Vector([phaux.x-pb.x,phaux.y-pb.y,phaux.z-pb.z])
        vz=cross(vx,vaux)
        vz=vz.Normalized()
        vy=cross(vx,vz)
        linname= str(uuid.uuid1())
        lin=prep.getTransfCooHandler.newLinearCrdTransf3d(linname)
        lin.xzVector=vz
        elements.defaultTransformation= lin.name
        elements.defaultMaterial=radMat.name
        n2=nodes.newNodeXYZ(ph.x,ph.y,ph.z)
        e=elements.newElement("ElasticBeam3d",xc.ID([boltNode.tag,hnod.tag]))
        predefined_spaces.setBearingBetweenNodes(prep,n2.tag,hnod.tag,releaseMatsNames,[vx,vy])
        if setRadii:  setRadii.elements.append(e)
        # seven radii released at both extremities
        for i in range(1,8):
            hnod=holeNodes[i]
            ph=hnod.getInitialPos3d
            vx=xc.Vector([ph.x-pb.x,ph.y-pb.y,ph.z-pb.z])
            vx=vx.Normalized()
            vy=cross(vx,vz)
            n1=nodes.newNodeXYZ(pb.x,pb.y,pb.z)
            n2=nodes.newNodeXYZ(ph.x,ph.y,ph.z)
            predefined_spaces.setBearingBetweenNodes(prep,boltNode.tag,n1.tag,releaseMatsNames,[vx,vy])
            elements.defaultTransformation= lin.name
            elements.defaultMaterial=radMat.name
            e=elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))
            predefined_spaces.setBearingBetweenNodes(prep,n2.tag,hnod.tag,releaseMatsNames,[vx,vy])
            if setRadii:  setRadii.elements.append(e)

    
#Welding classes

class WeldTyp(object):
    '''Define the basic parameters of a weld, that will be used to
    generate fillet or penetration seam welds, by using classes 
    FilletWeld, MultiFilletWeld or PenetrationWeld.

    :param weldSz: weld size (leg dimension or actual throat) (t)
    :param weldMetal: weld metal
    :param weldThroat: weld throat thickness (a) (defaults to None, in
                       which case it's calculated for 90º)
    '''
    def __init__(self,weldSz,weldMetal,weldThroat=None):
        self.weldSz=weldSz
        self.weldMetal=weldMetal
        if not weldThroat:
            self.weldThroat=self.weldSz/math.sqrt(2)
        else:
            self.weldThroat=weldThroat
    
    
class BaseWeld(object):
    '''Generation of a straight weld

    :ivar weldTyp: instance of class WeldTyp that defines the generic
                    parameters:

                    - weldSz: weld size (leg dimension or actual throat) (t)
                    - weldMetal: weld metal
                    - weldThroat: weld throat thickness (a)

    :ivar setWS1: set with the elements of the welding surface 1
    :ivar setWS2: set with the elements of the welding surface 2
    :ivar setName: name of the set of weld elements
    :ivar descr: description (defaults to '')
    :ivar weldExtrPoints: [P1, P2] XC-points or Pos3d at the extremities of the 
                           weld bead in the intersection of the two welding surfaces.
                            If not given, the geometric definition will be given by 
                           a line using attribute weldLine. (defaults to [])
    :ivar weldLine: XC-line or Segment3d along the weld bead (defaults to None)
    '''
    
    def __init__(self,weldTyp,setWS1,setWS2,setName=None,descr='',weldExtrPoints=[],weldLine=None):
        self.weldSz=weldTyp.weldSz
        self.weldMetal=weldTyp.weldMetal
        self.weldThroat=weldTyp.weldThroat
        self.setWS1=setWS1
        self.setWS2=setWS2
        self.setName=setName
        self.descr=descr
        self.weldExtrPoints=weldExtrPoints
        self.weldLine=weldLine
        self.weldSet=None
        
    def generateWeld(self,weldElSz,distWeldEl_WS1,nDiv=None,WS1sign=1,WS2sign=1):
        '''Generates nDiv+1 points of welding along the weld seam. At each point 
        creates two rigid offset elements and a weld element according to page 
        455 of reference [1].
        The welding elements are included in a set for further verification.

        :param weldElSz: size of the elements «weld» (=weldSz/2 for 
                         fillet welds,  =weldSz for penetration welds)
        :param distWeldEl_WS1: distance between the weld element and 
                        the middle surface of WS1
                        (=tWS1/2+weldSz/2 for fillet welds,
                         =tWS1/4 for penetration welds)
        :param fictE: fictitious Young's modulus for calculating tension stiffness
                      of weld elements (pages 457 and 460 of reference [1])
                      (=546.fu for fillet welds, =E for penetration welds)
        :param fictG: fictitious shear modulus for calculating shear stiffness of
                      weld elements (pages 457 and 460 of reference [1]).
                      (=546.fu for fillet welds, =G for penetration welds)

        :param nDiv: number of divisions in the weld bead to insert elements.
                     If None, elements are placed at a distance of weldSz
                     (defaults to None)
        :param WS1sign: face of welding surface 1 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        :param WS2sign: face of welding surface 2 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        '''
        prep= self.setWS1.getPreprocessor
        pnts=prep.getMultiBlockTopology.getPoints
        nodes= prep.getNodeHandler
        elements= prep.getElementHandler
        if len(self.weldExtrPoints)>1:
            if self.weldExtrPoints[0].type()=='XC::Pnt': 
                self.weldP1=self.weldExtrPoints[0].getPos
            else:
                self.weldP1=self.weldExtrPoints[0]
            if self.weldExtrPoints[1].type()=='XC::Pnt': 
                self.weldP2=self.weldExtrPoints[1].getPos
            else:
                self.weldP2=self.weldExtrPoints[1]
        elif self.weldLine:
            if self.weldLine.type()=='XC::Line':
                self.weldP1=pnts.get(self.weldLine.getKPoints()[0]).getPos
                self.weldP2=pnts.get(self.weldLine.getKPoints()[1]).getPos
            else:
                self.weldP1=self.weldLine.getFromPoint()
                self.weldP2=self.weldLine.getToPoint()
        else:
            lmsg.error('Missing geometric definition of weld '+ self.descr + ' two points or one line is needed')
        self.length=self.weldP1.dist(self.weldP2)
        if not nDiv: nDiv=int(round(self.length/(self.weldSz)))
        distWeldEl=self.length/nDiv
        if not self.setName: self.setName=str(uuid.uuid1())
        if prep.getSets.exists(self.setName):
            lmsg.warning('Set ',self.setName, ' already defined, check if weld elements should be in a new set')
        self.weldSet=prep.getSets.defSet(self.setName)

        # surfaces thickness
        self.tWS1=self.setWS1.getElements[0].getPhysicalProperties.getVectorMaterials[0].h   # thickness of surface 1is taken from only first element (constant thickness is supossed)
        self.tWS2=self.setWS2.getElements[0].getPhysicalProperties.getVectorMaterials[0].h   # thickness of surface 1is taken from only first element (constant thickness is supossed)
        self.minSz=ASTM_materials.getFilletWeldMinimumLegSheets(self.tWS1,self.tWS2)
        self.maxSz=ASTM_materials.getFilletWeldMaximumLegSheets(self.tWS1,self.tWS2)
        #elements on surfaces
        e1= self.setWS1.getNearestElement(self.weldP1)
        e2= self.setWS2.getNearestElement(self.weldP1)
        #vectors perpendicular to welding surfaces
        WS1sign=WS1sign/abs(WS1sign)
        WS2sign=WS2sign/abs(WS2sign)
        vps1=WS1sign*e1.getKVector3d(True)
        vps2=WS2sign*e2.getKVector3d(True)
        #extremity points and line in middle surface 1
        p1lWS1=self.weldP1+vps2*(self.tWS2/2.+self.weldSz/2.)
        p2lWS1=self.weldP2+vps2*(self.tWS2/2.+self.weldSz/2.)
        lWS1=geom.Segment3d(p1lWS1,p2lWS1)
        nod_lWS1=[(nodes.newNodeXYZ(pos.x,pos.y,pos.z)) for pos in lWS1.Divide(nDiv)]
        #extremity points and line in centroid of weld bead
        p1lW=p1lWS1+vps1*(distWeldEl_WS1)
        p2lW=p2lWS1+vps1*(distWeldEl_WS1)
        lW=geom.Segment3d(p1lW,p2lW)
        nod_lW=[(nodes.newNodeXYZ(pos.x,pos.y,pos.z)) for pos in lW.Divide(nDiv)]
        #extremity points and line in middle surface 2
        p1lWS2=self.weldP1+vps1*(distWeldEl_WS1)
        p2lWS2=self.weldP2+vps1*(distWeldEl_WS1)
        lWS2=geom.Segment3d(p1lWS2,p2lWS2)
        nod_lWS2=[(nodes.newNodeXYZ(pos.x,pos.y,pos.z)) for pos in lWS2.Divide(nDiv)]
        #extremity points and line on face of welding surface 2
        p1lFWS2=p1lWS2+vps2*(weldElSz)
        p2lFWS2=p2lWS2+vps2*(weldElSz)
        lFWS2=geom.Segment3d(p1lFWS2,p2lFWS2)
        nod_lFWS2=[(nodes.newNodeXYZ(pos.x,pos.y,pos.z)) for pos in lFWS2.Divide(nDiv)]
        #Cross section's properties
        L=lWS1.getLength()
        # fictitious moduli so that A.E=546.A.f_uw and A.G=546.A.f_u according to
        # sfiffness given in page 460 of reference [1]
        fictE=546*self.weldMetal.fu
        fictG=546*self.weldMetal.fu
        #  welding elements section
        A_w=L/nDiv*self.weldSz
        I_w=1/10*1/12*A_w**2
        sectWeldPr= xc.CrossSectionProperties3d()
        sectWeldPr.A=A_w; sectWeldPr.E=fictE ; sectWeldPr.G=fictG
        sectWeldPr.Iz=I_w ;  sectWeldPr.Iy=I_w ; sectWeldPr.J=I_w
        sectWeld=tm.defElasticSectionFromMechProp3d(prep, "sectWeld",sectWeldPr)
        # welding elements section in the extremities of the weld seam 
        sectWeldExtrPr=xc.CrossSectionProperties3d()
        sectWeldExtrPr.A=A_w/2.; sectWeldExtrPr.E=fictE ; sectWeldExtrPr.G=fictG
        sectWeldExtrPr.Iz=I_w/2. ;  sectWeldExtrPr.Iy=I_w/2. ; sectWeldExtrPr.J=I_w/2.
        sectWeldExtr=tm.defElasticSectionFromMechProp3d(prep, "sectWeldExtr",sectWeldExtrPr)
        #rigid elements section
        A_r=20*A_w
        I_r=1/12*A_r**2
        sectRigPr= xc.CrossSectionProperties3d()
        sectRigPr.A=A_r; sectRigPr.E=fictE ; sectRigPr.G=fictG
        sectRigPr.Iz=I_r ;  sectRigPr.Iy=I_r ; sectRigPr.J=I_r
        sectRig=tm.defElasticSectionFromMechProp3d(prep, "sectRig",sectRigPr)
         #rigid elements section  in the extremities of the weld seam 
        sectRigExtrPr=xc.CrossSectionProperties3d()
        sectRigExtrPr.A=A_r/2.; sectRigExtrPr.E=fictE ; sectRigExtrPr.G=fictG
        sectRigExtrPr.Iz=I_r/2. ;  sectRigExtrPr.Iy=I_r/2. ; sectRigExtrPr.J=I_r/2.
        sectRigExtr=tm.defElasticSectionFromMechProp3d(prep, "sectRigExtr",sectRigExtrPr)
        #linear transformation
        linname= str(uuid.uuid1())
        lin=prep.getTransfCooHandler.newLinearCrdTransf3d(linname)
        lin.xzVector= xc.Vector(vps1.cross(vps2))
        # set of rigid elements to glue to surface 1
        elements.defaultTransformation= lin.name
        elements.defaultMaterial=sectRigExtr.name
        e=elements.newElement("ElasticBeam3d",xc.ID([nod_lWS1[0].tag,nod_lW[0].tag]))
        e=elements.newElement("ElasticBeam3d",xc.ID([nod_lWS1[-1].tag,nod_lW[-1].tag]))
        elements.defaultMaterial=sectRig.name
        for i in range(1,nDiv):
            e=elements.newElement("ElasticBeam3d",xc.ID([nod_lWS1[i].tag,nod_lW[i].tag]))
        # set of rigid elements to glue to surface 2
        elements.defaultTransformation= lin.name
        elements.defaultMaterial=sectRigExtr.name
        e=elements.newElement("ElasticBeam3d",xc.ID([nod_lWS2[0].tag,nod_lFWS2[0].tag]))
        e=elements.newElement("ElasticBeam3d",xc.ID([nod_lWS2[-1].tag,nod_lFWS2[-1].tag]))
        elements.defaultMaterial=sectRig.name
        for i in range(1,nDiv):
            e=elements.newElement("ElasticBeam3d",xc.ID([nod_lWS2[i].tag,nod_lFWS2[i].tag]))
        # set of welding elemets
        #  extremity elements
        elements.defaultTransformation= lin.name
        elements.defaultMaterial=sectWeldExtr.name
        e=elements.newElement("ElasticBeam3d",xc.ID([nod_lFWS2[0].tag,nod_lW[0].tag]))
        e.setProp('ass_wL',distWeldEl/2.)
        self.weldSet.elements.append(e)
        e=elements.newElement("ElasticBeam3d",xc.ID([nod_lFWS2[-1].tag,nod_lW[-1].tag]))
        e.setProp('ass_wL',distWeldEl/2.)
        self.weldSet.elements.append(e)
        elements.defaultMaterial=sectWeld.name
        for i in range(1,nDiv):
            e=elements.newElement("ElasticBeam3d",xc.ID([nod_lFWS2[i].tag,nod_lW[i].tag]))
            e.setProp('ass_wL',distWeldEl)
            self.weldSet.elements.append(e)
        # glue rigid elements to plates
        gluedDOFs=[0,1,2,3,4,5]
        for n in nod_lWS1:
            nodePos= n.getInitialPos3d
            e=self.setWS1.getNearestElement(nodePos)
            dist= e.getDist(nodePos, True)
            glue=prep.getBoundaryCondHandler.newGlueNodeToElement(n,e,xc.ID(gluedDOFs))
        for n in nod_lWS2:
            nodePos= n.getInitialPos3d
            e=self.setWS2.getNearestElement(nodePos)
            dist= e.getDist(nodePos, True)
            glue=prep.getBoundaryCondHandler.newGlueNodeToElement(n,e,xc.ID(gluedDOFs))
        self.weldSet.fillDownwards()
        
    def getTotalIntForc(self):
        '''Return the total forces in directions parallel and perpendiculars to
        the weld seam for the calculated load case.
        '''
#        Fpar=0 ; Fperp1=0 ; Fperp2=0
        Fpar=0 ; Fperp=0
        for e in self.weldSet.elements:
            Fpar+=e.getVz()
            Fperp1=e.getN()
            Fperp2=e.getVy()
            Fperp+=math.sqrt(Fperp1**2+Fperp2**2)
#        Fperp=math.sqrt(Fperp1**2+Fperp2**2)
        return (Fpar,Fperp)


class FilletWeld(BaseWeld):
    '''Generation and verifications of a straight weld
    :ivar weldTyp: instance of class WeldTyp that defines the generic
                    parameters:

                    - weldSz: weld size (leg dimension or actual throat) (t)
                    - weldMetal: weld metal
                    - weldThroat: weld throat thickness (a)

    :ivar setWS1: set with the elements of the welding surface 1
    :ivar setWS2: set with the elements of the welding surface 2
    :ivar setName: name of the set of weld elements
    :ivar descr: description (defaults to '')
    :ivar weldExtrPoints: [P1, P2] XC-points or Pos3d at the extremities of the 
                           weld bead in the intersection of the two welding surfaces.
                            If not given, the geometric definition will be given by 
                           a line using attribute weldLine. (defaults to [])
    :ivar weldLine: XC-line or Segment3d along the weld bead (defaults to None)
    '''
    
    def __init__(self,weldTyp,setWS1,setWS2,setName=None,descr='',weldExtrPoints=[],weldLine=None):
        super(FilletWeld,self).__init__(weldTyp,setWS1,setWS2,setName,descr,weldExtrPoints,weldLine)
 
    def generateWeld(self,nDiv=None,WS1sign=1,WS2sign=1):
        '''Generates nDiv+1 points of welding along the weld seam. At each point 
        creates two rigid offset elements and a weld element according to page 
        455 of reference [1].
        The welding elements are included in a set for further verification.

        :param nDiv: number of divisions in the weld bead to insert elements.
                     If None, elements are placed at a distance of 2*weldSz
                     (defaults to None)

        :param WS1sign: face of welding surface 1 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        :param WS2sign: face of welding surface 2 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        '''
        prep=self.setWS1.getPreprocessor
        self.tWS1=self.setWS1.getElements[0].getPhysicalProperties.getVectorMaterials[0].h   # thickness of surface 1is taken from only first element (constant thickness is supossed)
        weldElSz=self.weldSz/2
        distWeldEl_WS1=self.tWS1/2+self.weldSz/2
        super(FilletWeld,self).generateWeld(weldElSz,distWeldEl_WS1,nDiv,WS1sign,WS2sign)
        
    def getWeldElemDesignStrength(self,weldEl,Fpar,Fperp):
        '''Return design strength of a weld element according
        to section J.2 of AISC (reference [2])
        '''
        if abs(Fpar) > 0:
            angleDir=math.atan(abs(Fperp/Fpar))
        else:
            angleDir=math.pi/2
        #directional strength increase
#        print('Fpar=', round(Fpar), ' Fperp=',round(Fperp), ' angleDir=',math.degrees(angleDir))
        dirSthIncr=1+0.5*(math.sin(angleDir))**1.5
        Fnw=0.6*self.weldMetal.fu*dirSthIncr
        Awe=weldEl.getProp('ass_wL')*self.weldThroat
        Rn_weldEl=Fnw*Awe
        return Rn_weldEl 
        
    def getCF_AISCverif(self,baseMetal,Phi=0.75):
        '''Return the capacity factor of the weld
        according to section J.2 of AISC (reference [2])

        :param baseMetal: base metal
        :param Phi: resistance factor (defaults to 0.75)
        '''
        sumCF=0
        for weldEl in self.weldSet.elements:
            Fpar=weldEl.getVz()
            Fperp1=weldEl.getN()
            Fperp2=weldEl.getVy()
            Fperp=math.sqrt(Fperp1**2+Fperp2**2)
            #Base metal design strength  (Sect. J2 AISC)
            A_BM=weldEl.getProp('ass_wL')*self.weldSz
            F_nBM=Phi*baseMetal.fu   #sect. J4-2 AISC)
            Rn_base=F_nBM*A_BM
            #Weld  design strengths  (Sect. J2 AISC)
            Rn_weldEl_V=self.getWeldElemDesignStrength(weldEl,Fpar,Fperp)
            #design shear internal forces
            V=math.sqrt(Fpar**2+Fperp**2)
            # Capacity factors
            Rn=min(Rn_base,Phi*Rn_weldEl_V)
            CFel=V/Rn
            sumCF+=CFel
#            print('elem:', weldEl.tag,' l=', round(weldEl.getProp('ass_wL'),4),' Rn_base=',round(Rn_base),' Rn_weldEl_V=',round(Rn_weldEl_V),' CF=',CFel)
        CF=sumCF/self.weldSet.elements.size
#        print('CF=',CF)
        return CF

class PenetrationWeld(BaseWeld):
    '''Generation and verifications of a straight weld
    :param weldTyp: instance of class WeldTyp that defines the generic
                    parameters:

                    - weldSz: weld size (leg dimension or actual throat) (t)
                    - weldMetal: weld metal
                    - weldThroat: weld throat thickness (a)

    :param setWS1: set with the elements of the welding surface 1
    :param setWS2: set with the elements of the welding surface 2
    :param setName: name of the set of weld elements
    :param descr: description (defaults to '')
    :param weldExtrPoints: [P1, P2] XC-points or Pos3d at the extremities of the 
                           weld bead in the intersection of the two welding surfaces.
                            If not given, the geometric definition will be given by 
                           a line using attribute weldLine. (defaults to [])
    :param weldLine: XC-line or Segment3d along the weld bead (defaults to None)
    '''
    
    def __init__(self,weldTyp,setWS1,setWS2,setName=None,descr='',weldExtrPoints=[],weldLine=None):
        super(PenetrationWeld,self).__init__(weldTyp,setWS1,setWS2,setName,descr,weldExtrPoints,weldLine)
 
    def generateWeld(self,nDiv=None,WS1sign=1,WS2sign=1):
        '''Generates nDiv+1 points of welding along the weld seam. At each point 
        creates two rigid offset elements and a weld element according to page 
        455 of reference [1].
        The welding elements are included in a set for further verification.

        :param nDiv: number of divisions in the weld bead to insert elements.
                     If None, elements are placed at a distance of 2*weldSz
                     (defaults to None)

        :param WS1sign: face of welding surface 1 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        :param WS2sign: face of welding surface 2 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        '''
        prep=self.setWS1.getPreprocessor
        self.tWS1=self.setWS1.getElements[0].getPhysicalProperties.getVectorMaterials[0].h   # thickness of surface 1is taken from only first element (constant thickness is supossed)
        weldElSz=self.weldSz
        distWeldEl_WS1=self.tWS1/4
        super(PenetrationWeld,self).generateWeld(weldElSz,distWeldEl_WS1,nDiv,WS1sign,WS2sign)
        

class MultiFilletWeld(object):
    ''''Generation and verifications of multiple welds

    :ivar weldTyp: instance of class WeldTyp that defines the generic
                    parameters:

                    - weldSz: weld size (leg dimension or actual throat) (t)
                    - weldMetal: weld metal
                    - weldThroat: weld throat thickness (a)
    :ivar setWS1: set with the elements of the welding surface 1
    :ivar setWS2: set with the elements of the welding surface 2
    :ivar setName: name of the set of weld elements
    :ivar lstLines: list of XC-lines or Segment3d (each line corresponding to one weld)
    :ivar genDescr: generic description (defaults to '')
    :ivar specDescr: list of specific description (each description
                      applies for the correspondig weld defined 
                      in lstLines), (defaults to [])
    '''
    def __init__(self,weldTyp,setWS1,setWS2,lstLines,setName=None,genDescr='',specDescr=[]):
        self.weldTyp=weldTyp
        self.setWS1=setWS1
        self.setWS2=setWS2
        self.setName=setName
        self.lstLines=lstLines
        self.genDescr=genDescr
        self.specDescr=specDescr
        self.lstWelds=list()

    def generateWeld(self,nDiv=None,WS1sign=1,WS2sign=1,bothSidesOfWS1=False,bothSidesOfWS2=False):
        ''''Generates nDiv+1 points of welding along each line. At each point 
        creates two rigid offset elements and a weld element according to page 
        455 of reference [1].
        The welding elements are included in a set for further verification.

        :param nDiv: number of divisions in the weld bead to insert elements.
                     If None, elements are placed at a distance of 2*weldSz
                     (defaults to None)
        :param WS1sign: face of welding surface 1 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        :param WS2sign: face of welding surface 2 to place the weld 
                        1 for positive face, -1 for negative face (defaults to 1)
        :param bothSidesOfWS1: if True, weld on both sides of surface SW1 
                               (WS1sign ignored), (defaults to False)
        :param bothSidesOfWS2: if True, weld on both sides of surface SW2 
                               (WS2sign ignored) (defaults to False)
        '''
        prep=self.setWS1.getPreprocessor
        pnts=prep.getMultiBlockTopology.getPoints
        if not self.setName: self.setName=str(uuid.uuid1())
        if len(self.specDescr) < len(self.lstLines):
            self.specDescr=['weld '+str(i) for i in range(len(self.lstLines))]
        for i in range(len(self.lstLines)):
            l=self.lstLines[i]
            if bothSidesOfWS1 or bothSidesOfWS2:
                self.lstWelds.append(FilletWeld(self.weldTyp,self.setWS1,self.setWS2,self.setName+str(i)+'_s1',descr=self.genDescr+', '+self.specDescr[i]+' side 1',weldLine=l))
                self.lstWelds.append(FilletWeld(self.weldTyp,self.setWS1,self.setWS2,self.setName+str(i)+'_s2',descr=self.genDescr+', '+self.specDescr[i]+' side 2',weldLine=l))
            else:
                 self.lstWelds.append(FilletWeld(self.weldTyp,self.setWS1,self.setWS2,self.setName+str(i),descr=self.genDescr+', '+self.specDescr[i],weldLine=l))
        for w in self.lstWelds:
            w.weldSet=None
        if bothSidesOfWS1:
            for i in range(0,len(self.lstWelds),2):
                self.lstWelds[i].generateWeld(nDiv,1,WS2sign)
                self.lstWelds[i+1].generateWeld(nDiv,-1,WS2sign)
        elif bothSidesOfWS2:
            for i in range(0,len(self.lstWelds),2):
                self.lstWelds[i].generateWeld(nDiv,WS1sign,1)
                self.lstWelds[i+1].generateWeld(nDiv,WS1sign,-1)
        else:
            for i in range(len(self.lstWelds)):
                self.lstWelds[i].generateWeld(nDiv,WS1sign,WS2sign)

    def getLstTotalIntForc(self):
        '''Return the list of total forces in directions parallel and 
        perpendiculars to the weld seam for the calculated load case.
        '''
        lstIF=list()
        for w in self.lstWelds:
            lstIF.append(w.getTotalIntForc())
        return lstIF
    
    def getLstCF_AISCverif(self,baseMetal,Phi=0.75):
        '''Return the list of capacity factors of the welds
        according to section J.2 of AISC (reference [2])

        :param Phi: resistance factor (defaults to 0.75)
        '''
        lstCF=list()
        for w in self.lstWelds:
            lstCF.append(w.getCF_AISCverif(baseMetal,Phi))
        return lstCF

    def getMaxCF_AISCverif(self,baseMetal,Phi=0.75):
        '''Return the maximum capacity factors of the welds
        according to section J.2 of AISC (reference [2])

        :param Phi: resistance factor (defaults to 0.75)
        '''
        lstCF=self.getLstCF_AISCverif(baseMetal,Phi)
        return max(lstCF)
    

# Functions to generate welds and bolts from XC connection models.
def gen_bolts_xc_conn_model(modelSpace,matchedBolts):
    '''Generate the bolts from the data elaborated in an XC-connection-model.
    Return the dictionary 'boltSets2Check' with the created sets of bolts
    to check.
    '''
    # Bolt sets dictionary
    boltSets2Check=dict()
    if(len(matchedBolts)>0):
        preprocessor= matchedBolts[0].line.getPreprocessor
        for blt in matchedBolts:
            grpName= blt.getBoltGroupName() # Name of the bolt group.
            if grpName not in boltSets2Check.keys():
                D=blt.getDiameter()
                mat=blt.getMaterial()
                boltSets2Check[grpName]={'boltSet': None, 'boltCheckTyp': None, 'boltSeed': None}
                boltSets2Check[grpName]['boltSet']=preprocessor.getSets.defSet(grpName)
    #            boltSets2Check[grpName]['boltSet'].description=blt.getSetDescription()
                boltSets2Check[grpName]['boltSet'].description=grpName.replace('_',' ')
                boltSets2Check[grpName]['boltCheckTyp']=ASTM_materials.BoltFastener(diameter=D, steelType=mat)
                boltSets2Check[grpName]['boltSeed']=Bolt(diam=D,mat=mat)
            bltSeed=boltSets2Check[grpName]['boltSeed']
            pntA=blt.endA.kPoint
            pntB=blt.endB.kPoint
            bltSeed.createBolt([pntA,pntB],grpName)  #bolt generation and addition to set
            #raddi end A
            if blt.endA.hole:
                pntHoleLst=blt.endA.hole.getVertices
                plateTh=blt.endA.getPlateThickness()
                bltSeed.generateRadii(plateTh,pntA,pntHoleLst)
            else:
                n=pntA.getNode()
                modelSpace.fixNode000_000(n.tag)
            #raddi end B
            if blt.endB.hole:
                pntHoleLst=blt.endB.hole.getVertices
                plateTh=blt.endB.getPlateThickness()
                bltSeed.generateRadii(plateTh,pntB,pntHoleLst)
            else:
                n=pntB.getNode()
                modelSpace.fixNode000_000(n.tag)
    else:
        lmsg.warning('matched bolts is empty.')
    return boltSets2Check
    
def gen_welds_xc_conn_model(welds , weldMetal, weldSzFactor=None, avlbWeldSz=None):
    '''Generate the welds from the data elaborated in an XC-connection-model.
    Return the dictionary 'welds2Check' with the created sets of welds
    to check.

    :param welds: list of WeldMetaData objects representing the welds in the connection.
    :param weldMetal: type of weld metal.
    :param weldSizeFactor: weld size factor (0 => minimum size, 1 => maximum size.)
    :param avlbWeldSz: available weld sizes.
    '''
    if avlbWeldSz: avlbWeldTyp=[WeldTyp(t,weldMetal) for t in avlbWeldSz]  #available weld types
    welds2Check=list()
    for w in welds:
        l= w.line
        descr= w.description
        # check if the orientation of the surface is the same as its elements
        # if yes, correction factor for its orientation is equal to 1,
        # otherwise this correction factor = -1 (the orientation is changed)
        vWS1= w.memberToWeld.getKVector
        if(len(w.memberToWeld.elements)<1):
            lmsg.error('attached member has no elements.')
            print(l.name)
        vWS1el= w.memberToWeld.elements[0].getKVector3d(True)
        WS1_corrfact= 1 if ((vWS1-vWS1el).getModulus()<1e-2) else -1
        tWS1= w.memberToWeld.elements[0].getPhysicalProperties.getVectorMaterials[0].h  #thickness of member to weld
        for fk in w.faceWelds.keys():
            f= w.faceWelds[fk]
            WS1= f.memberToWeld
            WS2= f.face
            if avlbWeldSz:
                tWS1=WS1.elements[0].getPhysicalProperties.getVectorMaterials[0].h
                tWS2=WS2.elements[0].getPhysicalProperties.getVectorMaterials[0].h
                minSz=ASTM_materials.getFilletWeldMinimumLegSheets(tWS1,tWS2)
                maxSz=ASTM_materials.getFilletWeldMaximumLegSheets(tWS1,tWS2)
                weightedSz=minSz+weldSzFactor*(maxSz-minSz)
                weldTyp=avlbWeldTyp[dsu.get_index_closest_inlist(avlbWeldSz,weightedSz)]
            else:
                weldTyp=WeldTyp(w.legSize,weldMetal)
            if f.side:
                # check if the orientation of the surface is the same as its elements
                vWS2= WS2.getKVector
                vWS2el=WS2.elements[0].getKVector3d(True)
                WS2_corrfact=1 if ((vWS2-vWS2el).getModulus()<1e-2) else -1
                WS2sign= float(f.side+str(1))*WS2_corrfact
                if str(f.orientation)=='+-' or str(f.orientation)=='-+':
                    welds2Check.append(MultiFilletWeld(
                        weldTyp,WS1,WS2,[l],genDescr=descr))
                    welds2Check[-1].generateWeld(WS2sign=WS2sign,bothSidesOfWS1=True)
                else:
                    #                    WS1sign= float(f.orientation+str(1))*WS1_corrfact #01/03/2021
                    WS1sign= float(str(f.orientation)+str(1))*WS1_corrfact
                    welds2Check.append(FilletWeld(
                        weldTyp,WS1,WS2,descr=descr,weldLine=l))
                    welds2Check[-1].generateWeld(WS1sign=WS1sign,WS2sign=WS2sign)
    return welds2Check

def gen_bolts_and_weld_elements(modelSpace, matchedBolts,  weldMetal,welds,weldSzFactor=None,avlbWeldSz=None):
    ''' Create the elements corresponding to bolds and welds.
    '''
    boltSets2Check= gen_bolts_xc_conn_model(modelSpace,matchedBolts)
    welds2Check= gen_welds_xc_conn_model(welds,weldMetal,weldSzFactor,avlbWeldSz)
    return boltSets2Check, welds2Check

            
# Miscellaneous functions                            
def change_weld_size(xcWelds,welds2change):
    '''Change the size of the welds in xcWelds that match 
    the properties defined in welds2change.

    :param xcWelds: list of welds generated by XC
    :param welds2change: dictionary of type 
          {'w1':{'name': 'baseplate', 'oldSize': 0.009, 't1': 0.025, 'newSize':0.010}}
    where 'name' is a string contained in the target weld description,
    'oldSize', 't1', 'newSize' are the size, thickness of the plate to weld 
    and desired size of the target weld which size we want to change.
    '''
    for wch in welds2change:
        name=welds2change[wch]['name'].lower()
        oldSize=welds2change[wch]['oldSize']
        t1=welds2change[wch]['t1']
        newSize=welds2change[wch]['newSize']
        for w in xcWelds:
            lmsg.info('weld size changed')
            wt1=w.memberToWeld.elements[0].getPhysicalProperties.getVectorMaterials[0].h
            if (name in w.getDescription().lower()) and (abs(oldSize-w.legSize)<1e-4) and (abs(t1-wt1)<1e-4):
                w.setLegSize(newSize)
                        
            
