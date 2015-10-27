# ValkyrieDB
A SQL-to-native-code compiler based on LLVM

## Building and Running the Project
Using `make` should build both MushroomCloud and Valkyrie projects. The binaries will get built in a folder bin by the Makefile. 

For runnng the project use _ValkyrieDB.sh_ and pass 2 parameters. First is the path of the data files where all the table data is stored, and the second is the path of the sql files, which contains the queries to be executed.

The query plan generated for the following sql script
```
CREATE TABLE NATION (nationkey INT , name CHAR (25) , regionkey INT REFERENCES REGION, comment VARCHAR (152) , PRIMARY KEY (nationkey))
SELECT * FROM NATION WHERE nationkey > 5;
```

will look like
```
PRINT[]
	PROJECT[*]
		SELECT[nationkey > 5]
			TABLE[nationkey=int, name=char, regionkey=int, comment=varchar]
```