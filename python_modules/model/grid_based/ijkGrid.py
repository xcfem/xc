# -*- coding: utf-8 -*-


'''ijkGrid.py: model generation based on a grid of 3D positions.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es" 

import xc_base
import geom
import xc

def getSupName(pt1,pt2,pt3,pt4):
  return 's'+'%04.0f' % pt1 +'%04.0f' % pt2 +'%04.0f' % pt3 +'%04.0f' % pt4

class IJKRange(object):
  '''Defines a range of indexes i,j,k in the grid to be used select the region bounded by the coordinates associated with those indexes.
  
  :ivar ijkMin: list with minimum value of indexes [minIindex,minJindex,minKindex]
  :ivar ijkMax: list with maximum value of indexes [maxIindex,maxJindex,maxKindex]
  '''
  def __init__(self,ijkMin,ijkMax):
    self.ijkMin= ijkMin
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
           e.g.: grid.IJKRange([minI,minJ,minK],[maxI,maxJ,maxK])
  :ivar vDisp: list of displacements in global X, Y, Z directions
           e.g. [dispX,dispY,dispZ]
  '''
  def __init__(self,range,vDisp):
    self.range= range
    self.vDisp= vDisp

class ijkGrid(object):
  ''' Possible (or allowed) X,Y,Z positions for the model''' 
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

  def lastXIndex(self):
    return len(self.gridCoo[0])-1
  def lastYIndex(self):
    return len(self.gridCoo[1])-1
  def lastZIndex(self):
    return len(self.gridCoo[2])-1

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
             


  def getTagPntGrid(self,indPnt):
    ''':returns: the tag of the point at indPnt=(i,j,k) index of the grid

    :param indPnt: grid indices that point to the global (X, Y, Z)  coordinates    
    '''
    tagPto= self.indices.getPnt(indPnt[0]+1,indPnt[1]+1,indPnt[2]+1).tag
    return tagPto

  def newQuadGridSurface(self,ind4Pnt):
    '''Generate the quadrangle surface defined by the 4 vertex whose indices 
    in the grid are passed as parameters. 

    :param ind4Pnt: tuple of ordered points defined by their grid indices (i,j,k)
    :returns: the quadrangle surface
    '''
    (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
    surfaces= self.prep.getCad.getSurfaces
    qs= surfaces.newQuadSurfacePts(pto1,pto2,pto3,pto4)
    qs.name= getSupName(pto1,pto2,pto3,pto4)
    qs.nDivI=1 #initialization values of number of divisions
    qs.nDivJ=1 
    return qs

  def getNameQuadGridSurface(self,ind4Pnt):
    '''Return the name of the quadrangle surface defined by the 4 vertex 
    whose indices in the grid are passed as parameters. 

    :param ind4Pnt: tuple of ordered points defined by their grid indices (i,j,k)
    :returns: the quadrangle surface
    '''
    (pto1,pto2,pto3,pto4)=tuple([self.getTagPntGrid(ind4Pnt[i]) for i in range(4)])
    nameSurf= getSupName(pto1,pto2,pto3,pto4)
    return nameSurf
      

  def generateAreas(self,ijkRange,dicQuadSurf):
    'generates the surfaces contained in a rectangle defined by the coordinates'
    'that correspond to the positions in the grid ijkRange.ijkMin=[posXmin,posYmin,posZmin] y'
    'ijkRange.ijkMax=[posXmax,posYmax,posZmax]'
    'it fills also the surfaces dictionary.'
    retval= list()
    i= ijkRange.ijkMin[0]
    j= ijkRange.ijkMin[1]
    k= ijkRange.ijkMin[2]
    if ijkRange.ijkMax[2]== ijkRange.ijkMin[2]:
        'surfaces in XY plane'
        while i<= ijkRange.ijkMax[0]-1:
            j= ijkRange.ijkMin[1]
            while j<= ijkRange.ijkMax[1]-1:
                indPtsQs=((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k))
                a= self.newQuadGridSurface(indPtsQs)
                retval.append(a)
                dicQuadSurf[a.name]= a
                j+=1
            i+=1
    elif ijkRange.ijkMax[1]== ijkRange.ijkMin[1]:
        'surfaces in XZ plane'
        while i<= ijkRange.ijkMax[0]-1:
            k= ijkRange.ijkMin[2]
            while k<= ijkRange.ijkMax[2]-1:
                indPtsQs=((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k))
                a= self.newQuadGridSurface(indPtsQs)
                retval.append(a)
                dicQuadSurf[a.name]= a
                k+=1
            i+=1
    elif ijkRange.ijkMax[0]== ijkRange.ijkMin[0]:
        'surfaces in YZ plane'
        while j<= ijkRange.ijkMax[1]-1:
            k= ijkRange.ijkMin[2]
            while k<= ijkRange.ijkMax[2]-1:
                indPtsQs=((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1))
                a= self.newQuadGridSurface(indPtsQs)
                retval.append(a)
                dicQuadSurf[a.name]= a
                k+=1
            j+=1
#    print dicQuadSurf
    return retval

  def generateLines(self,ijkRange,dicLin): 
    '''generates the lines in an axe parallel to one of the global axes. The function also adds
    the generated lines to the dictionary `dicLin={line_id: xc_line, ...}`

    :param ijkRange: range where lines are included (only one coordinate varies)
    :param dicLin: dictionary of lines `{line_id: xc_line, ...}`   
    '''
    retval= list()
    lines= self.prep.getCad.getLines
    i=ijkRange.ijkMin[0]
    j=ijkRange.ijkMin[1]
    k=ijkRange.ijkMin[2]
    if ijkRange.ijkMax[1]==ijkRange.ijkMin[1] and ijkRange.ijkMax[2]==ijkRange.ijkMin[2] :
        'línea paralela al eje X'
        while i<=ijkRange.ijkMax[0]-1:
            pto1=self.getTagPntGrid((i,j,k))
            pto2=self.getTagPntGrid((i+1,j,k))
            l=lines.newLine(pto1,pto2)
            l.nDiv=1     #se inicializa el nº de divisiones a 1 
            retval.append(l)
            nmbrLin='l'+'%04.0f' % pto1 +'%04.0f' % pto2 
            dicLin[nmbrLin]=l
            i+=1
    elif ijkRange.ijkMax[0]==ijkRange.ijkMin[0] and ijkRange.ijkMax[2]==ijkRange.ijkMin[2] :
        'línea paralela al eje Y'
        while j<=ijkRange.ijkMax[1]-1:
            pto1=self.getTagPntGrid((i,j,k))
            pto2=self.getTagPntGrid((i,j+1,k))
            l=lines.newLine(pto1,pto2)
            l.nDiv=1     #se inicializa el nº de divisiones a 1 
            retval.append(l)
            nmbrLin='l'+'%04.0f' % pto1 +'%04.0f' % pto2 
            dicLin[nmbrLin]=l
            j+=1
    elif ijkRange.ijkMax[0]==ijkRange.ijkMin[0] and ijkRange.ijkMax[1]==ijkRange.ijkMin[1] :
        'línea paralela al eje Z'
        while k<=ijkRange.ijkMax[2]-1:
            pto1=self.getTagPntGrid((i,j,k))
            pto2=self.getTagPntGrid((i,j,k+1))
            l=lines.newLine(pto1,pto2)
            l.nDiv=1     #se inicializa el nº de divisiones a 1 
            retval.append(l)
            nmbrLin='l'+'%04.0f' % pto1 +'%04.0f' % pto2 
            dicLin[nmbrLin]=l
            k+=1
#    print dicLin
    return retval

  def getSetInRange(self,ijkRange,dicQuadSurf,nmbrSet):
    'devuelve el set de entidades (surfaced and all entities related to them)'
    'contenidas en un rectángulo comprendido entre las coordenadas'
    'que corresponden a las posiciones en la rejilla ijkRange.ijkMin=[posXmin,posYmin,posZmin] y'
    'ijkRange.ijkMax=[posXmax,posYmax,posZmax]'
    retval= self.prep.getSets.defSet(nmbrSet)
    i= ijkRange.ijkMin[0]
    j= ijkRange.ijkMin[1]
    k= ijkRange.ijkMin[2]
    if ijkRange.ijkMax[2]== ijkRange.ijkMin[2]:
        'surfaces in XY plane'
        while i<= ijkRange.ijkMax[0]-1:
            j= ijkRange.ijkMin[1]
            while j<= ijkRange.ijkMax[1]-1:
                indPtsQs=((i,j,k),(i+1,j,k),(i+1,j+1,k),(i,j+1,k))
                nameSurf= self.getNameQuadGridSurface(indPtsQs)
                if nameSurf in dicQuadSurf:
                    retval.getSurfaces.append(dicQuadSurf[nameSurf])
                j+=1
            i+=1
    elif ijkRange.ijkMax[1]== ijkRange.ijkMin[1]:
        'surfaces in XZ plane'
        while i<= ijkRange.ijkMax[0]-1:
            k= ijkRange.ijkMin[2]
            while k<= ijkRange.ijkMax[2]-1:
                indPtsQs=((i,j,k),(i,j,k+1),(i+1,j,k+1),(i+1,j,k))
                nameSurf= self.getNameQuadGridSurface(indPtsQs)
                if nameSurf in dicQuadSurf:
                    retval.getSurfaces.append(dicQuadSurf[nameSurf])
                k+=1
            i+=1
    elif ijkRange.ijkMax[0]== ijkRange.ijkMin[0]:
        'surfaces in YZ plane'
        while j<= ijkRange.ijkMax[1]-1:
            k= ijkRange.ijkMin[2]
            while k<= ijkRange.ijkMax[2]-1:
                indPtsQs=((i,j,k),(i,j+1,k),(i,j+1,k+1),(i,j,k+1))
                nameSurf= self.getNameQuadGridSurface(indPtsQs)
                if nameSurf in dicQuadSurf:
                    retval.getSurfaces.append(dicQuadSurf[nameSurf])
                k+=1
            j+=1
    retval.fillDownwards()    
    return retval

  def getLstLinRange(self,ijkRange):
    ''':returns: a list of the lines inside a range of index 

    
    '''
    setLinBusq= self.prep.getSets["total"].getLines
    lstLinBusq= setLinBusq

    retval=[]
    
    imin= ijkRange.ijkMin[0]
    jmin= ijkRange.ijkMin[1]
    kmin= ijkRange.ijkMin[2]
    imax= ijkRange.ijkMax[0]
    jmax= ijkRange.ijkMax[1]
    kmax= ijkRange.ijkMax[2]

    'Lines parallels to X axis'
    for k in range(kmin,kmax+1):
        for j in range(jmin,jmax+1):
            for i in range(imin,imax):
                tagPto1= self.getTagPntGrid(i,j,k)
                tagPto2= self.getTagPntGrid(i+1,j,k)
                l= getLin2Pts(lstLinBusq,tagPto1,tagPto2)
                if l<> None:
                    retval.append(l)
    'Lines parallels to Y axis'
    for k in range(kmin,kmax+1):
        for i in range(imin,imax+1):
            for j in range(jmin,jmax):
                tagPto1= self.getTagPntGrid(i,j,k)
                tagPto2= self.getTagPntGrid(i,j+1,k)
                l= getLin2Pts(lstLinBusq,tagPto1,tagPto2)
                if l<> None:
                    retval.append(l)
    'Lines parallels to Z axis'
    for j in range(jmin,jmax+1):
        for i in range(imin,imax+1):
            for k in range(kmin,kmax):
                tagPto1= self.getTagPntGrid(i,j,k)
                tagPto2= self.getTagPntGrid(i,j,k+1)
                l= getLin2Pts(lstLinBusq,tagPto1,tagPto2)
                if l<> None:
                    retval.append(l)
    return retval

  def getSetPtosRange(self,ijkRange,nombre):
    '''devuelve un set de puntos contenidos en un rectángulo comprendido entre las coordenadas
    que corresponden a las posiciones en la rejilla ijkRange.ijkMin=[posXmin,posYmin,posZmin] y
    ijkRange.ijkMax=[posXmax,posYmax,posZmax]'''
    retval= self.prep.getSets.defSet(nombre)
    i= ijkRange.ijkMin[0]
    j= ijkRange.ijkMin[1]
    k= ijkRange.ijkMin[2]
    if ijkRange.ijkMax[2]== ijkRange.ijkMin[2]:
        'surfaces in XY plane'
        while i<= ijkRange.ijkMax[0]:
            j= ijkRange.ijkMin[1]
            while j<= ijkRange.ijkMax[1]:
                tgpto= self.getTagPntGrid(indPnt=(i,j,k))
                pto= puntos.get(tgpto)
                retval.getPoints.append(pto)
                j+=1
            i+=1
    elif ijkRange.ijkMax[1]== ijkRange.ijkMin[1]:
        'surfaces in XZ plane'
        while i<= ijkRange.ijkMax[0]:
            k= ijkRange.ijkMin[2]
            while k<= ijkRange.ijkMax[2]:
                tgpto= self.getTagPntGrid(indPnt=(i,j,k))
                pto= puntos.get(tgpto)
                #print pto.tag,pto.getPos.x,pto.getPos.y,pto.getPos.z
                retval.getPoints.append(pto)
                k+=1
            i+=1
    elif ijkRange.ijkMax[0]== ijkRange.ijkMin[0]:
        'surfaces in YZ plane'
        while j<= ijkRange.ijkMax[1]:
            k= ijkRange.ijkMin[2]
            while k<= ijkRange.ijkMax[2]:
              tgpto= self.getTagPntGrid(indPnt=(i,j,k))
              pto= puntos.get(tgpto)
              #print pto.tag,pto.getPos.x,pto.getPos.y,pto.getPos.z
              retval.getPoints.append(pto)
              k+=1
            j+=1
    return retval

  def appendLoadInRangeToCurrentLoadPattern(self,ijkRange,dicQuadSurf,nmbrSet,loadVector):
    s= self.getSetInRange(ijkRange,dicQuadSurf,nmbrSet)
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
        

  def appendEarthPressureToCurrentLoadPattern(self,ijkRange,dicQuadSurf,nmbrSet,earthPressLoadressure):
    s= self.getSetInRange(ijkRange,dicQuadSurf,nmbrSet)
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


