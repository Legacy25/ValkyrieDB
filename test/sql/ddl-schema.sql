create table Part(
	partkey INTEGER PRIMARY KEY, name, mfgr, brand, type, size INTEGER, container, retailprice real, comment);

create table Supplier(
	suppKey INTEGER PRIMARY KEY, name, address, nationkey INTEGER, phone, acctbal REAL, comment);

create table PartSupp(
	partKey INTEGER, suppKey INTEGER, availqty  INTEGER, supplycost REAL, comment);

create table Customer(CustKey INTEGER PRIMARY KEY, name, address, nationkey INTEGER, phone, acctbal REAL, mktsegment, comment);

create table Nation(
	nationkey  INTEGER PRIMARY KEY, name, regionkey INTEGER, comment);

create table Region(
	regionkey INTEGER PRIMARY KEY, name, comment);

create table LineItem(
	orderKey INTEGER, partKey INTEGER, suppKey INTEGER, lineNumber INTEGER, quantity INTEGER, extendedPrice REAL, discount REAL, tax REAL, returnFlag, lineStatus, shipDate, commitDate, receiptDate, shipInstruct, shipMode, comment);

create table Orders(
	orderKey INTEGER PRIMARY KEY, custKey INTEGER, orderStatus, totalPrice REAL, orderDate, orderPriority, clerk, shipPriority, comment);