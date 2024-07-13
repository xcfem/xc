# -*- coding: utf-8 -*-
''' Mesh 3-refinement code as defined in: «Algorithms for Quadrilateral and Hexahedral Mesh Generation» Robert Schneiders.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import xc

# Refinement templates.

# Point arrangement:
# 
#            13    14     15
#   12 +-----+-----+-----+
#      |                 |
#      |     9     10    |
#    8 +     +     +     + 11
#      |                 |
#      |     5     6     |
#    4 +     +     +     + 7
#      |                 |
#      |                 |
#   0  +-----+-----+-----+ 3
#            1     2

## Points.
refinementTemplatePoints_0= list()
for y in [-1, -1/3.0, 1/3.0, 1]:
    row= list()
    for x in [-1, -1/3.0, 1/3.0, 1]:
        row.append((x,y))
    refinementTemplatePoints_0+=row

### Rotated 90 degrees:
refinementTemplatePoints_90= list()
for p in refinementTemplatePoints_0:
    refinementTemplatePoints_90.append((-p[1], p[0]))
### Rotated 180 degrees:
refinementTemplatePoints_180= list()
for p in refinementTemplatePoints_0:
    refinementTemplatePoints_180.append((-p[0], -p[1]))
### Rotated 270 degrees:
refinementTemplatePoints_270= list()
for p in refinementTemplatePoints_180:
    refinementTemplatePoints_270.append((-p[1], p[0]))
refinementTemplatePoints= [refinementTemplatePoints_0, refinementTemplatePoints_90, refinementTemplatePoints_180, refinementTemplatePoints_270]
    
## Connectivity.
refinementTemplateConnectivity= dict()
refinementTemplateConnectivity['1']= [(0, 1, 5, 4), (1, 3, 15, 5), (4, 5, 15, 12)]
refinementTemplateConnectivity['2a']= [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (4, 5, 9, 12), (5, 6, 10, 9), (6, 7, 15, 10), (9, 10, 15, 12)] 
refinementTemplateConnectivity['2b']= [(0, 1, 5, 4),  (1, 3, 6, 5), (4, 5, 9, 12), (5, 6, 10, 9), (6, 3, 11, 10), (10, 11, 15, 14)]
refinementTemplateConnectivity['3']= [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (4, 5, 9, 12), (5, 6, 10, 9), (6, 7, 11, 10), (9, 10, 14, 12), (10, 11, 15, 14)]
refinementTemplateConnectivity['4']= [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (4, 5, 9, 8), (5, 6, 10, 9), (6, 7, 11, 10), (8, 9, 13, 12), (9, 10, 14, 13),  (10, 11, 15, 14)]

def set_refinement_templates(xcSet):
    ''' Set the refinement templates to the elements of the given set.

    :param xcSet: set whose elements will be given a refinement template.
    '''
    ## Assign the refinement template (figure 53).
    for e in xcSet.elements:
        markedNodes= [0, 0, 0, 0]
        markedNodesIndices= list()
        for i, n in enumerate(e.nodes):
            nodeSubdivisionLevel= 0
            if(n.hasProp('subdivisionLevel')):
                nodeSubdivisionLevel= n.getProp('subdivisionLevel')
            if(nodeSubdivisionLevel>0):
                markedNodes[i]= 1
                markedNodesIndices.append(i)
        markedNodesSum= sum(markedNodes)
        refinementTemplateCode= str(markedNodesSum)
        if(markedNodesSum == 2):
            diagonal= (abs(markedNodesIndices[1]-markedNodesIndices[0])>1)
            if(diagonal):
                refinementTemplateCode= '2b'
            else:
                refinementTemplateCode= '2a'
        e.setProp('refinementTemplateCode', refinementTemplateCode)
        e.setProp('markedNodesIndices', markedNodesIndices)
    
def compute_node_positions(xcSet):
    ''' Compute the position of the new nodes for the elements of the given set.

    :param xcSet: set whose elements will be refined.
    '''
    # Compute positions of the new nodes.
    newNodeDict= dict()
    newNodeId= 0
    newElementDict= dict()
    newElementId= 0
    edgeNodesDict= dict()
    elementsToRemove= list()
    for e in xcSet.elements:
        refinementTemplateCode= '0'
        if(e.hasProp('refinementTemplateCode')):
            refinementTemplateCode= e.getProp('refinementTemplateCode')
        if(refinementTemplateCode!='0'):
            elementsToRemove.append(e)
            markedNodesIndices= e.getProp('markedNodesIndices')
            templateRotation= markedNodesIndices[0]
            rotatedRefinementTemplatePoints= refinementTemplatePoints[templateRotation]
            # Get node subdivision levels.
            nodeSubdivisionLevels= list()
            elementNodes= list()
            for n in e.nodes:
                elementNodes.append(n)
                nodeSubdivisionLevel= 0
                if(n.hasProp('subdivisionLevel')):
                    nodeSubdivisionLevel= n.getProp('subdivisionLevel')
                nodeSubdivisionLevels.append(max(nodeSubdivisionLevel-1, 0))
            elementSubdivisionLevel= max(nodeSubdivisionLevels)
            nIndex= dict()
            nIndex[0]= templateRotation # pos. 0 in the point arrangement.
            nIndex[3]= (1+templateRotation) % 4 # pos. 3 in the pt. arrangement.
            nIndex[15]= (2+templateRotation) % 4 # pos. 15 in the pt. arrangement.
            nIndex[12]= (3+templateRotation) % 4 # pos. 12 in the pt. arrangement.
            # Corner nodes
            cornerNodes= dict()
            cornerNodes[0]= elementNodes[nIndex[0]]
            cornerNodes[3]= elementNodes[nIndex[3]]
            cornerNodes[15]= elementNodes[nIndex[15]]
            cornerNodes[12]= elementNodes[nIndex[12]]
            # Node subdivision levels.
            nSL= dict()
            nSL[0]= nodeSubdivisionLevels[nIndex[0]] # pos. 0 in the pt. arrangement.
            nSL[3]= nodeSubdivisionLevels[nIndex[3]] # pos. 3 in the pt. arrangement.
            nSL[15]= nodeSubdivisionLevels[nIndex[15]] # pos. 15 in the pt. arrangement.
            nSL[12]= nodeSubdivisionLevels[nIndex[12]] # pos. 12 in the pt. arrangement.
            # Edge subdivision levels.
            edge_0_3_subdivisionLevel= max(nSL[0], nSL[3])
            edge_3_15_subdivisionLevel= max(nSL[3], nSL[15])
            edge_15_12_subdivisionLevel= max(nSL[15], nSL[12])
            edge_12_0_subdivisionLevel= max(nSL[12], nSL[0])
            # Edge nodes.
            edge_0_3_nodes= (cornerNodes[0], cornerNodes[3])
            edge_3_15_nodes= (cornerNodes[3], cornerNodes[15])
            edge_15_12_nodes= (cornerNodes[15], cornerNodes[12])
            edge_12_0_nodes= (cornerNodes[12], cornerNodes[0])
            # Node positions.
            markedNodesLocalCoordinates= list()
            for mn in markedNodesIndices:
                markedNodesLocalCoordinates.append(e.getLocalCoordinatesOfNode(mn))
            connectivity= refinementTemplateConnectivity[refinementTemplateCode]
            newNodesCoordinates= list()
            for indexes in connectivity:
                newElementNodeIDs= list()
                for i in indexes:
                    existingNode= None
                    edgeNodes= list()
                    mainNode= None
                    sharedNode= True
                    if( i in [0, 3, 12, 15]): # corner nodes.
                        existingNode= cornerNodes[i]
                        nodeSubdivisionLevel= None # will be update later.
                    elif( i in [5, 6, 9, 10]): # interior nodes.
                        sharedNode= False
                        nodeSubdivisionLevel= max(elementSubdivisionLevel-1, 0)
                    else: # edge nodes.
                        if(i in [1, 2]):
                            edgeSubdivisionLevel= edge_0_3_subdivisionLevel
                            edgeNodes= [cornerNodes[0], cornerNodes[3]]
                        elif(i in [7, 11]):
                            edgeSubdivisionLevel= edge_3_15_subdivisionLevel
                            edgeNodes= [cornerNodes[3], cornerNodes[15]]
                        elif(i in [13, 14]):
                            edgeSubdivisionLevel= edge_15_12_subdivisionLevel
                            edgeNodes= [cornerNodes[15], cornerNodes[12]]
                        elif(i in [4, 8]):
                            edgeSubdivisionLevel= edge_12_0_subdivisionLevel
                            edgeNodes= [cornerNodes[12], cornerNodes[0]]
                        nodeSubdivisionLevel= edgeSubdivisionLevel


                    naturalCoord= rotatedRefinementTemplatePoints[i]
                    naturalCoord= xc.ParticlePos2d(naturalCoord[0], naturalCoord[1])
                    cartesianCoord= e.getCartesianCoordinates(naturalCoord, True)
                    if(len(edgeNodes)>1):
                        cornerNode0Tag= edgeNodes[0].tag
                        cornerNode1Tag= edgeNodes[1].tag
                        if(cornerNode0Tag<cornerNode1Tag):
                            cornerNode0Tag, cornerNode1Tag= cornerNode1Tag, cornerNode0Tag
                        edgeNodeKey= (cornerNode0Tag, cornerNode1Tag)
                        if(edgeNodeKey in edgeNodesDict):
                            edgeNodesIDs= edgeNodesDict[edgeNodeKey]
                            for otherNodeKey in edgeNodesIDs:
                                if(otherNodeKey!=newNodeId):
                                    otherNodeData= newNodeDict[otherNodeKey]
                                    otherCartesianCoord= otherNodeData['cartesian_coord']
                                    dist2= otherCartesianCoord.dist2(cartesianCoord)
                                    if(dist2<1e-6):
                                        mainNode= otherNodeKey
                                else:
                                    edgeNodesDict[edgeNodeKey].append(newNodeId)
                        else:
                            edgeNodesDict[edgeNodeKey]= [newNodeId]

                    newNodeDict[newNodeId]= {'from_element':e, 'natural_coord':naturalCoord, 'cartesian_coord':cartesianCoord, 'existing_node':existingNode, 'main_node': mainNode, 'shared_node':sharedNode, 'nodeSubdivisionLevel': nodeSubdivisionLevel}
                    newElementNodeIDs.append(newNodeId)
                    newNodeId+= 1
                newElementDict[newElementId]= {'from_element':e, 'node_ids':newElementNodeIDs}
                newElementId+= 1
    return {'new_nodes':newNodeDict, 'new_elements':newElementDict, 'elements_to_remove':elementsToRemove}

def create_new_nodes(modelSpace, nodePositions):
    ''' Create the nodes needed to refine the mesh.

    :param modelSpace: wrapper of the finite element model.
    :param nodePositions: dictionary returned by compute_node_positions 
                          containing the information about the position
                          of the new nodes.
    '''
    # Create new nodes.
    nodeSubstitutions= dict()
    alreadyVisited= dict()
    newNodeDict= nodePositions['new_nodes']
    for nodeKey in newNodeDict:
        if(nodeKey not in alreadyVisited):
          nodeData= newNodeDict[nodeKey]
          existingNode= nodeData['existing_node']
          sharedNode= nodeData['shared_node']
          mainNode= nodeData['main_node']
          cartesianCoord= nodeData['cartesian_coord']
          if(existingNode or sharedNode):
              if(existingNode): # corner node.
                  nodeSubstitutions[nodeKey]= existingNode.tag
                  nodeTag= existingNode.tag
              else: # edge node.
                  if(mainNode):
                      if(mainNode in alreadyVisited):
                          nodeSubstitutions[nodeKey]= alreadyVisited[mainNode]
                      else:
                          newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
                          nodeTag= newNode.tag
                          nodeSubstitutions[nodeKey]= nodeTag
                          nodeSubstitutions[mainNode]= nodeTag 
                  else:
                      newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
                      nodeSubstitutions[nodeKey]= newNode.tag
                      nodeTag= newNode.tag
          else:
              cartesianCoord= nodeData['cartesian_coord']
              newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
              nodeSubstitutions[nodeKey]= newNode.tag
              nodeTag= newNode.tag
          alreadyVisited[nodeKey]= nodeTag
    return nodeSubstitutions
          
def create_new_elements(modelSpace, nodePositions, nodeSubstitutions):
    ''' Create the elements needed to refine the mesh.

    :param modelSpace: wrapper of the finite element model.
    :param nodePositions: dictionary returned by compute_node_positions 
                          containing the information about the position
                          of the new nodes.
    :param nodeSubstitutions: dictionary containing the node identifiers
                              corresponding to each of the node keys in
                              newNodesDict.
    '''
    # Create new elements.
    newElementDict= nodePositions['new_elements']
    for eId in newElementDict:
        elementData= newElementDict[eId]
        nodeIds= elementData['node_ids']
        nodeTags= list()
        for nId in nodeIds:
            nodeTag= nodeSubstitutions[nId]
            nodeTags.append(nodeTag)
        newElement= modelSpace.newElement("FourNodeQuad",nodeTags)
        fromElement= elementData['from_element']
        print('Fix material assignment.')
        # newElement.copyMaterialFrom(fromElement)
