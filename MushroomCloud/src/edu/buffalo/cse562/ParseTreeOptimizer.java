package edu.buffalo.cse562;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import net.sf.jsqlparser.expression.BinaryExpression;
import net.sf.jsqlparser.expression.Expression;
import net.sf.jsqlparser.expression.Function;
import net.sf.jsqlparser.expression.Parenthesis;
import net.sf.jsqlparser.expression.operators.conditional.AndExpression;
import net.sf.jsqlparser.expression.operators.conditional.OrExpression;
import net.sf.jsqlparser.expression.operators.relational.EqualsTo;
import net.sf.jsqlparser.schema.Column;
import net.sf.jsqlparser.schema.Table;
import net.sf.jsqlparser.statement.select.AllColumns;
import net.sf.jsqlparser.statement.select.AllTableColumns;
import net.sf.jsqlparser.statement.select.SelectExpressionItem;
import net.sf.jsqlparser.statement.select.SelectItem;
import edu.buffalo.cse562.operators.CrossProductOperator;
import edu.buffalo.cse562.operators.ExternalSortOperator;
import edu.buffalo.cse562.operators.GraceHashJoinOperator;
import edu.buffalo.cse562.operators.Operator;
import edu.buffalo.cse562.operators.OrderByOperator;
import edu.buffalo.cse562.operators.ProjectScanOperator;
import edu.buffalo.cse562.operators.ProjectionGroupByAggregateOperator;
import edu.buffalo.cse562.operators.ProjectionOperator;
import edu.buffalo.cse562.operators.ScanOperator;
import edu.buffalo.cse562.operators.SelectionOperator;
import edu.buffalo.cse562.operators.SortMergeJoinOperator;
import edu.buffalo.cse562.schema.Schema;

public class ParseTreeOptimizer {

	private enum ClauseApplicability {
		ROOT, LEFT, RIGHT, INVALID;
	}
	
	
	private static Pattern p = Pattern.compile("[a-zA-Z.]+");
	private static ArrayList<Schema> schemas = ParseTreeGenerator.getTableSchemas();
	
	private static String[] keywords = {
		"sum", "avg", "count", "min", "max", 
		"case", "when", "and", "or", "then", "else", "end",
		"urgent", "high"
	};
	
	/* 
	 * The entry-point to the optimizer,
	 * we try to define several functions which
	 * apply a variety of rewriting techniques to optimize
	 * the query plan
	 */
	public static Operator optimize(Operator parseTree) {
		
		if(parseTree == null) {
			return null;
		}
		
		/*
		 * Query plan rewrites for faster evaluation
		 */
		
		/* Reorder joins for left deep */
		parseTree = reOrderJoins(parseTree);
		
		/* Push down projections */
		if(!(parseTree instanceof ScanOperator))
			if(Main.PUSH_DOWN_PROJECTIONS)
				parseTree = pushDownProjections(parseTree);
		
		/* Decompose Select Clauses and push them down appropriately */
		parseTree = initialSelectionDecomposition(parseTree);
		
		/* Replace Selection over Cross Product with appropriate Join */
		parseTree = findJoinPatternAndReplace(parseTree);
				
		/* Reorder Cross Products to facilitate more joins */
		parseTree = reOrderCrossProducts(parseTree);

		/* Replace Selection over Cross Product with appropriate Join */
		parseTree = findJoinPatternAndReplace1(parseTree);
		

		/* Decompose Select Clauses and push them down appropriately */
		parseTree = initialSelectionDecomposition(parseTree);

		/* Replace Selection over Cross Product with appropriate Join */
		if(Main.MEMORY_LIMITED) {
			parseTree = optimizeMemory(parseTree);
		}
		
		/* Other Patterns go here */
		
		/* Generate appropriate table names after optimization */
		parseTree.generateSchemaName();
		
		return parseTree;
		
	}
	
