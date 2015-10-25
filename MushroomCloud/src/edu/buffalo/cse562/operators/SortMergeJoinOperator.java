package edu.buffalo.cse562.operators;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.BinaryExpression;
import net.sf.jsqlparser.expression.DateValue;
import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LeafValue.InvalidLeaf;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import net.sf.jsqlparser.expression.operators.conditional.AndExpression;
import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.statement.select.OrderByElement;
import edu.buffalo.cse562.LeafValueComparator;
import edu.buffalo.cse562.ParseTreeOptimizer;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;

public class SortMergeJoinOperator implements Operator {

	
	private Schema schema;
	
	private Expression where;
	
	private Operator child1, child2;
	
	
	ArrayList<OrderByElement> arguments1, arguments2;
	
	
	LeafValue[] next1, next2, prev1, prev2;
	ArrayList<LeafValue[]> temp1, temp2, tempList;
	LeafValueComparator lvc1, lvc2;

	public SortMergeJoinOperator(GraceHashJoinOperator ghjOp) {
		this.where = ghjOp.getWhere();
		this.child1 = ghjOp.getLeft();
		this.child2 = ghjOp.getRight();

		arguments1 = new ArrayList<OrderByElement>();
		arguments2 = new ArrayList<OrderByElement>();
		
		whereToOrderByArguments(where); 
		
		this.child1 = new OrderByOperator(arguments1, child1);
		this.child2 = new OrderByOperator(arguments2, child2);
		
		temp1 = new ArrayList<LeafValue[]>();
		temp2 = new ArrayList<LeafValue[]>();
		tempList = new ArrayList<LeafValue[]>();
		
		buildSchema();
	}
	
	private ArrayList<ArrayList<OrderByElement>> whereToOrderByArguments(
			Expression where) {

		ArrayList<Expression> clauseList = new ArrayList<Expression>();
		
		if(where instanceof AndExpression) {
			clauseList = ParseTreeOptimizer.splitAndClauses(where);
		}
		else {
			clauseList.add(where);
		}
		
		for(Expression e:clauseList) {
			BinaryExpression be = (BinaryExpression) e;
			Column left = (Column) be.getLeftExpression();
			Column right = (Column) be.getRightExpression();
			
			OrderByElement obe1 = new OrderByElement();
			obe1.setAsc(true);
			obe1.setExpression(left);
			
			OrderByElement obe2 = new OrderByElement();
			obe2.setAsc(true);
			obe2.setExpression(right);
			
			
			if(findInSchema(left, child1.getSchema()) >= 0) {
				arguments1.add(obe1);
				arguments2.add(obe2);
			}
			else {
				arguments1.add(obe2);
				arguments2.add(obe1);
			}
		}
		
		
		return null;
	}

	private int findInSchema(Column column, Schema schema) {

		ArrayList<ColumnWithType> columnList = schema.getColumns();
		for(int i=0; i<columnList.size(); i++) {
			if(columnList.get(i).getWholeColumnName().equalsIgnoreCase(column.getWholeColumnName()))
				return i;
		}
		
		return -1;
	}

	private void buildSchema() {
		
		schema = new Schema();
		generateSchemaName();
		
		for(ColumnWithType c:child1.getSchema().getColumns()) {
			schema.addColumn(c);
		}
		
		for(ColumnWithType c:child2.getSchema().getColumns()) {
			schema.addColumn(c);
		}
		
	}
	
	@Override
	public Schema getSchema() {
		return schema;
	}

	@Override
	public void generateSchemaName() {
		child1.generateSchemaName();
		child2.generateSchemaName();
		
		schema.setTableName(
				child1.getSchema().getTableName() +
				" \u2A1D..." +
				" {" + where + "} " +
				child2.getSchema().getTableName()
				);
	}
	
	
	@Override
	public void initialize() {
		child1.initialize();
		child2.initialize();

		buildSchema();
		generateSchemaName();
		
		lvc1 = new LeafValueComparator(arguments1, child1.getSchema());
		lvc2 = new LeafValueComparator(arguments2, child2.getSchema());
		
		prev1 = child1.readOneTuple();
		prev2 = child2.readOneTuple();
		
		next1 = child1.readOneTuple();
		next2 = child2.readOneTuple();
	}

