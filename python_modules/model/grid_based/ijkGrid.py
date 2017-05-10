# -*- coding: utf-8 -*-

'''Particular geometry organization, linked to a framework of spaced points, 
usually referenced by their indices (i,j,k) that point to the global 
coordinates (x,y,z).

Generally, an ijkGrid contains the grid of points itself and all the 
geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
those entities, identified by specific names, are created, in order to 
facilitate and accelerate their creation, search, grouping in sets, ...

Several grids can coexist in a FE problem.
'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es" 

import xc_base
import geom
import xc


class IJKRange(object):
  '''Range of indexes (i,j,k) in the 3D-grid that defines a region 
  bounded by the coordinates associated with those indexes.
  
  :ivar ijkMin: tuple (or list) with minimum value of indexes 
        (minIindex,minJindex,minKindex) that point to (xmin,ymin,zmin)
  :ivar ijkMax: tuple (or list) with maximum value of indexes 
        (maxIindex,maxJindex,maxKindex) that point to (xmax,ymax,zmax)
  '''
  def __init__(self,ijkMin,ijkMax):
    if type(ijkMin)==list:
      self.ijkMin=tuple(ijkMin)
    else:
      self.ijkMin= ijkMin
    if type(ijkMax)==list:
      self.ijkMax=tuple(ijkMax)
    else:
      self.ijkMax= ijkMax
  def getIMin(self):
    ''':returns: the value of the index I minimum in the range '''
    return self.ijkMin[0]
  def getIMax(self):
    ''':returns: the value of the index I maximum in the range '''
    return self.ijkMax[0]
  def getJMin(self):
    ''':returns: the value of the index J minimum in the range '''
    return self.ijkMin[1]
  def getJMax(self):
    ''':returns: the value of the index J maximum in the range '''
    return self.ijkMax[1]
  def getKMin(self):
    ''':returns: the value of the index K minimum in the range '''
    return self.ijkMin[2]
  def getKMax(self):
    ''':returns: the value of the index K maximum in the range '''
    return self.ijkMax[2]
  def getRange(self,index):
    ''':returns: a list with the range of indexes between minIindex and maxIindex '''
    mn= self.ijkMin[index]
    mx= self.ijkMax[index]+1 #Last index included
    return range(mn,mx)
  def getIRange(self):
    ''':returns: a list with the range of indexes between minIindex and maxIindex '''
    return self.getRange(0)
  def getJRange(self):
    ''':returns: a list with the range of index between minJindex and maxJindex '''
    return self.getRange(1)

  def getKRange(self):
    ''':returns: a list with the range of indexes between minKindex and maxKindex '''
    return self.getRange(2)

  def __str__(self):
    return 'IRange: ' + str(self.getIRange()) + ' JRange: ' + str(self.getJRange()) + ' KRange: ' + str(self.getKRange())


def getLin2Pts(lstLinBusq,tPto1,tPto2):
    ''':returns: the line that belongs to the set `lstLinBusq` and whose
    starting and ending points are those of tags `tPto1` and `tPto2`
    '''
    broke_out= False
    for l in lstLinBusq:
        extr= l.getKPoints()
        if (extr[0]== tPto1 and extr[1]== tPto2) or (extr[0]== tPto2 and extr[1]== tPto1):
            broke_out= True
            break
    if not broke_out:
#        print "Didn't find the line"
        return
    else:
        return l

class moveRange(object):
  '''Applies a displacement to a range of grid points
  
  :ivar range: range of indexes in the grid 
           e.g.: grid.IJKRange((minI,minJ,minK),(maxI,maxJ,maxK))
  :ivar vDisp: list of displacements in global X, Y, Z directions
           e.g. [dispX,dispY,dispZ]
  '''
  def __init__(self,range,vDisp):
    self.range= range
    self.vDisp= vDisp

class ijkGrid(object):
  '''Particular geometry organization, linked to a framework of spaced points, 
  usually referenced by their indices (i,j,k) that point to the global 
  coordinates (x,y,z).

  Generally, an ijkGrid contains the grid of points itself and all the 
  geometric entities (lines, surfaces, ...) attached to it. Dictionaries with 
  those entities, identified by specific names, are created, in order to 
  facilitate and accelerate their creation, search, grouping in sets, ...

  Several grids can coexist in a FE problem.

  :ivar prep: preprocessor
  :ivar dicLin: dictionary with all the lines linked to the grid. 
        Each key is the name of a line, the value associated is the line 
        itself.
  :ivar dicQuadSurf: dictionary with all the surfaces linked to the grid. 
        Each key is the name of a surface, the value associated is the surface 
        itself.

  '''
  def __init__(self,prep,xList,yList,zList):
    self.gridCoo= list()
    self.gridCoo.append(xList)
    self.gridCoo.append(yList)
    self.gridCoo.append(zList)
    self.rangesToMove= list()
    self.prep= prep
    self.indices= self.prep.getCad.get3DNets.new3DNet()
    self.indices.dim(len(self.gridCoo[0]),len(self.gridCoo[1]),len(self.gridCoo[2]))
    self.pointCounter= 1
    self.prep.getCad.getSurfaces.defaultTag= 1
    self.dicQuadSurf=dict()
    self.dicLin=dict()
    
  def lastXIndex(self):
    return len(self.gridCoo[0])-1
  def lastYIndex(self):
    return len(self.gridCoo[1])-1
  def lastZIndex(self):
    return len(self.gridCoo[2])-1

  def getTagPntGrid(self,indPnt):
    '''return the tag of the point at indPnt=(i,j,k) index of the grid

    :param indPnt: grid indices that point to the global (X, Y, Z)  coordinates    '''
    tagPto= self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1).tag
    return tagPto

  def gridSurfName(self,pt1,pt2,pt3,pt4):
    return 's'+'%04.0f' % pt1 +'%04.0f' % pt2 +'%04.0f' % pt3 +'%04.0f' % pt4

  def gridLinName(self,pt1,pt2):
    return 'l'+'%04.0f' % pt1 +'%04.0f' % pt2
  
  def generatePoints(self):
    '''Point generation.'''
    points= self.prep.getCad.getPoints
    k= 1;
    for z in self.gridCoo[2]:
      j= 1
      for y in self.gridCoo[1]:
        i= 1
        for x in self.gridCoo[0]:
          pt= points.newPntIDPos3d(self.pointCounter,geom.Pos3d(x,y,z))
          self.indices.setPnt(i,j,k,pt.tag)
          self.pointCounter+=1; i+=1
        j+= 1
      k+= 1
    for rm in self.rangesToMove:
      r= rm.range
      vDisp= rm.vDisp
      for i in r.getIRange():
        for j in r.getJRange():
          for k in r.getKRange():
            tagp= self.getTagPntGrid(indPnt=(i,j,k))
            pt= points.get(tagp)
            pt.getPos.x+= vDisp[0]
            pt.getPos.y+= vDisp[1]
            pt.getPos.z+= vDisp[2]
             

  def newQuadGridSurface(self,ind4Pnt):
    '''Generate the quadrangle surface defined by the 4 vertex whose indices 
    in the grid are passed as parameters. 

    :param ind4Pnt: tuple of ordered points defined by their grid indices (i,j,k)
    :returns: the quadrangle surface
    '''
    (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
    surfaces= self.prep.getCad.getSurfaces
    qs= surfaces.newQuadSurfacePts(pto1,pto2,pto3,pto4)
    qs.name= self.gridSurfName(pto1,pto2,pto3,pto4)
    qs.nDivI=1 #initialization values of number of divisions
    qs.nDivJ=1 
    return qs

  def getNameQuadGridSurface(self,ind4Pnt):
    '''Return the name of the quadrangle surface defined by the 4 vertices 
    whose indices in the grid are passed as parameters. 

    :param ind4Pnt: tuple of ordered points defined by their grid indices 
                    (i,j,k)
    :returns: the quadrangle surface
    '''
    (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
    nameSurf= self.gridSurfName(pto1,pto2,pto3,pto4)
    return nameSurf

  def newGridLine(self,ind2Pnt):
    '''Generate the line defined by the 2 end-points whose indices 
    in the grid are passed as parameters. 

    :param ind2Pnt: tuple of ordered points defined by their grid indices 
                    (i,j,k)
    :returns: the line
    '''
    (pto1,pto2)=tuple([self.getTagPntGrid(ind2Pnt[i]) for i in range(2)])
    lines= self.prep.getCad.getLines
    ln= lines.newLine(pto1,pto2)
    ln.name= self.gridLinName(pto1,pto2)
    ln.nDiv=1 #initialization value
    return ln


  def generateSurfaces(self,ijkRange):
    '''generate the surfaces limited by a rectangle defined by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    Add those surfaces to the dictionary dicQuadSurf.
    '''
    retval= list()
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    (i,j,k)=(imin,jmin,kmin)  #initial values
    if kmax== kmin:
        'surfaces in XY plane'
        for i in range(imin,imax):
            for j in range(jmin,jmax):
                indPtsQs=((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k))
                a= self.newQuadGridSurface(indPtsQs)
                retval.append(a)
                self.dicQuadSurf[a.name]= a
    elif jmax== jmin:
        'surfaces in XZ plane'
        for i in range(imin,imax):
            for k in range(kmin,kmax):
                indPtsQs=((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k))
                a= self.newQuadGridSurface(indPtsQs)
                retval.append(a)
                self.dicQuadSurf[a.name]= a
    elif imax== imin:
        'surfaces in YZ plane'
        for j in range(jmin,jmax):
            for k in range(kmin,kmax):
                indPtsQs=((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1))
                a= self.newQuadGridSurface(indPtsQs)
                retval.append(a)
                self.dicQuadSurf[a.name]= a
    return retval

  def generateLines(self,ijkRange): 
    '''generates the lines in an axis parallel to one of the global axes. The function also adds
    the generated lines to the dictionary `dicLin={line_id: xc_line, ...}`

    :param ijkRange: range where lines are included (only one coordinate varies)
    '''
    retval= list()
    lines= self.prep.getCad.getLines
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    (i,j,k)=(imin,jmin,kmin)  #initial values
    if jmax==jmin and kmax==kmin :
        'line parallel to X-axis'
        for i in range(imin,imax):
            indPntsLn=((i,j,k),(i+1,j,k))
            l=self.newGridLine(indPntsLn)
            retval.append(l)
            self.dicLin[l.name]=l
    elif imax==imin and kmax==kmin :
        'line parallel to Y-axis'
        for j in range(jmin,jmax):
            indPntsLn=((i,j,k),(i,j+1,k))
            l=self.newGridLine(indPntsLn)
            retval.append(l)
            self.dicLin[l.name]=l
    elif imax==imin and jmax==jmin :
        'line parallel to Z-axis'
        for k in range(kmin,kmax):
            indPntsLn=((i,j,k),(i,j,k+1))
            l=self.newGridLine(indPntsLn)
            retval.append(l)
            self.dicLin[l.name]=l
    return retval

  def getSetInRange(self,ijkRange,nmbrSet):
    '''return the set of entities (surfaces and all lines, points, elements, 
    nodes, ... associated with them) in a region limited by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    retval= self.prep.getSets.defSet(nmbrSet)
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    (i,j,k)=(imin,jmin,kmin)
    if kmax== kmin:
        'surfaces in XY plane'
        for i in range(imin,imax):
            for j in range(jmin,jmax):
                indPtsQs=((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k))
                nameSurf= self.getNameQuadGridSurface(indPtsQs)
                if nameSurf in self.dicQuadSurf:
                    retval.getSurfaces.append(self.dicQuadSurf[nameSurf])
    elif jmax== jmin:
        'surfaces in XZ plane'
        for i in range(imin,imax):
            for k in range(kmin,kmax):
                indPtsQs=((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k))
                nameSurf= self.getNameQuadGridSurface(indPtsQs)
                if nameSurf in self.dicQuadSurf:
                    retval.getSurfaces.append(self.dicQuadSurf[nameSurf])
    elif imax== imin:
        'surfaces in YZ plane'
        for j in range(jmin,jmax):
            for k in range(kmin,kmax):
                indPtsQs=((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1))
                nameSurf= self.getNameQuadGridSurface(indPtsQs)
                if nameSurf in self.dicQuadSurf:
                    retval.getSurfaces.append(self.dicQuadSurf[nameSurf])
    retval.fillDownwards()    
    return retval

  def getLstLinRange(self,ijkRange):
    ''':returns: a list of the lines inside a range of index 

    
    '''
    setLinBusq= self.prep.getSets["total"].getLines
    lstLinBusq= setLinBusq

    retval=[]
    
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax

    'Lines parallel to X axis'
    for k in range(kmin,kmax+1):
        for j in range(jmin,jmax+1):
            for i in range(imin,imax):
                tagPto1= self.getTagPntGrid(i,j,k)
                tagPto2= self.getTagPntGrid(i+1,j,k)
                l= getLin2Pts(lstLinBusq,tagPto1,tagPto2)
                if l<> None:
                    retval.append(l)
    'Lines parallel to Y axis'
    for k in range(kmin,kmax+1):
        for i in range(imin,imax+1):
            for j in range(jmin,jmax):
                tagPto1= self.getTagPntGrid(i,j,k)
                tagPto2= self.getTagPntGrid(i,j+1,k)
                l= getLin2Pts(lstLinBusq,tagPto1,tagPto2)
                if l<> None:
                    retval.append(l)
    'Lines parallel to Z axis'
    for j in range(jmin,jmax+1):
        for i in range(imin,imax+1):
            for k in range(kmin,kmax):
                tagPto1= self.getTagPntGrid(i,j,k)
                tagPto2= self.getTagPntGrid(i,j,k+1)
                l= getLin2Pts(lstLinBusq,tagPto1,tagPto2)
                if l<> None:
                    retval.append(l)
    return retval


  def getSetPntRange(self,ijkRange,setName):
    '''return the set of points in a 3D grid-region limited by 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax)
    
    :param ijkRange: range for the search
    :param dicPnt: dictionary of points
    :param setName: name of the new set of points
    '''
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    lstTagPnt=[elf.getTagPntGrid(indPnt=(i,j,k)) for i in range(imin,imax+1) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)] #list of point tags to include in the set
    points= self.prep.getCad.getPoints
    retval= self.prep.getSets.defSet(setName)
    pntsSet=retval.getPoints
    for tg in lstTagPnt:
      pnt=points.get(tg)
      pntsSet.append(pnt)
    return retval
 
  def appendLoadInRangeToCurrentLoadPattern(self,ijkRange,nmbrSet,loadVector):
    s= self.getSetInRange(ijkRange,nmbrSet)
    sElem=s.getElements
    for e in sElem:
      #print e.tag
      e.vector3dUniformLoadGlobal(loadVector)

  def appendLoadBeamsInRangeToCurrentLoadPattern(self,ijkRange,loadVector,refSystem):
    lstLinRg=self.getLstLinRange(ijkRange)
    for l in lstLinRg:
      lElem=l.getElements()
      for e in lElem:
        if refSystem=='Local':
          e.vector3dUniformLoadLocal(loadVector)
        else:
          e.vector3dUniformLoadGlobal(loadVector)
        

  def appendEarthPressureToCurrentLoadPattern(self,ijkRange,nmbrSet,earthPressLoadressure):
    s= self.getSetInRange(ijkRange,nmbrSet)
    sElem=s.getElements
    for e in sElem:
      zElem=e.getCooCentroid(False)[2]
      presElem= earthPressLoadressure.getPressure(zElem) 
      if(presElem!=0.0):
        #print K, zT, zA, gammaT, gammaA,zElem, presElem
        vectorCarga=presElem*xc.Vector(earthPressLoadressure.vDir)
        e.vector3dUniformLoadGlobal(vectorCarga)
        

