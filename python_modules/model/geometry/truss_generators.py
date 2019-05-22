# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es" 

import xc_base
import geom
import xc
from miscUtils import LogMessages as lmsg
import uuid 

class TrussBase(object):
    '''Base class for truss generators.

    :ivar name: name of the truss (automatically assigned).
    :ivar rise: rise of the truss.
    :ivar moduleWidth:  width of each truss
    :ivar span:  truss span
    :ivar horizDirectionVector: direction of the chords.
    :ivar vertDirectionVector: direction of the vertical studs.
    '''
    def __init__(self,trussRise,trussModule, trussSpan, horizDirectionVector= geom.Vector3d(1.0,0.0,0.0), vertDirectionVector= geom.Vector3d(0,1.0,0.0)):
        '''Constructor.

            Args:
                :param trussRise: (float) rise of the truss.
                :param trussModule: (float) width of each truss.
                :param trussSpan: (float) span of each truss.
                :param horizDirectionVector: direction of the chords.
                :param vertDirectionVector: direction of the vertical studs.
        '''
        self.name= uuid.uuid4().hex
        self.rise= trussRise
        self.moduleWidth= trussModule
        self.span= trussSpan
        self.horizDirectionVector= horizDirectionVector
        self.vertDirectionVector= vertDirectionVector
        self.numberOfModules= round(self.span/self.moduleWidth,0)
        self.moduleWidth= self.span/self.numberOfModules
        
        self.lowerChordJointsPos= list()
        self.upperChordJointsPos= list()
        self.lowerChordPoints= list()
        self.upperChordPoints= list()
        self.lowerChordLines= list()
        self.upperChordLines= list()
        self.diagonalLines= list()
        self.posts= list()
        self.crdTransf= None
        self.lowerChordMaterial= None
        self.upperChordMaterial= None
        self.diagonalMaterial= None
        self.diagonalArea= None
        self.postsMaterial= None

    def createKeyPoints(self, feProblem, org= geom.Pos3d(0,0,0)):
        '''Create the key points for the Truss.'''
        self.computeJointPositions()
        self.preprocessor= feProblem.getPreprocessor
        kPoints= self.preprocessor.getMultiBlockTopology.getPoints # Point container.
        for j in self.lowerChordJointsPos:
            self.lowerChordPoints.append(kPoints.newPntFromPos3d(org+j))
        for j in self.upperChordJointsPos:
            self.upperChordPoints.append(kPoints.newPntFromPos3d(org+j))
            
    def createChordsGeometry(self):
        '''Creates the geometry of the truss chords.'''
        lines= self.preprocessor.getMultiBlockTopology.getLines # Line container.
        for p in list(zip(*(self.lowerChordPoints[start:] for start in range(0, 2)))):
            self.lowerChordLines.append(lines.newLine(p[0].tag, p[1].tag))
        for p in list(zip(*(self.upperChordPoints[start:] for start in range(0, 2)))):
            self.upperChordLines.append(lines.newLine(p[0].tag, p[1].tag))
        for l in self.lowerChordLines:
            l.nDiv= 2
        for l in self.upperChordLines:
            l.nDiv= 2

    def createGeometry(self, feProblem, org= geom.Pos3d(0,0,0)):
        '''Creates the geometry of the Truss.'''
        self.createKeyPoints(feProblem, org)
        self.createChordsGeometry()
        self.createDiagonalsGeometry()
        self.createPostsGeometry()

    def createSets(self):
        '''Defines the sets for the truss.'''
        sets= self.preprocessor.getSets
        self.trussSet= sets.defSet(self.name+'_set')
        self.lowerChordSet= sets.defSet(self.name+'_lowerChordSet')
        for l in self.lowerChordLines:
            self.lowerChordSet.getLines.append(l)
            self.trussSet.getLines.append(l)
        self.upperChordSet= sets.defSet(self.name+'_upperChordSet')
        for l in self.upperChordLines:
            self.upperChordSet.getLines.append(l)
            self.trussSet.getLines.append(l)
        self.diagonalSet= sets.defSet(self.name+'_diagonalSet')
        for l in self.diagonalLines:
            self.diagonalSet.getLines.append(l)
            self.trussSet.getLines.append(l)
        self.postsSet= sets.defSet(self.name+'_postsSet')
        for l in self.posts:
            self.postsSet.getLines.append(l)
            self.trussSet.getLines.append(l)
        self.fillDownwards()

    def getNormalVector(self):
        '''Get a vector normal to the truss plane.'''
        tmp= self.horizDirectionVector.cross(self.vertDirectionVector)
        return xc.Vector([tmp.x,tmp.y,tmp.z])

    def createCoordTransformation(self,coordTransfType= 'linear'):
        '''Creates a coordinate transformation for the truss elements.'''
        if(self.crdTransf):
            lmsg.warning("redefinition of coordinate transformation.\n")
        trfs= self.preprocessor.getTransfCooHandler
        coordTransfName= self.name+'_crdTransf'
        if coordTransfType.lower()=='linear':
            self.crdTransf= trfs.newLinearCrdTransf3d(coordTransfName)
        elif coordTransfType.lower()=='pdelta':
            self.crdTransf= trfs.newPDeltaCrdTransf3d(coordTransfName)
        elif coordTransfType.lower()=='corot':
            self.crdTransf= trfs.newCorotCrdTransf3d(coordTransfName)
        else:
            lmsg.warning("unknown coordinate transformation type '"+str(coordTransfType)+"'\n")
            self.crdTransf= trfs.newLinearCrdTransf3d(coordTransfName)
        self.crdTransf.xzVector= self.getNormalVector()
        return self.crdTransf

    def fillDownwards(self):
        '''Updates the sets of the truss.'''
        self.lowerChordSet.fillDownwards()
        self.upperChordSet.fillDownwards()
        self.diagonalSet.fillDownwards()
        self.postsSet.fillDownwards()
        self.trussSet.fillDownwards()
        
    def genMesh(self, feProblem):
        ''' Creates the elements.'''
        self.createGeometry(feProblem)
        self.createCoordTransformation()
        self.createSets()
        seedElemHandler= self.preprocessor.getElementHandler.seedElemHandler
        seedElemHandler.defaultTransformation= self.crdTransf.getName()  # Orientation of the element axis.
        # Lower chord
        seedElemHandler.defaultMaterial= self.lowerChordMaterial.name  # Material name.
        beam3d= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]));
        self.lowerChordSet.genMesh(xc.meshDir.I)  # Generate the elements.
        # Upper chord
        seedElemHandler.defaultMaterial= self.upperChordMaterial.name  # Material name.
        beam3d= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]));
        self.upperChordSet.genMesh(xc.meshDir.I)  # Generate the elements.
        # Diagonals
        seedElemHandler.defaultMaterial= self.diagonalMaterial.name  # Material name.
        seedElemHandler.dimElem= 3 #Bars defined ina a three-dimensional space.
        trussElem= seedElemHandler.newElement('Truss',xc.ID([0,0]));
        trussElem.area= self.diagonalArea
        self.diagonalSet.genMesh(xc.meshDir.I)  # Generate the elements.
        # End posts
        seedElemHandler.defaultMaterial= self.postsMaterial.name  # Material name.
        beam3d= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]));
        self.postsSet.genMesh(xc.meshDir.I)  # Generate the elements.
        self.fillDownwards()

