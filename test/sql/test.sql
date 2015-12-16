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

-- select * from lineitem where orderkey < 200;
-- select * from customer;
-- select nation.name, region.name from nation, region where nation.regionkey = region.regionkey;
-- select nation.name, region.name from nation, region where nation.regionkey = region.regionkey;

-- SELECT
--   orders.orderkey,
--   orders.totalprice,
--   lineitem.extendedprice, 
--   lineitem.discount, 
--   lineitem.quantity,
--   customer.name
-- FROM
--   customer,
--   orders,
--   lineitem 
-- WHERE
--   customer.custkey = orders.custkey
--   and lineitem.orderkey = orders.orderkey
--   and orders.totalprice > 460000.0;

--select * from customer where name = 'Customer#000000016';

SELECT orders.orderkey, orders.totalprice, lineitem.extendedprice, lineitem.discount, lineitem.quantity, customer.name
FROM customer, orders, lineitem
WHERE
    customer.custkey = orders.custkey
    and lineitem.orderkey = orders.orderkey
    and orders.totalprice > 460000.0
    AND orders.orderdate >= '1994-01-01'
    AND orders.orderdate <  '1995-01-01';