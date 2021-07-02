# -*- coding: utf-8 -*-
''' Import connection model.'''

from __future__ import division
from __future__ import print_function

import math
import xc_base
import geom
import materials
from import_export import block_topology_entities as bte
from misc_utils import log_messages as lmsg

steelModule= None # material module used to instantiate connection 
                  # elements (ASTM_materials, EC3_materials, ...)

class ImportedMetaData(object):
    ''' Imported meta data.

    :ivar blockProperties: labels and properties associated with the object.
    '''
    def __init__(self, block):
        ''' Constructor.

        :param block: block topology object (point, line, surface or volume).
        '''
        if(block):
            labels= None
            if(block.hasProp('labels')):
                labels= block.getProp('labels')
            attributes= block.getProp('attributes')
            self.blockProperties= bte.BlockProperties(labels, attributes)
        else:
            self.blockProperties= bte.BlockProperties()

    def getLabels(self):
        ''' Return the object labels.'''
        return self.blockProperties.labels
    
    def getAttributes(self):
        ''' Return the object labels.'''
        return self.blockProperties.labels
    
    def hasLabel(self, label):
        ''' Return true if the label argument is a member of
            the object labels.'''
        return self.blockProperties.hasLabel(label)

    def hasAttribute(self, key):
        ''' Return true if the key argument is a member of
            the attributes keys.'''
        return self.blockProperties.hasAttribute(key)
    
    def getAttribute(self, key):
        ''' Return the attribute corresponding to the 
            key argument in the dictionary.'''
        return self.blockProperties.getAttribute(key)

class ConnectionMeanMetaData(ImportedMetaData):
    ''' Meta data for a bolt or a weld.

    '''
    def getSetName(self):
        ''' Return a suitable name set from the object
            attributes.'''
        
        # Get attribute values
        keyLabels= list()
        attributes= ['jointId', 'objType', 'loadTag', 'side']
        for attr in attributes:
            value= self.getAttribute(attr)
            if(value): keyLabels.append(value)
        # labels= self.getLabels()
        # for label in labels:
        #     keyLabels.append(label)

        # Form the name.
        retval= ''
        if(len(keyLabels)>0):
            retval= keyLabels[0]
            for l in keyLabels[1:]:
                retval+='_'+str(l)
        return retval

    def getSetDescription(self):
        ''' Return the set description.'''
        setName= self.getSetName()
        setDescr= setName.replace('_',' ')
        return setDescr

class FaceWeldMetaData(ImportedMetaData):
    ''' Data defining weld position between two faces.

    :ivar face: face connected by the weld.
    :ivar side: side of the welded plate with respect to the connected
                face.
    :ivar angle: angle of the connected face with the welded plate.
    :ivar orientation: orientation of the connected faces with respect
                       to the welded plate.
    '''
    def __init__(self, face, memberToWeld, side= None, angle= None, orientation= None):
        ''' Constructor. Attributes are obtained from the
            point argument properties.

        :param face: face connected by the weld.
        :param memberToWeld: member that will be welded to this one.
        :param side: side of the welded plate with respect to the connected
                    face.
        :param angle: angle of the connected face with the welded plate.
        :param orientation: orientation of the connected faces with respect
                           to the welded plate.
        '''
        super(FaceWeldMetaData, self).__init__(face)
        self.face= face
        self.memberToWeld= memberToWeld
        if(side):
            self.side= side
        else:
            self.side= None
        if(angle):
            self.angle= angle
        if(orientation):
            self.orientation= orientation

    def getKVector(self):
        ''' Return the normal to the face.'''
        return self.face.getKVector

    def getVertices(self):
        ''' Return the face vertices.'''
        return self.face.getVertices
    
    def setFace(self, face):
        ''' Set the face to weld to.'''
        self.face= face
        
    def setAngle(self, angle):
        ''' Set the angle of the face to weld to.'''
        self.angle= angle
        
    def setOrientation(self, orientation):
        ''' Set the orientation of the face to weld to.'''
        self.orientation= orientation
        
    def getLegMinSize(self):
        ''' Return the minimum leg size of the weld.'''
        t1= self.memberToWeld.getProp('thickness')
        t2= self.face.getProp('thickness')
        return steelModule.getFilletWeldMinimumLegSheets(t1,t2)
    
    def getLegMaxSize(self):
        ''' Return the maximum leg size of the weld.'''
        t1= self.memberToWeld.getProp('thickness')
        t2= self.face.getProp('thickness')
        return steelModule.getFilletWeldMaximumLegSheets(t1,t2)


    def computeAngleAndOrientation(self):
        '''Compute the angle the face with
           the face that owns the weld.
        '''
        memberToWeldKVector= self.memberToWeld.getKVector
        memberToWeldCentroid= self.memberToWeld.getCentroid()
        self.angle= memberToWeldKVector.getAngle(self.face.getKVector)
        if((abs(self.angle)>1e-3) and (abs(self.angle-math.pi)>1e-3)):
            facePlane= self.face.getPlane()
            if(facePlane.positiveSide(memberToWeldCentroid)):
                self.side= '+'
            else:
                self.side= '-'
            vertices= self.face.getVertices
            orientation= ''
            memberToWeldPlane= self.memberToWeld.getPlane()
            for v in vertices:
                positiveSide= memberToWeldPlane.positiveSide(v.getPos)
                if(positiveSide):
                    orientation+='+'
                else:
                    negativeSide= memberToWeldPlane.negativeSide(v.getPos)
                    if(negativeSide):
                        orientation+='-'
        else:
            orientation= '0'
        self.orientation= ''.join(set(orientation))

    def report(self):
        ''' Print stuff.'''
        retval= '\n    <face id:' + str(self.face.tag) + '(side:'+ str(self.side) +' angle:'+str(math.degrees(self.angle))+'º, orientation:'+self.orientation+'), Lmin= '+str(self.getLegMinSize()*1e3)+' mm, Lmax= '+str(self.getLegMaxSize()*1e3)+' mm>'
        return retval
        
