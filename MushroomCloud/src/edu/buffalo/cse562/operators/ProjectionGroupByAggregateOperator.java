package edu.buffalo.cse562.operators;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Arrays;
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
import net.sf.jsqlparser.expression.Function;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import net.sf.jsqlparser.expression.LeafValue.InvalidLeaf;
import net.sf.jsqlparser.expression.operators.relational.ExpressionList;
import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.schema.Table;
import net.sf.jsqlparser.statement.select.AllTableColumns;
import net.sf.jsqlparser.statement.select.SelectExpressionItem;
import net.sf.jsqlparser.statement.select.SelectItem;

public class ProjectionGroupByAggregateOperator extends Eval implements Operator {

	/*
	 * Group-By-Aggregate-Projection Operator
	 * 		Scans over the child operator and does
	 * 		grouping, aggregation and projection
	 * 
	 * Constructor Variables
	 * 		A list of attributes to group on	
	 * 		A list of selected items
	 * 		The child operator
	 * 
	 * Working Set Size - Number of groups
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
	
	/* Counter variables and state required for grouping */
	private int index, maxpos, currentpos;
	private ArrayList<Double[]> sum, min, max, avg;
	private ArrayList<Integer[]> count;
	
	/* Contains mapping of seen values */
	private HashMap<String, Integer> seenValues;
	
	/* List of grouping attributes */
	private ArrayList<Column> columns;
	
	/* Boolean array for convenience */
	private boolean[] selectedCols;
	
	private ArrayList<LeafValue[]> output;
	
	public ProjectionGroupByAggregateOperator(ArrayList<Column> columns, List<SelectItem> selectItems, Operator child) {
		this.selectItems = selectItems;
		this.child = child;
		this.columns = columns;
		
		childSchema = child.getSchema();
		
		/*
		 * Initializations
		 */
		TypeCache = new HashMap<Column, ColumnInfo>();
		output = new ArrayList<LeafValue[]>();
		seenValues = new HashMap<String, Integer>();
		
		sum = new ArrayList<Double[]>();
		max = new ArrayList<Double[]>();
		min = new ArrayList<Double[]>();
		avg = new ArrayList<Double[]>();
		count = new ArrayList<Integer[]>();
		
		buildSchema();
	}