	private static Operator findJoinPatternAndReplace1(Operator parseTree) {
		if(parseTree == null) {
			/* Leaf Node */
			return null;
		}
		
		/* Recursively traverse the entire tree in order */
		parseTree.setLeft(findJoinPatternAndReplace1(parseTree.getLeft()));
		parseTree.setRight(findJoinPatternAndReplace1(parseTree.getRight()));

		if(parseTree instanceof SelectionOperator) {
			SelectionOperator select = (SelectionOperator) parseTree;
			Expression where = select.getWhere();
			
			Operator child = select.getLeft();
			Operator leftChild = child.getLeft();
			Operator rightChild = child.getRight();

			if(child instanceof CrossProductOperator) {
				/* Pattern Matched, replace it with Join */

				ArrayList<Expression> joinPredicates = new ArrayList<Expression>();
				ArrayList<Expression> clauseList = new ArrayList<Expression>();
				
				if(where instanceof AndExpression) {
					clauseList = splitAndClauses(where);
					
					for(int i=0; i<clauseList.size(); i++) {
						Expression clause = clauseList.get(i);
						if(isJoinCondition1(clause, leftChild, rightChild)
								//&& consistent(clause, joinPredicates)
								) {
							joinPredicates.add(clause);
							clauseList.remove(clause);
						}
					}
				}
				else {
					joinPredicates.add(where);
				}
				
				/* Pretty sure that joinPredicates will have at least
				 * one member, but we still check for it
				 */
				
				if(!clauseList.isEmpty()) {
					parseTree = new SelectionOperator(
							mergeClauses(clauseList),
							parseTree
							);
					if(!joinPredicates.isEmpty()) {
						parseTree.setLeft(new GraceHashJoinOperator(
							mergeClauses(joinPredicates) ,
							leftChild ,
							rightChild
							)
						);
					}
				} 
				else if(!joinPredicates.isEmpty()) {
					parseTree = new GraceHashJoinOperator(
							mergeClauses(joinPredicates) ,
							leftChild ,
							rightChild
							);
				}
			}
			else if(child instanceof GraceHashJoinOperator) {
				if(isJoinCondition1(where, leftChild, rightChild)) {
					GraceHashJoinOperator joinOp = (GraceHashJoinOperator) child;
					joinOp.appendWhere(where);
					parseTree = joinOp;
				}
					
			}
		}
		
		return parseTree;
	}

	private static boolean isJoinCondition1(Expression e,
			Operator leftChild, Operator rightChild) {
		
		if(e instanceof BinaryExpression) {
			BinaryExpression be = (BinaryExpression) e;
			if(!(be instanceof EqualsTo) 
					|| !(be.getLeftExpression() instanceof Column)
					|| !(be.getRightExpression() instanceof Column))
				return false;
			
			if(!(leftChild instanceof GraceHashJoinOperator && rightChild instanceof GraceHashJoinOperator)) {
				return false;
			}
			
			Column left = (Column) be.getLeftExpression();
			Column right = (Column) be.getRightExpression();
			
			String leftColTableName = left.getTable().getName();
			String rightColTableName = right.getTable().getName();
			
			if(leftChild.getSchema().getTableName().contains(leftColTableName)
					&& rightChild.getSchema().getTableName().contains(rightColTableName)
					) {
				return true;
			}
			
			if(leftChild.getSchema().getTableName().contains(rightColTableName)
					&& rightChild.getSchema().getTableName().contains(leftColTableName)
					) {
				return true;
			}
		}
		
		return false;
	}

