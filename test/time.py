#!/usr/bin/python

import os
import subprocess
import glob
import timeit
import sqlite3
import time

def readlinefromfile(file):
	f = open(file)
	line = f.readline()
	f.close()

	return line


def time_generic(sqlfiles, db, subproclist):
	times = {}

	print("{} times".format(db))

	for f in sqlfiles:
		subprocess.call(subproclist + [f])
		query = os.path.basename(f).split(".sql")[0]
		times[query] = reduce(lambda x,y: x+y, map(lambda x: float(x), readlinefromfile(os.path.join("../../SQL-Parser/output/", query+".stat")).split("|")))
		print("{} took {:.4f}s".format(query, times[query]))

	return times

def time_562(sqlfiles):
	return time_generic(sqlfiles, "562-SQL-Parser Mushroom Cloud", ["../../SQL-Parser/run.sh", "-q", "--data", "../ValkyrieDB/test/data/", "--time", "output/"])

def time_sqlite(sqlfiles):
	times = {}

	print("SQLite3 times")

	conn = sqlite3.connect('tpch.db')
	c = conn.cursor()

	for f in sqlfiles:
		fp = open(f)
		lines = fp.read()
		fp.close()
		query = os.path.basename(f).split(".sql")[0]
		stmts = filter(lambda x: x.upper().find("SELECT") != -1, map(lambda x: " ".join(x.split()), lines.split(";")))
		starttime = time.time()
		
		rows = 0
		for s in stmts:
			for row in c.execute(s):
				rows += 1
		
		times[query] = time.time() - starttime
		print("{} took {:.4f}s | {} rows".format(query, times[query], rows))

	return times

	

def main():
	sqlfiles = glob.glob("sql/tpch*.sql")
	times_sqlite = time_sqlite(sqlfiles)
	times_562 = time_562(sqlfiles)
	# Add other benchmarks here
	
if __name__ == "__main__":
    main()