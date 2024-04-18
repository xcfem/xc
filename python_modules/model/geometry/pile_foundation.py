import geom
import xc
from geotechnics.foundations import guia_cimentaciones_oc as guia
from model.mesh import finit_el_model as fem

def gen_pile_cap_1column_4piles(modelSpace,nodCol,distXpile,distYpile,Hpilecap,nameSetStruts,nameSetTies):
    '''Generate pile cap for one column with a pile foundation of 4 piles. Return the two sets of elements generated (struts and ties)  and the nodes where piles start (topNodPiles)

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodCol: node of the column
    :param distXpile: distance between piles in X direction
    :param distYpile: distance between piles in Y direction
    :param Hpilecap: pile-cap height
    :param nameSetStruts: name of the set of struts
    :param nameSetTies: name of the set of ties
    '''
    
    (xNodCol,yNodCol,zNodCol)=(nodCol.get3dCoo[0],nodCol.get3dCoo[1],nodCol.get3dCoo[2])
    preprocessor=modelSpace.preprocessor
    struts=preprocessor.getSets.defSet(nameSetStruts)
    ties=preprocessor.getSets.defSet(nameSetTies)
    Lx=distXpile/2.
    Ly=distYpile/2.
    nodes=preprocessor.getNodeHandler
    
    nod1= nodes.newNodeXYZ(xNodCol-Lx,yNodCol-Ly,zNodCol-0.85*Hpilecap)
    nod2= nodes.newNodeXYZ(xNodCol+Lx,yNodCol-Ly,zNodCol-0.85*Hpilecap)
    nod3= nodes.newNodeXYZ(xNodCol+Lx,yNodCol+Ly,zNodCol-0.85*Hpilecap)
    nod4= nodes.newNodeXYZ(xNodCol-Lx,yNodCol+Ly,zNodCol-0.85*Hpilecap)

    #Elements
    str1= modelSpace.setHugeTrussBetweenNodes(nodCol,nod1)
    str2= modelSpace.setHugeTrussBetweenNodes(nodCol,nod2)
    str3= modelSpace.setHugeTrussBetweenNodes(nodCol,nod3)
    str4= modelSpace.setHugeTrussBetweenNodes(nodCol,nod4)

    struts.getElements.append(str1)
    struts.getElements.append(str2)
    struts.getElements.append(str3)
    struts.getElements.append(str4)

    tie1= modelSpace.setHugeTrussBetweenNodes(nod1,nod2)
    tie2= modelSpace.setHugeTrussBetweenNodes(nod2,nod3)
    tie3= modelSpace.setHugeTrussBetweenNodes(nod3,nod4)
    tie4= modelSpace.setHugeTrussBetweenNodes(nod4,nod1)

    ties.getElements.append(tie1)
    ties.getElements.append(tie2)
    ties.getElements.append(tie3)
    ties.getElements.append(tie4)

    # Constraints
    modelSpace.fixNode('FFF_000',nodCol.tag)
    '''
    modelSpace.fixNode('F00_000',nod1.tag)
    modelSpace.fixNode('0F0_000',nod2.tag)
    modelSpace.fixNode('F00_000',nod3.tag)
    modelSpace.fixNode('0F0_000',nod4.tag)
    '''
    topNodPiles=[nod1,nod2,nod3,nod4]
    
    struts.fillDownwards()
    ties.fillDownwards()
    return struts,ties,topNodPiles