class WeldMetaData(ConnectionMeanMetaData):
    ''' Meta data for a weld.

    :ivar line: line corresponding to the weld position.
    :ivar memberToWeld: the plate being welded.
    :ivar faceWelds: position of the weld with respect to the
                     connected faces.
    :ivar legSize: weld leg size.
    '''
    def __init__(self, line, memberToWeld= None, faceWelds= None):
        ''' Constructor. Attributes are obtained from the
            point argument properties.
        '''
        super(WeldMetaData,self).__init__(line)
        self.line= line # line representing the welded connection.
        if(memberToWeld):
            self.memberToWeld= memberToWeld
        if(faceWelds):
            self.faceWelds= faceWelds

    def setMemberToWeld(self, memberToWeld):
        ''' Set the plate being welded.'''
        self.memberToWeld= memberToWeld

    def setLegSize(self, legSize):
        ''' Set weld leg size.'''
        self.legSize= legSize

    def extractDataFromProperties(self, blockDict):
        ''' Determine if the weld correspond to a flange plate
            (plate welded to the member flange) to a
            web gusset (welded to the member web)

            and the weld "memberToWeld" (the plate being welded to
            a connection member).

        :param blockDict: dictionary of imported XC blocks (lines, surfaces,...)
        '''
        if(self.hasAttribute('gussetType')):
            self.gusset= self.getAttribute('gussetType')
        if(self.hasAttribute('ownerId')):
            memberToWeld= self.getAttribute('ownerId')
            memberToWeldId= int(memberToWeld[1:])
            self.setMemberToWeld(blockDict[memberToWeldId])
            self.description= blockDict[memberToWeldId].getProp("attributes")['objType']
            if 'part' in blockDict[memberToWeldId].getProp("attributes"):
                  self.description+=' ' +blockDict[memberToWeldId].getProp("attributes")['part']
            self.description+=', joint '+self.getAttribute('jointId')
            self.description=self.description.replace('_',' ')  
        if(self.hasAttribute('legSize')):
            legSize= self.getAttribute('legSize')
            self.setLegSize(legSize)
        
    def populateFaces(self, xcSet):
        '''Get the faces that "touch" the line defining
           the weld.

        :param xcSet: set containing the candidate faces.
        '''
        vertices= self.line.getVertices
        p1= vertices[0].getPos
        p2= vertices[1].getPos
        tol= self.line.getLength()/1e3
        memberToWeldTag= self.memberToWeld.tag
        self.faceWelds= dict()
        for s in xcSet.surfaces:
            if(memberToWeldTag!=s.tag): # don't add the plate to weld.
                plg= s.getPolygon()
                d1= plg.dist(p1)
                d2= plg.dist(p2)
                d= max(d1,d2)
                if(d<tol):
                    self.faceWelds[s.tag]= FaceWeldMetaData(face= s, memberToWeld= self.memberToWeld)
        if(len(self.faceWelds)==0):
            lmsg.error('weld line from face: '+self.memberToWeld.name+' is not connected to any surface.')
        
    def computeAnglesAndOrientation(self):
        '''Compute the angles of those faces with
           the face that owns the weld.'''
        for wfKey in self.faceWelds:
            wf= self.faceWelds[wfKey]
            wf.computeAngleAndOrientation()
            
    def report(self):
        ''' Print stuff.'''
        outStr= ''
        for wfKey in self.faceWelds:
            wf= self.faceWelds[wfKey]
            outStr+= wf.report()
        print('weld: ', self.line.name, self.memberToWeld.tag, self.getSetName(), outStr)
        


