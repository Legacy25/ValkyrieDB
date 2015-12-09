/*SELECT
  lineitem.orderkey,
  sum(lineitem.extendedprice*(1-lineitem.discount)) as revenue, 
  orders.orderdate,
  orders.shippriority
FROM
  customer,
  orders,
  lineitem 
WHERE
  customer.mktsegment = 'BUILDING' and customer.custkey = orders.custkey
  and lineitem.orderkey = orders.orderkey 
  and orders.orderdate < DATE('1995-03-15')
  and lineitem.shipdate > DATE('1995-03-15')
GROUP BY lineitem.orderkey, orders.orderdate, orders.shippriority 
ORDER BY revenue desc, orders.orderdate;*/

select name, nationkey from nation, region where nation.regionkey = region.regionkey;
--select nation.name, region.name from nation, region where nation.regionkey = region.regionkey;
