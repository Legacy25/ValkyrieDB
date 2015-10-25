package edu.buffalo.cse562.operators;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.LeafValue;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;

public class CrossProductOperator implements Operator {

	/*
	 * Cross Product Operator
	 * 		Iterate over each combination
	 * 		of tuples from each child and emit the
	 * 		result.
	 * 
	 * 		We do this by means of a flag. The flag
	 * 		indicates the status of the second child.
	 * 		Whenever we reach the last tuple of the
	 * 		second child, we set the flag to true, so
	 * 		that we know that it is time to reset it.
	 * 		
	 * 		Initially, the flag is set to true. We
	 * 		first get one tuple from child1, then check
	 * 		the flag, and if its true, we reset child2,
	 * 		get the next tuple of child2, emit the
	 * 		concatenation of the two, set the flag
	 * 		to false. For the next call to 
	 * 		readOneTuple() we check the flag, which is
	 * 		now false. So we don't reset child2, just
	 * 		get its next tuple.
	 * 
	 *		We go on like this, till the time that the
	 *		next child2 tuple is null. At this point we
	 *		set the flag as true, get the next child1
	 *		tuple, reset child2, get the first child2
	 *		tuple and emit the result.
	 *
	 * 		We continue like this, till there are no more
	 * 		child1 tuples.
	 * 
	 * Constructor Variables
	 * 		Two child operators
	 * 
	 * Working Set Size - 1
	 */
	
	private Schema schema;			/* Schema for this table */
	
	
	private Operator child1, child2;		/* The two relations to cross product */

	boolean flag;							/*The flag to keep track of child2 */
	
	/* 
	 * These need to have class scope to preserve values
	 * between calls to readOneTuple()
	 */
	LeafValue next1[];			
	LeafValue next2[];

	public CrossProductOperator(Operator child1, Operator child2) {
		this.child1 = child1;
		this.child2 = child2;
		
		/* Initializations */
		flag = true;
		next1 = null;
		next2 = null;
		
		/* Build the new schema */
		buildSchema();
	}
	
	private void buildSchema() {
		
		schema = new Schema();
		generateSchemaName();

		/*
		 * Combine the schemas of the children to
		 * form the new schema. 
		 */
		for(ColumnWithType c:child1.getSchema().getColumns()) {
			schema.addColumn(c);
		}
		
		for(ColumnWithType c:child2.getSchema().getColumns()) {
			schema.addColumn(c);
		}
		
	}
	
	public void generateSchemaName() {
		child1.generateSchemaName();
		child2.generateSchemaName();
		
		schema.setTableName(
				child1.getSchema().getTableName() +
				" X " +
				child2.getSchema().getTableName()
		);
	}

	@Override
	public void initialize() {
		child1.initialize();
		child2.initialize();
		buildSchema();
		generateSchemaName();
	}
	
	@Override
	public LeafValue[] readOneTuple() {
		
		/*
		 * For an explanation of this code, see the first comment
		 */
		if(flag) {
			flag = false;
			if(next1 == null)
				next1 = child1.readOneTuple();
			if(next1 == null)
				return null;
		}

		next2 = child2.readOneTuple();
		if(next2 == null) {
			flag = true;
			child2.reset();
			next1 = child1.readOneTuple();
			next2 = child2.readOneTuple();
		}
		
		if(next1 == null || next2 == null)
			return null;
		
		/*
		 * Possibly, can be optimized, since we need to calculate only once,
		 * but kept here to preserve code clarity 
		 */
		int length = next1.length + next2.length;
		
		LeafValue ret[] = new LeafValue[length];
		for(int i=0; i<length; i++) {
			if(i<next1.length)
				ret[i] = next1[i];
			else
				ret[i] = next2[i-next1.length];
		}
		
		return ret;
	}

	@Override
	public void reset() {
		/* First reset the flag */
		flag = true;
		
		/* Then reset both children */
		child1.reset();
		child2.reset();
	}

	@Override
	public Schema getSchema() {
		return schema;
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
		map.put("TYPE", "PRODUCT");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}
