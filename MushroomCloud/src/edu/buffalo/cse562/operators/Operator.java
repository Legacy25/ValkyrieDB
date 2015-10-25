package edu.buffalo.cse562.operators;

import java.util.Map;

import net.sf.jsqlparser.expression.LeafValue;
import edu.buffalo.cse562.schema.Schema;

public interface Operator {

	public Schema getSchema();
	public void generateSchemaName();

	public void initialize();
	public LeafValue[] readOneTuple();
	public void reset();
	
	public Operator getLeft();
	public Operator getRight();
	
	public void setLeft(Operator o);
	public void setRight(Operator o);
	
	public Map<String, Object> getDetails();
	
}