def gen_pile_cap_Ncolumns_Nx2_piles(modelSpace,nodCols,distXpile,distYpile,Hpilecap,nameSetStruts,nameSetTies):
    '''Generate pile cap for N columns with a pile foundation of N (X direction) by  2 (Y direction) piles. Return the two sets of elements generated (struts and ties)  and the nodes where piles start (topNodPiles)

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodCols: [nodeCol1,nodeCol2, ..., nodeColN] nodes of columns 1, 2, .., N in order of
                    increasing coordinate X.
    :param distXpile: distance between piles in X direction
    :param distYpile: distance between piles in Y direction
    :param Hpilecap: pile-cap height
    :param nameSetStruts: name of the set of struts
    :param nameSetTies: name of the set of ties
    '''
    preprocessor=modelSpace.preprocessor
    nodes=preprocessor.getNodeHandler
    nodCol1=nodCols[0]
    nodCol2=nodCols[-1]
    (xCent,yCent,zCent)=((nodCol1.get3dCoo[0]+nodCol2.get3dCoo[0])/2.,
                         (nodCol1.get3dCoo[1]+nodCol2.get3dCoo[1])/2.,
                         nodCol1.get3dCoo[2])
    struts=preprocessor.getSets.defSet(nameSetStruts)
    ties=preprocessor.getSets.defSet(nameSetTies)
    Ly=distYpile/2.
    nRowsPil=len(nodCols) #number of rows of piles in X direction 
    distXPileExtr=(nRowsPil-1)*distXpile
    nodTopPiles=list()
    for i in range(nRowsPil):
        Lx=-distXPileExtr/2.+i*distXpile
        nodTopPiles.append([nodes.newNodeXYZ(xCent+Lx,yCent-Ly,zCent-0.85*Hpilecap),
                            nodes.newNodeXYZ(xCent+Lx,yCent+Ly,zCent-0.85*Hpilecap)])
    #strut generation
    for i in range(nRowsPil):
        struts.getElements.append(modelSpace.setHugeTrussBetweenNodes(nodCols[i],nodTopPiles[i][0]))
        struts.getElements.append(modelSpace.setHugeTrussBetweenNodes(nodCols[i],nodTopPiles[i][1]))
        ties.getElements.append(modelSpace.setHugeTrussBetweenNodes(nodTopPiles[i][0],nodTopPiles[i][1]))
    for i in range(nRowsPil-1):
        struts.getElements.append(modelSpace.setHugeTrussBetweenNodes(nodCols[i],nodCols[i+1]))
        ties.getElements.append(modelSpace.setHugeTrussBetweenNodes(nodTopPiles[i][0],nodTopPiles[i+1][0]))
        ties.getElements.append(modelSpace.setHugeTrussBetweenNodes(nodTopPiles[i][1],nodTopPiles[i+1][1]))
    # Constraints
    for n in nodCols:
        modelSpace.fixNode('FFF_000',n.tag)
    struts.fillDownwards()
    ties.fillDownwards()
    topNodPiles=[item for sublist in nodTopPiles for item in sublist] #flatten list
    return struts,ties,topNodPiles

def gen_pile_cap_2columns_4piles(modelSpace,nodCols,distXpile,distYpile,Hpilecap,nameSetStruts,nameSetTies):
    '''Generate pile cap for two columns with a pile foundation of 4 piles. Return the two sets of elements generated (struts and ties)  and the nodes where piles start (topNodPiles)

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodCols: [nodeCol1,nodeCol2] nodes of columns 1 and 2 where X 
                    coordinate of node 1 is less than X coord. of node 2
    :param distXpile: distance between piles in X direction
    :param distYpile: distance between piles in Y direction
    :param Hpilecap: pile-cap height
    :param nameSetStruts: name of the set of struts
    :param nameSetTies: name of the set of ties
    '''
    preprocessor=modelSpace.preprocessor
    nodCol1=nodCols[0]
    nodCol2=nodCols[1]
    (xCent,yCent,zCent)=((nodCol1.get3dCoo[0]+nodCol2.get3dCoo[0])/2.,
                         (nodCol1.get3dCoo[1]+nodCol2.get3dCoo[1])/2.,
                         nodCol1.get3dCoo[2])
    
#    (xNodCol,yNodCol,zNodCol)=(nodCol.get3dCoo[0],nodCol.get3dCoo[1],nodCol.get3dCoo[2])
    struts=preprocessor.getSets.defSet(nameSetStruts)
    ties=preprocessor.getSets.defSet(nameSetTies)
    Lx=distXpile/2.
    Ly=distYpile/2.

    nodes=preprocessor.getNodeHandler
    nod1= nodes.newNodeXYZ(xCent-Lx,yCent-Ly,zCent-0.85*Hpilecap)
    nod2= nodes.newNodeXYZ(xCent+Lx,yCent-Ly,zCent-0.85*Hpilecap)
    nod3= nodes.newNodeXYZ(xCent+Lx,yCent+Ly,zCent-0.85*Hpilecap)
    nod4= nodes.newNodeXYZ(xCent-Lx,yCent+Ly,zCent-0.85*Hpilecap)

    #Strut elements
    str1= modelSpace.setHugeTrussBetweenNodes(nodCol1,nod1)
    str2= modelSpace.setHugeTrussBetweenNodes(nodCol2,nod2)
    str3= modelSpace.setHugeTrussBetweenNodes(nodCol2,nod3)
    str4= modelSpace.setHugeTrussBetweenNodes(nodCol1,nod4)
    str5= modelSpace.setHugeTrussBetweenNodes(nodCol1,nodCol2)

    struts.getElements.append(str1)
    struts.getElements.append(str2)
    struts.getElements.append(str3)
    struts.getElements.append(str4)
    struts.getElements.append(str5)

    #Tie elements
    tie1= modelSpace.setHugeTrussBetweenNodes(nod1,nod2)
    tie2= modelSpace.setHugeTrussBetweenNodes(nod2,nod3)
    tie3= modelSpace.setHugeTrussBetweenNodes(nod3,nod4)
    tie4= modelSpace.setHugeTrussBetweenNodes(nod4,nod1)

    ties.getElements.append(tie1)
    ties.getElements.append(tie2)
    ties.getElements.append(tie3)
    ties.getElements.append(tie4)

    # Constraints
    modelSpace.fixNode('FFF_000',nodCol1.tag)
    modelSpace.fixNode('FFF_000',nodCol2.tag)
    '''
    modelSpace.fixNode('F00_000',nod1.tag)
    modelSpace.fixNode('0F0_000',nod2.tag)
    modelSpace.fixNode('F00_000',nod3.tag)
    modelSpace.fixNode('0F0_000',nod4.tag)
    '''
    topNodPiles=[nod1,nod2,nod3,nod4]
    
    struts.fillDownwards()
    ties.fillDownwards()
    return struts,ties,topNodPiles

