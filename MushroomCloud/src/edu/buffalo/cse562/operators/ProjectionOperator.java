package edu.buffalo.cse562.operators;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import edu.buffalo.cse562.Eval;
import edu.buffalo.cse562.schema.ColumnInfo;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;
import net.sf.jsqlparser.expression.DateValue;
import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import net.sf.jsqlparser.expression.LeafValue.InvalidLeaf;
import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.schema.Table;
import net.sf.jsqlparser.statement.select.AllColumns;
import net.sf.jsqlparser.statement.select.AllTableColumns;
import net.sf.jsqlparser.statement.select.SelectExpressionItem;
import net.sf.jsqlparser.statement.select.SelectItem;

public class ProjectionOperator extends Eval implements Operator {

	/*
	 * Projection Operator
	 * 		Scans over the child operator and
	 * 		projects only the columns represented
	 * 		by the List of selectItems
	 * 
	 * Constructor Variables
	 * 		A list of selected items
	 * 		The child operator
	 * 
	 * Working Set Size - 1
	 */
	
	private Schema schema;					/* Schema for this table */

	
	private List<SelectItem> selectItems;	/* The list of projections */
	private Operator child;					/* The child operator */
	
	
	private Schema childSchema;				/* The child's schema, required to
	 												build the new schema */	
	
	/* 
	 * next[] needs to be a class variable, because eval()
	 * needs access to it
	 */
	private LeafValue next[];
	
	/*
	 * TypeCache serves the same purpose as in
	 * the Selection Operator, refer to comments
	 * in the Selection Operator to learn more
	 */
	private HashMap<Column, ColumnInfo> TypeCache;
	
	
	public ProjectionOperator(List<SelectItem> selectItems, Operator child) {
		this.selectItems = selectItems;
		this.child = child;
		
		childSchema = child.getSchema();
		
		/* Initializations */
		TypeCache = new HashMap<Column, ColumnInfo>();
		
		/* Build the new schema */
		buildSchema();
	}
	
	private void buildSchema() {
		/*
		 * Generates the schema from the child schema and select-items list
		 * 
		 * Create a new schema
		 * and set an appropriate table name,
		 * for book-keeping
		 * 
		 * The TypeCache is also generated here
		 */
		schema = new Schema();
		generateSchemaName();
		
		/* k keeps track of the column we are about to add to the schema */
		int k = 0;
		
		ColumnWithType col = null;
		Iterator<SelectItem> i = selectItems.iterator();
		
		while(i.hasNext()) {
			SelectItem si = i.next();
			
			col = new ColumnWithType(
					new Table(),
					null, 
					null, 
					k);
			
			if(si instanceof SelectExpressionItem) {
				
				SelectExpressionItem sei = (SelectExpressionItem) si;
				Expression expr = sei.getExpression();
				Column arg0 = (Column) expr;
				
				col.setColumnName(expr.toString());
				if(sei.getAlias() != null) {
					col.setColumnName(sei.getAlias());
				}
				
				for(int j=0; j<childSchema.getColumns().size(); j++) {
					if(arg0.getWholeColumnName().equalsIgnoreCase(childSchema.getColumns().get(j).getWholeColumnName().toString())
							|| arg0.getWholeColumnName().equalsIgnoreCase(childSchema.getColumns().get(j).getColumnName().toString())) {
						
						col.setColumnType(childSchema.getColumns().get(j).getColumnType());
						col.setColumnNumber(k);			// Location in the new schema
						k++;
						
						TypeCache.put(
								arg0 ,
								new ColumnInfo(
										col.getColumnType() ,
										j				// Location in the old schema
										)
								);
						
						schema.addColumn(col);
						break;
					}
				}
			}
			
			else if(si instanceof AllTableColumns) {
				AllTableColumns atc = (AllTableColumns) si;
				Table t = atc.getTable();
				
				for(int j=0; j<childSchema.getColumns().size(); j++) {
					if(childSchema.getColumns().get(j).getTable().getName().equalsIgnoreCase(t.getName())) {
						
						col.setColumnType(childSchema.getColumns().get(j).getColumnType());
						col.setColumnNumber(k);
						k++;
						
						TypeCache.put(
								col.getColumn() ,
								new ColumnInfo(
										col.getColumnType() ,
										j
										)
								);
						
						schema.addColumn(col);
					}
				}
			}
			else if(si instanceof AllColumns) {
				for(ColumnWithType c : childSchema.getColumns()) {
					schema.addColumn(c);
				}
			}
			else {
				System.err.println("Unrecognized SelectItem)");
			}
		}
	}
	

