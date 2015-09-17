package edu.buffalo.valkyrie;

import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;

import org.robovm.llvm.Context;
import org.robovm.llvm.Function;
import org.robovm.llvm.Module;

public class LLVMMain {
	
	public static void main(String[] args) {
		String c = "extern void printf(const char*, ...);\n" + "int main() {\n"
				+ "    printf(\"Hello world!\");\n" + "}\n";
		try (Context context = new Context()) {
			try (Module m = Module.parseClangString(context, c, "test.c",
					"arm64-unknown-ios")) {
				Set<String> functionNames = new TreeSet<>();
				for (Function f : m.getFunctions()) {
					functionNames.add(f.getName());
				}
				Iterator<String> it = functionNames.iterator();
				while (it.hasNext())
					System.out.println(it.next());
			}
		}
	}
	
}