def extractWelds(xcSet, blockDict):
    ''' Return a list with the lines corresponding
        to weld beads in the set.

    :param xcSet: set containing the objects that will be picked on to
                     populate the list.
    :param blockDict: dictionary of imported XC blocks (lines, surfaces,...)
    '''
    retval= list()
    for line in xcSet.getLines:
        if(line.hasProp('attributes')):
            attributes= line.getProp('attributes')
            if('objType' in attributes):
                objType= attributes['objType']
                if(objType=='weld'): # line is a weld bead
                    wmd= WeldMetaData(line)
                    retval.append(wmd)

    # Determine if the weld correspond to a flange plate
    # (plate welded to the member flange) to a
    # web plate (welded to the member web)
    # and the weld "memberToWeld" (the plate being welded to
    # a connection member)
    for w in retval:
        w.extractDataFromProperties(blockDict)

    # Get the faces that "touch" the line defining
    # the weld.
    for w in retval:
        w.populateFaces(xcSet)

    # Compute the angles of those faces with
    # the face that owns the weld.
    for w in retval:
        w.computeAnglesAndOrientation()
    return retval

def populateSets(setsFromLabels, xcSet):
    ''' Populate the sets based on the object labels.

    :param setsFromLabelsDict: dictionary containing the sets to popùlate. The
                               objects (points, lines, surfaces,...) which 
                               have a label that correspond to a dictionary 
                               key will be added to the set corresponding to 
                               the key. The object labeled as holes are 
                               exceptions to this rule and are returned
                               in a list to further processing.
    :param xcSet: set containing the objects that will be picked on to
                     populate the previous ones.
    '''
    holes= list() # Bolt holes.
    for s in xcSet.getSurfaces:
        surfaceAttributes= s.getProp('attributes')
        objType= surfaceAttributes['objType']
        jointId= surfaceAttributes['jointId']
        if(jointId):
            s.setProp('joint',jointId)
        if('ownerId' in surfaceAttributes):
            ownerId= surfaceAttributes['ownerId']
            if(ownerId):
                if(objType=='hole_center'):
                    s.setProp('hole_center_owner', ownerId)
                elif(objType=='hole'):
                    s.setProp('hole_owner', ownerId)
                else:
                    lmsg.error("Object type: '"+ objType+"' unknown.")
        if('matId' in surfaceAttributes):
            s.setProp('matId',surfaceAttributes['matId'])
        if(objType=='gusset_plate'):
            if('attachedTo' in surfaceAttributes):
                s.setProp('member', surfaceAttributes['attachedTo'])
            else:
                lmsg.error('Gusset plate with no attached member.')
        if(objType in ['column', 'bolted_plate', 'baseplate']):
            s.setProp('member', objType)
        if('loadTag' in surfaceAttributes):
            loadTag= surfaceAttributes['loadTag']
            if(loadTag):
                s.setProp('load_tag', loadTag)
                ld= surfaceAttributes['loadDirI']
                s.setProp('load_dir_i',geom.Vector3d(ld[0],ld[1],ld[2]))
                ld= surfaceAttributes['loadDirJ']
                s.setProp('load_dir_j',geom.Vector3d(ld[0],ld[1],ld[2]))
                ld= surfaceAttributes['loadDirK']
                s.setProp('load_dir_k',geom.Vector3d(ld[0],ld[1],ld[2]))
        if(objType=='hole'): # surface is a hole
            if('attachedTo' in surfaceAttributes):
                s.setProp('gusset', surfaceAttributes['attachedTo'])
            # else:
            #     lmsg.error('hole: '+str(s.tag)+' without attachment.')
            holes.append(s)
        else: # regular surface.
            if(objType in setsFromLabels):
                xcSet= setsFromLabels[objType]
                xcSet.getSurfaces.append(s)
            # if(s.hasProp('labels')):
            #     surfaceLabels= s.getProp('labels')
            #     for key in setsFromLabels:
            #         if(surfaceLabels.count(key)>0):
            #             xcSet= setsFromLabels[key]
            #             xcSet.getSurfaces.append(s)
    return holes

