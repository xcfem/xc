import mysql.connector

def uploadCombinationTable(combDict, dataBaseName, tableName, groupName, subGroupName, usr, pwd, host= 'localhost'):
    '''Upload the combinations in a MySQL table.

    :param combDict: dictionary containing the combinations.
    :param dataBaseName: name of the MySQL database.
    :param tableName: name of the table storing the combinations.
    :param groupName: name of the group of combinations.
    :param subGroupName: name of the subgroup of combinations.
    :param usr: MySQL user name.
    :param pwd: MySQL password.
    :param host: MySQL host name.
    '''
    db_connection = mysql.connector.connect(
    host= host,
    user= usr,
    passwd= pwd,
    database= dataBaseName
    )
    db_cursor = db_connection.cursor()
    #Here creating database table as student'
    db_cursor.execute('DROP TABLE IF EXISTS '+tableName)
    db_cursor.execute('CREATE TABLE IF NOT EXISTS '+tableName+' (idComb INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, group CHAR(20), subgroup CHAR(10), descomp CHAR(100))')
    for combName in combDict:
        descomp= combDict[combName]
        db_cursor.execute('INSERT INTO tableName (group,subgroup,descomp) VALUES ('+groupName+','+subGroupName+','+getName+')')

