# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc

class SetToMesh(object):
    '''Base for classes that mesh a primitive set.
    
    :ivar primitiveSet: set of lines/surfaces/blocks.
    :ivar elemSize: mean size of the elements
    :ivar elemType: type of element for the mesh.
    '''
    def __init__(self,primitiveSet,elemSize,elemType):
        '''Constructor.'''
        self.primitiveSet= primitiveSet
        self.elemSize= elemSize
        self.elemType= elemType

def getDefaultCoodinateTransformation(self,coordTransfName,vDir):
    '''Creates a default coordinate transformation.''' 
    trfs= preprocessor.getTransfCooLoader
    trYGlobal=trfs.newPDeltaCrdTransf3d(coordTransfName) #XXX PDelta???
    trYGlobal.xzVector= vDir
    

class LinSetToMesh(SetToMesh):
    '''Define the parameters to mesh a set of lines. The method generateMesh
    meshes those lines and adds the nodes and elements created to the set .
    
    :ivar matSect: instance of the class BeamMaterialData that defines the 
          material-section to be applied to the set of lines.
    :ivar vDirLAxZ: direction vector for the element local axis Z 
          defined as xc.Vector([x,y,z]). This is the direction in which
          the Z local axis of the reinforced concrete sections will be
          oriented (i.e. in the case of rectangular sections this Z 
          local axis of the RC section is parallel to the dimension
          defined as width of the rectangle)
    :ivar dimElemSpace: dimension of the element space (defaults to 3)
    '''
    def __init__(self,linSet,matSect,elemSize,vDirLAxZ,elemType='elastic_beam_3d',dimElemSpace=3):
        super(LinSetToMesh,self).__init__(linSet,elemSize,elemType)
        self.matSect= matSect
        self.vDirLAxZ= vDirLAxZ
        self.dimElemSpace=dimElemSpace

    def getSeedElement(self, preprocessor,coordTransf):
        '''Return the element that will be use to mesh the set.

          :param coordTransf: element coordinate transformation object.
        '''
        seedElemLoader= preprocessor.getElementLoader.seedElemLoader
        seedElemLoader.defaultMaterial= self.matSect.name
        seedElemLoader.dimElem= self.dimElemSpace
        seedElemLoader.defaultTransformation= coordTransfName
        return seedElemLoader.newElement(self.elemType,xc.ID([0,0]))

    def generateMesh(self, preprocessor,coordTransf= None):
        '''Generate the mesh for the line set.

          :param coordTransf: element coordinate transformation object.
        '''
        cTrf= coordTransf
        if(cTrf==None):
            cTrf= getDefaultCoordinateTransformation('trYGlobal',self.vDirLAxZ)
        elem= self.getSeedElement(cTrf.getName())
        for l in self.primitiveSet.getLines:
            l.setElemSize(self.elemSize)
            l.genMesh(xc.meshDir.I)
        self.primitiveSet.fillDownwards()

   
class SurfSetToMesh(SetToMesh):
    '''Define the parameters to mesh a set of surfaces. The method generateMesh
    meshes those surfaces and adds the nodes and elements created to the set .
    
    :ivar surfSet: set of surfaces.
    :ivar matSect: instance of the class DeckMaterialData that defines the 
          material-section to be applied to the set of surfaces.
    :ivar elemSize: mean size of the elements
    :ivar elemType: type of element for the mesh (defaults to 'shell_mitc4')
    '''
    def __init__(self,surfSet,matSect,elemSize,elemType='shell_mitc4'):
        super(SurfSetToMesh,self).__init__(surfSet,elemSize,elemType)
        self.matSect= matSect

    def getSeedElement(self, preprocessor):
        '''Return the element that will be use to mesh the set.'''
        seedElemLoader= preprocessor.getElementLoader.seedElemLoader
        seedElemLoader.defaultMaterial= self.matSect.name
        return seedElemLoader.newElement(self.elemType,xc.ID([0,0,0,0]))

    def generateMesh(self, preprocessor):
        '''Generate the mesh for the surface set.'''
        for s in self.primitiveSet.getSurfaces:
            s.setElemSizeIJ(self.elemSize,self.elemSize)
        preprocessor.getCad.getSurfaces.conciliaNDivs()
        elem= self.getSeedElement(preprocessor)
        for s in self.primitiveSet.getSurfaces:
            s.genMesh(xc.meshDir.I)
        self.primitiveSet.fillDownwards()


def multi_mesh(preprocessor,lstMeshSets):
    '''Mesh all the mesh-sets included in lstMeshSets

    :param preprocessor: preprocessor
    :param lstMeshSets: list of instances of classes LinSetToMesh or 
           SurfSetToMesh to be meshed
    '''
    for ms in lstMeshSets:
        ms.generateMesh(preprocessor)
