#!/usr/bin/python

import os
import sys
import shutil
import subprocess
import glob
import sqlite3

home = os.path.dirname(os.path.realpath(__file__))
tpchgenrepo = "https://github.com/Legacy25/tpch-dbgen.git"
tpchdb = "tpch.db"
sf = sys.argv[1]

def clonerepo(repo):
	os.chdir(home)
	if(os.path.exists("tpch-dbgen")):
		shutil.rmtree("tpch-dbgen")
	subprocess.call(["git", "clone", repo])

def initdatafolder():
	os.chdir(home)
	if(os.path.exists("data")):
		shutil.rmtree("data")
	os.mkdir("data")
	os.chdir("tpch-dbgen")
	subprocess.call(["make"])
	subprocess.call(["./dbgen", "-s", sf])
	tblfiles = glob.glob("*.tbl")
	for f in tblfiles:
		os.rename(f, os.path.join("../data", f))
	os.chdir("..")

def createDB(dbname):
	if(os.path.exists(tpchdb)):
		os.remove(tpchdb)
	conn = sqlite3.connect(tpchdb)
	return conn.cursor()

def initDB(cursor):
	f = open("sql/ddl-schema.sql")
	sql = f.read()
	stmts = sql.split(";")
	for s in stmts:
		cursor.execute(s)

def loaddata(cursor):
	tblfiles = glob.glob("data/*.tbl")

	for f in tblfiles:
		arg = ".import {} {}".format(f, f.split("/")[1].split(".tbl")[0])
		subprocess.call(["sqlite3", "tpch.db", arg])

def main():
	clonerepo(tpchgenrepo)
	initdatafolder()
	cursor = createDB(tpchdb)
	initDB(cursor)
	loaddata(cursor)

if __name__ == "__main__":
    main()