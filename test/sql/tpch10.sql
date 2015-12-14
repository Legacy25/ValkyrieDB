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
        nation
WHERE   
        customer.custkey = orders.custkey
        AND   lineitem.orderkey = orders.orderkey
        AND   customer.nationkey = nation.nationkey;