package edu.buffalo.cse562.schema;

public class ColumnInfo {
	
	/*
	 * A helper class to contain information necessary for the TypeCache
	 * optimizations in several Operators
	 */
	
	public String type;
	public int pos;
	
	
	public ColumnInfo(String type, int pos) {
		this.type = type;
		this.pos = pos;
	}
	
}