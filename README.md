# ValkyrieDB
A SQL-to-native-code compiler based on LLVM

ValkyrieDB is a simple query processing engine that can take any query plan in JSON format and turn it into LLVM assembly code. Right now the system has support for Selection, Projection and Equi-joins, and expressions based on numeric datatypes.

ValkyrieDB uses a Java project called Mushroom Cloud to convert SQL to a JSON-formatted query plan. Mushroom Cloud is based on this GitHubber's [SQL-Evaluator](https://github.com/Legacy25/SQL-Evaluator) project. It uses JSqlParser to parse the SQL, and builds a relational-algebra tree, applies optimization passes on that tree and evaluates it. Mushroom Cloud gets rid of the evaluation and instead just prints out the optimized plan that is fed into ValkyrieDB.



## Build

```
$> make
``` 
This will build both MushroomCloud and Valkyrie. The binaries will get built in `bin/`. 

## Run

You can run this project in two ways. Use a shell script to fire off one time queries.

```
$> ./ValkyrieDB.sh <path/to/sqlfile.sql>
```

You can also use an interactive python shell where you can type in your queries.

```
$> ./Shell.py
valkyrie> SELECT * FROM nation;
```

When running the python shell, Valkyrie will in additon to the results of the query, also output two pieces of debugging information.

1. The query plan received
2. The generated LLVM

## Data and queries

The project by default looks for data in `test/data`. To run queries against a table called `foo` drop in foo's data as a `|` separated file in the `test/data` folder with the name `foo.tbl` and Valkyrie will find it.