	private static Operator reOrderJoins(Operator parseTree) {
		if(parseTree == null) {
			return null;
		}
		
		if(parseTree instanceof CrossProductOperator) {
			ArrayList<ScanOperator> relations = 
					findAllRelations(parseTree);
			
			Collections.sort(relations, new Comparator<ScanOperator>() {

				@Override
				public int compare(ScanOperator o1, ScanOperator o2) {
					long rC1 = o1.getSchema().getRowCount();
					long rC2 = o2.getSchema().getRowCount();
					long diff = rC1 - rC2;
					return (int) diff;
				}
			});

			/*relations.sort(new Comparator<ScanOperator>() {

				@Override
				public int compare(ScanOperator o1, ScanOperator o2) {
					long rC1 = o1.getSchema().getRowCount();
					long rC2 = o2.getSchema().getRowCount();
					long diff = rC1 - rC2;
					return (int) diff;
				}
			});*/
			
			parseTree = new CrossProductOperator(relations.get(0), relations.get(1));
			
			for(int i=2; i<relations.size(); i++) {
				parseTree = new CrossProductOperator(parseTree, relations.get(i));
			}
		}
		
		parseTree.setLeft(reOrderJoins(parseTree.getLeft()));
		parseTree.setRight(reOrderJoins(parseTree.getRight()));
		return parseTree;
	}

	private static ArrayList<ScanOperator> findAllRelations(Operator parseTree) {
		ArrayList<ScanOperator> relations = new ArrayList<ScanOperator>();
		while(parseTree instanceof CrossProductOperator) {
			relations.add((ScanOperator) parseTree.getRight());
			parseTree = parseTree.getLeft();
		}
		
		relations.add((ScanOperator) parseTree);
		return relations;
	}

	private static Operator pushDownProjections(Operator parseTree) {
		
		HashSet<String> projections = new HashSet<String>();
		projections.addAll(findProjectedColumns(parseTree, projections));
		parseTree = projectOutUnnecessaryColumns(parseTree, projections);
		
		return parseTree;
	}

	private static Operator projectOutUnnecessaryColumns(Operator parseTree,
			HashSet<String> projections) {
		
		if(parseTree == null)
			return null;
		
		if(parseTree instanceof ScanOperator) {
			parseTree = new ProjectScanOperator(parseTree.getSchema(), projections);
		}
		
		parseTree.setLeft(projectOutUnnecessaryColumns(parseTree.getLeft(), projections));
		parseTree.setRight(projectOutUnnecessaryColumns(parseTree.getRight(), projections));
		
		return parseTree;
	}

	private static HashSet<String> findProjectedColumns(
			Operator parseTree, HashSet<String> projections) {
		
		if(parseTree == null)
			return projections;
		
		if(parseTree instanceof ProjectionGroupByAggregateOperator
				|| parseTree instanceof ProjectionOperator) {
			
			List<SelectItem> selectItems = null;
			
			if(parseTree instanceof ProjectionGroupByAggregateOperator) {

				ProjectionGroupByAggregateOperator po = 
						(ProjectionGroupByAggregateOperator) parseTree;
				
				selectItems = po.getSelectItems();
			}
			else {
				ProjectionOperator po = 
						(ProjectionOperator) parseTree;
				
				selectItems = po.getSelectItems();
			}
			
			Iterator<SelectItem> i = selectItems.iterator();
			
			while(i.hasNext()) {
				SelectItem si = i.next();
				if(si instanceof SelectExpressionItem) {
					
					SelectExpressionItem sei = (SelectExpressionItem) si;
					Expression expr = sei.getExpression();
					
					if(expr instanceof Function) {
						Matcher m = p.matcher(expr.toString());
						
						while(m.find()) {
							String res = m.group();
							if(!isAKeyWord(res)) {
								if(res.contains("."))
									res = res.split("\\.")[1];

								projections.add(res.toLowerCase());
							}
						}
					}
					else if (expr instanceof Column){
						projections.add(((Column) expr).getColumnName().toLowerCase());
					}
				}
				
				else if(si instanceof AllTableColumns) {
					AllTableColumns atc = (AllTableColumns) si;
					Table t = atc.getTable();

					for(Schema s:schemas) {
						if(s.getTableName().equalsIgnoreCase(t.getName())) {
							for(Column c : s.getColumns()) {
								projections.add(c.getColumnName().toLowerCase());
							}
							break;
						}
					}
				}
				else if(si instanceof AllColumns) {
					for(Schema s:schemas) {
						for(Column c : s.getColumns()) {
							projections.add(c.getColumnName().toLowerCase());
						}
					}
				}
			}
		}
		else if (parseTree instanceof SelectionOperator) {
			
			SelectionOperator so = (SelectionOperator) parseTree;
			Expression where = so.getWhere();
			ArrayList<Expression> clauseList = splitAndClauses(where);
			
			for(Expression e:clauseList) {
				ArrayList<Expression> orList = splitOrClauses(e);
				
				for(Expression clause:orList) {
					
					if(clause instanceof BinaryExpression) {
						Expression left = ((BinaryExpression) clause).getLeftExpression();
						Expression right = ((BinaryExpression) clause).getRightExpression();
						
						if(left instanceof Column) {
							projections.add(((Column) left).getColumnName().toLowerCase());
						}
						
						if(right instanceof Column) {
							projections.add(((Column) right).getColumnName().toLowerCase());
						}
					}
				}
			}
		}

		/* Recursively traverse the tree top down */
		projections.addAll(findProjectedColumns(parseTree.getLeft(), projections));
		projections.addAll(findProjectedColumns(parseTree.getRight(), projections));
		
		return projections;
	}