# Warren truss web style ascii art:
#     +---+-------+-------+---+  Upper chord.
#     |  / \     / \     / \  |
#     | /   \   /   \   /   \ |
#     |/     \ /     \ /     \|
#     +-------+-------+-------+  Lower chord.

class WarrenTruss(TrussBase):
    '''Warren truss.

    :ivar lowerChordJointsPos: (list) truss lower chord joint positions.
    :ivar upperChordJointsPos: (list) truss upper chord joint positions
    :ivar lowerChordPoints: (list) truss lower chord key points.
    :ivar upperChordPoints: (list) truss upper chord key points.
    :ivar lowerChordLines: (list) truss lower chord lines.
    :ivar upperChordLines: (list) truss upper chord lines.
    :ivar diagonalLines: (list) truss diagonals.
    :ivar posts: (list) vertical lines at the ends of the truss.
    :ivar lowerChordMaterial: material for the lower chord elements.
    :ivar upperChordMaterial: material for the upper chord elements.
    :ivar diagonalLines: material for the diagonals.
    :ivar diagonalArea: area of the diagonals cross-section.
    :ivar postsMaterial: material for the posts.
    '''
    def __init__(self,trussRise,trussModule, trussSpan, horizDirectionVector= geom.Vector3d(1.0,0.0,0.0), vertDirectionVector= geom.Vector3d(0,0.0,1.0)):
        '''Constructor.

            Args:
                :param trussRise: (float) rise of the truss.
                :param trussModule: (float) width of each truss.
                :param trussSpan: (float) span of each truss.
                :param horizDirectionVector: direction of the chords.
                :param vertDirectionVector: direction of the vertical studs.
        '''
        super(WarrenTruss,self).__init__(trussRise,trussModule, trussSpan, horizDirectionVector, vertDirectionVector)

    def computeJointPositions(self):
        '''Compute the positions of the truss joints.'''
        lowerChordX= list()
        for i in range(0,int(self.numberOfModules+1)):
            lowerChordX.append(i)
        upperChordX= [0.0]
        for i in range(0,int(self.numberOfModules)):
            upperChordX.append(i+0.5)
        upperChordX.append(self.numberOfModules)

        for x in lowerChordX:
            self.lowerChordJointsPos.append(self.moduleWidth*x*self.horizDirectionVector)
        for x in upperChordX:
            self.upperChordJointsPos.append(self.moduleWidth*x*self.horizDirectionVector+self.rise*self.vertDirectionVector)
    
    def createDiagonalsGeometry(self):
        '''Creates the geometry of the truss diagonals.'''
        lines= self.preprocessor.getMultiBlockTopology.getLines # Line container.
        for i in range(0,len(self.lowerChordPoints)-1):
            self.diagonalLines.append(lines.newLine(self.lowerChordPoints[i].tag,self.upperChordPoints[i+1].tag))
            self.diagonalLines.append(lines.newLine(self.lowerChordPoints[i+1].tag,self.upperChordPoints[i+1].tag))
        for l in self.diagonalLines:
            l.nDiv= 1

    def createPostsGeometry(self):
        '''Creates the geometry of the truss posts.'''
        lines= self.preprocessor.getMultiBlockTopology.getLines # Line container.
        self.posts.append(lines.newLine(self.lowerChordPoints[0].tag,self.upperChordPoints[0].tag))
        self.posts.append(lines.newLine(self.lowerChordPoints[-1].tag,self.upperChordPoints[-1].tag))
        for l in self.posts:
            l.nDiv= 2
            

        



