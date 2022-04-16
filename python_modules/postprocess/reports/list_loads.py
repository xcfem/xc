# -*- coding: utf-8 -*-
''' Create load lists in LaTeX.'''

from __future__ import print_function
from __future__ import division

def uniformLoadList(loadPattern, outputFile, load, fmt):
    ''' List of uniform loads.

    :param loadPattern: load pattern. 
    :param outputFile: output file name.
    :param fmt: format for numbers.
    '''
    preprocessor= loadPatter.getPreprocessor()
    elemTags= loadPattern.getElementTags()
    for i in elemTags:
        e= preprocessor.getElementHandler.getElement(i)
        outputFile.write(e.tag+" & "+loadPattern+" & unif. & & "+fmt.format(load[0]/1e3)+" & "+fmt.format(load[1]/1e3)+" & "+fmt.format(load[2]/1e3)+"\\\\\n")

def punctualLoadList(loadPattern, outputFile, load, fmt):
    ''' List of punctual loads.

    :param loadPattern: load pattern. 
    :param outputFile: output file name.
    :param fmt: format for numbers.
    '''
    xLoad= x
    preprocessor= loadPatter.getPreprocessor()
    elemTags= loadPattern.getElementTags()
    for i in elemTags:
        e= preprocessor.getElementHandler.getElement(i)
        outputFile.write(e.tag+" & "+loadPattern+" & punt. & "+xLoad+" & "+fmt.format(load[0]/1e3)+" & "+fmt.format(load[1]/1e3)+" & "+fmt.format(load[2]/1e3)+"\\\\\n")

def ElementLoadList(loadPattern, outputFile, fmt):
    ''' List loads on elements.

    :param loadPattern: load pattern. 
    :param outputFile: output file name.
    :param fmt: format for numbers.
    '''
    caption= 'Loads on elements'
    defCampos= "|r|c|c|r|r|r|r|"
    idsCampos= "Id & Acc. & Type & x & Fx & Fy &Fz \\\\\n - & - & - &  & kN/m & kN/m & kN/m "
    cabeceraSupertabular(outputFile,7,defCampos,idsCampos,caption)
    preprocessor= loadPatter.getPreprocessor()
    loads= preprocessor.getLoadPatterns
    print("Number of load patterns: "+numLoadPatterns)
    print("Number of loads on elements: "+ loadPattern.getNumEleLoads())
    eleLoads= loadPattern.getEleLoads()
    for l in eleLoads:
        if(l.category=="nodef"):
            load= l.getGlobalForces
        elif(l.category=="uniform"):
            uniformLoadList(loadPattern,outputFile,load,fmt)
        elif(l.category=="punctual"):
            punctualLoadList(loadPattern,outputFile,load,fmt)
    cierraSupertabular(outputFile) 

def listNodalLoads(loadPattern, outputFile, fmt):
    ''' List loads on nodes.

    :param loadPattern: load pattern. 
    :param outputFile: output file name.
    :param fmt: format for numbers.
    '''
    caption= "Nodal loads"
    defCampos= "|r|c|c|r|r|r|r|"
    idsCampos= "Id & Acc. & Type & x & Fx & Fy &Fz \\\\\n - & - & - &  & kN & kN & kN "
    cabeceraSupertabular(outputFile,7,defCampos,idsCampos,caption) 
    print("Number of nodal loads: "+ loadPattern.getNumNodalLoads())
    nodalLoads= loadPattern.getNodalLoads()
    for l in nodalLoads:
        load= l.load
        outputFile.write(l.nod.tag+" & "+loadPattern+" & nod. &  & "+fmt.format(load[0]/1e3)+" & "+fmt.format(load[1]/1e3)+" & "+fmt.format(load[2]/1e3)+"\\\\\n")
    cierraSupertabular(outputFile) 


