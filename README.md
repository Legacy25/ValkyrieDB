# ValkyrieDB
A SQL-to-native-code compiler based on LLVM

## Building and Running the Project
Using `make` should build both MushroomCloud and Valkyrie projects. The binaries will get built in a folder bin by the Makefile. 

For runnng the project use _ValkyrieDB.sh_ . This shell script runs the test.sql file present in test/sql folder.

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

This project can also be run in a commandline mode by running 
```
python valkyrie.py
```

Queries on tables present in the test/data folder should get executed.
```
valkyrie> select * from nation;
; ModuleID = 'LLVM'

@0 = private unnamed_addr constant [5 x i8] c"%lu|\00"
@1 = private unnamed_addr constant [5 x i8] c"%lf|\00"
@2 = private unnamed_addr constant [4 x i8] c"%s|\00"
@3 = private unnamed_addr constant [4 x i8] c"%s|\00"
@4 = private unnamed_addr constant [2 x i8] c"\0A\00"

declare i32 @printf(i8*, ...)

define i32 @llvmStart() {
entry:
  %loopVar0 = alloca i32
  store i32 0, i32* %loopVar0
  br label %loopBody1

loopBody1:                                        ; preds = %afterloop9, %entry
  %0 = load i32* %loopVar0
  %1 = getelementptr inbounds i64** inttoptr (i64 28841056 to i64**), i32 %0
  %2 = load i64** %1
  %loopVar2 = alloca i32
  store i32 0, i32* %loopVar2
  br label %loopBody3

loopBody3:                                        ; preds = %afterswitch4, %loopBody1
  %3 = load i32* %loopVar2
  %4 = getelementptr inbounds i32* inttoptr (i64 28821200 to i32*), i32 %3
  %5 = load i32* %4
  %6 = getelementptr inbounds i64* %2, i32 %3
  %7 = load i64* %6
  switch i32 %5, label %afterswitch4 [
    i32 1, label %longcase5
    i32 2, label %doublecase6
    i32 3, label %stringcase7
    i32 4, label %datecase8
  ]

afterswitch4:                                     ; preds = %datecase8, %stringcase7, %doublecase6, %longcase5, %loopBody3
  %8 = add i32 %3, 1
  store i32 %8, i32* %loopVar2
  %9 = icmp slt i32 %8, 4
  br i1 %9, label %loopBody3, label %afterloop9

longcase5:                                        ; preds = %loopBody3
  %10 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @0, i32 0, i32 0), i64 %7)
  br label %afterswitch4

doublecase6:                                      ; preds = %loopBody3
  %11 = uitofp i64 %7 to double
  %12 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @1, i32 0, i32 0), double %11)
  br label %afterswitch4

stringcase7:                                      ; preds = %loopBody3
  %13 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @2, i32 0, i32 0), i64 %7)
  br label %afterswitch4

datecase8:                                        ; preds = %loopBody3
  %14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @3, i32 0, i32 0), i64 %7)
  br label %afterswitch4

afterloop9:                                       ; preds = %afterswitch4
  %15 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @4, i32 0, i32 0))
  %16 = add i32 %0, 1
  store i32 %16, i32* %loopVar0
  %17 = icmp slt i32 %16, 25
  br i1 %17, label %loopBody1, label %afterloop10

afterloop10:                                      ; preds = %afterloop9
  ret i32 0
}

PRINT[]
	PROJECT[*]
		TABLE[NATION=/home/vinayak/CourseWork/Databases_and_Programming_Languages/Project/ValkyrieDB/./test/data/NATION.tbl, nationkey=int, name=char, regionkey=int, comment=varchar]


Compilation successful, now executing...

0|ALGERIA|0| haggle. carefully final deposits detect slyly agai|
1|ARGENTINA|1|al foxes promise slyly according to the regular accounts. bold requests alon|
2|BRAZIL|1|y alongside of the pending deposits. carefully special packages are about the ironic forges. slyly special |
3|CANADA|1|eas hang ironic, silent packages. slyly regular packages are furiously over the tithes. fluffily bold|
4|EGYPT|4|y above the carefully unusual theodolites. final dugouts are quickly across the furiously regular d|
5|ETHIOPIA|0|ven packages wake quickly. regu|
6|FRANCE|3|refully final requests. regular, ironi|
7|GERMANY|3|l platelets. regular accounts x-ray: unusual, regular acco|
8|INDIA|2|ss excuses cajole slyly across the packages. deposits print aroun|
9|INDONESIA|2| slyly express asymptotes. regular deposits haggle slyly. carefully ironic hockey players sleep blithely. carefull|
10|IRAN|4|efully alongside of the slyly final dependencies. |
11|IRAQ|4|nic deposits boost atop the quickly final requests? quickly regula|
12|JAPAN|2|ously. final, express gifts cajole a|
13|JORDAN|4|ic deposits are blithely about the carefully regular pa|
14|KENYA|0| pending excuses haggle furiously deposits. pending, express pinto beans wake fluffily past t|
15|MOROCCO|0|rns. blithely bold courts among the closely regular packages use furiously bold platelets?|
16|MOZAMBIQUE|0|s. ironic, unusual asymptotes wake blithely r|
17|PERU|1|platelets. blithely pending dependencies use fluffily across the even pinto beans. carefully silent accoun|
18|CHINA|2|c dependencies. furiously express notornis sleep slyly regular accounts. ideas sleep. depos|
19|ROMANIA|3|ular asymptotes are about the furious multipliers. express dependencies nag above the ironically ironic account|
20|SAUDI ARABIA|4|ts. silent requests haggle. closely express packages sleep across the blithely|
21|VIETNAM|2|hely enticingly express accounts. even, final |
22|RUSSIA|3| requests against the platelets use never according to the quickly regular pint|
23|UNITED KINGDOM|3|eans boost carefully special requests. accounts are. carefull|
24|UNITED STATES|1|y final packages. slow foxes cajole quickly. quickly silent platelets breach ironic accounts. unusual pinto be|
```