	@Override
	public LeafValue[] readOneTuple() {
		
		LeafValue[] ret = null;
		
		if(tempList.size() > 0) {
			ret = tempList.get(tempList.size() - 1);
			tempList.remove(tempList.size() - 1);
			
			return ret;
		}
		
		temp1.clear();
		temp2.clear();
		
		while(prev1 != null && prev2!= null) {
			int prev1prev2Comp = compare(prev1, prev2);
			if(prev1prev2Comp == 0) {
				ret = joinAndEmit(prev1, prev2);
				
				boolean cond1 = next1 != null && compare(next1, prev2) == 0;
				boolean cond2 = next2 != null && compare(prev1, next2) == 0;
				
				if(cond1 && cond2) {
					while(prev1 != null && lvc1.compare(prev1, next1) == 0) {
						temp1.add(prev1);
						prev1 = next1;
						next1 = child1.readOneTuple();
					}
					temp1.add(prev1);
					prev1 = next1;
					next1 = child1.readOneTuple();
					
					while(prev2 != null && lvc2.compare(prev2, next2) == 0) {
						temp2.add(prev2);
						prev2 = next2;
						next2 = child2.readOneTuple();
					}
					temp2.add(prev2);
					prev2 = next2;
					next2 = child2.readOneTuple();
					
					for(int i=0; i<temp1.size(); i++) {
						for(int j=0; j<temp2.size(); j++) {
							if(i == 0 && j == 0)
								continue;
							tempList.add(joinAndEmit(temp1.get(i), temp2.get(j)));
						}
					}
				}
				else if(cond1) {
					prev1 = next1;
					next1 = child1.readOneTuple();
				}
				else if(cond2) {
					prev2 = next2;
					next2 = child2.readOneTuple();
				}
				else {
					prev1 = next1;
					prev2 = next2;
					next1 = child1.readOneTuple();
					next2 = child2.readOneTuple();
				}
				break;
			}
			else if(prev1prev2Comp > 0) {
				prev2 = next2;
				next2 = child2.readOneTuple();
			}
			else {
				prev1 = next1;
				next1 = child1.readOneTuple();
			}
		}
		
		return ret;
	}

	private LeafValue[] joinAndEmit(LeafValue[] left, LeafValue[] right) {
		int joinedLength = left.length + right.length;
		LeafValue[] joinedTuple = new LeafValue[joinedLength];
		
		for(int i=0; i<joinedLength; i++) {
			if(i < left.length) {
				joinedTuple[i] = left[i];
			}
			else {
				joinedTuple[i] = right[i - left.length];
			}
		}
		
		return joinedTuple;
	}

	private int compare(LeafValue[] left, LeafValue[] right) {
		for(int i=0; i<arguments1.size(); i++) {
			int pos1 = findInSchema((Column) arguments1.get(i).getExpression()
					, child1.getSchema());
			int pos2 = findInSchema((Column) arguments2.get(i).getExpression()
					, child2.getSchema());

			String type = "";
			LeafValue element = left[pos1];
			
			if(element instanceof LongValue) {
				type = "int";
			}
			else if(element instanceof DoubleValue) {
				type = "decimal";
			}
			else if(element instanceof StringValue) {
				type = "string";
			}
			else if(element instanceof DateValue) {
				type = "date";
			}
			
			switch(type) {
			case "int":
				try {
					if(left[pos1].toLong() == right[pos2].toLong())
						continue;
					if(left[pos1].toLong() > right[pos2].toLong()) {
						return 1;
					}
					else {
						return -1;
					}
				} catch (InvalidLeaf e) {
					break;
				}
			case "decimal":
				try {
					if(left[pos1].toDouble() == right[pos2].toDouble())
						continue;
					if(left[pos1].toDouble() > right[pos2].toDouble()) {
						return 1;
					}
					else {
						return -1;
					}
				} catch (InvalidLeaf e) {
					break;
				}
			case "string":
			case "date":
				if(left[pos1].toString().equalsIgnoreCase(right[pos2].toString())) {
					continue;
				}
				if(left[pos1].toString().compareToIgnoreCase(right[pos2].toString()) > 0) {
					return 1;
				}
				else {
					return -1;
				}
			default:
				/* Handle */
			}
		}
		return 0;
	}

	@Override
	public void reset() {
		child1.reset();
		child2.reset();
	}

	@Override
	public Operator getLeft() {
		return child1;
	}

	@Override
	public Operator getRight() {
		return child2;
	}

	@Override
	public void setLeft(Operator o) {
		child1 = o;
	}

	@Override
	public void setRight(Operator o) {
		child2 = o;
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		expList.add(where.toString());
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child1.getDetails());
		srcs.add(child2.getDetails());
		map.put("TYPE", "SORTMERGE");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}