	private static boolean isAKeyWord(String group) {

		for(String s:keywords) {
			if(group.equalsIgnoreCase(s))
				return true;
		}
		
		return false;
	}

	private static Operator reOrderCrossProducts(Operator parseTree) {
		if(parseTree == null) {
			return null;
		}
		
		
		/* Recursively traverse the entire tree bottom up */
		parseTree.setLeft(reOrderCrossProducts(parseTree.getLeft()));
		parseTree.setRight(reOrderCrossProducts(parseTree.getRight()));
		
		
		if(parseTree instanceof SelectionOperator) {
			SelectionOperator select = (SelectionOperator) parseTree;
			Expression w = select.getWhere();
			ArrayList<Expression> clauseList = splitAndClauses(w);
			
			for(Expression where : clauseList) {
				if(isJoinPredicate(where)) {
					if(select.getLeft() instanceof GraceHashJoinOperator) {
						Operator cpOperatorParent = findCrossProduct(select);
						if(cpOperatorParent != null) {
							CrossProductOperator cpOperator = 
									(CrossProductOperator) cpOperatorParent.getLeft();
							
							Operator newCrossProduct = null;
							
							if(relationIsLeftOfCrossProduct(where, cpOperator)) {
								cpOperatorParent.setLeft(cpOperator.getRight());
								
								newCrossProduct = new CrossProductOperator(
										cpOperator.getLeft(),
										select.getLeft()
										);
							}
							else {
								cpOperatorParent.setLeft(cpOperator.getLeft());
								
								newCrossProduct = new CrossProductOperator(
										cpOperator.getRight(),
										select.getLeft()
										);
							}

							
							parseTree.setLeft(newCrossProduct);
						}
					}
				}
			}
		}
		
		return parseTree;
	}
	
	private static boolean relationIsLeftOfCrossProduct(Expression where,
			CrossProductOperator cpOperator) {

		BinaryExpression joinClause = (BinaryExpression) where;
		Column leftColumn = (Column) joinClause.getLeftExpression();
		Schema leftSchema = cpOperator.getLeft().getSchema();
		Schema rightSchema = cpOperator.getRight().getSchema();

		if(belongsToSchema(leftSchema, rightSchema, leftColumn) 
				== ClauseApplicability.INVALID) {
			return false;
		}
		
		return true;
	}

	private static Operator findCrossProduct(
			Operator o) {
		if(o == null)
			return null;
		
		if(o.getLeft() instanceof CrossProductOperator)
			return o;
		
		Operator cpOperator = findCrossProduct(o.getLeft());
		if(cpOperator == null)
			return findCrossProduct(o.getLeft());
		else
			return cpOperator;
	}

	/* Pushing down Selects through Cross Products - helper methods */

