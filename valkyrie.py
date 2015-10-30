import subprocess

while(True):
	query = raw_input("valkyrie> ")
	schemasFile = open("test/sql/tpch_schemas.sql", "r")
	schemas = schemasFile.read()
	schemasFile.close()
	f = open("bin/t.sql", "w")
	f.write(schemas)
	f.write(query)
	f.close()
	mushroomCloud = subprocess.Popen(['java', '-jar', './bin/MushroomCloud.jar', '--data', './test/data', './bin/t.sql'], stdout=subprocess.PIPE)
	json = mushroomCloud.stdout.readline()
	#print "generated json :" + json
	valkyrie = subprocess.Popen(['./bin/llvmruntime'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	result = valkyrie.communicate(input=json)
	print result[0]