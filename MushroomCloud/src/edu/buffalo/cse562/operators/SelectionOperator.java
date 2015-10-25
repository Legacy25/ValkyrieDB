package edu.buffalo.cse562.operators;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.BooleanValue;
import net.sf.jsqlparser.expression.DateValue;
import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import net.sf.jsqlparser.schema.Column;
import edu.buffalo.cse562.Eval;
import edu.buffalo.cse562.ParseTreeOptimizer;
import edu.buffalo.cse562.schema.ColumnInfo;
import edu.buffalo.cse562.schema.Schema;

public class SelectionOperator extends Eval implements Operator {

	/*
	 * Selection Operator
	 * 		Scans over the child operator and
	 * 		emits only the tuples matching
	 * 		the selection predicate represented
	 * 		by the where Expression
	 * 
	 * Constructor Variables
	 * 		The where clause
	 * 		The child operator
	 * 
	 * Working Set Size - 1
	 */
	
	private Schema schema;			/* Schema for this table */
	

	private Expression where;		/* Selection predicates */
	private Operator child;			/* Child operator */

	/* 
	 * next[] needs to be a class variable, because eval()
	 * needs access to it
	 */
	private LeafValue next[];
	
	/* 
	 * 
	 * TypeCache is a HashMap that enables
	 * optimization for the eval function,
	 * this cache enables constant time access
	 * to the type information for a Column,
	 * we do not need to iterate over all columns
	 * to find the type of the column
	 * for every single tuple,
	 * once we have built the cache while
	 * processing the first tuple, that information
	 * can be looked up in this Map.
	 * 
	 */
	private HashMap<Column, ColumnInfo> TypeCache;
	
	private ArrayList<Expression> clauseList; 
		
	
	
	public SelectionOperator(Expression where, Operator child) {
		this.where = where;
		this.child = child;
		
		/* Schema is unchanged from the child's schema */
		schema = new Schema(child.getSchema());
		
		/* Set an appropriate table name, for book-keeping */
		generateSchemaName();
		
		/* Initializations */
		TypeCache = new HashMap<Column, ColumnInfo>();
	}

	public void generateSchemaName() {
		child.generateSchemaName();
		schema.setTableName("\u03C3 {" + where + "}(" + child.getSchema().getTableName() + ")");
	}
	
	@Override
	public Schema getSchema() {
		return schema;
	}


	@Override
	public void initialize() {
		child.initialize();
		schema = child.getSchema();
		generateSchemaName();
		clauseList = ParseTreeOptimizer.splitAndClauses(where);
	}
	
	@Override
	public LeafValue[] readOneTuple() {
		
		/*
		 * Keep iterating over tuples till there are no more tuples
		 */
		while((next = child.readOneTuple()) != null) {
			BooleanValue test = null;
			try {
				/*
				 * Test the validity of the selection predicate
				 * for the current tuple, represented by next[]
				 */
				for(int i=0; i<clauseList.size(); i++) {
					test = (BooleanValue) eval(clauseList.get(i));
					if(!test.getValue())
						break;
				}
				
				
			} catch (SQLException e) {
				e.printStackTrace();
			}
			
			/*
			 * If the predicate is true, emit this tuple, 
			 * otherwise continue to the next tuple
			 */
			if(test.getValue()) {
				return next;
			}
		}
		
		/* No more tuples, so return null */
		return null;
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
		String type = null;
		int pos = 0;
		
		
		if(TypeCache.containsKey(arg0)) {
			/*
			 * Cache already contains the information, just need
			 * to locate it
			 */
			type = TypeCache.get(arg0).type;
			pos = TypeCache.get(arg0).pos;
		}
		else {
			/*
			 * This will happen for the first tuple only,
			 * we generate the ColumnInfo for this Column
			 * and store it in TypeCache, so that we do not
			 * need to go through this for every subsequent
			 * tuple
			 */
			for(int i=0; i<schema.getColumns().size(); i++) {
				/* 
				 * Loop over all columns and 
				 * try to find a column with
				 * the same name as arg0
				 */
				if(arg0.getWholeColumnName().equalsIgnoreCase(schema.getColumns().get(i).getWholeColumnName().toString())
						|| arg0.getWholeColumnName().equalsIgnoreCase(schema.getColumns().get(i).getColumnName().toString())) {
					type = schema.getColumns().get(i).getColumnType();
					pos = i;
					TypeCache.put(arg0, new ColumnInfo(type, pos));
					break;
				}
			}
		}
		
		switch(type) {
		case "int":
			lv = (LongValue) next[pos];
			break;
		case "decimal":
			lv = (DoubleValue) next[pos];
			break;
		case "char":
		case "varchar":
		case "string":
			lv = (StringValue) next[pos];
			break;
		case "date":
			lv = new DateValue(next[pos].toString());
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
	
	public void setWhere(Expression where) {
		this.where = where;
	}
	
	public Expression getWhere() {
		return where;
	}
	
	@Override
	public void setLeft(Operator o) {
		child = o;
	}

	@Override
	public void setRight(Operator o) {

	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child.getDetails());
		expList.add(where.toString());
		map.put("TYPE", "SELECT");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}