	@Override
	public void generateSchemaName() {
		child.generateSchemaName();
		childSchema = child.getSchema();
		schema.setTableName("\u03C0(" + childSchema.getTableName() + ")");
	}
	

	@Override
	public Schema getSchema() {
		return schema;
	}
	

	@Override
	public void initialize() {
		child.initialize();
		
		childSchema = child.getSchema();
		
		/* Build the new schema */
		buildSchema();
	}
	
	@Override
	public LeafValue[] readOneTuple() {
		next = child.readOneTuple();
		if(next == null) {
			/*
			 * No more tuples
			 */
			return null;
		}
		
		LeafValue[] ret = new LeafValue[schema.getColumns().size()];
		
		/*
		 * Iterate over each select item and compute the 
		 * projection accordingly
		 */
		int k = 0;
		Iterator<SelectItem> i = selectItems.iterator();
		while(i.hasNext()) {
			
			SelectItem si = i.next();
			
			if(si instanceof SelectExpressionItem) {
			
				SelectExpressionItem sei = (SelectExpressionItem) si;
				Expression expr = sei.getExpression();
				
				try {
					ret[k] = eval(expr);
				} catch (SQLException e) {
					e.printStackTrace();
				}
				
			}
			
			else if(si instanceof AllTableColumns) {
				AllTableColumns atc = (AllTableColumns) si;
				Table t = atc.getTable();
				
				for(int j=0; j<childSchema.getColumns().size(); j++) {
					if(childSchema.getColumns().get(j).getTable().getName().equalsIgnoreCase(t.getName())) {
						ret[k] = next[j];
						k++;
					}
				}
				
				k--;
			}
			
			else if(si instanceof AllColumns) {
				ret = next;
				k += ret.length;
			}
			
			else {
				System.err.println("Unrecognized SelectItem");
			}
			
			k++;
		}
		
		return ret;			
	}

	@Override
	public void reset() {
		/* 
		 * Just need to reset the child,
		 * no other state information is kept,
		 * so nothing else to clean up
		 */
		child.reset();
	}
	
	

	@Override
	public LeafValue eval(Column arg0) throws SQLException {
		/* Necessary initializations */
		LeafValue lv = null;

		String type = TypeCache.get(arg0).type;
		int pos = TypeCache.get(arg0).pos;
		
		
		switch(type) {
		case "int":
			try {
				lv = new LongValue(next[pos].toLong());

			} catch (InvalidLeaf e) {
				System.err.println("Invalid column type for given function");
				e.printStackTrace();
				System.exit(1);
			}
			break;
		case "decimal":
			try {
				lv = new DoubleValue(next[pos].toDouble());
			} catch (InvalidLeaf e) {
				System.err.println("Invalid column type for given function");
				e.printStackTrace();
				System.exit(1);
			}
			break;
		case "char":
		case "varchar":
		case "string":
			lv = new StringValue(next[pos].toString());
			break;
		case "date":
			lv = new DateValue(" "+next[pos].toString()+" ");
			break;
		default:
			throw new SQLException();
		}
		
		return lv;
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
	
	public List<SelectItem> getSelectItems() {
		return selectItems;
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		for(SelectItem item : selectItems)
			expList.add(item.toString());
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child.getDetails());
		map.put("TYPE", "PROJECT");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}
	
}