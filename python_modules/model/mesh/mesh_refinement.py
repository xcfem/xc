# -*- coding: utf-8 -*-
''' Mesh 3-refinement code as defined in: "Algorithms for Quadrilateral and Hexahedral Mesh Generation" Robert Schneiders.'''

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
refinementTemplateConnectivity['2b']= [(0, 1, 5, 4),  (1, 3, 6, 5), (4, 5, 9, 12), (5, 6, 10, 9), (6, 3, 11, 10), (10, 11, 15, 14), (9, 10, 14, 12)]
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
            if(markedNodesIndices==[0,3]): # if last side.
                markedNodesIndices= [3,0]
            indicesDist= abs(markedNodesIndices[1]-markedNodesIndices[0])
            diagonal= (indicesDist==2)
            if(diagonal):
                refinementTemplateCode= '2b'
            else:
                refinementTemplateCode= '2a'
        elif(markedNodesSum == 3):
            if(markedNodesIndices==[0, 2, 3]):
                markedNodesIndices= [2, 3, 0]
            elif(markedNodesIndices==[0, 1, 3]):
                markedNodesIndices= [3, 0, 1]
        e.setProp('refinementTemplateCode', refinementTemplateCode)
        e.setProp('markedNodesIndices', markedNodesIndices)

class EdgeNodesDict(object):
    ''' Dictionary containing the nodes on the edges of the already existing
        elements.

    '''
    def __init__(self):
        ''' Constructor.'''
        self.edgeNodesDict= dict()

    @staticmethod
    def getEdgeKey(edgeNodes):
        ''' Compute the edge key from the given end nodes.
        
        :param edgeNodes: list containing the two end nodes of the edge.
        '''
        retval= None
        if(len(edgeNodes)>1):
            cornerNode0Tag= edgeNodes[0].tag
            cornerNode1Tag= edgeNodes[1].tag
            if(cornerNode0Tag<cornerNode1Tag):
                cornerNode0Tag, cornerNode1Tag= cornerNode1Tag, cornerNode0Tag
            retval= (cornerNode0Tag, cornerNode1Tag)
        return retval
        
    def findNodesOnEdge(self, edgeNodes):
        ''' Return the nodes on the edge with the given end nodes.
        
        :param edgeNodes: list containing the two end nodes of the edge.
        '''
        retval= list()
        edgeKey= EdgeNodesDict.getEdgeKey(edgeNodes)
        if(edgeKey):
            if(edgeKey in self.edgeNodesDict):
                retval= self.edgeNodesDict[edgeKey]
        return retval

    def findMatchingNode(self, edgeNodes, cartesianCoordinates, nodeDict):
        ''' Return the node on the same edge with the same position (if any).

        :param edgeNodes: list containing the two end nodes of the edge.
        :param cartesianCoordinates: cartesian coordinates of the matching node.
        :param nodeDict: dictionary containing the nodes already created.
        '''
        retval= None
        edgeNodesIDs= self.findNodesOnEdge(edgeNodes)
        for otherNodeId in edgeNodesIDs:
            otherNodeData= nodeDict[otherNodeId]
            otherCartesianCoord= otherNodeData['cartesian_coord']
            dist2= otherCartesianCoord.dist2(cartesianCoordinates)
            if(dist2<1e-6):
                retval= otherNodeId
                break
        return retval

    def newEdgeNode(self, edgeNodes, nodeId):
        ''' Append the node to the list of nodes of the corresponding edge.

        :param edgeNodes: list containing the two end nodes of the edge.
        :param nodeId: identifier of the node.
        '''
        edgeKey= EdgeNodesDict.getEdgeKey(edgeNodes)
        if(edgeKey):
            if(edgeKey in self.edgeNodesDict):
                self.edgeNodesDict[edgeKey].append(nodeId)
            else:
                self.edgeNodesDict[edgeKey]= [nodeId]
        
        
