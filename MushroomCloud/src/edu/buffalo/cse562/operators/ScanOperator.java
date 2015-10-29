package edu.buffalo.cse562.operators;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.sf.jsqlparser.expression.DateValue;
import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;
import edu.buffalo.cse562.schema.ColumnWithType;
import edu.buffalo.cse562.schema.Schema;

public class ScanOperator implements Operator {

	/*
	 * Scan Operator
	 * 		Scans over a file, known from the schema
	 * 		and emits a tuple as a LeafValue array
	 * 
	 * Constructor Variables
	 * 		A schema, representing the relation
	 * 		to be scanned
	 * 
	 * Working Set Size - 1
	 */
	
	private Schema schema;
	
	
	private BufferedReader br;
	private File f;
	private FileReader fr;

	
	private String line;
	
	public ScanOperator(Schema schema) {		
		/* 
		 * Get the initial schema,
		 * the schemas for the rest of the operators
		 * are ultimately generated
		 * using this information
		 */
		this.schema = new Schema(schema);
		/*
		 * Make a new File object using the file location
		 * information in the relation's schema. The
		 * initial schema is generated during parsetree
		 * creation
		 */
		f = new File(schema.getTableFile());
		
		line = null;
	}
	
	public void generateSchemaName() {
		
	}

	@Override
	public Schema getSchema() {
		return schema;
	}

	@Override
	public void initialize() {

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
		
		/* Split the tuple into attributes using the '|' delimiter */
		String cols[] = line.split("\\|");
		
		/* LeafValue array that will hold the tuple to be returned */
		LeafValue ret[] = new LeafValue[cols.length];
		
		/* 
		 * Iterate over each column according to schema's type information
		 * and populate the ret array with appropriate value and LeafValue
		 * type
		 */
		for(int i=0; i<cols.length; i++) {
			String type = schema.getColumns().get(i).getColumnType();
			
			switch(type) {
			case "int":
				ret[i] = new LongValue(cols[i]);
				break;
			
			case "decimal":
				ret[i] = new DoubleValue(cols[i]);
				break;
			
			case "char":
			case "varchar":
			case "string":
				/* Blank spaces are appended to account for JSQLParser's weirdness */
				ret[i] = new StringValue(" "+cols[i]+" ");
				break;

			case "date":
				/* Same deal as string */
				ret[i] = new DateValue(" "+cols[i]+" ");
				break;
			default:
				System.err.println("Unknown column type");
			}
		}
		
		/* Return the generated tuple */
		return ret;
	}

	@Override
	public void reset() {
		/* 
		 * Just initialize to reset the buffered reader,
		 * no other state information is maintained 
		 * that would need to be reset
		 * 
		 * The file is also closed so that we do not run
		 * out of file resources
		 */
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

	public String getString() {
		return line;
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		expList.add(schema.getTableName() + "=" + schema.getTableFile());
		for(ColumnWithType col : schema.getColumns())
			expList.add(col.getColumnName() + "=" + col.getColumnType());
		List<Object> srcs = new ArrayList<Object>();
		map.put("TYPE", "TABLE");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}