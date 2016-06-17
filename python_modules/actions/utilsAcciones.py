# Devuelve una lista con los nombres de los patrones de carga
def listaNombresLoadPatterns(preprocessor):
  ''':returns: list of load pattern names

  :param preprocessor: preprocessor name
  '''
  return preprocessor.loadLoader.getLoadPatterns.getKeys()