def setEntLstSurf(preprocessor,lstSurf,nmbrSet):
  ''':returns: the set of entities associated with the surfaces in the list `lstSurf`
  '''
  retval= preprocessor.getSets.defSet(nmbrSet)
  for s in lstSurf:
    retval.getSurfaces.append(s)
  retval.fillDownwards()
  return retval



#Funciones geométricas simples

def lstNodesPLine(setBusq,lstPtsPLine):
    ''':returns: the subset of nodes from the set `setBusq` that belong to the line defined by the successive points in list `lstPts`
    '''
    nodAux= setBusq.getNodes
    retval= list() 
    for i in range(0,len(lstPtsPLine)-1):
        segmAux= geom.LineSegment3d(lstPtsPLine[i].getPos,lstPtsPLine[i+1].getPos)
        for n in nodAux:
            p= n.getInitialPos3d
            d= p.distSegmento3d(segmAux)
            if(d<0.01):
                retval.append(n)
    retval= list(set(retval))       #elimina nudos repetidos
    return retval

def setLin2lstLin(setLin):
    ''':returns: a list with the lines included in the set `setLin` '''
    retval= list()
    linAux= setLin.getLines
    for l in linAux:
        retval.append(l)
    return retval
    
def setPnt2lstPnt(setPnt):
    ''':returns: a list with the points included in set `setPto` '''
    retval= list()
    pntAux= setPnt.getPoints
    for p in pntAux:
        retval.append(p)
    return retval
 
def setSurf2lstSurf(setSurf):
    ''':returns: a list with the surfaces included in the set `setSurf`'''
    retval= list()
    surfAux= setSurf.getSurfaces
    for s in surfAux:
        retval.append(s)
    return retval

def setNod2lstNod(setNod):
    ''':returns: a list with the nodes included in the set `setNod` '''
    retval= list()
    nodAux= setNod.getNodes
    for n in nodAux:
        retval.append(n)
    return retval

def setElem2lstElem(setElem):
    ''':returns: a list with the elements included in the set `setElem` '''
    retval= list()
    elemAux= setElem.getElements
    for n in elemAux:
        retval.append(n)
    return retval
   
def lstUnionSetsSurf(setSurf1,setSurf2):
    ''':returns: a list with the tags of the surfaces inlcuded in the union of sets `setSurf1` and `setSurf2` '''
    retval= list()
    for s in setSurf1.getSufaces:
        retval.append(s.tag)
    for s in setSurf2.getSufaces:
        retval.append(s.tag)
    retval= list(set(retval))       #elimina tags repetidos
    return retval


