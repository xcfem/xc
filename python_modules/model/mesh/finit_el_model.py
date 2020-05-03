# -*- coding: utf-8 -*-


__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc
from misc_utils import log_messages as lmsg

class SetToMesh(object):
    '''Base for classes that mesh a primitive set.
    
    :ivar primitiveSet: set of lines/surfaces/blocks.
    :ivar elemSize: mean size of the elements (in elemSize= None do not 
                    change number of divisions).
    :ivar elemType: type of element for the mesh.
    '''
    def __init__(self,primitiveSet,elemSize,elemType):
        '''Constructor.'''
        self.primitiveSet= primitiveSet
        self.elemSize= elemSize
        self.elemType= elemType

class RawLineSetToMesh(SetToMesh):
    '''Define the parameters to mesh a set of lines. The method generateMesh
    meshes those lines and adds the nodes and elements created to the set .
    
    :ivar matSect: instance of the class BeamMaterialData that defines the 
          material-section to be applied to the set of lines.
    :ivar elemSize: mean size of the elements (in elemSize= None do not 
                    change number of divisions).
    :ivar coordTransf: coordinate transformation for the elements.
    :ivar elemType: type of element for the mesh (defaults to 'ElasticBeam3d').
    :ivar dimElemSpace: dimension of the element space (defaults to 3)
    '''
    def __init__(self,linSet,matSect,elemSize,coordTransf, elemType='ElasticBeam3d',dimElemSpace=3):
        super(RawLineSetToMesh,self).__init__(linSet,elemSize,elemType)
        self.matSect= matSect
        self.coordinateTransformation= coordTransf
        self.dimElemSpace=dimElemSpace

    def getSeedElement(self, preprocessor):
        '''Return the element that will be use to mesh the set.'''
        seedElemHandler= preprocessor.getElementHandler.seedElemHandler
        seedElemHandler.defaultMaterial= self.matSect.name
        seedElemHandler.dimElem= self.dimElemSpace
        if(self.coordinateTransformation):
            seedElemHandler.defaultTransformation= self.coordinateTransformation.getName()
        else:
            seedElemHandler.defaultTransformation= 'None'
        return seedElemHandler.newElement(self.elemType,xc.ID([0,0]))

    def generateMesh(self, preprocessor,sectGeom='N'):
        '''Generate the mesh for the line set. 

        :param sectGeom: ='Y' if want to create the property  'sectionGeometry'
                  for each element (defaults to 'N')
        '''
        elem= self.getSeedElement(preprocessor)
        for l in self.primitiveSet.getLines:
            if(self.elemSize): #If elemSize==None don't touch the number of divisions.
                l.setElemSize(self.elemSize)
            l.genMesh(xc.meshDir.I)
            if sectGeom[0].lower()=='y':
                for e in l.getElements:
                    e.setProp('sectionGeometry',self.matSect)
        self.primitiveSet.fillDownwards()

def getDefaultCoordinateTransformation(preprocessor,coordTransfName,coordTransfType,vDir):
    '''Creates a default coordinate transformation.''' 
    trfs= preprocessor.getTransfCooHandler
    if coordTransfType.lower()=='pdelta':
        retval= trfs.newPDeltaCrdTransf3d(coordTransfName)
    elif coordTransfType.lower()=='corot':
        retval= trfs.newCorotCrdTransf3d(coordTransfName)
    else:
        retval= trfs.newLinearCrdTransf3d(coordTransfName)
    retval.xzVector= vDir
    '''   13.11.2017 
    retval= trfs.getCoordTransf(coordTransfName)
    if(retval==None):
      msg= "getDefaultCoordinateTransformation creates a PDelta"
      msg+= " 3D coordinate transformation which is not necessarily"
      msg+= " the best choice for your element."
      lmsg.warning(msg)
      retval= trfs.newPDeltaCrdTransf3d(coordTransfName) #XXX PDelta???
      retval.xzVector= vDir
    '''
    return retval


class LinSetToMesh(RawLineSetToMesh):
    '''Define the parameters to mesh a set of lines. The method generateMesh
    meshes those lines and adds the nodes and elements created to the set .
    
    :ivar matSect: instance of the class BeamMaterialData that defines the 
          material-section to be applied to the set of lines.
    :ivar elemSize: mean size of the elements (in elemSize= None do not 
                    change number of divisions).
    :ivar vDirLAxZ: direction vector for the element local axis Z 
          defined as xc.Vector([x,y,z]). This is the direction in which
          the Z local axis of the sections will be
          oriented (i.e. in the case of rectangular sections this Z 
          local axis of the section is parallel to the dimension
          defined as width of the rectangle)
    :ivar dimElemSpace: dimension of the element space (defaults to 3)
    :ivar coordTransfType: type of coordinate transformation. Available 
                       types: 'linear', 'PDelta' and 'corot' (defaults to 
                       'linear') 
    '''
    def __init__(self,linSet,matSect,elemSize,vDirLAxZ, elemType='ElasticBeam3d',dimElemSpace=3,coordTransfType='linear'):
        self.vDirLAxZ= vDirLAxZ
        cTrf= None
        if(coordTransfType!= None):
          trfName= linSet.name+'trYGlobal'
          cTrf= getDefaultCoordinateTransformation(linSet.owner,trfName,coordTransfType,self.vDirLAxZ)
        super(LinSetToMesh,self).__init__(linSet,matSect,elemSize,cTrf,elemType,dimElemSpace)

   
