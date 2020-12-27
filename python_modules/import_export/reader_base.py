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
import datetime
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
        self.propertyDict= {}
        self.lines= {}
        self.facesTree= {}

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
                bp= self.propertyDict[key]
                retval.appendPoint(id= counter,x= p[0],y= p[1],z= p[2], pointProperties= bp)
                counter+= 1

            counter= 0
            for key in self.lines:
                line= self.lines[key]
                bp= self.propertyDict[key]
                block= bte.BlockRecord(counter,'line',line, blockProperties= bp)
                retval.appendBlock(block)
                counter+= 1

            for name in self.getNamesToImport():
                fg= self.facesTree[name]
                for key in fg:
                    face= fg[key]
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
        points, layers= self.extractPoints()
        indexDict= None
        if(len(points)>0):
            self.kPoints= [points[0]]
            pointName= layers[0][0]
            objProperties= layers[0][1]
            self.propertyDict[pointName]= objProperties
            indexDict= dict()
            indexDict[0]= pointName
            for p, l in zip(points, layers):
                pointName= l[0]
                objProperties= l[1]
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
