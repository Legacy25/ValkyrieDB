package edu.buffalo.cse562;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.Function;
import net.sf.jsqlparser.parser.CCJSqlParser;
import net.sf.jsqlparser.parser.ParseException;
import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.schema.Table;
import net.sf.jsqlparser.statement.Statement;
import net.sf.jsqlparser.statement.create.table.ColumnDefinition;
import net.sf.jsqlparser.statement.create.table.CreateTable;
import net.sf.jsqlparser.statement.select.FromItem;
import net.sf.jsqlparser.statement.select.Join;
import net.sf.jsqlparser.statement.select.OrderByElement;
import net.sf.jsqlparser.statement.select.PlainSelect;
import net.sf.jsqlparser.statement.select.Select;
import net.sf.jsqlparser.statement.select.SelectBody;
import net.sf.jsqlparser.statement.select.SelectExpressionItem;
import net.sf.jsqlparser.statement.select.SelectItem;
import net.sf.jsqlparser.statement.select.SubJoin;
import net.sf.jsqlparser.statement.select.SubSelect;
import net.sf.jsqlparser.statement.select.Union;
import edu.buffalo.cse562.exceptions.UnsupportedStatementException;
import edu.buffalo.cse562.operators.DistinctOperator;
import edu.buffalo.cse562.operators.ProjectionGroupByAggregateOperator;
import edu.buffalo.cse562.operators.CrossProductOperator;
import edu.buffalo.cse562.operators.LimitOperator;
import edu.buffalo.cse562.operators.Operator;
import edu.buffalo.cse562.operators.OrderByOperator;
import edu.buffalo.cse562.operators.ProjectionOperator;
import edu.buffalo.cse562.operators.ScanOperator;
import edu.buffalo.cse562.operators.SelectionOperator;
import edu.buffalo.cse562.operators.UnionOperator;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;

public class ParseTreeGenerator {

	/* The tables HashMap keeps a mapping of tables to their corresponding schemas */
	private static ArrayList<Schema> tables = new ArrayList<Schema>();
	
