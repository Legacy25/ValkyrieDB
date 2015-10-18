package edu.buffalo.cse562;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;

import edu.buffalo.cse562.schema.Schema;
import net.sf.jsqlparser.expression.DateValue;
import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import net.sf.jsqlparser.expression.LeafValue.InvalidLeaf;
import net.sf.jsqlparser.statement.select.OrderByElement;

public class LeafValueComparator implements Comparator<LeafValue[]> {

	
	private Schema schema;
	
	/* Holds the sort key attributes */
	private ArrayList<OrderByElement> arguments;
	
	/* Holds the indexes of the sort key attributes */
	private HashMap<OrderByElement, Integer> columnIndexes; 

	
	public LeafValueComparator(ArrayList<OrderByElement> arguments ,
			Schema schema) {

		this.arguments = arguments;
		this.schema = schema;
		
		columnIndexes = new HashMap<OrderByElement, Integer>();
		
		findColumns();
		
	}
	
	
	/*
	 * Helper function to find the appropriate column indexes on which to sort
	 */
	private void findColumns() {
		
		for(OrderByElement o:arguments) {
			String columnName = o.getExpression().toString();
			
			for(int i=0; i<schema.getColumns().size(); i++) {
				if(schema.getColumns().get(i).getColumnName().toLowerCase().indexOf(columnName.toLowerCase()) >= 0
						|| columnName.toLowerCase().indexOf(schema.getColumns().get(i).getColumnName().toLowerCase()) >= 0) {
					
					columnIndexes.put(o, i);
					break;
					
				}
			}
		}
		
	}
	
	
	
	@Override
	public int compare(LeafValue[] o1, LeafValue[] o2) {
		String type = "";
		int returnVal = 0;
		
		for(int i=arguments.size()-1; i>=0; i--) {
			OrderByElement o = arguments.get(i);
			int column = columnIndexes.get(o);
			boolean isAsc = o.isAsc();
			LeafValue element = o1[column];
			
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
			case "decimal":
				try {
					if(o1[column].toDouble() == o2[column].toDouble())
						continue;
					if(o1[column].toDouble() > o2[column].toDouble()) {
						if(isAsc) {
							returnVal = 1;
						}
						else {
							returnVal = -1;
						}
					}
					else {
						if(isAsc) {
							returnVal = -1;
						}
						else {
							returnVal = 1;
						}
					}
					break;
				} catch (InvalidLeaf e) {
					break;
				}
			case "string":
			case "date":
				if(o1[column].toString().equalsIgnoreCase(o2[column].toString())) {
					continue;
				}
				if(o1[column].toString().compareToIgnoreCase(o2[column].toString()) > 0) {
					if(isAsc) {
						returnVal = 1;
					}
					else {
						returnVal = -1;
					}
				}
				else {
					if(isAsc) {
						returnVal = -1;
					}
					else {
						returnVal = 1;
					}
				}
				break;
			default:
				/* Handle */
			}
		}

		return returnVal;
	}
}
