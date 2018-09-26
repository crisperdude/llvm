#include "llvm/ADT/STLExtras.h"
#include "parser.h"
#include <iostream>
#include <cctype>

// Parser for Kaleidoscope
int Parser::cur_tok;
Lexer Parser::lexer;
binop_precedence_t Parser::binop_precedence;
BinopPrecedenceConstructor Parser::binop_precedence_constructor;

// Fills in the precendence table
BinopPrecedenceConstructor::BinopPrecedenceConstructor()
{
  // list of binary operators and their precendence
  Parser::binop_precedence['<'] = 10;
  Parser::binop_precedence['+'] = 20;
  Parser::binop_precedence['-'] = 30;
  Parser::binop_precedence['*'] = 40;  // highest priority
}
  
std::unique_ptr<ExprAST> Parser::parseNumberExpr()
{
  auto result = llvm::make_unique<NumberExprAST>(lexer.numVal);
  getNextToken(); // consume the number
  return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parseParenExpr()
{
  getNextToken(); // consume the '('
  auto V = parseExpression();
  if (!V)
  {
	return nullptr;
  }
  if (cur_tok != ')')
  {
	return logError("expected ')'");
  }
  getNextToken(); // eat next ')'
  return V;
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr()
{
  std::string id_name = Lexer::identifierStr;

  getNextToken(); // consume the identifier
  
  if (cur_tok != '(') // simple variable reference (not a function call)
  {
	return llvm::make_unique<VariableExprAST>(id_name);
  }
  // call
  getNextToken(); // eat the '('
  expr_ast_vector_t args;
  if (cur_tok != ')') 
  {
	while (1) 
	{
	  if (auto arg = parseExpression())
	  {
		args.push_back(std::move(arg));
	  }
	  else
	  {
		return nullptr;
	  }
	  if (cur_tok == ')')
	  {
		break;
	  }
	  if (cur_tok != ',')
	  {
		return logError("Expected ')' or ',' in argument list");
	  }
	  getNextToken();
	}
  }
  // consume next ')'
  getNextToken();
  return llvm::make_unique<CallExprAST>(id_name, std::move(args));
}

std::unique_ptr<ExprAST> Parser::parseExpression()
{
  auto lhs = parsePrimary();
  if (!lhs)
  {
	return nullptr;
  }
  //return ParseBinOpRHS(0, std::move(lhs));
  return nullptr;
}

std::unique_ptr<ExprAST> Parser::parsePrimary()
{
  switch (cur_tok)
  {
  default: 
  {
    return logError("Unknown token when expecting an expression");
  }
  case tok_identifier:
  {
    return parseIdentifierExpr();
  }
  case tok_number:
  {
    return parseNumberExpr(); 
  }
  case '(':
  {
    return parseParenExpr();
  }
  }
}

// get the precedence given a binary operator
int Parser::getTokPrecedence()
{
  int tok_prec;
  // invalid token
  if (!isascii(cur_tok))
  {
    return -1;
  }
  // try finding the operator in the table
  tok_prec = binop_precedence[cur_tok];
  // not found
  if (tok_prec <= 0)
  {
    return -1;
  }
  return tok_prec;
}

std::unique_ptr<ExprAST> Parser::logError(const char * str)
{
  std::cerr << "LogError: " << str << std::endl;
  return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::logErrorP(const char * str)
{
  logError(str);
  return nullptr;
}
