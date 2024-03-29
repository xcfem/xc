# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import sympy
import tempfile
import os

def toLaTeX(inputStr):
    ''' Add escape characters as needed to display the input string in LaTeX
        correctly.

    :param inputStr: input string.
    '''
    retval= inputStr.replace('\\(','(')
    retval= retval.replace('\\)',')')
    retval= retval.replace('\\[','[')
    retval= retval.replace('\\]',']')
    retval= retval.replace('\\]',']')
    retval= retval.replace('_','\\_')
    retval= retval.replace('%','\\%')
    retval= retval.replace('$','\\$')
    retval= retval.replace('&','\\&')
    #retval= retval.replace('>','$>$')
    return retval

def mathExprToLaTeX(mathExprStr):
    ''' Convert the math expression in the input string to LaTeX.

    :param mathExprStr: math expression input string.
    '''
    return sympy.latex(sympy.sympify(mathExprStr))
  
def latex_to_pdf(latexCode:str, pdfFileName):
    ''' Creates a PDF file from the given LaTeX code.

    :param latexCode: text in LaTeX format.
    :param pdfFileName: output file name.
    '''
    texDocument= u'''\documentclass{article}
    \\usepackage[utf8]{inputenc}
    \\usepackage{longtable}
    \\begin{document}
    **LaTeXCode**
    \end{document}
    '''
    texDocument= texDocument.replace('**LaTeXCode**', latexCode)
    with tempfile.NamedTemporaryFile(mode= 'w', delete= False) as tmp:
        tmp.write(texDocument)
        outputFileName= tmp.name
    os.rename(outputFileName, outputFileName+'.tex')
    outputFileName+='.tex'
    dirName= os.path.dirname(outputFileName)
    pdfLaTeXCommand= 'pdflatex --interaction=batchmode'
    pdfLaTeXCommand+= ' -output-directory '+ dirName 
    pdfLaTeXCommand+= ' ' + outputFileName
    print(pdfLaTeXCommand)
    os.system(pdfLaTeXCommand)    
    os.system(pdfLaTeXCommand)    
    os.system(pdfLaTeXCommand)
    pdfOutput= outputFileName.replace('.tex', '.pdf')
    os.replace(pdfOutput, pdfFileName)
    ### Remove unneeded files
    for extension in ['.aux', '.log']:
        fName= outputFileName.replace('.tex', extension)
        os.unlink(fName)
    os.unlink(outputFileName)
