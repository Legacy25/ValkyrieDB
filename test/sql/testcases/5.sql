SELECT  
        customer.custkey, 
        customer.name, 
        customer.acctbal,
        nation.name,
        customer.address,
        customer.phone,
        customer.comment,
        lineitem.extendedprice, lineitem.discount
FROM    
        customer,
        orders,
        lineitem,
        supplier,
        nation, 
        region
WHERE   
        customer.custkey = orders.custkey
        AND   lineitem.orderkey = orders.orderkey
        AND   customer.nationkey = supplier.nationkey
        AND   supplier.nationkey = nation.nationkey 
        AND   nation.regionkey = region.regionkey 
        AND   region.name = 'ASIA'
        AND   orders.orderdate >= '1994-01-01'
        AND   orders.orderdate <  '1995-01-01';