	private static Operator optimizeMemory(Operator parseTree) {
		
		/* Find memory blocking operator patters and
		 * replace with non-blocking equivalent patterns
		 */
		
		if(parseTree == null) {
			return null;
		}
		
		if(parseTree instanceof GraceHashJoinOperator) {
			parseTree = new SortMergeJoinOperator(((GraceHashJoinOperator) parseTree));
		}
		
		if(parseTree instanceof OrderByOperator) {
			parseTree = new ExternalSortOperator((OrderByOperator) parseTree, Main.BLOCK);
		}
		
		
		/* Recursively traverse the tree top down */
		parseTree.setLeft(optimizeMemory(parseTree.getLeft()));
		parseTree.setRight(optimizeMemory(parseTree.getRight()));
		
		return parseTree;
	
	}

	private static ClauseApplicability checkClauseApplicability(
			Schema left, Schema right, Expression clause
			) {
		
		/* Captures a lot of the expressions we can actually reorder */
		if(clause instanceof BinaryExpression) {
			BinaryExpression binaryClause = (BinaryExpression) clause;
			Expression leftClause = binaryClause.getLeftExpression();
			
			if( ! (leftClause instanceof Column) ) {
				/* This clause type not supported, default to ROOT */
				return ClauseApplicability.ROOT;
			}

			/* Checking which schema the left column belongs to */
			ClauseApplicability ret = belongsToSchema(
					left ,
					right ,
					(Column) leftClause
					);
			
			Expression rightClause = binaryClause.getRightExpression();
			
			/* If the rightClause is also a Column */
			if(rightClause instanceof Column) {
				if(ret != belongsToSchema(
						left ,
						right ,
						(Column) rightClause)
						) {
					/* 
					 * If the two do not agree, the only option can be root,
					 */
					return ClauseApplicability.ROOT;
				}
			}			

			/* 
			 * ret not null means that even if there are two columns
			 * in the expression, they agree on which side of
			 * the schemas they fall on, so just return one of
			 * the two values, even if the agreed value is ROOT itself
			 */
			if(ret != null)
				return ret;
		}
		else if(clause instanceof Parenthesis) {
			Parenthesis parenClause = (Parenthesis) clause;
			Expression parenExpr = parenClause.getExpression();

			if(parenExpr instanceof OrExpression) {
				ArrayList<Expression> orClauseList = splitOrClauses(parenExpr);
				
				if(orClauseList.isEmpty()) {
					/* Safety check */
					return ClauseApplicability.INVALID;
				}
				
				ClauseApplicability firstCa = checkClauseApplicability(left, right, orClauseList.get(0));
				
				for(int i=1; i<orClauseList.size(); i++) {
					if(checkClauseApplicability(left, right, orClauseList.get(i)) != firstCa)
						return ClauseApplicability.ROOT;
				}
				
				return firstCa;
			}
		}
		
		return ClauseApplicability.INVALID;
		
	}
	
	public static ArrayList<Expression> splitOrClauses(Expression e) {
		ArrayList<Expression> ret = new ArrayList<Expression>();
		if(e instanceof OrExpression){
			OrExpression a = (OrExpression) e;
			ret.addAll(splitOrClauses(a.getLeftExpression()));
			ret.addAll(splitOrClauses(a.getRightExpression()));
		}
		else {
			ret.add(e);
		}
		
		return ret;
	}

	private static ClauseApplicability 
		belongsToSchema(Schema left, Schema right, Column column) {
		
		for(Column col : left.getColumns()) {
			if(column.getWholeColumnName().equalsIgnoreCase(col.getWholeColumnName()))
				return ClauseApplicability.LEFT;
		}
		
		for(Column col : right.getColumns()) {
			if(column.getWholeColumnName().equalsIgnoreCase(col.getWholeColumnName()))
				return ClauseApplicability.RIGHT;
		}
		return ClauseApplicability.ROOT;
	}

	
	
