/*
 * SQL Evaluator Engine
 * Authors: Arindam Nandi
 * 			Saptarshi Bhattacharjee
 * 			Sayaritra Pal
 * 
 * Spring 2015
 */

package edu.buffalo.cse562;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
//import java.util.function.Consumer;

import org.json.JSONObject;

import edu.buffalo.cse562.operators.Operator;

public class Main {	
	
	public static final int BILLION = 1000*1000*1000;
	
	/* 
	 * Stores the swap directory
	 * Need application-wide access to this, so a static global
	 */
	public static File SWAP = null;
	
	/*
	 * Initialize a file name counter required for bookkeeping
	 * in external k-way merge sort
	 */
	public static int FILE_UUID = 0;
	
	/*
	 * Controls the size of the I/O buffer used to retrieve
	 * records from file to memory. Varying this gives some
	 * interesting variations in query processing time
	 */
	public static int BLOCK = 10000;
	
	/*
	 * Provides application wide access to information 
	 * regarding whether there is a memory limit,
	 * this is of use to the parse-tree optimizer
	 * in choosing non-blocking operators
	 */
	public static boolean MEMORY_LIMITED = false;
	
	/*
	 * Enabled by setting the --debug flag on the CLI.
	 * Mainly used to print out various debugging info
	 * like query plans and processing times
	 */
	public static boolean DEBUG = false;
	
	/*
	 * --fileout flag redirects the results to files
	 * instead of the console
	 */
	public static boolean FILE_OUTPUT = false;
	public static File FILE_OUTPUT_DIR = null;
	
	/*
	 * --time flag prints out query execution times to a file
	 */
	public static boolean TIME_OUTPUT = false;
	public static File TIME_OUTPUT_DIR = null;
	
	
	/*
	 * -q Quiet mode
	 */
	public static boolean QUIET = false;
	
	/*
	 * Execute mode, either dumps a representation of the query plan 
	 * or executes the query
	 */
	public static boolean EXECUTE = false;

	public static boolean PUSH_DOWN_PROJECTIONS = false;
	
	public static void main(String[] args) {
		
		DEBUG = false;
		
		/* Stores the data directories */
		ArrayList<String> dataDirs = new ArrayList<String>();
		
		/* Stores the SQL files */
		ArrayList<File> sqlFiles = new ArrayList<File>();
		
		/* Stores query generation times for each query */
		ArrayList<Double> qgenTime = new ArrayList<Double>();
		
		/* Stores query execution times for each query */
		ArrayList<Double> qexecTime = new ArrayList<Double>();
		
		/*
		 * CLI argument parsing
		 */		
		for(int i=0; i<args.length; i++) {
			if(args[i].equalsIgnoreCase("--data")) {
				dataDirs.add(args[i+1]);
				i++;
			}
			else if(args[i].equalsIgnoreCase("--exec")) {
				EXECUTE = true;
			}
			else if(args[i].equalsIgnoreCase("--pdp")) {
				PUSH_DOWN_PROJECTIONS = true;
			}
			else if(args[i].equalsIgnoreCase("--swap")) {
				SWAP = new File(args[i+1]);
				MEMORY_LIMITED = true;
				i++;
			}
			else if(args[i].equalsIgnoreCase("--debug")) {
				DEBUG = true;
			}
			else if(args[i].equalsIgnoreCase("--fileout")) {
				FILE_OUTPUT = true;
				FILE_OUTPUT_DIR = new File(args[i+1]);
				i++;
			}
			else if(args[i].equalsIgnoreCase("--time")) {
				TIME_OUTPUT = true;
				TIME_OUTPUT_DIR = new File(args[i+1]);
				i++;
			}
			else if(args[i].equals("-q")) {
				QUIET = true;
			}
			else {
				sqlFiles.add(new File(args[i]));
			}
		}
		
		/*
		 * Empty the swap directory of all contents so that
		 * there are no file naming conflicts
		 */
		if(SWAP != null) {
			for(File f:SWAP.listFiles()) {
				try {
					Files.deleteIfExists(f.toPath());
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
		/* 
		 * Keep track of query time locally.
		 */
		long globalStart = System.nanoTime();
		
		/* Generate the RA AST for each query */
		ArrayList<Operator> parseTreeList = new ArrayList<Operator>();
		for(int i = 0; i < sqlFiles.size(); i++) {
			File f = sqlFiles.get(i);
			long localStart = System.nanoTime();
			parseTreeList.add(ParseTreeGenerator.generate(dataDirs, f));
			
			/* Compute the generation time for this query */
			qgenTime.add((double) (System.nanoTime() - localStart)/BILLION);
		}
		
		/* Optimize each AST */
		for(int i = 0; i < parseTreeList.size(); i++) {
			Operator parseTree = parseTreeList.get(i);
			long localStart = System.nanoTime();
			parseTreeList.set(i, ParseTreeOptimizer.optimize(parseTree));

			/* Compute the optimization time for this query
			 * and add it to the generation time 
			 */
			qgenTime.set(i, qgenTime.get(i) + (double) (System.nanoTime() - localStart)/BILLION);
			
			/*
			 * Display the query plan
			 */
			/*if(Main.DEBUG) {
				parseTreeList.forEach(new Consumer<Operator>() {
					@Override
					public void accept(Operator t) {
						System.out.println(t.getSchema());
					}
				});
			}*/
		}
		
		/*
		 * Optimized query plans ready
		 */
		
		long totalGenerateTime = System.nanoTime();
		
		if(Main.EXECUTE){
			/* Now evaluate each parse-tree */
			for(int i=0; i< parseTreeList.size(); i++) {
				long localStart = System.nanoTime();
				
				if(parseTreeList.get(i) != null) {
					if(FILE_OUTPUT) {
						ParseTreeEvaluator.output(
								parseTreeList.get(i), 
								new File(
										FILE_OUTPUT_DIR,
										sqlFiles.get(i).getName().split(".sql")[0] + ".out"
										)
								);
					} else {
						ParseTreeEvaluator.output(parseTreeList.get(i));
					}
				}
					
				qexecTime.add((double) (System.nanoTime() - localStart)/BILLION);
			}
		} else {
			for (Operator operator : parseTreeList) {
				if(operator != null) {
					Map<String, Object> root = new HashMap<String, Object>();
					root.put("TYPE", "PRINT");
					root.put("SRC", operator.getDetails());
					root.put("EXPRESSION", new ArrayList<Object>());
					System.out.println(new JSONObject(root));
				}
			}
		}
			
		
		double totalGenTime = (double) (totalGenerateTime - globalStart)/BILLION;
		double totalExecTime = (double) (System.nanoTime() - totalGenerateTime)/BILLION;
		
		/* DEBUG */
		/* Show query times */
		if(Main.DEBUG) {
			
			System.err.println("TOTAL GENERATE TIME: "
					+ (totalGenTime) 
					+ "s");
			
			System.err.println("TOTAL EXECUTION TIME: "
					+ (totalExecTime)
					+ "s");
		
		}
		
		if(Main.TIME_OUTPUT) {
			for(int i = 0; i < sqlFiles.size(); i++) {
				try {
					if(!TIME_OUTPUT_DIR.exists()) {
						TIME_OUTPUT_DIR.mkdirs();
					}
					BufferedWriter bw = new BufferedWriter(
							new FileWriter(
									new File(
											TIME_OUTPUT_DIR,
											sqlFiles.get(i).getName().split(".sql")[0]+".stat")
									)
							);
					
					bw.write(qgenTime.get(i)+"|"+qexecTime.get(i));
					bw.flush();
					bw.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}
}

