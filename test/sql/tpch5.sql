SELECT
	nation.name,
	lineitem.extendedprice, lineitem.discount
FROM
	region,
    nation,
    customer,
    orders,
    lineitem,
	supplier
WHERE
	customer.custkey = orders.custkey
	and lineitem.orderkey = orders.orderkey
	and lineitem.suppkey = supplier.suppkey
	and customer.nationkey = supplier.nationkey
	and supplier.nationkey = nation.nationkey
	and nation.regionkey = region.regionkey;