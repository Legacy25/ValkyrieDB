package edu.buffalo.cse562.operators;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.LeafValue;
import edu.buffalo.cse562.schema.Schema;

public class UnionOperator implements Operator {
	
	/*
	 * Union Operator
	 * 		Scans over the first child operator,
	 * 		then scans over the second one
	 * 
	 * Constructor Variables
	 * 		Two child operators
	 * 
	 * Working Set Size - 1
	 */
	
	private Schema schema;			/* Schema for this table */
	
	
	private Operator child1, child2;		/* The two relations to the union */
	
	private boolean child1Done;		/* A flag to keep track of
										whether child1 is done or not */
	
	public UnionOperator(Operator child1, Operator child2) {
		this.child1 = child1;
		this.child2 = child2;
		
		/* Schema is unchanged from the child's schema */
		schema = new Schema(child1.getSchema());
		
		/* Set an appropriate table name, for book-keeping */
		generateSchemaName();

		/* Initially the flag is off course false */
		child1Done = false;
	}

	
	@Override
	public void initialize() {
		child1.initialize();
		child2.initialize();
		
		/* Schema is unchanged from the child's schema */
		schema = new Schema(child1.getSchema());
		
		/* Set an appropriate table name, for book-keeping */
		generateSchemaName();
	}


	@Override
	public void generateSchemaName() {
		child1.generateSchemaName();
		child2.generateSchemaName();
		
		schema.setTableName("(" + child1.getSchema().getTableName() + " U " + child2.getSchema().getTableName() + ")");
	}
	
	@Override
	public Schema getSchema() {
		return schema;
	}

	@Override
	public LeafValue[] readOneTuple() {
		
		if(!child1Done) {
			LeafValue[] next = child1.readOneTuple();
			if(next == null) {
				child1Done = true;
				
				/* 
				 * Safe to do this, because the recursion stack will
				 * grow by only one call, however big the input
				 */
				return readOneTuple();
			}
			
			return next;
		}
		
		/* 
		 * No need to check for null, since the return will anyway be
		 * null if the next value is null
		 */
		return child2.readOneTuple();
			
	}

	@Override
	public void reset() {
		/* First reset the flag */
		child1Done = false;
		
		/* Then reset the children */
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
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child1.getDetails());
		srcs.add(child2.getDetails());
		map.put("TYPE", "UNION");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}