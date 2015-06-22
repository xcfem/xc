import os
def salvaCombinacion(prb,nmbComb, tagComb, pth,db):
  nombreArchivo= pth+nmbComb+".db"
  os.system("rm -f "+nombreArchivo)
  db= prb.newDatabase("BerkeleyDB",nombreArchivo)
  db.save(tagSaveFase0)

class DatabaseHelperSolve:
  nombrePrevia= ""
  tagPrevia= -1
  db= None
  def __init__(self,db):
    self.nombrePrevia= ""
    self.tagPrevia= .1
    self.db= db
  def helpSolve(self,comb):
    previa= comb.getCombPrevia()
    if(previa!=None):
      self.nombrePrevia= previa.getName
      self.tagPrevia= previa.tag
      self.restore()  
  def restore(self):
    if(self.tagPrevia>0):
      self.db.restore(self.tagPrevia*100)
  def solveComb(self,mdlr,comb,analysis):
    mdlr.resetLoadCase()
    self.helpSolve(comb)
    ''' 
    print "nombrePrevia= ",nombrePrevia
    print "tag= ",comb.tag
    print "tagPrevia= ",tagPrevia
    print "descomp previa= ",getDescompCombPrevia
    print "resto sobre previa= ",getDescompRestoSobrePrevia
    '''
    comb.addToDomain()
    analOk= analysis.analyze(1)
    self.db.save(comb.tag*100)
    comb.removeFromDomain()
    