	/* Function to find a table within the provided Data Directories */
	private static String findFile(ArrayList<String> dataDirs, String tableName) {
		for(String dDirs : dataDirs) {
			File dir = new File(dDirs);
			File files[] = dir.listFiles();
			for(File f : files) {
				if(f.getName().equalsIgnoreCase(tableName+".tbl"))
					return f.getAbsolutePath();
			}
		}
		return null;
	}
	
	
	private static Schema findSchema(String fileName) {

		for(int i=0; i<tables.size(); i++) {
			if(tables.get(i).getTableName().equalsIgnoreCase(fileName))
				return tables.get(i);
		}
		
		System.out.println("Couldn't find schema for table "+fileName);
		System.exit(-1);
		return null;
	}
	
	
	/* 
	 * The meat of Checkpoint 1, parses each statement
	 *  and generates a corresponding parse-tree
	 */
	public static Operator generate(ArrayList<String> dataDirs, File sqlFile) {
		
		Operator parseTree = null;
		Statement statement = null;
		
		try {
			CCJSqlParser parser = new CCJSqlParser(new FileReader(sqlFile));
			/*
			 * Keep looping till we get all statements.
			 * 
			 * Each time the parser gives us an object of a class that implements
			 * the Statement interface.
			 * 
			 * We only concern ourselves with two kinds of Statement - CreateTable & Select
			 */

			while((statement = parser.Statement()) != null) {
				/*
				 * CREATE TABLE Statement
				 * 
				 * We do not actually create a table, the object is to
				 * extract information to generate a schema, that can
				 * be used later to process SELECT queries
				 * 
				 */				

				if(statement instanceof CreateTable) {
					CreateTable cTable = (CreateTable) statement;
					
					String tableName = cTable.getTable().toString();
					String tableFile = findFile(dataDirs, tableName);
					
					if(tableFile == null) {
						System.err.println("Table "+ tableName + " not found in any "
								+ "of the specified directories!");
						System.exit(1);
					}
					
					/* Generate the schema for this table */
					Schema schema = new Schema(tableName, tableFile);
					@SuppressWarnings("unchecked")
					List<ColumnDefinition> columnDefinitions
								= cTable.getColumnDefinitions();
					
					int k = 0;
					for(ColumnDefinition colDef : columnDefinitions) {	
						String name = colDef.getColumnName().toLowerCase();
						String type = colDef.getColDataType().getDataType().toLowerCase();
						if(type.equalsIgnoreCase("integer")) {
							type = "int";
						}
						ColumnWithType c = new ColumnWithType(
								cTable.getTable(),
								name, 
								type,
								k
								);
						k++;
						schema.addColumn(c);
					}

					/* Store schema for later use */
					tables.add(schema);
				}
				
				/*
				 * SELECT Statement
				 * 
				 * This has a field of type SelectBody, which is an interface
				 * 
				 * SelectBody has two implementing classes, PlainSelect and Union 
				 */				
				else if(statement instanceof Select) {
				
					SelectBody body = ((Select) statement).getSelectBody();
					if(body instanceof PlainSelect) {
						PlainSelect ps = (PlainSelect) body;
						parseTree = parsePlainSelect(ps);
					}
					else if(body instanceof Union) {
						Union union = (Union) body;
						
						@SuppressWarnings("rawtypes")
						Iterator i = union.getPlainSelects().iterator();
						/* 
						 * A Union may have two or more PlainSelects. To deal
						 * with this we insert the first two PlainSelect statements first
						 * 
						 * Then we iterate over the rest(if any) PlainSelect
						 * statements, till there are no more to add
						 */
						parseTree =
									new UnionOperator(
											parsePlainSelect((PlainSelect) i.next()) , 
											parsePlainSelect((PlainSelect) i.next())
								);
						
						while(i.hasNext()) {
							parseTree =
										new UnionOperator(
												parseTree ,		// The union of the first two
												parsePlainSelect((PlainSelect) i.next())
									);
						}
					}
					
					/*
					 * Unsupported Statement
					 */					
					else {
						throw new UnsupportedStatementException();
					}
					return parseTree;
				}
			}
		} catch (FileNotFoundException | ParseException | UnsupportedStatementException e) {
			e.printStackTrace();
		}
		
		return parseTree;
		
	}
	