	private static Operator initialSelectionDecomposition(Operator parseTree) {
		if(parseTree == null) {
			/* Leaf Node */
			return null;
		}
		
		if(parseTree instanceof SelectionOperator) {
			SelectionOperator select = (SelectionOperator) parseTree;
			Operator child = select.getLeft();
			
			if(child instanceof CrossProductOperator || child instanceof GraceHashJoinOperator) {
				Operator crossProductLeftChild = child.getLeft();
				Operator crossProductRightChild = child.getRight();
				
				ArrayList<Expression> clauseList = splitAndClauses(select.getWhere());
				
				ArrayList<Expression> parentList = new ArrayList<Expression>();
				ArrayList<Expression> leftList = new ArrayList<Expression>();
				ArrayList<Expression> rightList = new ArrayList<Expression>();
				ArrayList<Expression> invalidList = new ArrayList<Expression>();
				
				for(Expression clause : clauseList) {
					ClauseApplicability ca = checkClauseApplicability(
							crossProductLeftChild.getSchema() , 
							crossProductRightChild.getSchema() ,
							clause
							);
					
					switch(ca) {
					case ROOT:
						parentList.add(clause);
						break;
					case LEFT:
						leftList.add(clause);
						break;
					case RIGHT:
						rightList.add(clause);
						break;
					case INVALID:
						invalidList.add(clause);
						break;
					}
				}

				
				if(!leftList.isEmpty()) {
					Expression where = mergeClauses(leftList);
					child.setLeft(new SelectionOperator(where, crossProductLeftChild));
				}
				
				if(!rightList.isEmpty()) {
					Expression where = mergeClauses(rightList);
					child.setRight(new SelectionOperator(where, crossProductRightChild));
				}
				
				if(!parentList.isEmpty()) {
					Expression where = mergeClauses(parentList);
					parseTree = new SelectionOperator(where, child);
				}
				else {
					parseTree = child;
				}
				
				if(!invalidList.isEmpty()) {
					Expression where = mergeClauses(invalidList);
					parseTree = new SelectionOperator(where, parseTree);
				}
			}
		}
		
		/* Recursively traverse the entire tree in order */
		parseTree.setLeft(initialSelectionDecomposition(parseTree.getLeft()));
		parseTree.setRight(initialSelectionDecomposition(parseTree.getRight()));
		
		return parseTree;
	}

	public static Expression mergeClauses(ArrayList<Expression> expressionList) {
		if(expressionList.isEmpty()) {
			return null;
		}
		
		if(expressionList.size() == 1) {
			return expressionList.get(0);
		}
		else {
			Expression ret = new AndExpression(expressionList.get(0), expressionList.get(1));
			for(int i=2; i<expressionList.size(); i++) {
				ret = new AndExpression(ret, expressionList.get(i));
			}
			
			return ret;
		}
	}
	
	
	
	/* Replace Selection over Cross Products with Joins */
	
