//  $Id$
using System;
using System.Text;
using System.IO;

namespace Lisp {

public class Lexer {
    private StreamReader stream;
    private char[] buffer;
    private char c;
    int bufpos;
    int buflen;

    public class EOFException : Exception {
    };

    public enum TokenType {
        EOF,
        OPEN_PAREN,
        CLOSE_PAREN,
        SYMBOL,
        STRING,
        INTEGER,
        REAL,
        TRUE,
        FALSE
    };

    private StringBuilder TokenStringBuilder;
    public string TokenString {
        get { return TokenStringBuilder.ToString(); }
    }
    public int LineNumber;

    public Lexer(StreamReader stream) {
        this.stream = stream;
        buffer = new char[1025];
        NextChar();
    }

    public TokenType GetNextToken() {
        try {
            while(Char.IsWhiteSpace(c)) {
                NextChar();
                if(c == '\n')
                    LineNumber++;
            }

            TokenStringBuilder = new StringBuilder();

            switch(c) {
                case ';': // comment
                    while(true) {
                        NextChar();
                        if(c == '\n') {
                            LineNumber++;
                            break;
                        }
                    }
                    NextChar();
                    return GetNextToken();
                case '(':
                    NextChar();
                    return TokenType.OPEN_PAREN;
                case ')':
                    NextChar();
                    return TokenType.CLOSE_PAREN;
                case '"': { // string
                    int startline = LineNumber;
                    while(true) {
                        NextChar();
                        if(c == '"')
                            break;

                        if(c == '\\') {
                            NextChar();
                            switch(c) {
                                case 'n':
                                    c = '\n';
                                    break;
                                case 't':
                                    c = '\t';
                                    break;
                            }
                        }
                        TokenStringBuilder.Append(c);
                    }
                    NextChar();
                    return TokenType.STRING;
                }
                case '#': // constant
                    NextChar();
                    while(Char.IsLetterOrDigit(c) || c == '_') {
                        TokenStringBuilder.Append(c);
                        NextChar();
                    }
                    if(TokenString == "t")
                        return TokenType.TRUE;
                    if(TokenString == "f")
                        return TokenType.FALSE;

                    throw new Exception("Unknown constant '"
                            + TokenString + "'");
                default:
                    if(Char.IsDigit(c) || c == '-') {
                        bool have_nondigits = false;
                        bool have_digits = false;
                        int have_floating_point = 0;

                        do {
                            if(Char.IsDigit(c))
                                have_digits = true;
                            else if(c == '.')
                                have_floating_point++;
                            else if(Char.IsLetter(c) || c == '_')
                                have_nondigits = true;

                            TokenStringBuilder.Append(c);
                            NextChar();
                        } while(!Char.IsWhiteSpace(c) && c != '\"' && c != '('
                                && c != ')' && c != ';');

                        if(have_nondigits || !have_digits
                                || have_floating_point > 1)
                            return TokenType.SYMBOL;
                        else if(have_floating_point == 1)
                            return TokenType.REAL;
                        else
                            return TokenType.INTEGER;
                    } else {
                        do {
                            TokenStringBuilder.Append(c);
                            NextChar();
                        } while(!Char.IsWhiteSpace(c) && c != '\"' && c != '('
                                && c != ')' && c != ';');

                        return TokenType.SYMBOL;
                    }
            }
        } catch(EOFException) {
            return TokenType.EOF;
        }
    }

    private void NextChar() {
        if(bufpos >= buflen) {
            if(!stream.BaseStream.CanRead)
                throw new EOFException();
            buflen = stream.Read(buffer, 0, 1024);
            bufpos = 0;
            // following hack appends an additional ' ' at the end of the file
            // to avoid problems when parsing symbols/elements and a sudden EOF:
            // This way we can avoid the need for an unget function.
            if(!stream.BaseStream.CanRead) {
                buffer[buflen] = ' ';
                ++buflen;
            }
        }
        c = buffer[bufpos++];
    }
}

}