def gen_pile_cap_2columns_3X2Ypiles(modelSpace,nodCols,distXpile,distYpile,Hpilecap,nameSetStruts,nameSetTies):
    '''Generate pile cap for two columns with a pile foundation of 3 piles in X direction and 2 piles in Y direction. Return the two sets of elements generated (struts and ties) and the nodes where piles start (topNodPiles)

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param nodCols: [nodeCol1,nodeCol2] nodes of columns 1 and 2 where X 
                  coordinate of node 1 is less than X coord. of node 2
    :param distXpile: distance between piles in X direction
    :param distYpile: distance between piles in Y direction
    :param Hpilecap: pile-cap height
    :param nameSetStruts: name of the set of struts
    :param nameSetTies: name of the set of ties
    '''
    preprocessor=modelSpace.preprocessor
    nodCol1=nodCols[0]
    nodCol2=nodCols[1]
    (xCent,yCent,zCent)=((nodCol1.get3dCoo[0]+nodCol2.get3dCoo[0])/2.,
                         (nodCol1.get3dCoo[1]+nodCol2.get3dCoo[1])/2.,
                         nodCol1.get3dCoo[2])
    struts=preprocessor.getSets.defSet(nameSetStruts)
    ties=preprocessor.getSets.defSet(nameSetTies)
    Lx=distXpile
    Ly=distYpile/2.

    nodes=preprocessor.getNodeHandler
    nod1= nodes.newNodeXYZ(xCent-Lx,yCent-Ly,zCent-0.85*Hpilecap)
    nod2= nodes.newNodeXYZ(xCent,yCent-Ly,zCent-0.85*Hpilecap)
    nod3= nodes.newNodeXYZ(xCent+Lx,yCent-Ly,zCent-0.85*Hpilecap)
    nod4= nodes.newNodeXYZ(xCent+Lx,yCent+Ly,zCent-0.85*Hpilecap)
    nod5= nodes.newNodeXYZ(xCent,yCent+Ly,zCent-0.85*Hpilecap)
    nod6= nodes.newNodeXYZ(xCent-Lx,yCent+Ly,zCent-0.85*Hpilecap)

    #Elements
    str1= modelSpace.setHugeTrussBetweenNodes(nodCol1,nod1)
    str2= modelSpace.setHugeTrussBetweenNodes(nodCol1,nod2)
    str3= modelSpace.setHugeTrussBetweenNodes(nodCol1,nod5)
    str4= modelSpace.setHugeTrussBetweenNodes(nodCol1,nod6)

    str5= modelSpace.setHugeTrussBetweenNodes(nodCol2,nod2)
    str6= modelSpace.setHugeTrussBetweenNodes(nodCol2,nod3)
    str7= modelSpace.setHugeTrussBetweenNodes(nodCol2,nod4)
    str8= modelSpace.setHugeTrussBetweenNodes(nodCol2,nod5)

    struts.getElements.append(str1)
    struts.getElements.append(str2)
    struts.getElements.append(str3)
    struts.getElements.append(str4)
    struts.getElements.append(str5)
    struts.getElements.append(str6)
    struts.getElements.append(str7)
    struts.getElements.append(str8)

    tie1= modelSpace.setHugeTrussBetweenNodes(nod1,nod2)
    tie2= modelSpace.setHugeTrussBetweenNodes(nod2,nod3)
    tie3= modelSpace.setHugeTrussBetweenNodes(nod3,nod4)
    tie4= modelSpace.setHugeTrussBetweenNodes(nod4,nod5)
    tie5= modelSpace.setHugeTrussBetweenNodes(nod5,nod6)
    tie6= modelSpace.setHugeTrussBetweenNodes(nod6,nod1)
    tie7= modelSpace.setHugeTrussBetweenNodes(nod2,nod5)


    ties.getElements.append(tie1)
    ties.getElements.append(tie2)
    ties.getElements.append(tie3)
    ties.getElements.append(tie4)
    ties.getElements.append(tie5)
    ties.getElements.append(tie6)
    ties.getElements.append(tie7)

    # Constraints
    modelSpace.fixNode('FFF_000',nodCol1.tag)
    modelSpace.fixNode('FFF_000',nodCol2.tag)
    '''    
    modelSpace.fixNode('F00_000',nod1.tag)
    modelSpace.fixNode('FF0_000',nod2.tag)
    modelSpace.fixNode('0F0_000',nod3.tag)
    modelSpace.fixNode('F00_000',nod4.tag)
    modelSpace.fixNode('FF0_000',nod5.tag)
    modelSpace.fixNode('0F0_000',nod6.tag)
    '''
    topNodPiles=[nod1,nod2,nod3,nod4,nod5,nod6]
    
    struts.fillDownwards()
    ties.fillDownwards()
    return struts,ties,topNodPiles


