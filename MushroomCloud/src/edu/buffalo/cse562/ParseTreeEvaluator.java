package edu.buffalo.cse562;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DecimalFormat;

import net.sf.jsqlparser.expression.DoubleValue;
import net.sf.jsqlparser.expression.LeafValue;
import net.sf.jsqlparser.expression.LeafValue.InvalidLeaf;
import net.sf.jsqlparser.expression.StringValue;
import edu.buffalo.cse562.operators.Operator;

public class ParseTreeEvaluator {

	public static void output(Operator operator) {
		output(operator, null);
	}
	
	public static void output(Operator operator, File f) {

		if(operator == null) {
			return;
		}
		
		operator.initialize();
		
		BufferedWriter bw = null;
		if(f != null) {
			try {
				if(!f.getParentFile().exists()) {
					if(!f.getParentFile().mkdirs()) {
						System.err.println("Couldn't create output directory! Exiting...");
						System.exit(1);
					}
				}
				bw = new BufferedWriter(new FileWriter(f));
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		LeafValue res[] = null;
		/* Keep getting a tuple and displaying it till we exhaust the root operator */
		while((res = operator.readOneTuple()) != null) {
			String s = tupleToString(res);
			if(bw == null) {
				if(!Main.QUIET) System.out.println(s);
			}
			else {
				try {
					bw.write(s+"\n");
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		if(bw != null) {
			try {
				bw.flush();
				bw.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	

	/* Formatting logic for tuples to strings */
	public static String tupleToString(LeafValue res[]) {
		boolean flag = false;
		String result = "";
		DecimalFormat twoDForm = new DecimalFormat("#.############");
		
		for(int i=0; i<res.length; i++) {
			if(flag)
				result += "|";
			
			if(!flag)
				flag = true;
			
			if(res[i] instanceof StringValue) {
				String str = res[i].toString();
				result += str.substring(1, str.length() - 1);				
			}
			else if(res[i] instanceof DoubleValue) {
			    try {
			    	result += twoDForm.format(res[i].toDouble());
				} catch (NumberFormatException | InvalidLeaf e) {
					e.printStackTrace();
				}
				
			}
			else {				
				result += res[i].toString();
			}
		}
		
		return result;
	}

}
