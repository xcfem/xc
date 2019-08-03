# -*- coding: utf-8 -*-
'''This module is on the way of being replaced by others
'''

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
    '''return the value of the index I minimum in the range '''
    return self.ijkMin[0]
  def getIMax(self):
    '''return the value of the index I maximum in the range '''
    return self.ijkMax[0]
  def getJMin(self):
    '''return the value of the index J minimum in the range '''
    return self.ijkMin[1]
  def getJMax(self):
    '''return the value of the index J maximum in the range '''
    return self.ijkMax[1]
  def getKMin(self):
    '''return the value of the index K minimum in the range '''
    return self.ijkMin[2]
  def getKMax(self):
    '''return the value of the index K maximum in the range '''
    return self.ijkMax[2]
  def getRange(self,index):
    '''return a list with the range of indexes between minIindex and maxIindex '''
    mn= self.ijkMin[index]
    mx= self.ijkMax[index]+1 #Last index included
    return range(mn,mx)
  def getIRange(self):
    '''return a list with the range of indexes between minIindex and maxIindex '''
    return self.getRange(0)
  def getJRange(self):
    '''return a list with the range of index between minJindex and maxJindex '''
    return self.getRange(1)

  def getKRange(self):
    '''return a list with the range of indexes between minKindex and maxKindex '''
    return self.getRange(2)

  def __str__(self):
    return 'IRange: ' + str(self.getIRange()) + ' JRange: ' + str(self.getJRange()) + ' KRange: ' + str(self.getKRange())

  def extractIncludedIJranges(self,step=1):
    '''return a list with all the sub-ranges included in the IJKRange 
    for which the index K is a constant. Graphically it can be seen as
    the set of planes 'parallel' to global XY included in the region defined
    by the IJKRange 

    :ivar step: K step to select the XY planes (defaults to 1)
    '''
    (imin,jmin,kmin)=self.ijkMin
    (imax,jmax,kmax)=self.ijkMax
    lstIJrang=[IJKRange((imin,jmin,k),(imax,jmax,k)) for k in range(kmin,kmax+1,step)]
    return lstIJrang

  def extractIncludedIKranges(self,step=1):
    '''return a list with all the sub-ranges included in the IJKRange 
    for which the index J is a constant. Graphically it can be seen as
    the set of planes 'parallel' to global XZ included in the region defined
    by the IJKRange 

    :param step: J step to select the XZ planes (defaults to 1)
    '''
    (imin,jmin,kmin)=self.ijkMin
    (imax,jmax,kmax)=self.ijkMax
    lstIKrang=[IJKRange((imin,j,kmin),(imax,j,kmax)) for j in range(jmin,jmax+1,step)]
    return lstIKrang

  def extractIncludedJKranges(self,step=1):
    '''return a list with all the sub-ranges included in the IJKRange 
    for which the index I is a constant. Graphically it can be seen as
    the set of planes 'parallel' to global YZ included in the region defined
    by the IJKRange 

    :param step: I step to select the XZ planes (defaults to 1)
    '''
    (imin,jmin,kmin)=self.ijkMin
    (imax,jmax,kmax)=self.ijkMax
    lstJKrang=[IJKRange((i,jmin,kmin),(i,jmax,kmax)) for i in range(imin,imax+1,step)]
    return lstJKrang

  def extractIncludedIranges(self,stepJ=1,stepK=1):
    '''return a list with all the sub-ranges included in the IJKRange 
    for which the indices J and K are a constant. Graphically it can be seen as
    the set of lines 'parallel' to global X axis included in the region defined
    by the IJKRange 

    :param stepJ: step to select the lines in Y direction (defaults to 1)
    :param stepK step to select the lines in Z direction (defaults to 1)
    '''
    (imin,jmin,kmin)=self.ijkMin
    (imax,jmax,kmax)=self.ijkMax
    lstIrang=[IJKRange((imin,j,k),(imax,j,k)) for j in range(jmin,jmax+1,stepJ) for k in range(kmin,kmax+1,stepK)]
    return lstIrang

  def extractIncludedJranges(self,stepI=1,stepK=1):
    '''return a list with all the sub-ranges included in the IJKRange 
    for which the indices I and K are a constant. Graphically it can be seen as
    the set of lines 'parallel' to global Y axis included in the region defined
    by the IJKRange 

    :param stepI: step to select the lines in X direction (defaults to 1)
    :param stepK step to select the lines in Z direction (defaults to 1)
    '''
    (imin,jmin,kmin)=self.ijkMin
    (imax,jmax,kmax)=self.ijkMax
    lstJrang=[IJKRange((i,jmin,k),(i,jmax,k)) for i in range(imin,imax+1,stepI) for k in range(kmin,kmax+1,stepK)]
    return lstJrang

  def extractIncludedKranges(self,stepI=1,stepJ=1):
    '''return a list with all the sub-ranges included in the IJKRange 
    for which the indices I and J are a constant. Graphically it can be seen as
    the set of lines 'parallel' to global Z axis included in the region defined
    by the IJKRange 

    :param stepI: step to select the lines in X direction (defaults to 1)
    :param stepJ step to select the lines in Y direction (defaults to 1)
    '''
    (imin,jmin,kmin)=self.ijkMin
    (imax,jmax,kmax)=self.ijkMax
    lstKrang=[IJKRange((i,j,kmin),(i,j,kmax)) for i in range(imin,imax+1,stepI) for j in range(jmin,jmax+1,stepJ) ]
    return lstKrang


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
    self.indices= self.prep.getMultiBlockTopology.get3DNets.new3DNet()
    self.indices.dim(len(self.gridCoo[0]),len(self.gridCoo[1]),len(self.gridCoo[2]))
    self.pointCounter=0
    self.dicQuadSurf=dict()
    self.dicLin=dict()
    
  def lastXIndex(self):
    return len(self.gridCoo[0])-1
  def lastYIndex(self):
    return len(self.gridCoo[1])-1
  def lastZIndex(self):
    return len(self.gridCoo[2])-1

  def getTagPntGrid(self,indPnt):
    '''Return the tag of the point at indPnt=(i,j,k) index of the grid.

    :param indPnt: grid indices that point to the global (X, Y, Z)  coordinates    '''
    tagPto= self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1).tag
    return tagPto

  def gridSurfName(self,pt1,pt2,pt3,pt4):
    '''Return the name of the quadrangle surface defined by 4 points.

    :param pt1,pt2,pt3,pt4: tags of the points (in right order) that define the 
           surface 
    '''
    return 's'+'%04.0f' % pt1 +'%04.0f' % pt2 +'%04.0f' % pt3 +'%04.0f' % pt4


  def gridLinName(self,pt1,pt2):
    '''Return the name of the line defined by 2 points

    :param pt1,pt2: tags of the points (in right order) that define the line

    '''
    return 'l'+'%04.0f' % pt1 +'%04.0f' % pt2
  
  def getNmSurfInRange(self,ijkRange):
    '''Return a list with the names of the surfaces limited by a volume 
    defined by the coordinates that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    'surfaces in XY plane'
    indPtSurfXY=[((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k)) for j in range(jmin,jmax) for i in range(imin,imax) for k in range (kmin,kmax+1)]
    nmSurfXY=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXY]
    'surfaces in XZ plane'
    indPtSurfXZ=[((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k)) for k in range(kmin,kmax) for i in range(imin,imax) for j in range(jmin,jmax+1)]
    nmSurfXZ=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfXZ]
    'surfaces in YZ plane'
    indPtSurfYZ=[((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1)) for k in range(kmin,kmax) for j in range(jmin,jmax) for i in range(imin,imax+1)]
    nmSurfYZ=[self.getNameQuadGridSurface(indPtsQs) for indPtsQs in indPtSurfYZ]
    return (nmSurfXY+nmSurfXZ+nmSurfYZ)
                
  def getNmLinInRange(self,ijkRange):
    '''Return a list with the names of the surfaces limited by a volume 
    defined by the coordinates that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    'lines parallel to X-axis'
    indPtLinX=[((i,j,k),(i+1,j,k)) for  i in range(imin,imax) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)]
    nmLinX=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinX]
    'lines parallel to Y-axis'
    indPtLinY=[((i,j,k),(i,j+1,k)) for j in range(jmin,jmax)  for  i in range(imin,imax+1) for k in range(kmin,kmax+1)]
    nmLinY=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinY]
    'lines parallel to Z-axis'
    indPtLinZ=[((i,j,k),(i,j,k+1)) for  k in range(kmin,kmax) for i in range(imin,imax+1) for j in range(jmin,jmax+1)]
    nmLinZ=[self.getNameGridLine(indPtsLn) for indPtsLn in indPtLinZ]
    return (nmLinX+nmLinY+nmLinZ)
    
  def generatePoints(self):
    '''Point generation.'''
    points= self.prep.getMultiBlockTopology.getPoints
    inicTag=points.defaultTag
    lstPt=[(i+1,j+1,k+1,self.gridCoo[0][i],self.gridCoo[1][j],self.gridCoo[2][k]) for i in range(len(self.gridCoo[0])) for j in range(len(self.gridCoo[1])) for k in range(len(self.gridCoo[2])) ]
    for p in lstPt:
      (i,j,k,x,y,z)=p
      self.pointCounter+=1
      pnt=points.newPntIDPos3d(self.pointCounter+inicTag,geom.Pos3d(x,y,z))
      self.indices.setPnt(i,j,k,pnt.tag)
    for rm in self.rangesToMove:
      r= rm.range
      vDisp= rm.vDisp
      for i in r.getIRange():
        for j in r.getJRange():
          for k in r.getKRange():
            tagp= self.getTagPntGrid(indPnt=(i,j,k))
            pnt= points.get(tagp)
            pnt.getPos.x+= vDisp[0]
            pnt.getPos.y+= vDisp[1]
            pnt.getPos.z+= vDisp[2]
             

  def newQuadGridSurface(self,surfName):
    '''Generate the quadrangle surface defined by the 4 vertex whose tags
    are implicit in the name of the surface.

    :param surfName: name given to the grid surface
    return the quadrangle surface
    '''
    points= self.prep.getMultiBlockTopology.getPoints
    (tgPt1,tgPt2,tgPt3,tgPt4)=(int(surfName[1:5]),int(surfName[5:9]),int(surfName[9:13]),int(surfName[13:17]))
    surfaces= self.prep.getMultiBlockTopology.getSurfaces
    qs= surfaces.newQuadSurfacePts(tgPt1,tgPt2,tgPt3,tgPt4)
    qs.name= surfName
    qs.nDivI=1 #initialization values of number of divisions
    qs.nDivJ=1 
    return qs

  def getNameQuadGridSurface(self,ind4Pnt):
    '''Return the name of the quadrangle surface defined by the 4 vertices 
    whose indices in the grid are passed as parameters. 

    :param ind4Pnt: tuple of ordered points defined by their grid indices 
                    (i,j,k)
    return the quadrangle surface
    '''
    (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
    nameSurf= self.gridSurfName(pto1,pto2,pto3,pto4)
    return nameSurf

  def getNameGridLine(self,ind2Pnt):
    '''Return the name of the line defined by the 2 vertices 
    whose indices in the grid are passed as parameters. 

    :param ind2Pnt: tuple of ordered points defined by their grid indices 
                    (i,j,k)
    return the line
    '''
    (pto1,pto2)=tuple([self.getTagPntGrid(ind2Pnt[i]) for i in range(2)])
    nameLin= self.gridLinName(pto1,pto2)
    return nameLin

  def newGridLine(self,linName):
    '''Generate the line defined by the 2 end-points whose tags
    are implicit in the name of the line.

    :param linName: name given to the grid line
    return the line
    '''
    points= self.prep.getMultiBlockTopology.getPoints
    (tgPt1,tgPt2)=(int(linName[1:5]),int(linName[5:9]))
    lines= self.prep.getMultiBlockTopology.getLines
    ln= lines.newLine(tgPt1,tgPt2)
    ln.name= linName
    ln.nDiv=1 #initialization value
    return ln


  def genSurfOneRegion(self,ijkRange):
    '''generate the surfaces limited by a region defined by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    Add those surfaces to the dictionary dicQuadSurf.
    '''
    retval= list()   #*eliminate this variable
    nmSurfinRang=self.getNmSurfInRange(ijkRange)
    for nameSurf in nmSurfinRang:
      s= self.newQuadGridSurface(nameSurf)
      self.dicQuadSurf[nameSurf]=s
      retval.append(s)
    return retval

  def genSurfMultiRegion(self,lstIJKRange):
    '''generate the surfaces limited by all the regions included in the 
    list of ijkRanges passed as parameter.
    Each region defines a volume limited by the coordinates    
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    Add those surfaces to the dictionary dicQuadSurf.
    '''
    retval= list()   #*eliminate this variable
    for rg in lstIJKRange:
      retval.append(self.genSurfOneRegion(rg))
    return retval

  def genLinOneRegion(self,ijkRange): 
    '''generate the lines limited by a region defined by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    Add those lines to the dictionary dicLin.
    '''

    retval= list()  #*eliminate this variable
    nmLinInRang=self.getNmLinInRange(ijkRange)
    for nameLin in nmLinInRang:
      l=self.newGridLine(nameLin)
      self.dicLin[nameLin]=l
      retval.append(l)
    return retval

  def genLinMultiRegion(self,lstIJKRange):
    '''generate the lines limited by all the regions included in the 
    list of ijkRanges passed as parameter.
    Each region defines a volume limited by the coordinates    
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    Add those lines to the dictionary dicLin.
    '''
    retval= list()   #*eliminate this variable
    for rg in lstIJKRange:
      retval.append(self.genLinOneRegion(rg))
    return retval
  
  def getSetSurfOneRegion(self,ijkRange,nmbrSet):
    '''return the set of surfaces and all the entities(lines, points, elements, 
    nodes, ...) associated with them in a region limited by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    setSurf= self.prep.getSets.defSet(nmbrSet)
    nmSurfinRang=self.getNmSurfInRange(ijkRange)
    for nameSurf in nmSurfinRang:
      if nameSurf in self.dicQuadSurf:
        setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
    setSurf.fillDownwards()    
    return setSurf

  def getSetSurfMultiRegion(self,lstIJKRange,nmbrSet):
    '''return the set of surfaces and all the entities(lines, points, elements, 
    nodes, ...) associated with them in a all the regions  included in the 
    list of ijkRanges passed as parameter.
    Each region defines a volume limited by the coordinates    
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    setSurf= self.prep.getSets.defSet(nmbrSet)
    for rg in lstIJKRange:
      nmSurfinRang=self.getNmSurfInRange(rg)
      for nameSurf in nmSurfinRang:
        if nameSurf in self.dicQuadSurf:
          setSurf.getSurfaces.append(self.dicQuadSurf[nameSurf])
    setSurf.fillDownwards()    
    return setSurf

  def getSetLinOneRegion(self,ijkRange,nmbrSet):
    '''return the set of lines and all the entities(points, elements, 
    nodes, ...) associated with them in a region limited by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    setLin= self.prep.getSets.defSet(nmbrSet)
    nmLininRang=self.getNmLinInRange(ijkRange)
    for nameLin in nmLininRang:
      if nameLin in self.dicLin:
        setLin.getLines.append(self.dicLin[nameLin])
    setLin.fillDownwards()    
    return setLin

  def getSetLinMultiRegion(self,lstIJKRange,nmbrSet):
    '''return the set of lines and all the entities(points, elements, 
    nodes, ...) associated with them in a all the regions  included in the 
    list of ijkRanges passed as parameter.
    Each region defines a volume limited by the coordinates    
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    setLin= self.prep.getSets.defSet(nmbrSet)
    for rg in lstIJKRange:
      nmLininRang=self.getNmLinInRange(rg)
      for nameLin in nmLininRang:
        if nameLin in self.dicLin:
          setLin.getLines.append(self.dicLin[nameLin])
    setLin.fillDownwards()    
    return setLin
  
  def getLstLinRange(self,ijkRange):
    '''return a list of lines in a region limited by the coordinates
    that correspond to the indices in the grid 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax). 
    '''
    nmLinInRang=self.getNmLinInRange(ijkRange)
    for nameLin in nmLinInRang:
      if nameLin in self.dicLin:
        retval.append[dicLin[nameLin]]
    return retval
    

  def getSetPntRange(self,ijkRange,setName):
    '''return the set of points in a 3D grid-region limited by 
    ijkRange.ijkMin=(indXmin,indYmin,indZmin) and
    ijkRange.ijkMax=(indXmax,indYmax,indZmax)
    
    :param ijkRange: range for the search
    :param setName: name of the new set of points
    '''
    (imin,jmin,kmin)=ijkRange.ijkMin
    (imax,jmax,kmax)=ijkRange.ijkMax
    lstTagPnt=[elf.getTagPntGrid(indPnt=(i,j,k)) for i in range(imin,imax+1) for j in range(jmin,jmax+1) for k in range(kmin,kmax+1)] #list of point tags to include in the set
    points= self.prep.getMultiBlockTopology.getPoints
    retval= self.prep.getSets.defSet(setName)
    pntsSet=retval.getPoints
    for tg in lstTagPnt:
      pnt=points.get(tg)
      pntsSet.append(pnt)
    return retval
 
  def appendLoadInRangeToCurrentLoadPattern(self,ijkRange,nmbrSet,loadVector):
    s= self.getSetSurfOneRegion(ijkRange,nmbrSet)
    sElem=s.getElements
    for e in sElem:
      e.vector3dUniformLoadGlobal(loadVector)

  def appendLoadBeamsInRangeToCurrentLoadPattern(self,ijkRange,loadVector,refSystem):
    lstLinRg=self.getLstLinRange(ijkRange)
    for l in lstLinRg:
      lElem=l.elements
      for e in lElem:
        if refSystem=='Local':
          e.vector3dUniformLoadLocal(loadVector)
        else:
          e.vector3dUniformLoadGlobal(loadVector)
        

  def appendEarthPressureToCurrentLoadPattern(self,ijkRange,nmbrSet,earthPressLoadressure):
    s= self.getSetSurfOneRegion(ijkRange,nmbrSet)
    sElem=s.getElements
    for e in sElem:
      zElem=e.getCooCentroid(False)[2]
      presElem= earthPressLoadressure.getPressure(zElem) 
      if(presElem!=0.0):
        #print K, zT, zA, gammaT, gammaA,zElem, presElem
        vectorCarga=presElem*xc.Vector(earthPressLoadressure.vDir)
        e.vector3dUniformLoadGlobal(vectorCarga)
        

def setEntLstSurf(preprocessor,lstSurf,nmbrSet):
  '''return the set of entities associated with the surfaces in the list `lstSurf`
  '''
  retval= preprocessor.getSets.defSet(nmbrSet)
  for s in lstSurf:
    retval.getSurfaces.append(s)
  retval.fillDownwards()
  return retval



#Simple geometric functions

def lstNodesPLine(setBusq,lstPtsPLine):
    '''return the subset of nodes from the set `setBusq` that belong to the line defined by the successive points in list `lstPts`
    '''
    nodAux= setBusq.getNodes
    retval= list() 
    for i in range(0,len(lstPtsPLine)-1):
        segmAux= geom.LineSegment3d(lstPtsPLine[i].getPos,lstPtsPLine[i+1].getPos)
        for n in nodAux:
            p= n.getInitialPos3d
            d= p.distSegment3d(segmAux)
            if(d<0.01):
                retval.append(n)
    retval= list(set(retval))       #elimina nudos repetidos
    return retval

def setLin2lstLin(setLin):
    '''return a list with the lines included in the set `setLin` '''
    retval= list()
    linAux= setLin.getLines
    for l in linAux:
        retval.append(l)
    return retval
    
def setPnt2lstPnt(setPnt):
    '''return a list with the points included in set `setPto` '''
    retval= list()
    pntAux= setPnt.getPoints
    for p in pntAux:
        retval.append(p)
    return retval
 
def setSurf2lstSurf(setSurf):
    '''return a list with the surfaces included in the set `setSurf`'''
    retval= list()
    surfAux= setSurf.getSurfaces
    for s in surfAux:
        retval.append(s)
    return retval

def setNod2lstNod(setNod):
    '''return a list with the nodes included in the set `setNod` '''
    retval= list()
    nodAux= setNod.getNodes
    for n in nodAux:
        retval.append(n)
    return retval

def setElem2lstElem(setElem):
    '''return a list with the elements included in the set `setElem` '''
    retval= list()
    elemAux= setElem.getElements
    for n in elemAux:
        retval.append(n)
    return retval
   
def lstUnionSetsSurf(setSurf1,setSurf2):
    '''return a list with the tags of the surfaces inlcuded in the union of sets `setSurf1` and `setSurf2` '''
    retval= list()
    for s in setSurf1.getSufaces:
        retval.append(s.tag)
    for s in setSurf2.getSufaces:
        retval.append(s.tag)
    retval= list(set(retval))       #elimina tags repetidos
    return retval


def getLin2Pts(lstLinBusq,tPto1,tPto2):
    '''return the line that belongs to the set `lstLinBusq` and whose
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