def pickHoleCenters(xcSet, blockDict):
    ''' Pick the hole centers from the objects from the argument set. Assign 
        to them the following properties:

        - hole_center_owner: hole surface which center correspond to this.
        - diameter: diameter of the hole.
        - material: bolt steel type.

    :param xcSet: XC set to pick the objects from.
    :param blockDict: dictionary of imported XC blocks (lines, surfaces,...)
    '''
    holeCenters= dict()   
    for p in xcSet.getPoints:
        pointAttributes= p.getProp('attributes')
        objType= pointAttributes['objType']
        if(objType=='hole_center' or objType=='anchor_point'):
            holeCenters[p.tag]= p
            ownerId= pointAttributes['ownerId']
            p.setProp('hole_center_owner', ownerId)
            diameter= pointAttributes['diameter']
            p.setProp('diameter',diameter)
            moduleName= steelModule.__name__
            material= eval(moduleName+'.'+pointAttributes['boltMaterial'])
            p.setProp('material', material)
    return holeCenters

class BoltEndMetaData(ConnectionMeanMetaData):
    ''' Meta data for a bolt end (head or nut).

    :ivar kPoint: point corresponding to the bolt end.
    :ivar hole: octagonal surface that defines the hole.
    :ivar plate: surface containing the hole.
    :ivar diameter: bolt diameter.
    :ivar material: bolt steel type.
    '''
    def __init__(self, kPoint, blockDict):
        ''' Constructor. Attributes are obtained from the
            point argument properties.

        :param blockDict: dictionary of imported XC blocks (lines, surfaces,...)
        '''
        super(BoltEndMetaData,self).__init__(kPoint)
        self.kPoint= kPoint # hole center.
        self.hole= None
        if(kPoint.hasProp('hole_center_owner')):
            holeName=  kPoint.getProp('hole_center_owner') # octagonal hole
            if(holeName): # point on a plate
                holeId= int(holeName[1:])
                self.hole= blockDict[holeId]
                self.plate= None
                if(self.hole):
                    fName= self.hole.getProp('hole_owner')
                    fId= int(fName[1:])
                    self.plate= blockDict[fId]
        self.material= kPoint.getProp('material') # steel type
        self.diameter= kPoint.getProp('diameter') # bolt diameter
        
    def getMaterial(self):
        ''' Return the bolt material.'''
        return self.material

    def getDiameter(self):
        ''' Return the bolt diameter.'''
        return self.diameter

    def getPlateThickness(self):
        ''' Return the plate thickness.'''
        retval= 0.0
        if(self.plate):
            retval= self.plate.getProp('thickness')
        return retval
    
    def report(self):
        ''' Print info.'''
        outStr= 'hole center: '+ self.kPoint.name
        if(self.hole):
            outStr+= ' hole: '+ self.hole.name
            outStr+= ' plate thickness: '+ str(self.getPlateThickness())
        outStr+= ' diameter: '+ str(self.diameter)
        outStr+= ' material: '+ self.material.name
        print(outStr)

