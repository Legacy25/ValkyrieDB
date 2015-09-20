CREATE TABLE LINEITEM (
        orderkey       INT REFERENCES ORDERS,
        partkey        INT,
        suppkey        INT REFERENCES SUPPLIERS,
        linenumber     INT,
        quantity       DECIMAL,
        extendedprice  DECIMAL,
        discount       DECIMAL,
        tax            DECIMAL,
        returnflag     CHAR(1),
        linestatus     CHAR(1),
        shipdate       DATE,
        commitdate     DATE,
        receiptdate    DATE,
        shipinstruct   CHAR(25),
        shipmode       CHAR(10),
        comment        VARCHAR(44),
        PRIMARY KEY (orderkey, linenumber)
    );
CREATE TABLE ORDERS (
        orderkey       INT,
        custkey        INT REFERENCES CUSTOMER,
        orderstatus    CHAR(1),
        totalprice     DECIMAL,
        orderdate      DATE,
        orderpriority  CHAR(15),
        clerk          CHAR(15),
        shippriority   INT,
        comment        VARCHAR(79),
        PRIMARY KEY (orderkey)
    );
CREATE TABLE PART (
        partkey         INT,
        name            VARCHAR(55),
        mfgr            CHAR(25),
        brand           CHAR(10),
        type            VARCHAR(25),
        size            INT,
        container       CHAR(10),
        retailprice     DECIMAL,
        comment         VARCHAR(23),
        PRIMARY KEY (partkey)
    ); 
CREATE TABLE CUSTOMER (
        custkey         INT,
        name            VARCHAR(25),
        address         VARCHAR(40),
        nationkey       INT REFERENCES NATION,
        phone           CHAR(15),
        acctbal         DECIMAL,
        mktsegment      CHAR(10),
        comment         VARCHAR(117),
        PRIMARY KEY     (custkey)
    );
CREATE TABLE SUPPLIER (
        suppkey         INT,
        name            CHAR(25),
        address         VARCHAR(40),
        nationkey       INT REFERENCES NATION,
        phone           CHAR(15),
        acctbal         DECIMAL,
        comment         VARCHAR(101),
        PRIMARY KEY (suppkey)
    );
CREATE TABLE PARTSUPP (
        partkey         INT REFERENCES PART,
        suppkey         INT REFERENCES SUPPLIER,
        availqty        INT,
        supplycost      DECIMAL,
        comment         VARCHAR(199),
        PRIMARY KEY (partkey, suppkey)
    ); 
CREATE TABLE NATION (
        nationkey    INT,
        name         CHAR(25),
        regionkey    INT REFERENCES REGION,
        comment      VARCHAR(152), 
        PRIMARY KEY (nationkey)
    );
CREATE TABLE REGION (
        regionkey    INT,
        name         CHAR(25),
        comment      VARCHAR(152),
        PRIMARY KEY (regionkey)
    );

SELECT  
        customer.custkey, 
        customer.name, 
        customer.acctbal,
        nation.name,
        customer.address,
        customer.phone,
        customer.comment,
        SUM(lineitem.extendedprice * (1 - lineitem.discount)) AS revenue
FROM    
        customer, 
        orders,
        lineitem,
        nation
WHERE   
        customer.custkey = orders.custkey
        AND   lineitem.orderkey = orders.orderkey
        AND   orders.orderdate >= DATE('1993-10-01')
        AND   orders.orderdate < DATE('1994-01-01')
        AND   lineitem.returnflag = 'R'
        AND   customer.nationkey = nation.nationkey
GROUP BY 
        customer.custkey, customer.name, customer.acctbal, customer.phone, nation.name, customer.address, customer.comment;