def compute_node_positions(xcSet):
    ''' Compute the position of the new nodes for the elements of the given set.

    :param xcSet: set whose elements will be refined.
    '''
    # Compute positions of the new nodes.
    newNodeDict= dict()
    newNodeId= 0
    newElementDict= dict()
    newElementId= 0
    edgeNodesDict= EdgeNodesDict()
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
            # Compute element subdivision level.
            elementSubdivisionLevel= max(nodeSubdivisionLevels)
            if(elementSubdivisionLevel>0):
                e.setProp('subdivisionLevel', elementSubdivisionLevel)
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
            edge_0_3_subdivisionLevel= min(nSL[0], nSL[3])
            edge_3_15_subdivisionLevel= min(nSL[3], nSL[15])
            edge_15_12_subdivisionLevel= min(nSL[15], nSL[12])
            edge_12_0_subdivisionLevel= min(nSL[12], nSL[0])
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
            indexesAlreadyVisited= dict()
            for indexes in connectivity:
                newElementNodeIDs= list()
                for i in indexes:
                    if(i not in indexesAlreadyVisited):
                        existingNode= None
                        edgeNodes= list()
                        matchingNode= None
                        sharedNode= True
                        if( i in [0, 3, 12, 15]): # corner nodes.
                            existingNode= cornerNodes[i]
                            nodeSubdivisionLevel= None # will be update later.
                        elif( i in [5, 6, 9, 10]): # interior nodes.
                            sharedNode= False
                            if(refinementTemplateCode=='4'):
                                nodeSubdivisionLevel= max(elementSubdivisionLevel, 0)
                            else:
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
                        edgeNodesIDs= edgeNodesDict.findNodesOnEdge(edgeNodes)
                        matchingNode= edgeNodesDict.findMatchingNode(edgeNodes= edgeNodes, cartesianCoordinates= cartesianCoord, nodeDict= newNodeDict)
                        if(matchingNode is None):
                            edgeNodesDict.newEdgeNode(edgeNodes= edgeNodes, nodeId= newNodeId)
                        newNodeDict[newNodeId]= {'from_element':e, 'natural_coord':naturalCoord, 'cartesian_coord':cartesianCoord, 'existing_node':existingNode, 'matching_node': matchingNode, 'shared_node':sharedNode, 'node_subdivision_level': nodeSubdivisionLevel}
                        newElementNodeIDs.append(newNodeId)
                        indexesAlreadyVisited[i]= newNodeId
                        newNodeId+= 1
                    else:
                        existingNodeId= indexesAlreadyVisited[i]
                        newElementNodeIDs.append(existingNodeId)
                newElementDict[newElementId]= {'from_element':e, 'node_ids':newElementNodeIDs, 'element_subdivision_level': elementSubdivisionLevel}
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
          matchingNode= nodeData['matching_node']
          cartesianCoord= nodeData['cartesian_coord']
          if(existingNode or sharedNode):
              if(existingNode): # corner node.
                  nodeSubstitutions[nodeKey]= existingNode.tag
                  subdivisionLevel= 0
                  if(existingNode.hasProp('subdivisionLevel')):
                      subdivisionLevel= existingNode.getProp('subdivisionLevel')
                  if(subdivisionLevel!=0):
                      existingNode.setProp('subdivisionLevel', subdivisionLevel-1)
                  nodeTag= existingNode.tag
              else: # edge node.
                  if(matchingNode):
                      if(matchingNode in alreadyVisited):
                          nodeSubstitutions[nodeKey]= alreadyVisited[matchingNode]
                      else:
                          newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
                          subdivisionLevel= nodeData['node_subdivision_level']
                          if(subdivisionLevel!=0):
                              newNode.setProp('subdivisionLevel', subdivisionLevel)
                          nodeTag= newNode.tag
                          nodeSubstitutions[nodeKey]= nodeTag
                          nodeSubstitutions[matchingNode]= nodeTag 
                  else:
                      newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
                      subdivisionLevel= nodeData['node_subdivision_level']
                      if(subdivisionLevel!=0):
                          newNode.setProp('subdivisionLevel', subdivisionLevel)
                      nodeSubstitutions[nodeKey]= newNode.tag
                      nodeTag= newNode.tag
          else:
              cartesianCoord= nodeData['cartesian_coord']
              newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
              subdivisionLevel= nodeData['node_subdivision_level']
              if(subdivisionLevel!=0):
                  newNode.setProp('subdivisionLevel', subdivisionLevel)
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
    elementsToRemove= dict()
    for eId in newElementDict:
        elementData= newElementDict[eId]
        nodeIds= elementData['node_ids']
        nodeTags= list()
        for nId in nodeIds:
            nodeTag= nodeSubstitutions[nId]
            nodeTags.append(nodeTag)
        newElement= modelSpace.newElement("FourNodeQuad",nodeTags)
        fromElement= elementData['from_element']
        # Transfer material.
        newElement.copyMaterialFrom(fromElement, True)
        # Transfer properties.
        newElement.copyPropsFrom(fromElement)
        # Transfer sets.
        modelSpace.copyElementSets(fromElement, newElement)
        # Transfer loads.
        modelSpace.copyElementalLoads(fromElement, newElement)
        # Mark the old element.
        if(fromElement.tag not in elementsToRemove):
            elementsToRemove[fromElement.tag]= fromElement
    for eleTag in elementsToRemove:
        oldElement= elementsToRemove[eleTag]
        modelSpace.removeElement(oldElement)