def gen_piles(modelSpace,topNodPiles,pileDiam,pileLenght,pileMat,eSize,pileType,bearingCapPile,soils,nameSetPiles,alphaK=[1,1,1]):
    '''Generate piles that start in a pile-cap. Return the set of piles created.

    :param modelSpace: PredefinedSpace object used to create the FE model
                       (see predefined_spaces.py).
    :param topNodPiles: nodes of the pile-cap where piles start.
    :param pileDiam: diameter of the pile.
    :param pileLenght: length of each pile.
    :param pileMat: pile section-material.
    :param eSize: size of the elements.
    :param pileType: type of pile 'endBearing' or 'friction'.
    :param bearingCapPile: total bearing capacity of the pile
    :param soils: list of soil definition  [(zBottom,type, prop), ...]  where 'zBottom' is the global 
                  Z coordinate of the bottom level of the soil, type is the type o soil ("sand" or "clay")
                  prop is the soil property : compactness [Pa/m] for sandy soils and undrained soil shear
                  strength for clay soils.
    :param nameSetPiles: name of the set of piles created.
    :param alphaK: coefficients [alphaKh_x,alphaKh_y,alphaKh_z] to take into account the pile group
                    effect (defaults to [1,1,1])
    '''
    preprocessor=modelSpace.preprocessor
    piles=preprocessor.getSets.defSet(nameSetPiles)
    for n in topNodPiles:
        auxPileSet=preprocessor.getSets.defSet('auxPileSet')
        x,y,z=n.getCoo[0],n.getCoo[1],n.getCoo[2]
        p1=preprocessor.getMultiBlockTopology.getPoints.newPoint(geom.Pos3d(x,y,z))
        p0=preprocessor.getMultiBlockTopology.getPoints.newPoint(geom.Pos3d(x,y,z-pileLenght))
        l=preprocessor.getMultiBlockTopology.getLines.newLine(p0.tag,p1.tag)
        auxPileSet.getLines.append(l)
        auxPileSet.fillDownwards()
        pile_mesh=fem.LinSetToMesh(linSet=auxPileSet,matSect=pileMat,elemSize=eSize,vDirLAxZ=xc.Vector([0,1,0]),elemType='ElasticBeam3d',dimElemSpace=3,coordTransfType='linear')
        fem.multi_mesh(preprocessor=preprocessor,lstMeshSets=[pile_mesh])
        soilLayers= guia.SoilLayers(soilProfile= soils, groundLevel= z)
        pileBC=guia.PileFoundation(pileSet=auxPileSet,pileDiam=pileDiam,E=pileMat.E,pileType=pileType,pileBearingCapacity=bearingCapPile, soilLayers=soilLayers)
        pileBC.generateSpringsPile(alphaK[0],alphaK[1],alphaK[2])
        springs=pileBC.springs
        n1=p1.getNode()
        n0=p0.getNode()
        modelSpace.fixNode('FF0_000',n0.tag)
        modelSpace.setRigidBeamBetweenNodes(n.tag,n1.tag)
        piles+=auxPileSet
        preprocessor.getSets.removeSet('auxPileSet')
    return piles