Some sample queries based on the [TPC-H Schemas](http://blog.2ndquadrant.com/wp-content/uploads/2015/11/image_thumb2.png) that are supported can be found in [`test/sql/testcases`](https://github.com/Legacy25/ValkyrieDB/tree/master/test/sql/testcases). 

To run these queries, first generate the TPC-H tables using any tpch-dbgen and drop in the `.tbl` files in the data folder. Alternatively, you could also use the `test/tpch-data-setup.py <scale-factor>` script. Scale-factor 1 will generate a 1 GB dataset. This script is also used to setup a SQLite database `test/tpch.db` which can be used to validate Valkyrie's results and have a point of comparison with regards to performance. 

To get Valkyrie to find data from other directories, we have to run Mushroom Cloud and Valkyrie separately to have access to Mushroom Cloud's flags.

```
$> java -jar ./bin/MushroomCloud.jar --data <path/to/data> [sqlfile1.sql sqlfile2.sql ...] > [tempfile.json]
$> ./bin/llvmruntime < [tempfile.json]
```

To get Mushroom Cloud to evaluate the query plan, use the `--exec` flag.

## Sample query run

Let's first fire up the shell.

```
$> ./Shell.py
```

Now you get a prompt like this -

```
valkyrie>
```

Now let's type in a query -

```
valkyrie> select nation.name, region.name from nation, region where nation.regionkey = region.regionkey;
```

This generates this output -

```
PRINT[]
	PROJECT[nation.name, region.name]
		JOIN[nation.regionkey = region.regionkey]
			TABLE[NATION=/home/arindam/scratch/ValkyrieDB/./test/data/nation.tbl, nationkey=int, name=char, regionkey=int, comment=varchar]
			TABLE[REGION=/home/arindam/scratch/ValkyrieDB/./test/data/region.tbl, regionkey=int, name=char, comment=varchar]
			
; ModuleID = 'LLVM'

@0 = private unnamed_addr constant [5 x i8] c"%lu|\00"
@1 = private unnamed_addr constant [5 x i8] c"%lf|\00"
@2 = private unnamed_addr constant [4 x i8] c"%s|\00"
@3 = private unnamed_addr constant [4 x i8] c"%s|\00"
@4 = private unnamed_addr constant [2 x i8] c"\0A\00"

declare i32 @printf(i8*, ...)

define i32 @llvmStart() {
entry:
  br label %blockloader0

blockloader0:                                     ; preds = %loopBody4, %entry
  %0 = call i64 @schemaController(i64 22214032, i64 2)
  %1 = icmp ne i64 %0, 0
  br i1 %1, label %loophead2, label %afterblockloader1

afterblockloader1:                                ; preds = %blockloader0
  %2 = call i64 @schemaController(i64 22214032, i64 3)
  br label %blockloader5

loophead2:                                        ; preds = %blockloader0
  %3 = call i64 @schemaController(i64 22214032, i64 0)
  %4 = inttoptr i64 %3 to i64**
  %loopVar3 = alloca i64
  store i64 0, i64* %loopVar3
  br label %loopBody4

loopBody4:                                        ; preds = %loopBody4, %loophead2
  %5 = load i64* %loopVar3
  %6 = getelementptr inbounds i64** %4, i64 %5
  %7 = load i64** %6
  %8 = getelementptr inbounds i64* %7, i32 0
  %9 = load i64* %8
  store i64 %9, i64* inttoptr (i64 22218864 to i64*)
  %10 = getelementptr inbounds i64* %7, i32 1
  %11 = load i64* %10
  store i64 %11, i64* getelementptr (i64* inttoptr (i64 22218864 to i64*), i32 1)
  %12 = getelementptr inbounds i64* %7, i32 2
  %13 = load i64* %12
  store i64 %13, i64* getelementptr (i64* inttoptr (i64 22218864 to i64*), i32 2)
  %14 = getelementptr inbounds i64* %7, i32 0
  %15 = load i64* %14
  store i64 %15, i64* inttoptr (i64 22219792 to i64*)
  call void @hasher(i64 22280736, i64* inttoptr (i64 22219792 to i64*), i32 1, i64* inttoptr (i64 22218864 to i64*), i32 3)
  %16 = add i64 %5, i32 1
  store i64 %16, i64* %loopVar3
  %17 = icmp slt i64 %16, %0
  br i1 %17, label %loopBody4, label %blockloader0

blockloader5:                                     ; preds = %loopBody9, %afterblockloader1
  %18 = call i64 @schemaController(i64 22281360, i64 2)
  %19 = icmp ne i64 %18, 0
  br i1 %19, label %loophead7, label %afterblockloader6

afterblockloader6:                                ; preds = %blockloader5
  %20 = call i64 @schemaController(i64 22281360, i64 3)
  %21 = call i64 @schemaController(i64 22285248, i64 0)
  %22 = inttoptr i64 %21 to i64**
  %23 = call i64 @schemaController(i64 22285248, i64 1)
  %loopVar10 = alloca i64
  store i64 0, i64* %loopVar10
  br label %condCheck11

loophead7:                                        ; preds = %blockloader5
  %24 = call i64 @schemaController(i64 22281360, i64 0)
  %25 = inttoptr i64 %24 to i64**
  %loopVar8 = alloca i64
  store i64 0, i64* %loopVar8
  br label %loopBody9

loopBody9:                                        ; preds = %loopBody9, %loophead7
  %26 = load i64* %loopVar8
  %27 = getelementptr inbounds i64** %25, i64 %26
  %28 = load i64** %27
  %29 = getelementptr inbounds i64* %28, i32 0
  %30 = load i64* %29
  store i64 %30, i64* inttoptr (i64 22247328 to i64*)
  %31 = getelementptr inbounds i64* %28, i32 1
  %32 = load i64* %31
  store i64 %32, i64* getelementptr (i64* inttoptr (i64 22247328 to i64*), i32 1)
  %33 = getelementptr inbounds i64* %28, i32 2
  %34 = load i64* %33
  store i64 %34, i64* getelementptr (i64* inttoptr (i64 22247328 to i64*), i32 2)
  %35 = getelementptr inbounds i64* %28, i32 3
  %36 = load i64* %35
  store i64 %36, i64* getelementptr (i64* inttoptr (i64 22247328 to i64*), i32 3)
  %37 = getelementptr inbounds i64* %28, i32 2
  %38 = load i64* %37
  store i64 %38, i64* inttoptr (i64 22246256 to i64*)
  call void @joiner(i64 22280736, i64* inttoptr (i64 22246256 to i64*), i32 1, i64* inttoptr (i64 22247328 to i64*), i32 4)
  %39 = add i64 %26, i32 1
  store i64 %39, i64* %loopVar8
  %40 = icmp slt i64 %39, %18
  br i1 %40, label %loopBody9, label %blockloader5

condCheck11:                                      ; preds = %loopBody12, %afterblockloader6
  %41 = load i64* %loopVar10
  %42 = icmp slt i64 %41, %23
  br i1 %42, label %loopBody12, label %afterloop13

loopBody12:                                       ; preds = %condCheck11
  %43 = load i64* %loopVar10
  %44 = getelementptr inbounds i64** %22, i64 %43
  %45 = load i64** %44
  %46 = getelementptr inbounds i64* %45, i32 4
  %47 = load i64* %46
  %48 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @2, i32 0, i32 0), i64 %47)
  %49 = getelementptr inbounds i64* %45, i32 1
  %50 = load i64* %49
  %51 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @2, i32 0, i32 0), i64 %50)
  %52 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @4, i32 0, i32 0))
  %53 = add i64 %43, 1
  store i64 %53, i64* %loopVar10
  br label %condCheck11

afterloop13:                                      ; preds = %condCheck11
  %54 = call i64 @schemaController(i64 22285248, i64 3)
  ret i32 0
}

declare void @hasher(i64, i64, i32, i64, i32)

declare void @joiner(i64, i64, i32, i64, i32)

declare i64 @schemaController(i64, i32)

Compilation successful, now executing...

ALGERIA|AFRICA|
ARGENTINA|AMERICA|
BRAZIL|AMERICA|
CANADA|AMERICA|
EGYPT|MIDDLE EAST|
ETHIOPIA|AFRICA|
FRANCE|EUROPE|
GERMANY|EUROPE|
INDIA|ASIA|
INDONESIA|ASIA|
IRAN|MIDDLE EAST|
IRAQ|MIDDLE EAST|
JAPAN|ASIA|
JORDAN|MIDDLE EAST|
KENYA|AFRICA|
MOROCCO|AFRICA|
MOZAMBIQUE|AFRICA|
PERU|AMERICA|
CHINA|ASIA|
ROMANIA|EUROPE|
SAUDI ARABIA|MIDDLE EAST|
VIETNAM|ASIA|
RUSSIA|EUROPE|
UNITED KINGDOM|EUROPE|
UNITED STATES|AMERICA|

valkyrie> 

```
