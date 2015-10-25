package edu.buffalo.cse562.operators;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import edu.buffalo.cse562.LeafValueComparator;
import edu.buffalo.cse562.schema.Schema;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.statement.select.OrderByElement;

public class OrderByOperator implements Operator {

	/*
	 * Order By Order
	 * 		Sorts the child relation
	 * 
	 * Constructor Variables
	 * 		Order By attributes list
	 * 		The child operator
	 * 
	 * Working Set Size - The size of the relation
	 */
	
	private Schema schema;			/* Schema for this table */
	
	
	private Operator child;			/* The child operator to be sorted */
	
	/* Holds the sorted relation in memory */
	private ArrayList<LeafValue[]> tempList;		
	
	/* Holds the index into tempList of the next tuple to be emitted */
	private int index;
	

	/* Holds the sort key attributes */
	private ArrayList<OrderByElement> arguments;
	

	public OrderByOperator(ArrayList<OrderByElement> arguments, Operator child) {
		this.arguments = arguments;
		this.child = child;			/* Schema is unchanged from the child's schema */
		
		schema = new Schema(child.getSchema());
		
		/* Set an appropriate table name, for book-keeping */
		generateSchemaName();
		
		/* Initializations */
		tempList = new ArrayList<LeafValue[]>();
		
		index = 0;
		
	}

	public void generateSchemaName() {
		child.generateSchemaName();
		schema.setTableName(child.getSchema().getTableName());
	}
	

	@Override
	public Schema getSchema() {
		return schema;
	}

	@Override
	public void initialize() {
		child.initialize();
		
		generateSchemaName();
		
		/* 
		 * Since this is an in-memory operation, initialize will load the
		 * entire child relation into memory before sorting and then sort
		 * it in memory
		 */
		sort();
		
	}

	@Override
	public LeafValue[] readOneTuple() {
		/* Emit the next tuple in tempList, if any */
		if(index < tempList.size()) {
			LeafValue[] ret = tempList.get(index);
			index++;
			return ret;
		}

		/* Reached end of tempList, no more tuples to return */
		return null;
	}

	@Override
	public void reset() {
		/* First set the index to 0 */
		index = 0;
		
		/* Then reset the child */
		child.reset();
	}

	
	private void sort() {
		/* The sort function, calls on the sorting routine */
		
		/* Load entire child relation into tempList */
		LeafValue[] next = child.readOneTuple();
		while (next != null) {
			tempList.add(next);
			next = child.readOneTuple();
		}
				
		/* Sort tempList using the sorting routine */
		Collections.sort(tempList, new LeafValueComparator(arguments, schema));
	}
	
	@Override
	public Operator getLeft() {
		return child;
	}

	@Override
	public Operator getRight() {
		return null;
	}
	
	@Override
	public void setLeft(Operator o) {
		child = o;
	}

	@Override
	public void setRight(Operator o) {

	}

	public ArrayList<OrderByElement> getArguments() {
		return arguments;
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		for(OrderByElement arg : arguments)
			expList.add(arg.toString());
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child.getDetails());
		map.put("TYPE", "ORDERBY");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}