	private static Operator findJoinPatternAndReplace(Operator parseTree) {
		if(parseTree == null) {
			/* Leaf Node */
			return null;
		}
		
		/* Recursively traverse the entire tree in order */
		parseTree.setLeft(findJoinPatternAndReplace(parseTree.getLeft()));
		parseTree.setRight(findJoinPatternAndReplace(parseTree.getRight()));

		if(parseTree instanceof SelectionOperator) {
			SelectionOperator select = (SelectionOperator) parseTree;
			Expression where = select.getWhere();
			
			Operator child = select.getLeft();
			Operator leftChild = child.getLeft();
			Operator rightChild = child.getRight();

			if(child instanceof CrossProductOperator) {
				/* Pattern Matched, replace it with Join */

				ArrayList<Expression> joinPredicates = new ArrayList<Expression>();
				ArrayList<Expression> clauseList = new ArrayList<Expression>();
				
				if(where instanceof AndExpression) {
					clauseList = splitAndClauses(where);
					
					for(int i=0; i<clauseList.size(); i++) {
						Expression clause = clauseList.get(i);
						if(isJoinCondition(clause, leftChild, rightChild)
								//&& consistent(clause, joinPredicates)
								) {
							joinPredicates.add(clause);
							clauseList.remove(clause);
						}
					}
				}
				else {
					joinPredicates.add(where);
				}
				
				/* Pretty sure that joinPredicates will have at least
				 * one member, but we still check for it
				 */
				
				if(!clauseList.isEmpty()) {
					parseTree = new SelectionOperator(
							mergeClauses(clauseList),
							parseTree
							);
					if(!joinPredicates.isEmpty()) {
						parseTree.setLeft(new GraceHashJoinOperator(
							mergeClauses(joinPredicates) ,
							leftChild ,
							rightChild
							)
						);
					}
				} 
				else if(!joinPredicates.isEmpty()) {
					parseTree = new GraceHashJoinOperator(
							mergeClauses(joinPredicates) ,
							leftChild ,
							rightChild
							);
				}
			}
			else if(child instanceof GraceHashJoinOperator) {
				if(isJoinCondition(where, leftChild, rightChild)) {
					GraceHashJoinOperator joinOp = (GraceHashJoinOperator) child;
					joinOp.appendWhere(where);
					parseTree = joinOp;
				}
					
			}
		}
		
		return parseTree;
	}
	
	private static boolean isJoinCondition(Expression e,
			Operator leftChild, Operator rightChild) {
		
		if(e instanceof BinaryExpression) {
			BinaryExpression be = (BinaryExpression) e;
			if(!(be instanceof EqualsTo) 
					|| !(be.getLeftExpression() instanceof Column)
					|| !(be.getRightExpression() instanceof Column))
				return false;
			
			if(leftChild instanceof ProjectScanOperator && rightChild instanceof ProjectScanOperator) {
				return true;
			}
			
			Column left = (Column) be.getLeftExpression();
			Column right = (Column) be.getRightExpression();
			
			String leftColTableName = left.getTable().getName();
			String rightColTableName = right.getTable().getName();
			
			if(rightColTableName.equalsIgnoreCase(rightChild.getSchema().getTableName())) {
				if(leftColTableName.equalsIgnoreCase(leftChild.getRight().getSchema().getTableName())) {
					return true;
				}
			}
			else if(leftColTableName.equalsIgnoreCase(rightChild.getSchema().getTableName())) {
				if(rightColTableName.equalsIgnoreCase(leftChild.getRight().getSchema().getTableName())) {
					return true;
				}
			}
		}
		
		return false;
	}

	public static ArrayList<Expression> splitAndClauses(Expression e) {
	  
		ArrayList<Expression> ret = new ArrayList<Expression>();
		if(e instanceof AndExpression){
			AndExpression a = (AndExpression) e;
			ret.addAll(splitAndClauses(a.getLeftExpression()));
			ret.addAll(splitAndClauses(a.getRightExpression()));
		}
		else {
			ret.add(e);
		}
		
		return ret;
	}
	
	public static Boolean isJoinPredicate(Expression e) {
		
		if(e instanceof BinaryExpression) {
			BinaryExpression be = (BinaryExpression) e;
			if(be.toString().contains("=")
					&& be.getLeftExpression() instanceof Column
					&& be.getRightExpression() instanceof Column)
				
				return true;
		}
		
		return false;
	}
	
	
	
	
	
	
	public static String[] split(
			String line, char delim) {
		
        ArrayList<String> result = new ArrayList<String>();
        
        if (line != null && line.length() > 0) {
            int index1 = 0;
            int index2 = line.indexOf(delim);
            
            while (index2 >= 0) {
                String token = line.substring(index1, index2);
                result.add(token);
                index1 = index2 + 1;
                index2 = line.indexOf(delim, index1);
            }
            
            if (index1 < line.length() - 1) {
                result.add(line.substring(index1));
            }
        }
        
        String[] ret = new String[result.size()];
        int i = 0;
        for(String s : result) {
        	ret[i] = s;
        	i++;
        }
        
        return ret;
    }
}
