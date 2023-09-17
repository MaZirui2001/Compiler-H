lexer grammar relop;

Equal:          '=';
NonEqual:       '<>';
Less:           '<';
Greater:        '>';
LessEqual:      '<=';
GreaterEqual:   '>=';

EndofInput:     [\n]|[\r\n];
OtherChar:      (~([=]|[<>]|[<]|[>]|[<=]|[>=]|[\n]|[\r\n]))+;