#!/usr/bin/python

import subprocess

def promptLoop():
	while(True):
		query = raw_input("valkyrie> ")
		if(query == "q" or query == "quit"):
			break

		with open('./bin/t.sql', 'w') as f:
			f.write(query)

		mushroomCloud = subprocess.Popen(
			['java', '-jar', './bin/MushroomCloud.jar', 
			'--data', './test/data', 
			'./test/sql/tpch_schemas.sql', './bin/t.sql'], 
			stdout=subprocess.PIPE
			)

		json = mushroomCloud.stdout.readline()
		valkyrie = subprocess.Popen(['./bin/llvmruntime'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		result = valkyrie.communicate(input=json)
		print result[0]

if __name__ == "__main__":
	promptLoop()