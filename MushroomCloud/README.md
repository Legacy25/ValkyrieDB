# SQL Evaluator
A SQL evaluator done as part of a graduate database class

This project translates SQL to an abstract syntax tree (AST) representation and then applies relational algebra rewrites to find an optimum query evaluation strategy. Efficient joins and external sort algorithms enable it to work with huge datasets even on limited memory.

# Build and run

To build, just run the build script -

```
> ./build.sh
```

Do not forget to add executable permission to the script using ```chmod +x```

This will compile all classes in the ```src``` directory and place them in an automatically created ```bin```directory.

To run, you can use the included run script -

```
> ./run.sh
```

# Sample data and queries

Several sample datasets and sql queries are included. These can be found in the ```data``` folder and ```sql`` folders respectively.

There is a set of small sanity check relations ```r, s, t``` comprising of just integer data. A bite-sized TPC-H dataset has been included to test TPC-H queries against. To generate a larger dataset please use a DBGen program for the TPC-H benchmark.

This is one that has been tested and works.
https://github.com/electrum/tpch-dbgen

# Syntax

```
> run.sh --data <path/to/data> [ --swap <path/to/swap> ] <sqlfile1> <sqlfile2> ...
```

The data flag points to the data directory. The swap flag points to a directory used for out of memory operations like external sorts.

# Example

```
> ./run.sh --data data sql/tpch1.sql 
A|F|3608|3617399.8|3415815.6154|3550622.3881|25.5887|25655.3177|0.0535|141
N|F|98|96050.28|93793.9484|94868.9501|32.6667|32016.76|0.0233|3
N|O|7917|7922719.62|7540013.3753|7850451.2831|25.9574|25976.1299|0.0487|305
R|F|3269|3260914.61|3079298.8793|3200628.9333|24.3955|24335.1837|0.0516|134
```

# Additional Flags

SQL-Parser supports a few additional flags to generate output and show debugging information. 

```--degug``` flag enables debugging information like generated query plans, optimized query plans and execution times.

```-q``` quiet mode, results are not output to the console.

```--fileout <dir>``` results are written to a file named ```<sqlfile-modulo-.sql>.out``` in the specified directory, which will be created if it doesn't exist.

```--time <dir>``` Query plan generation time and query execution times are written as a ```|``` separated tuple to a file named ```<sqlfile-modulo-.sql>.stat``` in the specified directory, which will be created if it doesn't exist.
