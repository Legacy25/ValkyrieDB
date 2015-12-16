SELECT
        nation.name,
        lineitem.extendedprice,
        lineitem.discount
FROM
        orders,
        lineitem,
        supplier,
        nation,
        region
WHERE
        lineitem.orderkey = orders.orderkey
        AND   supplier.nationkey = nation.nationkey
        AND   nation.regionkey = region.regionkey
        AND   region.name = 'ASIA'
        AND   orders.orderdate >= '1994-01-01'
        AND   orders.orderdate <  '1995-01-01'
        AND   lineitem.suppkey = supplier.suppkey;