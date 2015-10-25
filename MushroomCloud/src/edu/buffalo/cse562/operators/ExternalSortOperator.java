package edu.buffalo.cse562.operators;

import java.util.ArrayList;

import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.statement.select.OrderByElement;
import edu.buffalo.cse562.schema.Schema;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import edu.buffalo.cse562.LeafValueComparator;
import edu.buffalo.cse562.Main;
import net.sf.jsqlparser.expression.DateValue;
import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.LongValue;
import net.sf.jsqlparser.expression.StringValue;

public class ExternalSortOperator implements Operator {

	/*
	 * External Sort Operator
	 * 		Sorts the child relation without blocking
	 * 
	 * Constructor Variables
	 * 		Order By attributes list
	 * 		The child operator
	 * 
	 * Working Set Size - Memory buffer size
	 */
	private Schema schema;				/* Schema for this table */

	private Operator child;				/* The child operator to be sorted */


	/* Holds the sort key attributes */
	private ArrayList<OrderByElement> arguments;
	
	/* Holds the sorted relation in memory temporarily */
	private ArrayList<LeafValue[]> tempList;

	/* For book-keeping	 */
	private HashMap<Integer, String> tempFileMap;
	private ArrayList<BufferedReader> filePointers;
	private String outputFileName;

	private boolean newlineFlag = false;
	
	/* Maximum number of rows at a time in memory, i.e. maximum rows in a swap file*/
	private int maxRows;

	LeafValueComparator lvc;
	BufferedReader br;

	
	
	public ExternalSortOperator(OrderByOperator o, int maxRows) {
		this.arguments = o.getArguments();
		this.child = o.getLeft();
		this.maxRows = maxRows;

		schema = new Schema(child.getSchema());

		/* Set an appropriate table name, for book-keeping */
		generateSchemaName();
		

		tempList = new ArrayList<LeafValue[]>();
		tempFileMap = new HashMap<Integer, String>();
		filePointers = new ArrayList<BufferedReader>();

		br = null;
	}

	@Override
	public Schema getSchema() {
		return schema;
	}

	@Override
	public void generateSchemaName() {
		child.generateSchemaName();
		schema.setTableName("O{ext} (" + child.getSchema().getTableName() + ")");
	}

