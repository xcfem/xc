# -*- coding: utf-8 -*-

'''Generation of reactions repports.'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

from misc.latex import supertabular

def listNodeReactions(preprocessor, nmbComb, nodeList, fmt, outputFile, ltxSectioning, title, inclInertia= False):
    '''Prints reactions for each of the nodes from the list.

    :param preprocessor: XC preprocessor for the finite element problem.
    :param nmbComb: name of the load combination.
    :param nodeList: node list.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    :param inclInertia: include inertia effects (defaults to false).
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= title
    defCampos= "|l|r|r|r|r|r|r|r|"
    idsCampos= "Caso & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
    supertabular.cabeceraSupertabular(outputFile,8,defCampos,idsCampos,caption)
    nodes= preprocessor.getNodeHandler
    nodes.calculateNodalReactions(inclInertia,1e-7)

    for nod in nodeList:
        outputFile.write(nmbComb+" & "+nod.tag+" & ")
        if(nod.tag>=0):
            reac= nod.getReaction()
            outputFile.write(fmt.format(reac[0]/1e3)+" & "+fmt.format(reac[1]/1e3)+" & "+fmt.format(reac[2]/1e3)+" & ")
            outputFile.write(fmt.format(reac[3]/1e3)+" & "+fmt.format(reac[4]/1e3)+" & "+fmt.format(reac[5]/1e3)+"\\\\\n")
        else:
            outputFile.write("\\multicolumn{6}{|l|}{undefined node.}\\\\\n")
    supertabular.cierraSupertabular(outputFile)

def listPointReactions(preprocessor, nmbComb, pointList, fmt, outputFile, ltxSectioning, title, inclInertia= False):
    '''Print the reactions associated with the nodes associated to the
       points from the list.

    :param preprocessor: XC preprocessor for the finite element problem.
    :param nmbComb: name of the load combination.
    :param pointList: point list.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    :param inclInertia: include inertia effects (defaults to false).
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= title
    defCampos= "|l|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & IdP & IdN & Fx & Fy & Fz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
    supertabular.cabeceraSupertabular(outputFile,9,defCampos,idsCampos,caption)
    nodes= preprocessor.getNodeHandler
    nodes.calculateNodalReactions(inclInertia,1e-7)

    for iArranque in pointList:
        pto= preprocessor.getMultiBlockTopology.getPoint(iArranque)
        iNode= pto.getNodeTag()
        outputFile.write(nmbComb+" & "+iArranque+" & "+iNode+" & ")
        if(iNode>=0):
            nod= nodes.getNode(iNode)
            reac= nod.getReaction()
            outputFile.write(fmt.format(reac[0]/1e3)+" & "+fmt.format(reac[1]/1e3)+" & "+fmt.format(reac[2]/1e3)+" & ")
            outputFile.write(fmt.format(reac[3]/1e3)+" & "+fmt.format(reac[4]/1e3)+" & "+fmt.format(reac[5]/1e3)+"\\\\\n")
        else:
            outputFile.write("\\multicolumn{6}{|l|}{undefined node.}\\\\\n")
    supertabular.cierraSupertabular(outputFile)

def listPointReactionsCSV(preprocessor, nmbComb,pointList, fmt, outputFile, inclInertia= False):
    '''
       Prints the reactions in CSV (comma separated values) format.

    :param preprocessor: XC preprocessor for the finite element problem.
    :param nmbComb: name of the load combination.
    :param pointList: point list.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    :param inclInertia: include inertia effects (defaults to false).
    '''
    outputFile.write("Caso ; IdP ; IdN ; Fx ; Fy ; Fz ; Mx ; My ; Mz\n")
    outputFile.write(" - ; - ; - ; kN ; kN ; kN ; kN m ; kN m ; kN m \n")

    nodes= preprocessor.getNodeHandler
    nodes.calculateNodalReactions(inclInertia, 1e-7)

    for iArranque in pointList:
        pto= preprocessor.getMultiBlockTopology.getPoint(iArranque)
        iNode= pto.getNodeTag()
        outputFile.write(nmbComb+" ; "+iArranque+" ; "+iNode+" ; ")
        if(iNode>=0):
            nod= nodes.getNode(iNode)
            reac= nod.getReaction()
            outputFile.write(fmt.format(reac[0]/1e3)+" ; "+fmt.format(reac[1]/1e3)+" ; "+fmt.format(reac[2]/1e3)+" ; ")
            outputFile.write(fmt.format(reac[3]/1e3)+" ; "+fmt.format(reac[4]/1e3)+" ; "+fmt.format(reac[5]/1e3)+"\n")
        else:
            outputFile.write("undefined node.\n")

class ReactionsListRecord(object):
    formato= '{:7.2f}'
    sectionHeadingA= "subsection"
    titulo= "Reactions"
    sectionHeadingB= "subsubsection"
    nodeLists= []
    listaCabeceras= []

    def write(self,preprocessor, nmbComb, outputFile):
        outputFile.write("\\"+self.sectionHeadingA+"{"+self.titulo+"}\n")
        for j, l in enumerate(self.nodeLists):
            listNodeReactions(preprocessor= preprocessor, nmbComb= nmbComb, nodeList= l, fmt= self.formato, outputFile= outputFile, ltxSectioning= self.sectionHeadingB, title= self.listaCabeceras[j])

