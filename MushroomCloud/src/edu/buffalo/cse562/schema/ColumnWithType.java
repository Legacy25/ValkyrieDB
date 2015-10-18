package edu.buffalo.cse562.schema;

import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.schema.Table;

public class ColumnWithType extends Column {

	/*
	 * An augmentation of the Column class to include
	 * type information and the attribute order information
	 */
	
	private String columnType;
	private int columnNumber;

	
	public ColumnWithType(Table table, String columnName, String columnType, int columnNumber) {
		super(table, columnName);
		this.columnType = columnType;
		this.columnNumber = columnNumber;
	}
	
	public ColumnWithType(Column col, String columnType, int columnNumber) {
		super(col.getTable(), col.getColumnName());
		this.columnType = columnType;
		this.columnNumber = columnNumber;
	}

	public ColumnWithType(ColumnWithType col) {
		super(col.getTable(), col.getColumnName());
		this.columnType = col.columnType;
		this.columnNumber = col.columnNumber;
	}
	
	/* Getters and Setters */
	public String getColumnType() {
		return columnType;
	}

	public void setColumnType(String columnType) {
		this.columnType = columnType;
	}

	public int getColumnNumber() {
		return columnNumber;
	}

	public void setColumnNumber(int columnNumber) {
		this.columnNumber = columnNumber;
	}
	
	public Column getColumn() {
		return new Column(this.getTable(), this.getColumnName());
	}
	
	
	
	@Override
	public String toString() {
		return this.getColumn().getWholeColumnName();
	}
}
