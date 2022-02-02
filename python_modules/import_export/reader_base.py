# -*- coding: utf-8 -*-
''' Base for classes that import from DXF, FreeCAD,...'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2020, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.ortega.ort@gmail.com"

import re
import sys
import datetime
import math
from scipy.spatial.distance import cdist
from misc_utils import log_messages as lmsg
from import_export import block_topology_entities as bte

def nameToImport(testName,namesToImport):
    '''Return true if the name matches one of the regular expressions
       contained in the second argument.

       :param testName: name to be tested.
       :param namesToImport: list of regular expressions to be tested.
    '''
    for regExp in namesToImport:
        if(re.match(regExp,testName)):
            return True
    return False

class ReaderBase(object):
    '''Base class for DXF and FreeCAD readers.

     :ivar fileName: file name to import.
     :ivar getRelativeCoo: coordinate transformation to be applied to the
                            points.
     :ivar threshold: minimum distance between two points to be
                       considered different points. All the points 
                       that are closer than the threshold distance 
                       are melted into one k-point.
     :ivar impLines: if true import lines.
     :ivar impSurfaces: if true import surfaces.
     :ivar propertyDict: dictionary that relies each object name with its properties (labels and attributes).
     :ivar lines: dictionary storing the imported lines.
     :ivar facesTree: dictionary storing the imported faces.
    '''
    def __init__(self,fileName, getRelativeCoo, threshold= 0.01, importLines= True, importSurfaces= True):
        ''' Constructor.

           :param fileName: name of the file to import.
           :param getRelativeCoo: coordinate transformation to be applied to the
                                  points.
           :param threshold: minimum distance between two points to be
                             considered different points. All the points 
                             that are closer than the threshold distance 
                             are melted into one k-point.
           :param importLines: if true import lines.
           :param importSurfaces: if true import surfaces.
        '''
        self.fileName= fileName
        self.getRelativeCoo= getRelativeCoo
        self.threshold= threshold
        self.impLines= importLines
        self.impSurfaces= importSurfaces
        self.propertyDict= dict()
        self.lines= dict()
        self.facesTree= dict()

    def getIndexNearestPoint(self, pt):
        return cdist([pt], self.kPoints).argmin()

    def getNearestPoint(self, pt):
        return self.kPoints[self.getIndexNearestPoint(pt)]
    
    def exportBlockTopology(self, name):
        retval= bte.BlockData()
        retval.name= name
        retval.fileName= self.fileName
        retval.logMessage= '# imported from file: '+self.fileName+' on '
        retval.logMessage+= str(datetime.datetime.now())

        counter= 0
        if(self.kPoints):
            for p in self.kPoints:
                key= self.kPointsNames[counter]
                bp= bte.BlockProperties()
                bp.copyFrom(self.propertyDict[key])
                # Add labels and attributes of points
                for k in self.points:
                    pp= self.points[k][0]
                    if(pp==counter):
                        bp.extend(self.propertyDict[k])
                retval.appendPoint(id= counter,x= p[0],y= p[1],z= p[2], pointProperties= bp)
                counter+= 1

            counter= 0
            for key, line in self.lines.items():
                bp= self.propertyDict[key]
                block= bte.BlockRecord(counter,'line',line, blockProperties= bp)
                retval.appendBlock(block)
                counter+= 1

            for name in self.getNamesToImport():
                fg= self.facesTree[name]
                for key, face in fg.items():
                    bp= self.propertyDict[key]
                    block= bte.BlockRecord(counter,'face',face, blockProperties= bp)
                    retval.appendBlock(block)
                    counter+= 1
        else:
            lmsg.warning('Nothing to export.')
        return retval
    
    def getOrientation(self, p0, p1, tol):
        ''' Return the points indexes in the order that makes the resulting
            vector to point outwards the origin.

        :param p0: first point.
        :param p1: second point.
        :param tol: tolerance to consider that two coordinates are equal.
        '''
        idx0= self.getIndexNearestPoint(p0)
        idx1= self.getIndexNearestPoint(p1)
        v0= self.kPoints[idx0]
        v1= self.kPoints[idx1]
        deltas= [abs(v0[0]-v1[0]), abs(v0[1]-v1[1]), abs(v0[2]-v1[2])]
        indexes= sorted(range(len(deltas)), key=lambda k: deltas[k])
        i0= indexes[2]; i1= indexes[1]; i2= indexes[0]
        if(v0[i0]>v1[i0]): # r1<r2
            idx0, idx1= idx1, idx0 # swap
        elif(abs(v0[i0]-v1[i0])<tol): # x1==x2
            if(v0[i1]>v1[i1]):
                idx0, idx1= idx1, idx0 # swap
            elif(abs(v0[i1]-v1[i1])<tol): # y1==y2
                if(v0[i2]>v1[i2]):
                    idx0, idx1= idx1, idx0 # swap
        return idx0, idx1
    
    def selectKPoints(self):
        '''Selects the k-points to be used in the model. All the points that
           are closer than the threshold distance are melted into one k-point.
        '''
        points, properties= self.extractPoints()
        indexDict= None
        keys= list(points.keys())
        if(len(keys)>0):
            # Append first point
            pointName= keys[0]
            self.kPoints= [points[pointName]]
            objProperties= properties[pointName]
            self.propertyDict[pointName]= objProperties
            indexDict= dict()
            indexDict[0]= pointName
            # Iterate through the rest of the points
            for pointName in keys[1:]:
                p= points[pointName]
                objProperties= properties[pointName]
                indexNearestPoint= self.getIndexNearestPoint(p)
                nearestPoint= self.kPoints[indexNearestPoint]
                dist= cdist([p],[nearestPoint])[0][0]
                if(dist>self.threshold): # new point.
                    indexNearestPoint= len(self.kPoints) # The point itself.
                    self.kPoints.append(p)
                    self.propertyDict[pointName]= objProperties
                    indexDict[indexNearestPoint]= pointName
                else:
                    pointName= indexDict[indexNearestPoint]
                    self.propertyDict[pointName].extend(objProperties)
        else:
            lmsg.warning('No points in :'+self.fileName+' file.')
        return indexDict
        
def populateSetsFromEntitiesLabels(labelSetPairs, xcSet):
    ''' Populate the sets in the labelSetPairs arguments according to
        the labels of the entities in the xcSet argument.

    :param labelSetPairs: list of pairs formed for a regular expression and
                          a set to populate with the entities whose labels
                          match the regular expression.
    :param xcSet: set of the entities that will be classified according to
                  its labels.
    '''
    # Classify points.
    for p in xcSet.points: # for each point in the set.
        labels= p.getProp('labels') # point labels.
        for pair in labelSetPairs:
            regEx= re.compile(pair[0]) # regular expression
            setToPopulate= pair[1] # set to populate
            for lbl in labels:
                if(regEx.match(lbl)): # if the label match the regular expr.
                    setToPopulate.getPoints.append(p) # put the point in the set

    # Classify lines.
    for l in xcSet.lines: # for each line in the set.
        labels= l.getProp('labels') # line labels.
        for pair in labelSetPairs:
            regEx= re.compile(pair[0]) # regular expression
            setToPopulate= pair[1] # set to populate
            for lbl in labels:
                if(regEx.match(lbl)): # if the label match the regular expr
                    setToPopulate.getLines.append(l) # put the line in the set
                    
    # Classify surfaces.
    for s in xcSet.surfaces: # for each line in the set.
        labels= s.getProp('labels') # surface labels.
        for pair in labelSetPairs:
            regEx= re.compile(pair[0]) # regular expression
            setToPopulate= pair[1] # set to populate
            for lbl in labels:
                if(regEx.match(lbl)): # if the label match the regular expr
                    setToPopulate.getSurfaces.append(s) # put the surface in the set
                    
    # Classify bodies.
    for b in xcSet.bodies: # for each line in the set.
        labels= b.getProp('labels') # body labels.
        for pair in labelSetPairs:
            regEx= re.compile(pair[0]) # regular expression
            setToPopulate= pair[1] # set to populate
            for lbl in labels:
                if(regEx.match(lbl)): # if the label match the regular expr
                    setToPopulate.getBodies.append(b) # put the body in the set

    # Fill sets downwards.
    visited= list()
    for pair in labelSetPairs:
        setToPopulate= pair[1]
        if(setToPopulate.empty()):
            functionName= sys._getframe(0).f_code.co_name
            lmsg.warning(functionName+'; set: \''+setToPopulate.name+'\' is empty.')
        if(setToPopulate not in visited):
            setToPopulate.fillDownwards()
        visited.append(setToPopulate)

def findLabel(members, labelToFind):
    ''' Search for the member(s) with the label argument.

    :param members: container with the members to search on.
    :param labelToFind: label to find. 
    '''
    retval= list()
    for m in members:
        labels= m.getProp('labels')
        if(labelToFind in labels):
            retval.append(m)
    return retval

def findTouchingMember(members, posToFind):
    ''' Search for the member which has the nearest vertex 
        to the position argument.

    :param members: container with the members to search on.
    :param posToFind: position to approach. 
    '''
    retval= None
    vertexIndex= None
    minDist= 6.023e23
    for member in members:
        vertices= member.getVertices
        for i, v in enumerate(vertices):
            d2= v.getPos.dist2(posToFind)
            if(d2<minDist):
                minDist= d2
                vertexIndex= i
                retval= member
    return retval, vertexIndex, math.sqrt(minDist)
 
def findConnectedMember(members, labelToFind, posToFind):
    ''' Search for the member which has the label argument
        and is the nearest vertex to the position argument.

    :param members: container with the members to search on.
    :param labelToFind: label to find. 
    :param posToFind: position to approach. 
    '''
    # Search for the members which has the label.
    connectedMembers= findLabel(members, labelToFind)
    retval= None
    if(len(connectedMembers)>0):
        retval= findTouchingMember(connectedMembers, posToFind)
    else:
        funcName= sys._getframe(0).f_code.co_name
        lmsg.error(funcName+'; no members found with label: \''+labelToFind+'\'')
    # Search for the touching member between the connected.
    return retval