def compute_subdivision_levels(xcSet):
    ''' Compute the subdivision levels of the elements of the given set.

    :param xcSet: set whose subdivision levels will be computed.
    '''
    maxSubdivisionLevel= 0
    for e in xcSet.elements:
        elementSubdivisionLevel= 0
        for n in e.nodes:
            nodeSubdivisionLevel= 0
            if(n.hasProp('subdivisionLevel')):
                nodeSubdivisionLevel= n.getProp('subdivisionLevel')
            elementSubdivisionLevel= max(elementSubdivisionLevel, nodeSubdivisionLevel)
        e.setProp('subdivisionLevel', elementSubdivisionLevel)
        maxSubdivisionLevel= max(maxSubdivisionLevel, elementSubdivisionLevel)
    return maxSubdivisionLevel

def get_max_node_subdivision_level(xcSet):
    ''' Compute the maximum node subdivision level among the nodes of the 
        given set.

    :param xcSet: set whose maximum subdivision level among its nodes will be computed.
    '''
    maxSubdivisionLevel= 0
    for n in xcSet.nodes:
        nodeSubdivisionLevel= 0
        if(n.hasProp('subdivisionLevel')):
            nodeSubdivisionLevel= n.getProp('subdivisionLevel')
        maxSubdivisionLevel= max(maxSubdivisionLevel, nodeSubdivisionLevel)
    return maxSubdivisionLevel

def refinement_step(modelSpace, xcSet):
    ''' Perform a refinement step on the given set.

    :param modelSpace: wrapper of the finite element model.
    :param xcSet: set whose elements will be refined.
    '''
    set_refinement_templates(xcSet)

    nodePositions= compute_node_positions(xcSet)
    nodeSubstitutions= create_new_nodes(modelSpace= modelSpace, nodePositions= nodePositions)
    create_new_elements(modelSpace= modelSpace, nodePositions= nodePositions, nodeSubstitutions= nodeSubstitutions)
    
def compute_node_subdivision_level(xcSet):
    ''' Compute the subdivision level of the nodes from the subdivision level
        of the elements according to expression (3) of the article.

    :param xcSet: set whose elements will be refined.
    '''
    ## Compute node subdivision level according with expression (3) of the article.
    maxSubdivisionLevel= 0
    for n in xcSet.nodes:
        connectedElements= n.getConnectedElements()
        nodeSubdivisionLevel= 0
        for e in connectedElements:
            elementSubdivisionLevel= 0
            if(e.hasProp('subdivisionLevel')):
               elementSubdivisionLevel= e.getProp('subdivisionLevel')
            nodeSubdivisionLevel= max(elementSubdivisionLevel,nodeSubdivisionLevel)
        n.setProp('subdivisionLevel', nodeSubdivisionLevel)
        maxSubdivisionLevel= max(maxSubdivisionLevel, nodeSubdivisionLevel)
    return maxSubdivisionLevel
    
def refine_mesh(modelSpace, xcSet):
    ''' Perform a mesh refinement using the so called "3-refinement" procedure
        as described in the article "Algorithms for Quadrilateral and 
        Hexahedral Mesh Generation" Robert Schneiders.'

    :param modelSpace: wrapper of the finite element model.
    :param xcSet: set whose elements will be refined.
    '''
    retval= compute_node_subdivision_level(xcSet= xcSet)
    while retval>0:
        refinement_step(modelSpace= modelSpace, xcSet= xcSet)
        compute_subdivision_levels(xcSet= xcSet)
        retval= get_max_node_subdivision_level(xcSet= xcSet)
    return retval
    
