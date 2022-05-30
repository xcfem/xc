# -*- coding: utf-8 -*-

from tabulate import tabulate

def listKPts(kPts, fmt):
    ''' Return a tabulate object containing the point data

       :param kPts: iterable containing the points.
       :param fmt: format for coordinates.
    '''
    #caption= "Points of the set: "+xcSet.name
    headers= ['Id', 'x', 'y', 'z']
    rows= list()
    for p in kPts:
        rows.append([p.tag,fmt.format(p.pos.x),fmt.format(p.pos.y),fmt.format(p.pos.z)])
    return tabulate(rows, headers= headers)

def listaLineasSet(lines):
    ''' Returns a tabulate object containing the line data.

       :param lines: iterable containing the lines.
    '''
    #caption= "Lines of set: "+xcSet.name
    headers= ['Id', 'nDiv', 'Points']
    rows= list()
    for l in lines:
        rows.append([l.tag,l.ndiv,l.kPts])
    return tabulate(rows, headers= headers)

def listNodeSet(nodes, fmt):
    ''' Returns a tabulate object containing the node data.

       :param nodes: iterable containing the nodes.
       :param fmt: format for coordinates.
    '''
    #caption= "Nodes from set: "+xcSet.name
    headers=  ['Id', 'x', 'y', 'z']
    rows= list()
    for n in nodes:
        pos= n.getPos()
        rows.append([n.tag, fmt.format(pos.x), fmt.format(pos.y), fmt.format(pos.z)])
    return tabulate(rows, headers= headers)

def listElementSet(elements):
    ''' Returns a tabulate object containing the element data.

       :param elements: iterable containing the elements.
    '''
    # caption= "Elementos del conjunto: "+xcSet.name
    headers= ['Id', 'Type', 'Nodes']
    rows= list()
    for e in elements:
        rows.append([e.tag, e.getNodeTags()])
    return tabulate(rows, headers= headers)

def listTrussElementSet(elements):
    ''' Returns a tabulate object containing the truss element data.

       :param elements: iterable containing the elements.
    '''
    # caption= "Elementos del conjunto: "+xcSet.name
    headers= ['Id', 'nI', 'nJ', 'Material']
    rows= list()
    for e in elements:
        rows.append([e.tag, e.getNode(0).tag, e.getNode(1).tag, e,getMaterial().name])
    return tabulate(rows, headers= headers)

def listBarElementSet(elements):
    ''' Returns a tabulate object containing the bar element data.

       :param elements: iterable containing the elements.
    '''
    #caption= "Elemento from set: "+setName
    headers= ['Id', 'nI', 'nJ','Sc.', 'E', 'G', 'Area', 'alpha', 'J', 'Iy', 'Iz']
    #units=   ['-', '-', '-', '-', 'GPa', 'GPa', 'cm2', '-', 'cm4', 'cm4', 'cm4']
    rows= list()
    for e in elements:
        row= [str(e.tag), str(e.nod(0).tag), str(e.nod(1).tag)]
        sections= e.getSections()
        for k, s in enumerate(sections):
            row.append(k)
            row.append(fmt.format(s.getE()/1e9), fmt.format(s.getG()/1e9), fmt.format(s.getA()*1e4), fmt.format(s.getAlpha()), )
            row.append(fmt.format(s.getJ()*1e6), fmt.format(s.getIy()*1e6), fmt.format(s.getIz()*1e6),"\\\\\n")
        rows.append(row)
    return tabulate(rows, headers= headers)

def listSet(xcSet, elementListFunction, fmt):
    ''' Returs a dictionary with the tabulate objects containing 
        the set components.

       :param xcSet: XC set.
       :param elementListFunction: function to be used for the element list.
       :param fmt: format for coordinates.
    '''
    retval= dict()
    retval['kPts']= listKPtsSet(xcSet.points,fmt) 
    retval['lines']= listLineSet(xcSet.lines) 
    retval['nodes']= listNodeSet(xcSet.nodes,fmt) 
    retval['elements']= elementListFunction(xcSet.elements,fmt)
    return retval


def listLineEntities(line, nmbProcLstElementos, fmt):
    ''' Returs a dictionary with the tabulate objects containing 
        the nodes and elements of the line.

       :param lineName: name of the line.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''

    retval= dict()
    retval['nodes']= listNodeSet(line.nodes,fmt) 
    retval['elements']= elementListFunction(line.elements) 
    return retval