# Fan truss web style ascii art:
#     +---+-------+-------+---+  Upper chord.
#     |  / \  |  / \  |  / \  |
#     | /   \ | /   \ | /   \ |
#     |/     \|/     \|/     \|
#     +-------+-------+-------+  Lower chord.

class FanTruss(WarrenTruss):
    '''Fan truss.

    '''
    def __init__(self, trussRise, trussModule, trussSpan, horizDirectionVector= geom.Vector3d(1.0,0.0,0.0), vertDirectionVector= geom.Vector3d(0,0.0,1.0)):
        '''Constructor.

            Args:
                :param trussRise: (float) rise of the truss.
                :param trussModule: (float) width of each truss.
                :param trussSpan: (float) span of each truss.
                :param horizDirectionVector: direction of the chords.
                :param vertDirectionVector: direction of the vertical studs.
        '''
        super(FanTruss,self).__init__(trussRise,trussModule, trussSpan, horizDirectionVector, vertDirectionVector)

    def computeJointPositions(self):
        '''Compute the positions of the truss joints.'''
        lowerChordX= list()
        for i in range(0,int(self.numberOfModules+1)):
            lowerChordX.append(i)
        upperChordX= list()
        for i in range(0,int(self.numberOfModules)):
            upperChordX.append(i)
            upperChordX.append(i+0.5)
        upperChordX.append(self.numberOfModules)

        for x in lowerChordX:
            self.lowerChordJointsPos.append(self.moduleWidth*x*self.horizDirectionVector)
        for x in upperChordX:
            self.upperChordJointsPos.append(self.moduleWidth*x*self.horizDirectionVector+self.rise*self.vertDirectionVector)
        
    def createPostsGeometry(self):
        '''Creates the geometry of the Truss.'''
        super(FanTruss,self).createPostsGeometry()
        #Add intermediate posts.
        lines= self.preprocessor.getMultiBlockTopology.getLines # Line container
        for i in range(1,len(self.lowerChordPoints)-1):
            self.posts.append(lines.newLine(self.lowerChordPoints[i].tag,self.upperChordPoints[2*i].tag))
        for l in self.posts:
            l.nDiv= 2

    def createDiagonalsGeometry(self):
        '''Creates the geometry of the truss diagonals.'''
        lines= self.preprocessor.getMultiBlockTopology.getLines # Line container.
        for i in range(0,len(self.lowerChordPoints)-1):
            self.diagonalLines.append(lines.newLine(self.lowerChordPoints[i].tag,self.upperChordPoints[2*i+1].tag))
            self.diagonalLines.append(lines.newLine(self.lowerChordPoints[i+1].tag,self.upperChordPoints[2*i+1].tag))
        for l in self.diagonalLines:
            l.nDiv= 1
