#!/usr/bin/python

import os
import subprocess
import glob
import sqlite3

tpchgenrepo = "git@github.com:Legacy25/tpch-dbgen.git"
tpchdb = "tpch.db"

def clonerepo(repo):
	subprocess.call(["git", "clone", repo])

def initdatafolder():
	os.mkdir("data")
	os.chdir("tpch-dbgen")
	subprocess.call(["make"])
	subprocess.call(["./dbgen", "-s", "0.1"])
	tblfiles = glob.glob("*.tbl")
	for f in tblfiles:
		os.rename(f, os.path.join("../data", f))

	os.chdir("..")

def createDB(dbname):
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