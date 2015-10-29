package edu.buffalo.cse562.operators;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;

public class ProjectScanOperator implements Operator {

	private Schema oldSchema;
	private Schema newSchema;
	
	
	private HashSet<String> selectedColumnNames;
	private boolean[] selectedCols;
	
	
	private BufferedReader br;
	private File f;
	private FileReader fr;
	
	
	
	
	public ProjectScanOperator(Schema schema, HashSet<String> selectedColumnNames) {
		this.oldSchema = schema;
		this.selectedColumnNames = selectedColumnNames;
		
		
		f = new File(schema.getTableFile());
		
		selectedCols = new boolean[schema.getColumns().size()];
		Arrays.fill(selectedCols, false);
		
		buildSchema();
	}
	
	private void buildSchema() {
		newSchema = new Schema(oldSchema);
		newSchema.clearColumns();
		
		int i = 0;
		for(ColumnWithType c : oldSchema.getColumns()) {
			if(selectedColumnNames.contains(c.getColumnName().toLowerCase())) {
				newSchema.addColumn(c);
				selectedCols[i] = true;
			}
			
			i++;
		}
	}
	
	@Override
	public Schema getSchema() {
		return newSchema;
	}

	@Override
	public void generateSchemaName() {
		
	}

	@Override
	public void initialize() {

		buildSchema();
		
		try {
			fr = new FileReader(f);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		br = new BufferedReader(fr);	
		
		
	}

	@Override
	public LeafValue[] readOneTuple() {
		if(br == null) {
			/* 
			 * May happen if the operator is not 
			 * initialized before use
			 */
			return null;
		}

		String line = null;
		try {
			/* Read the next '|' delimited line which is the
			 * next tuple
			 */
			line = br.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		if(line == null) {
			/*
			 * No more tuples to be read
			 */
			return null;
		}
		
		return constructTuple(line);
		
		
	}

	public LeafValue[] constructTuple(String line) {
		/* Split the tuple into attributes using the '|' delimiter */
		String cols[] = line.split("\\|");
		
		/* LeafValue array that will hold the tuple to be returned */
		LeafValue ret[] = new LeafValue[newSchema.getColumns().size()];
		
		/* 
		 * Iterate over each column according to schema's type information
		 * and populate the ret array with appropriate value and LeafValue
		 * type
		 */
		int k = 0;
		for(int i=0; i<cols.length; i++) {
			if(selectedCols[i]) {
				String type = oldSchema.getColumns().get(i).getColumnType();
				
				switch(type) {
				case "int":
					ret[k] = new LongValue(cols[i]);
					break;
				
				case "decimal":
					ret[k] = new DoubleValue(cols[i]);
					break;
				
				case "char":
				case "varchar":
				case "string":
					/* Blank spaces are appended to account for JSQLParser's weirdness */
					ret[k] = new StringValue(" "+cols[i]+" ");
					break;

				case "date":
					/* Same deal as string */
					ret[k] = new StringValue(" "+cols[i]+" ");
					break;
				default:
					System.err.println("Unknown column type");
				}
				
				k++;
			}
		}
		
		/* Return the generated tuple */
		return ret;
	}
	
	@Override
	public void reset() {
		
		try {
			fr.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		initialize();
		
	}

	@Override
	public Operator getLeft() {
		return null;
	}

	@Override
	public Operator getRight() {
		return null;
	}

	@Override
	public void setLeft(Operator o) {
		
	}

	@Override
	public void setRight(Operator o) {
		
	}

	public Schema getOldSchema() {
		return oldSchema;
	}

	public HashSet<String> getSelectedColumns() {
		return selectedColumnNames;
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		expList.add(oldSchema.getTableName() + "=" + oldSchema.getTableFile());
		for(ColumnWithType col : oldSchema.getColumns())
			expList.add(col.getColumnName() + "=" + col.getColumnType());
		List<Object> srcs = new ArrayList<Object>();
		map.put("TYPE", "TABLE");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}
