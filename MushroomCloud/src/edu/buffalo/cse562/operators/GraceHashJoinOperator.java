package edu.buffalo.cse562.operators;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.BinaryExpression;
import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LeafValue.InvalidLeaf;
import net.sf.jsqlparser.expression.operators.conditional.AndExpression;
import net.sf.jsqlparser.schema.Column;
import edu.buffalo.cse562.ParseTreeOptimizer;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;

public class GraceHashJoinOperator implements Operator {

	/*
	 * External Hash Join Operator
	 * 		Build hashes over both child relations,
	 * 		and join them in memory
	 * 
	 * Constructor Variables
	 * 		A where predicate to give the join clause
	 * 		Two child operators
	 * 
	 * Working Set Size - Size of the hash of the smaller table
	 */	
	
	private Schema schema;			/* Schema for this table */
	
	
	private Expression where;				/* The join clause */
	private Operator child1, child2;		/* The two relations to cross product */
	
	private int joinedLength;
	private ArrayList<LeafValue[]> tempList;		/* Temporary list that holds the joined tuples */
	
	/* Hashes for the children's keys */
	private HashMap<Long, ArrayList<LeafValue []>> hash;
	
	/* Boolean array that contains selected columns for both relations */
	private int selectedCols1;
	private int selectedCols2;
	private int child1Length;
	private int child2Length;
	
	public GraceHashJoinOperator(Expression where, Operator child1, Operator child2) {
		this.where =  where;
		this.child1 = child1;
		this.child2 = child2;
		
		tempList = new ArrayList<LeafValue[]>();
		hash = new HashMap<Long, ArrayList<LeafValue[]>>(1000000, (float) 0.5);
		
		buildSchema();
	}
	
	public void appendWhere(Expression where) {
		this.where = new AndExpression(this.where, where);
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
	public void generateSchemaName() {
		child1.generateSchemaName();
		child2.generateSchemaName();
		
		schema.setTableName(
				"(" +
				child1.getSchema().getTableName() +
				" \u2A1D" +
				" {" + where + "} " +
				child2.getSchema().getTableName() +
				")"
				);
		
	}
	
	@Override
	public Schema getSchema() {
		return schema;
	}
	
	@Override
	public void initialize() {
		/* First initialize the children */
		child1.initialize();
		child2.initialize();

		child1Length = child1.getSchema().getColumns().size();
		child2Length = child2.getSchema().getColumns().size();
		
		/* Initializations */
		selectedCols1 = -1;
		selectedCols2 = -1;
		joinedLength = child1Length + child2Length;
		
		/* Get the columns on which to build the key */
		getSelectedColumns();
		
		/* Build the hash on both tables */
		try {
			buildHash();
		} catch(OutOfMemoryError e) {
			e.printStackTrace();
		}

		/* Generate the temporary list of joined tuples */
		buildJoin();
		buildSchema();
		
	}
	
	private void getSelectedColumns() {
		
		ArrayList<Expression> clauseList = new ArrayList<Expression>();
		
		if(where instanceof AndExpression) {
			clauseList.addAll(ParseTreeOptimizer.splitAndClauses(where));
		}
		else {
			clauseList.add(where);
		}
		
		for(Expression clause : clauseList) {
			BinaryExpression binaryClause = (BinaryExpression) clause;
			Column left = (Column) binaryClause.getLeftExpression();
			Column right = (Column) binaryClause.getRightExpression();
			
			int pos = findInSchema(left, child1.getSchema());
			if(pos < 0) {
				selectedCols2 = findInSchema(left, child2.getSchema());
				selectedCols1 = findInSchema(right, child1.getSchema());
			}
			else {
				selectedCols1 = pos;
				selectedCols2 = findInSchema(right, child2.getSchema());
			}
			
		}
		
	}
	
	private int findInSchema(Column col, Schema schema) {

		ArrayList<ColumnWithType> columnList = schema.getColumns();
		for(int i=0; i<columnList.size(); i++) {
			if(columnList.get(i).getWholeColumnName().equalsIgnoreCase(col.getWholeColumnName()))
				return i;
		}
		
		return -1;
	}

	private void buildHash(){
		
		LeafValue[] next;

		while((next = child1.readOneTuple()) != null) {
			Long key = null;
			try {
				key = next[selectedCols1].toLong();
			} catch (InvalidLeaf e) {
				e.printStackTrace();
			}
			
			if(!hash.containsKey(key)) {
				ArrayList<LeafValue[]> toBeAdded = new ArrayList<LeafValue[]>();
				toBeAdded.add(next);
				hash.put(key, toBeAdded);
				continue;
			}

			hash.get(key).add(next);
		}

	}
	
	@Override
	public LeafValue[] readOneTuple() {
		if(tempList.isEmpty()) {
			/* 
			 * no more tuples to return
			 */
			return null; 
		}
		
		/* Temporary return tuple */
		LeafValue[] next = tempList.get(0);
		
		/* Remove tuple from tempList */
		tempList.remove(0);
		
		return next;
	}
	
	private void buildJoin(){
		
		LeafValue[] next = null;
		
		while((next = child2.readOneTuple()) != null) {
			Long key = null;
			try {
				key = next[selectedCols2].toLong();
			} catch (InvalidLeaf e) {
				e.printStackTrace();
			}
			
			
			ArrayList<LeafValue[]> matchedTuples = hash.get(key);
			
			if(matchedTuples == null) {
				continue;
			}
			
			for(LeafValue[] left : matchedTuples) {
				LeafValue[] joinedTuple = new LeafValue[joinedLength];
				for(int i=0; i<joinedLength; i++) {
					if(i < child1Length) {
						joinedTuple[i] = left[i];
					}
					else {
						joinedTuple[i] = next[i - child1Length];
					}
				}
				tempList.add(joinedTuple);
			}
			
		}

		/* Clear the caches, we don't need them anymore */
		hash.clear();
	}
	
	
	public void reset(){
		/* First reset the children */
		child1.reset();
		child2.reset();
		
		tempList.clear();
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
	
	public Expression getWhere() {
		return where;
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child1.getDetails());
		srcs.add(child2.getDetails());
		expList.add(where.toString());
		map.put("TYPE", "JOIN");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}
}
