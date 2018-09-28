#ifndef _PARSER_H_
#define _PARSER_H_

#include <memory>
#include <map>

#include "lexer.h"
#include "ast.h"

typedef std::map<char, int> binop_precedence_t;

class BinopPrecedenceConstructor
{
public:
  BinopPrecedenceConstructor();
};

// Parser for Kaleidoscope
class Parser {
private:
  friend class BinopPrecedenceConstructor;

  static int cur_tok;
  static Lexer lexer;
  static binop_precedence_t binop_precedence; // bin op precendence table
  static BinopPrecedenceConstructor binop_precedence_constructor;
  
private:
  static int getNextToken() { return lexer.getToken(); }

  // numberexpr ::= number
  static std::unique_ptr<ExprAST> parseNumberExpr();
  // parenexpr ::= '(' expression ')'
  static std::unique_ptr<ExprAST> parseParenExpr();
  // identifierexpr
  //   ::= identifier
  //   ::= identifier '(' expression * ')'
  static std::unique_ptr<ExprAST> parseIdentifierExpr();
  // expression 
  //   ::= binary binoprhs
  static std::unique_ptr<ExprAST> parseExpression();
  // binoprhs
  //   ::= (['+'|'-'|'<'|'*']primary)*
  static std::unique_ptr<ExprAST> parseBinOpRHS(int expr_prec,
												std::unique_ptr<ExprAST> lhs);
  // primary
  //   ::= identifierexpr
  //   ::= numberexpr
  //   ::= parenexpr
  static std::unique_ptr<ExprAST> parsePrimary();

  // get the precedence given a binary operator
  static int getTokPrecedence();

  // error handling
  static std::unique_ptr<ExprAST> logError(const char * str);
  static std::unique_ptr<PrototypeAST> logErrorP(const char * str);
};

#endif