	@Override
	public void initialize() {
		child.initialize();
		schema = new Schema(child.getSchema());

		/* Set an appropriate table name, for book-keeping */
		generateSchemaName();
		
		lvc = new LeafValueComparator(arguments, schema);
		outputFileName = Integer.valueOf(Main.FILE_UUID++).toString()+"_output";
		
		partitionAndSortData();
		
		try {
			mergeSortedPartitions();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	private void partitionAndSortData() {
		
		String fileName = "", tuple = "";
		int j = 0, counter = 0;

		LeafValue[] next = child.readOneTuple();
		
		while (next != null) {
			while ((j < maxRows) && (next != null)) {
				tempList.add(next);
				next = child.readOneTuple();
				j++;
			}

			if (j == maxRows || next == null) {
				/* Sort tempList using the sorting routine */
				Collections.sort(tempList, new LeafValueComparator(arguments, schema));
				
				fileName = Integer.valueOf(Main.FILE_UUID++).toString()
						+ "_block_"
						+ counter;
				
				BufferedWriter bw = null;
				
				try {
					File f = new File(Main.SWAP, fileName);
					bw = new BufferedWriter(new FileWriter(f));
				} catch (IOException e) {
					e.printStackTrace();
				}
				
				Iterator<LeafValue[]> i = tempList.iterator();
				while (i.hasNext()) {
					LeafValue[] lv = i.next();
					tuple = serializeTuple(lv) + (i.hasNext() ? "\n" : "");
					try {
						bw.write(tuple);
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
				
				tempList.clear();
				j = 0;
				tempFileMap.put(counter, fileName);
				
				counter++;
				
				try {
					bw.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		tempList.clear();
		
		/* 
		 * Reset the child since we read it
		 * This preserves the semantics of the Operator interface
		 */
		child.reset();
	}

	private String serializeTuple(LeafValue[] leafvalue) {
		
		if(leafvalue.length == 0)
			return "";
		
		StringBuilder row = new StringBuilder(500);
		row.append(leafvalue[0].toString());
		
		for(int i=1; i<leafvalue.length; i++) {
			row.append('|');
			row.append(leafvalue[i].toString());
		}
		
		return row.toString();
	}

	private LeafValue[] unSerializeTuple(String row) {
		
		String tokens[] = row.split("\\|");
		String type = "";

		LeafValue[] tuple = new LeafValue[tokens.length];
		
		for (int i=0; i<tokens.length; i++) {
			type = schema.getColumns().get(i).getColumnType();
			
			switch(type) {
			case "int":
				tuple[i] = new LongValue(tokens[i]);
				break;

			
			case "decimal":
				tuple[i] = new DoubleValue(tokens[i]);
				break;

			case "char":
			case "varchar":
			case "string":
				/* Blank spaces are appended to account for JSQLParser's weirdness */
				tuple[i] = new StringValue(tokens[i]);
				break;

			case "date":
				/* Same deal as string */
				tuple[i] = new DateValue(" "+tokens[i]+" ");
				break;
			default:
				System.err.println("Unknown column type");
			}

		}
		return tuple;
	}

	private void mergeSortedPartitions() throws IOException {
		
		openFilePointers();
		
		LeafValue[][] tempList = new LeafValue[filePointers.size()][];
		ArrayList<LeafValue[]> outputBuffer = new ArrayList<LeafValue[]>();
		
		int finished = 0;
		
		for(int i=0; i<filePointers.size(); i++) {
			tempList[i] = unSerializeTuple(filePointers.get(i).readLine());
		}
		
		while(finished < filePointers.size()) {
			int minPos = findMinTuple(tempList);
			
			outputBuffer.add(tempList[minPos]);
			
			String nextRow = filePointers.get(minPos).readLine();
			if(nextRow == null) {
				finished++;
				tempList[minPos] = null;
			}
			else {
				tempList[minPos] = unSerializeTuple(nextRow);
			}
			
			if(outputBuffer.size() == maxRows) {
				
				flush(outputBuffer);
				outputBuffer.clear();
			}
		}

		flush(outputBuffer);
		outputBuffer.clear();
		
		closeFilePointers();
		
	}

	private void flush(ArrayList<LeafValue[]> outputBuffer) {

		BufferedWriter bw = null;
		
		try {
			bw = new BufferedWriter(
					new FileWriter(
							new File(Main.SWAP, outputFileName), true
							)
					);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		if(newlineFlag) {
			try {
				bw.write("\n");
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		for(int i=0; i<outputBuffer.size(); i++) {
			String tuple = serializeTuple(outputBuffer.get(i));
			try {
				bw.write(i == outputBuffer.size() - 1 ? tuple : tuple + "\n");
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		if(!newlineFlag) {
			newlineFlag = true;
		}
		
		try {
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private int findMinTuple(LeafValue[][] list) {
		
		int minPos = -1;
		
		LeafValue[] min = list[0];
		if(min != null)
			minPos = 0;
		
		for(int i=0; i<list.length; i++) {
			if(min == null && list[i] != null) {
				min = list[i];
				minPos = i;
			}
			else if(min != null && list[i] != null) {
				if(lvc.compare(min, list[i]) > 0) {
					min = list[i];
					minPos = i;
				}
			}
		}
		
		return minPos;
	}

	private void openFilePointers() {

		for (int i=0; i<tempFileMap.size(); i++) {
			try {
				br = new BufferedReader(new FileReader(new File(Main.SWAP, tempFileMap.get(i))));
				filePointers.add(br);
				br = null;
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
		}
	}

	private void closeFilePointers() {
		
		for (int i=0; i<filePointers.size(); i++) {
			try {
				filePointers.get(i).close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		filePointers.clear();
	}


	
	
	@Override
	public LeafValue[] readOneTuple() {
		String row = "";
		
		if (br == null) {
			try {
				br = new BufferedReader(new FileReader(new File(Main.SWAP, outputFileName)));
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
		}

		try {
			if ((row = br.readLine()) != null) {
				return unSerializeTuple(row);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;


	}

	@Override
	public void reset() {
		child.reset();
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
		this.child = o;
	}

	@Override
	public void setRight(Operator o) {
		/* Nothing to do */
	}

	@Override
	public Map<String, Object> getDetails() {
		Map<String, Object> map = new HashMap<String, Object>();
		List<Object> expList = new ArrayList<Object>();
		List<Object> srcs = new ArrayList<Object>();
		for(OrderByElement arg : arguments)
			expList.add(arg.toString());
		srcs.add(child.getDetails());
		map.put("TYPE", "SORT");
		map.put("EXPRESSION", expList);
		map.put("SRC", srcs);
		return map;
	}

}