class BoltMetaData(object):
    ''' Meta data for a bolt.

    :ivar endA: point corresponding to a bolt end.
    :ivar endB: point corresponding to the other bolt end.
    :ivar line: line between endA and endB
    '''
    def __init__(self, kPointA, line, kPointB, blockDict):
        ''' Constructor.

        :param kPointA: point corresponding to a bolt end.
        :param kPointB: point corresponding to the other bolt end.
        :param line: line between kPointA and kPointB
        '''
        self.endA= BoltEndMetaData(kPointA, blockDict)
        self.line= line
        self.endB= BoltEndMetaData(kPointB, blockDict)

    def getDiameter(self):
        ''' Return the bolt diameter.'''
        return self.endA.getDiameter()
        
    def getMaterial(self):
        ''' Return the bolt material.'''
        return self.endA.getMaterial()
                
    def getSetName(self):
        ''' Return a suitable name set from the point labels.'''
        return self.endA.getSetName()
    
    def getAttribute(self, key):
        ''' Return the value of the attribute corresponding
            to the key argument.'''
        retval= self.endA.getAttribute(key)
        if(not retval):
            retval= self.endB.getAttribute(key)
        return retval

    def getBoltGroupName(self):
        ''' Return the name of the bolt group (aka the value of the boltGroup attribute).'''
        return self.getAttribute('boltGroup')

    def getSetDescription(self):
        ''' Return the set description.'''
        return self.endA.getSetDescription()

    def report(self):
        ''' Print stuff.'''
        self.endA.report()
        self.endB.report()
        print('set name:', self.getSetName())
        print('set description: ', self.getSetDescription())

        

def matchHoleCenters(xcSet, blockDict):
    ''' Match hole centers that correspond to the extremities of
        the same bolt.

    :param xcSet: set to pick the hole centers from.
    :param blockDict: dictionary of imported XC blocks (lines, surfaces,...)
    '''
    holeCentersDict= pickHoleCenters(xcSet, blockDict)
    trios= list()
    visited= list()
    for key in holeCentersDict:
        p= holeCentersDict[key]
        if(not (key in visited)):
            visited.append(key)
            connectedLines= p.getConnectedEdges
            if(len(connectedLines)>0): # Only one connected line here
                points= connectedLines[0].getKPoints()
                otherPoint= points[0]
                if(otherPoint==p.tag): # is the other one.
                    otherPoint= points[1]
                visited.append(otherPoint)
                trios.append((p.tag, connectedLines[0], otherPoint))

    matchedBolts= list()
    for trio in trios:
        pA= holeCentersDict[trio[0]]
        line= trio[1]
        pB= holeCentersDict[trio[2]]
        matchedBolts.append(BoltMetaData(pA, line, pB, blockDict))

    return matchedBolts

def matchHoles(holes, blockDict):
    '''Put the holes in its place (in its polygonal face).

    :param holes: holes to put in its face.
    :param blockDict: dictionary of imported XC blocks (lines, surfaces,...)
    '''
    for h in holes:
        h.setNDiv(1)
        owrId= int(h.getProp('hole_owner')[1:])
        owner= blockDict[owrId]
        holeDiameter= 2.0*math.sqrt(h.getArea()/math.pi)
        owner.addHole(h)
        owner.setProp('holeDiameter', holeDiameter)

def getLoadCaseNames(loadData):
    ''' Return the load names implicitly defined
        in the load data argument.

    :param loadData: dictionary containing the information about internal loads.
    '''
    retval= list()
    for key in loadData:
        loads= loadData[key]
        # Add load cases to XC model.
        for comb in loads:
            if(not comb in retval):
                retval.append(comb)
    return retval
        
def getLoadedSides(xcSet):
    ''' Return the sides affected by loads.'''
    retval= dict()
    for s in xcSet.surfaces:
        if(s.hasProp('load_tag')): # is loaded
            loadTag= s.getProp('load_tag')
            loadDirI= s.getProp('load_dir_i')
            loadDirJ= s.getProp('load_dir_j')
            loadDirK= s.getProp('load_dir_k')
            member= None
            if(s.hasProp('member')):
                member= s.getProp('member')
            ## Get the lines pointed by the load direction: loaded sides.
            faceCenter= s.getCentroid()
            sideList= list()
            if(loadTag in retval):
                sideList.extend(retval[loadTag]['sideList'])
            for side in s.getSides:
                sideCenter= side.getCentroid()
                sideDir= sideCenter-faceCenter
                angle= loadDirI.getAngle(sideDir)
                if(abs(angle)<1e-3):
                    sideList.append(side)
            if(len(sideList)==0):
                lmsg.warning('side list empty for load tag: '+str(loadTag))
            retval[loadTag]= {'member':member, 'I':loadDirI,'J':loadDirJ,'K':loadDirK, 'sideList':sideList}
    return retval
