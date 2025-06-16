//  $Id$
using System;
using System.IO;

namespace Lisp
{

public class Parser {
    public enum LispType {
        START_LIST,
        END_LIST,
        SYMBOL,
        INTEGER,
        STRING,
        REAL,
        BOOLEAN
    };
    private Lexer lexer;
    private Lexer.TokenType token;

    public Parser(StreamReader stream) {
        lexer = new Lexer(stream);
    }

    public bool Parse() {
        token = lexer.GetNextToken();
        if(delayinc) {
            depth++;
            delayinc = false;
        }
        if(token == Lexer.TokenType.EOF) {
            depth = 0;
            return false;
        }

        /*
        Console.WriteLine("Token: " + token.ToString() + " - " +
                lexer.TokenString);
        */
        switch(token) {
            case Lexer.TokenType.CLOSE_PAREN:
                if(depth == 0)
                    throw new Exception("Parse Error: unexpected )");
                depth--;
                type = LispType.END_LIST;
                break;
            case Lexer.TokenType.OPEN_PAREN:
                type = LispType.START_LIST;
                delayinc = true;
                break;
            case Lexer.TokenType.SYMBOL:
                type = LispType.SYMBOL;
                break;
            case Lexer.TokenType.STRING:
                type = LispType.STRING;
                break;
            case Lexer.TokenType.TRUE:
                type = LispType.BOOLEAN;
                break;
            case Lexer.TokenType.INTEGER:
                type = LispType.INTEGER;
                break;
        }
        return true;
    }

    public static void ParseIntList(Parser parser, System.Collections.Generic.List<int> intList) {
	int d = parser.Depth;
	while(parser.Depth >= d) {
		intList.Add(parser.IntegerValue);
		parser.Parse();
	}
    }

    private LispType type;
    public LispType Type {
        get { return type; }
    }
    private bool delayinc;
    private int depth;
    public int Depth {
        get { return depth; }
    }
    //public int IntValue
    public string SymbolValue {
        get { return lexer.TokenString; }
    }
    public string StringValue {
        get { return lexer.TokenString; }
    }
    public int IntegerValue {
        get { return Int32.Parse(lexer.TokenString); }
    }
    public bool BoolValue {
        get { return StringValue == "t"; }
    }
    public float FloatValue {
        get { return Single.Parse(lexer.TokenString); }
    }
}

}