	@SuppressWarnings("unchecked")
	private static Operator parsePlainSelect(PlainSelect ps)
			throws UnsupportedStatementException {
		
		Operator parseTree = null;
	
		//==================FROM CLAUSE=================================
		
		FromItem fi = ps.getFromItem();
		
		
		/*
		 * FromItem is an interface, it has 3 classes -
		 * Table, SubJoin and SubSelect
		 * 
		 * We handle each differently
		 */
		if(fi instanceof Table) {
			Table table = (Table) fi;
			Schema schema = findSchema(table.getName().toString());
			
			// Handle alias if present
			if(fi.getAlias() != null) {
				schema.setTableName(fi.getAlias());
			}
			
			parseTree = new ScanOperator(schema);
		}
		
		if(fi instanceof SubJoin) {
			
		}
		
		if(fi instanceof SubSelect) {
			
			Operator subSelectTree;
			SelectBody body = ((SubSelect) fi).getSelectBody();
			
			if(body instanceof PlainSelect) {
				PlainSelect ps1 = (PlainSelect) body;
				subSelectTree = parsePlainSelect(ps1);
			}
			else if(body instanceof Union) {
				Union union = (Union) body;
				
				@SuppressWarnings("rawtypes")
				Iterator i = union.getPlainSelects().iterator();
				
				subSelectTree =
						new UnionOperator(
								parsePlainSelect(
										(PlainSelect) i.next()) , 
										parsePlainSelect((PlainSelect) i.next())
						);
				
				while(i.hasNext()) {
					subSelectTree =
							new UnionOperator(
									subSelectTree ,
									parsePlainSelect((PlainSelect) i.next())
							);
				}
			}
			
			/*
			 * Unsupported Statement
			 */
			
			else {
				throw new UnsupportedStatementException();
			}
			
			subSelectTree.getSchema().setTableName(fi.getAlias());
			parseTree = subSelectTree;
			
		}
		
		
		
		
		//====================JOINS====================================

		if(ps.getJoins() != null){
			@SuppressWarnings("rawtypes")
			Iterator i = ps.getJoins().iterator();
			while(i.hasNext()) {
				Join join = (Join) i.next();
				
				Operator right = 
						new ScanOperator(
								findSchema(join.getRightItem().toString())
						);
				
				parseTree =
						new CrossProductOperator(
								parseTree ,
								right
						);
				
				if(join.getOnExpression() != null) {
					parseTree =
							new SelectionOperator(
									join.getOnExpression() ,
									parseTree.getLeft()
							);										
				}
			}
		}
		
		
		
		
		//======================SELECTION=====================================
		
		if(ps.getWhere() != null) {
			parseTree =
					new SelectionOperator(
							ps.getWhere() ,
							parseTree
					);
		
		}						
		
		
		//======================AGGREGATE QUERIES=============================
		
		
		/*
		 * Check if the query is an aggregate query
		 * by seeing if either group by columns list
		 * is null or seeing if there are any
		 * functions in the expression list
		 * 
		 * 
		 */
		Boolean aggregateQuery = false;
		if(ps.getGroupByColumnReferences() != null)
			aggregateQuery = true;
		
		if(!aggregateQuery) {
			@SuppressWarnings("rawtypes")
			Iterator i = ps.getSelectItems().iterator();
			while(i.hasNext()) {
				SelectItem si = (SelectItem) i.next();
				
				if(si instanceof SelectExpressionItem) {
					SelectExpressionItem sei = (SelectExpressionItem) si;
					Expression expr = sei.getExpression();
					if(expr instanceof Function) {
						aggregateQuery = true;
						break;
					}
				}
			}							
		}
		
		/*
		 * If its an aggregate query, use the 
		 * group by aggregate operator
		 */
		if(aggregateQuery) {
			
			ArrayList<Column> selectedColumns = new ArrayList<Column>();
			
			if(ps.getGroupByColumnReferences() != null) {
				@SuppressWarnings("rawtypes")
				Iterator i = ps.getGroupByColumnReferences().iterator();
				while(i.hasNext()) {
					selectedColumns.add((Column) i.next());
				}
			}
			
			if(parseTree.getLeft() != null)
				parseTree = 
						new ProjectionGroupByAggregateOperator(
								selectedColumns ,
								ps.getSelectItems() ,
								parseTree
						);
			
		}
		
		
		
		//=======================HAVING========================================
		
		if(ps.getHaving() != null) {
			parseTree = 
					new SelectionOperator(
							ps.getHaving() ,
							parseTree
					);

		}
		
		
		
		//=====================PROJECTION======================================
		
		/*
		 * Only use projection for non-aggregate queries
		 */
		if(!aggregateQuery && ps.getSelectItems() != null) {
			parseTree = 
					new ProjectionOperator(
							ps.getSelectItems() ,
							parseTree
					);
		}
		
		
		
		
		//========================ORDER BY======================================
		
		if(ps.getOrderByElements() != null) {
			ArrayList<OrderByElement> obe = (ArrayList<OrderByElement>) ps.getOrderByElements();
			
			parseTree =
					new OrderByOperator(
							obe ,
							parseTree
					);

		}
		
		
		
		
		
		//=========================DISTINCT=====================================
		
		if(ps.getDistinct() != null) {
			parseTree = 
					new DistinctOperator(
							parseTree
					);
		}
		
		
		//=======================LIMIT==========================================
		
		if(ps.getLimit() != null) {
			parseTree =
					new LimitOperator(
							ps.getLimit().getRowCount() ,
							parseTree
					);
		}
		
		//======================================================================
		
		
		return parseTree;
		
		
	}
	
	
	public static ArrayList<Schema> getTableSchemas() {
		return tables;
	}
	
}