class SurfSetToMesh(SetToMesh):
    '''Define the parameters to mesh a set of surfaces. The method generateMesh
    meshes those surfaces and adds the nodes and elements created to the set .
    
    :ivar surfSet: set of surfaces.
    :ivar matSect: instance of the class DeckMaterialData that defines the 
          material-section to be applied to the set of surfaces.
    :ivar elemSize: mean size of the elements (in elemSize= None do not 
                    change number of divisions).
    :ivar elemType: type of element for the mesh (defaults to 'ShellMITC4')
    '''
    def __init__(self,surfSet,matSect,elemSize,elemType='ShellMITC4'):
        super(SurfSetToMesh,self).__init__(surfSet,elemSize,elemType)
        self.matSect= matSect

    def getSeedElement(self, preprocessor):
        '''Return the element that will be use to mesh the set.'''
        seedElemHandler= preprocessor.getElementHandler.seedElemHandler
        seedElemHandler.defaultMaterial= self.matSect.name
        return seedElemHandler.newElement(self.elemType,xc.ID([0,0,0,0]))

    def generateMesh(self, preprocessor,sectGeom='N'):
        '''Generate the mesh for the surface set.

        :param sectGeom: ='Y' if want to create the property  'sectionGeometry'
                  for each element (defaults to 'N')
        '''
        for s in self.primitiveSet.getSurfaces:
            if(self.elemSize): #If elemSize= None don't touch the number of divisions.
                s.setElemSizeIJ(self.elemSize,self.elemSize)
        preprocessor.getMultiBlockTopology.getSurfaces.conciliaNDivs()
        elem= self.getSeedElement(preprocessor)
        for s in self.primitiveSet.getSurfaces:
            s.genMesh(xc.meshDir.I)
            if sectGeom[0].lower()=='y':
                for e in s.getElements:
                    e.setProp('sectionGeometry',self.matSect)
        self.primitiveSet.fillDownwards()


def multi_mesh(preprocessor,lstMeshSets,sectGeom='N'):
    '''Mesh all the mesh-sets included in lstMeshSets

    :param preprocessor: preprocessor
    :param lstMeshSets: list of instances of classes LinSetToMesh or 
           SurfSetToMesh to be meshed
    :param sectGeom: ='Y' if want to create the property  'sectionGeometry'
                  for each element (defaults to 'N')
    '''
    for ms in lstMeshSets:
        ms.generateMesh(preprocessor,sectGeom)

def assign_ndiv_to_lines_in_set(lnSet,ndiv):
    '''Assign a number of divisions = ndiv to all the lines included 
    in set linSet
    '''
    lnSet.fillDownwards()
    for l in lnSet.getLines:
        l.nDiv=ndiv
    return

def createBeam2Pnts(preprocessor,startPnt,endPnt,setName,matSect,elemSize,vDirLAxZ,elemType='ElasticBeam3d',dimElemSpace=3,coordTransfType='linear',sectGeom='N'):
    '''Return a set of beam elements created from startPnt to endPnt.
    
    :param preprocessor: preprocessor
    :param startPnt: coordinates of first beam extremity (defined as 
          geom.Pos3d(x,y,z)
    :param startPnt: coordinates of end beam extremity (defined as 
          geom.Pos3d(x,y,z)
    :param setName: name of the set of entities created
    :param matSect: instance of the class BeamMaterialData that defines the 
          material-section to be applied to the set of lines.
    :param elemSize: mean size of the elements
    :param vDirLAxZ: direction vector for the element local axis Z 
          defined as xc.Vector([x,y,z]). This is the direction in which
          the Z local axis of the sections will be
          oriented (i.e. in the case of rectangular sections this Z 
          local axis of the section is parallel to the dimension
          defined as width of the rectangle)
    :param dimElemSpace: dimension of the element space (defaults to 3)
    :param coordTransfType: type of coordinate transformation. Available 
                       types: 'linear', 'PDelta' and 'corot' (defaults to 
                       'linear') 
    :param sectGeom: ='Y' if want to create the property  'sectionGeometry'
                  for each element (defaults to 'N')
    '''
    s=preprocessor.getSets.defSet(setName)
    ext1=preprocessor.getMultiBlockTopology.getPoints.newPntFromPos3d(startPnt)
    ext2=preprocessor.getMultiBlockTopology.getPoints.newPntFromPos3d(endPnt)
    l=preprocessor.getMultiBlockTopology.getLines.newLine(ext1.tag,ext2.tag)
    s.getLines.append(l)
    sMesh=LinSetToMesh(s,matSect,elemSize,vDirLAxZ,elemType,dimElemSpace,coordTransfType)
    sMesh.generateMesh(preprocessor,sectGeom)
    return s
    