	private void getSelectedColumns() {

		selectedCols = new boolean[childSchema.getColumns().size()];
		Arrays.fill(selectedCols, false);
		
		ArrayList<ColumnWithType> schemaCols = childSchema.getColumns();
		
		for(int i=0; i<columns.size(); i++) {
			for(int j=0; j<schemaCols.size(); j++) {
				if(columns.get(i).getWholeColumnName().equalsIgnoreCase(schemaCols.get(j).getColumnName())
						|| columns.get(i).getWholeColumnName().equalsIgnoreCase(schemaCols.get(j).getWholeColumnName())) {
					selectedCols[j] = true;
				}
			}
		}
		
	}
	private void addCounterRow() {
		
		int size = selectItems.size();
		
		sum.add(new Double[size]);
		avg.add(new Double[size]);
		max.add(new Double[size]);
		min.add(new Double[size]);
		count.add(new Integer[size]);
		
		
	}
	private void resetCounters(int i) {

		Arrays.fill(count.get(i), 0);
		Arrays.fill(avg.get(i), new Double(0));
		Arrays.fill(max.get(i), new Double(0));
		Arrays.fill(min.get(i), new Double(0));
		Arrays.fill(sum.get(i), new Double(0));
	}
	private void initializeMinMax(int i) {
		
		int k = 0;
		Iterator<SelectItem> i1 = selectItems.iterator();
		while(i1.hasNext()) {
			
			SelectItem si = i1.next();
			
			if(si instanceof SelectExpressionItem) {
			
				SelectExpressionItem sei = (SelectExpressionItem) si;
				Expression expr = sei.getExpression();
				
				if(expr instanceof Function) {
					
					if(((Function) expr).getName().equalsIgnoreCase("MAX")
							|| ((Function) expr).getName().equalsIgnoreCase("MIN")) {
						try {
							min.get(i)[k] = max.get(i)[k] = eval(expr).toDouble();
						} catch (InvalidLeaf | SQLException e) {
							
						}
					}
				}
			}
			k++;
		}
		
	}
	private void generateOutput() {
		
		while((next = child.readOneTuple()) != null) {
			
			String key = "";
			
			for(int i=0; i<selectedCols.length; i++) {
				if(selectedCols[i]) {
					key += next[i].toString();
				}
			}
			
			Integer currentposObj = seenValues.get(key);
			if(currentposObj != null) {
				currentpos = currentposObj.intValue();
			}
			else {
				maxpos++;
				currentpos = maxpos;
				addCounterRow();
				initializeMinMax(currentpos);
				
				output.add(null);
				seenValues.put(key, currentpos);
				
				resetCounters(currentpos);
			}
			
			LeafValue[] ret = generateReturn();
			output.set(currentpos, ret);
		}
		
	}
	public LeafValue[] generateReturn() {
		LeafValue ret[] = new LeafValue[schema.getColumns().size()];
		
		int k = 0;
		Iterator<SelectItem> i = selectItems.iterator();
		while(i.hasNext()) {
			
			SelectItem si = i.next();
			
			if(si instanceof SelectExpressionItem) {
			
				SelectExpressionItem sei = (SelectExpressionItem) si;
				Expression expr = sei.getExpression();
				
				if(expr instanceof Function) {
					Function fun = (Function) expr;
					String funName = fun.getName();
					ExpressionList paramList = ((Function) expr).getParameters();
					Expression ex = null;
					if(paramList != null) {
						ex = (Expression) ((Function) expr).getParameters().getExpressions().get(0);
					}
					else {
						ex = childSchema.getColumns().get(0);
					}

					LeafValue reslv = null;
					try {
						reslv = eval(ex);
					} catch (SQLException e) {
						e.printStackTrace();
					}
					
					//===============================SUM=======================================
					if(funName.equalsIgnoreCase("SUM")) {
						
						try {
							sum.get(currentpos)[k] += reslv.toDouble();
						} catch (InvalidLeaf e) {
							System.exit(1);
						}
						ret[k] = new DoubleValue(sum.get(currentpos)[k]);

					}
					
					//===============================AVG=======================================
					else if(funName.equalsIgnoreCase("AVG")) {
						
						try {
							double res = reslv.toDouble();
							sum.get(currentpos)[k] += res;
							count.get(currentpos)[k]++;
							avg.get(currentpos)[k] = sum.get(currentpos)[k] / count.get(currentpos)[k];
						} catch (InvalidLeaf e) {
							System.exit(1);
						}
						ret[k] = new DoubleValue(avg.get(currentpos)[k]);		
						
					}
					
					//===============================COUNT=======================================
					else if(funName.equalsIgnoreCase("COUNT")) {
						count.get(currentpos)[k]++;
						ret[k] = new DoubleValue(count.get(currentpos)[k]);		
					}
					
					//===============================MIN=======================================
					else if(funName.equalsIgnoreCase("MIN")) {
						try {
							double res = reslv.toDouble();
							if(min.get(currentpos)[k] > res)
								min.get(currentpos)[k] = res;
						} catch (InvalidLeaf e) {
							System.exit(1);
						}
						ret[k] = new DoubleValue(min.get(currentpos)[k]);		
					}
					
					//===============================MAX=======================================
					else if(funName.equalsIgnoreCase("MAX")) {
						try {
							double res = reslv.toDouble();
							if(max.get(currentpos)[k] < res)
								max.get(currentpos)[k] = res;
						} catch (InvalidLeaf e) {
							System.exit(1);
						}
						ret[k] = new DoubleValue(max.get(currentpos)[k]);		
					}
				}
				else {
					try {
						ret[k] = eval(expr);
					} catch (SQLException e) {
						System.exit(1);
					}
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
			else {
				System.err.println("Unrecognized SelectItem)");
				System.exit(1);
			}
			
			k++;
		}
		
		return ret;
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
		TypeCache.clear();
		
		/* k keeps track of the column we are about to add to the schema */
		int k = 0;
		
		ColumnWithType col = null;
		Iterator<SelectItem> i = selectItems.iterator();
		
		while(i.hasNext()) {
			SelectItem si = i.next();
			if(si instanceof SelectExpressionItem) {
				
				col = new ColumnWithType(
						new Table(),
						null, 
						null, 
						k);
				
				SelectExpressionItem sei = (SelectExpressionItem) si;
				Expression expr = sei.getExpression();
				
				if(expr instanceof Function) {
					col.setColumnName(expr.toString());
					if(sei.getAlias() != null) {
						col.setColumnName(sei.getAlias());
					}
					
					col.setColumnType("decimal");		// Assume all functions compute double answers
					schema.addColumn(col);
				}
				else if (expr instanceof Column){
					Column arg0 = (Column) expr;
					col.setColumnName(arg0.getWholeColumnName());
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
			else {
				System.err.println("Unrecognized SelectItem)");
			}
		}
	}


	@Override
	public Schema getSchema() {
		return schema;
	}

	@Override
	public void initialize() {
		child.initialize();
		childSchema = child.getSchema();
		
		index = 0;
		maxpos = -1;
		currentpos = 0;

		seenValues.clear();
		
		getSelectedColumns();
		buildSchema();

		generateOutput();
	}
	
	@Override
	public LeafValue[] readOneTuple() {
		
		if(index < output.size()) {
			LeafValue[] ret = output.get(index);
			index++;
			return ret;
		}

		
		return null;
		
	}

	@Override
	public void reset() {
		/* First clean up state information, and reset the index */
		index = 0;
		
		/* Then reset the child operator */
		child.reset();
	}
	
	
	@Override
	public LeafValue eval(Column arg0) throws SQLException {
		
		LeafValue lv = null;
		String type = null;
		int pos = 0;
		
		if(TypeCache.containsKey(arg0)) {

			type = TypeCache.get(arg0).type;
			pos = TypeCache.get(arg0).pos;
		}
		else {

			for(int i=0; i<childSchema.getColumns().size(); i++) {
				
				if(arg0.getWholeColumnName().equalsIgnoreCase(childSchema.getColumns().get(i).getWholeColumnName().toString())
						|| arg0.getWholeColumnName().equalsIgnoreCase(childSchema.getColumns().get(i).getColumnName().toString())) {
					type = childSchema.getColumns().get(i).getColumnType();
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
			lv = (DateValue) new DateValue(next[pos].toString());
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
	public void generateSchemaName() {
		child.generateSchemaName();
		schema.setTableName("Agg [ " + child.getSchema().getTableName() + " ]");
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		for(SelectItem item : selectItems)
			expList.add(item.toString());
		List<Object> srcs = new ArrayList<Object>();
		srcs.add(child.getDetails());
		map.put("TYPE", "PROJECTGROUPBY");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}
}
