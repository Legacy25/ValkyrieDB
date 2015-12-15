SELECT
  orders.orderkey,
  orders.totalprice,
  lineitem.extendedprice, 
  lineitem.discount, 
  lineitem.quantity,
  customer.name
FROM
  customer,
  orders,
  lineitem 
WHERE
  customer.custkey = orders.custkey
  and lineitem.orderkey = orders.orderkey
  and orders.totalprice > 460000.0;
