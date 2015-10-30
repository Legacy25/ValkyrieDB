CREATE TABLE NATION (nationkey INT , name CHAR (25) , regionkey INT REFERENCES REGION, comment VARCHAR (152) , PRIMARY KEY (nationkey))
SELECT * FROM NATION;

 --CREATE TABLE LINEITEM (orderkey INT REFERENCES ORDERS, partkey INT REFERENCES PARTS, suppkey INT REFERENCES SUPPLIERS, linenumber INT , quantity DECIMAL , extendedprice DECIMAL , discount DECIMAL , tax DECIMAL , returnflag CHAR (1) , linestatus CHAR (1) , shipdate DATE , commitdate DATE , receiptdate DATE , shipinstruct CHAR (25) , shipmode CHAR (10) , comment VARCHAR (44) , PRIMARY KEY (orderkey, linenumber))
 --SELECT * FROM LINEITEM;

/* CREATE TABLE CUSTOMER (
        custkey         INT,
        name            VARCHAR(25),
        address         VARCHAR(40),
        nationkey       INT,
        phone           CHAR(15),
        acctbal         DECIMAL,
        mktsegment      CHAR(10),
        comment         VARCHAR(117)
    );
 Select * from Customer;*/