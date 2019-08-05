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

class TrussGeometry(object):
    '''Base class for truss overall geometry.

    :ivar name: name of the truss (automatically assigned).
    :ivar lowerChordAxis: axis of the lower chord.
    :ivar upperChordAxis: axis of the upper chord.
    :ivar moduleWidth:  width of each truss
    '''
    def __init__(self, lowerChordAxis, upperChordAxis,trussModule):
        '''Constructor.

            Args:
                :param lowerChordAxis: axis of the lower chord.
                :param upperChordAxis: axis of the upper chord.
                :param trussModule: (float) width of each truss.
        '''
        self.name= uuid.uuid4().hex
        self.lowerChordAxis= lowerChordAxis
        self.upperChordAxis= upperChordAxis
        self.moduleWidth= trussModule
        self.numberOfModules= round(self.span()/self.moduleWidth,0)
        self.moduleWidth= self.span()/self.numberOfModules
        
        self.lowerChordJointsPos= list()
        self.upperChordJointsPos= list()
        self.lowerChordPoints= list()
        self.upperChordPoints= list()
        self.lowerChordLines= list()
        self.upperChordLines= list()
        self.diagonalLines= list()
        self.posts= list()
        self.crdTransf= None
        
    def span(self):
        '''Return the span of the truss.'''
        return self.lowerChordAxis.getLength()
    def getLowerChordDirection(self):
        return self.lowerChordAxis.getVDir().normalized()
    def getUpperChordDirection(self):
        return self.upperChordAxis.getVDir().normalized()
    def getUpDirection(self):
        v1= self.upperChordAxis.getFromPoint()-self.lowerChordAxis.getFromPoint()
        v2= self.upperChordAxis.getToPoint()-self.lowerChordAxis.getToPoint()
        return (0.5*(v1+v2)).normalized()
    def getNormalVector(self):
        '''Get a vector normal to the truss plane.'''
        tmp= self.getLowerChordDirection().cross(self.getUpDirection())
        return xc.Vector([tmp.x,tmp.y,tmp.z])
    def populateChords(self, lowerChordX, upperChordX):
        '''Create the positions along the chords.'''
        for x in lowerChordX:
            self.lowerChordJointsPos.append(self.lowerChordAxis.getFromPoint()+self.moduleWidth*x*self.getLowerChordDirection())
        for x in upperChordX:
            self.upperChordJointsPos.append(self.upperChordAxis.getFromPoint()+self.moduleWidth*x*self.getUpperChordDirection())
    
class TrussBase(TrussGeometry):
    '''Base class for truss generators.

    '''
    def __init__(self, lowerChordAxis, upperChordAxis,trussModule):
        '''Constructor.

            Args:
                :param lowerChordAxis: axis of the lower chord.
                :param upperChordAxis: axis of the upper chord.
                :param trussModule: (float) width of each truss.
        '''
        super(TrussBase,self).__init__(lowerChordAxis, upperChordAxis,trussModule)
         
        self.lowerChordMaterial= None
        self.upperChordMaterial= None
        self.diagonalMaterial= None
        self.diagonalArea= None
        self.postsMaterial= None

    def createKeyPoints(self, feProblem):
        '''Create the key points for the Truss.'''
        self.computeJointPositions()
        self.preprocessor= feProblem.getPreprocessor
        kPoints= self.preprocessor.getMultiBlockTopology.getPoints # Point container.
        for j in self.lowerChordJointsPos:
            self.lowerChordPoints.append(kPoints.newPntFromPos3d(j))
        for j in self.upperChordJointsPos:
            self.upperChordPoints.append(kPoints.newPntFromPos3d(j))
            
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

    def createGeometry(self, feProblem):
        '''Creates the geometry of the Truss.'''
        self.createKeyPoints(feProblem)
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

    def createSelfWeightLoads(self,grav):
        ''' Create the self weight loads for the
            elements of the truss.

            :param rho: material density
            :param grav: gravity acceleration (vector).
        '''
        self.upperChordSet.createInertiaLoads(-grav)
        self.lowerChordSet.createInertiaLoads(-grav)
        self.postsSet.createInertiaLoads(-grav)
        self.diagonalSet.createInertiaLoads(-grav)

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
    def __init__(self, lowerChordAxis, upperChordAxis,trussModule):
        '''Constructor.

           Args:
              :param lowerChordAxis: axis of the lower chord.
              :param upperChordAxis: axis of the upper chord.
              :param trussModule: (float) width of each truss.
        '''
        super(WarrenTruss,self).__init__(lowerChordAxis, upperChordAxis,trussModule)

    def computeJointPositions(self):
        '''Compute the positions of the truss joints.'''
        lowerChordX= list()
        for i in range(0,int(self.numberOfModules+1)):
            lowerChordX.append(i)
        upperChordX= [0.0]
        for i in range(0,int(self.numberOfModules)):
            upperChordX.append(i+0.5)
        upperChordX.append(self.numberOfModules)
        self.populateChords(lowerChordX,upperChordX)
    
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
#     +---+---+---+---+---+---+  Upper chord.
#     |  / \  |  / \  |  / \  |
#     | /   \ | /   \ | /   \ |
#     |/     \|/     \|/     \|
#     +-------+-------+-------+  Lower chord.

class FanTruss(WarrenTruss):
    '''Fan truss.

    '''
    def __init__(self, lowerChordAxis, upperChordAxis,trussModule):
        '''Constructor.

           Args:
              :param lowerChordAxis: axis of the lower chord.
              :param upperChordAxis: axis of the upper chord.
              :param trussModule: (float) width of each truss.

        '''
        super(FanTruss,self).__init__(lowerChordAxis, upperChordAxis,trussModule)

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
        self.populateChords(lowerChordX,upperChordX)
        
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
