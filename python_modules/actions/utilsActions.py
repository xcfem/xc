# Devuelve una lista con los nombres de los patrones de carga
def listaNombresLoadPatterns(preprocessor):
  ''':returns: list of load pattern names

  :param preprocessor: preprocessor name
  '''
  return preprocessor.loadLoader.getLoadPatterns.getKeys()


def staggeredPatternType1(lIndX,lIndY,indZ):
    ''':returns: a list of grid ranges that delimit a set of staggered 
    patterned surfaces, i.e. in a floor of a building, from the following parameters:

    :parameter lIndX: list of X-index that point to the X-coordinates of the strips edges.
    :parameter lIndY: list of Y-index that point to the Y-coordinates of the strips edges.
    :parameter indZ: Z-index that points to the Z-coordinate of all the surfaces

    The first vertex considered is (lIndX[0],lIndY[0],indZ)
    '''
    indSupPatt=[]
    for i in range(0,len(lIndX)-1,2):
        for j in range(0,len(lIndY)-1,2):
            inds=ijkGrid.IJKRange([lIndX[i],lIndY[j],indZ],[lIndX[i+1],lIndY[j+1],indZ])
            indSupPatt.append(inds)
    for i in range(1,len(lIndX)-1,2):
        for j in range(1,len(lIndY)-1,2):
            inds=ijkGrid.IJKRange([lIndX[i],lIndY[j],indZ],[lIndX[i+1],lIndY[j+1],indZ])
            indSupPatt.append(inds)
    return indSupPatt
def staggeredPatternType2(lIndX,lIndY,indZ):
    ''':returns: a list of grid ranges that delimit a set of staggered 
    patterned surfaces, i.e. in a floor of a buildin from the following parameters:

    :parameter lIndX: list of X-index that point to the X-coordinates of the strips edges.
    :parameter lIndY: list of Y-index that point to the Y-coordinates of the strips edges.
    :parameter indZ: Z-index that points to the Z-coordinate of all the surfaces

    The first vertex considered is (lIndX[0],lIndY[1],indZ)
    '''
    indSupPatt=[]
    for i in range(0,len(lIndX)-1,2):
        for j in range(1,len(lIndY)-1,2):
            inds=ijkGrid.IJKRange([lIndX[i],lIndY[j],indZ],[lIndX[i+1],lIndY[j+1],indZ])
            indSupPatt.append(inds)
    for i in range(1,len(lIndX)-1,2):
        for j in range(0,len(lIndY)-1,2):
            inds=ijkGrid.IJKRange([lIndX[i],lIndY[j],indZ],[lIndX[i+1],lIndY[j+1],indZ])
            indSupPatt.append(inds)
    return indSupPatt

