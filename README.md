# spl_parser
SimpleProgrammingLanguage RD-parser for assignment

'id' = (l+_)(l+d+_)*

'number' = d*

## SPL Grammar
    <spl_program> ::= <block> '.'
    <block> ::= 'begin' {<dcl>';'} <st_list> 'end'
    <dcl> ::= ('label'|'integer')'id'{',''id'}
    <st_list> ::= <st> {';'<st>}
    <st> ::= 'id'<H> | <statement_else>
    <H> ::= ':'<statement> | {('*'|'/')<factor>} {('+'|'-')<term>} '=>' 'id' {'=>''id'}
    <statement> ::= <assign> | <goto_st> | <if_st> | <write_st> | <block>
    <statement_else> ::= <assign_else> | <goto_st> | <if_st> | <write_st> | <block>
    <assign> ::= <exp> '=>' 'id' {'=>''id'}
    <assign_else> ::= <exp_else> '=>' 'id' {'=>''id'}
    <goto_st>::= 'goto' 'id'
    <if_st> ::= 'if' <condition> 'then' <st_list> ['else'<st_list>] 'fi'
    <write_st> ::= 'output' '('<exp> {','<exp>} ')'
    <condition> ::= <exp> ('<'|'>'|'=')<exp>
    <exp> ::= <term> {('+'|'-')<term>}
    <exp_else> ::= ('input'|'number'|'('<exp>')') {('*'|'/')<factor>} {('+'|'-')<term>}
    <term> ::= <factor> {('*'|'/')<factor>}
    <factor> ::= 'input' | 'id' | 'number' | '('<exp>')'

## Converted to SLL(1)
    <spl_program> ::= <block>'.'
    <block> ::= 'begin'{<dcl>';'}<st_list>'end'
    <dcl> ::= ('label'|'integer')'id'{',''id'}
    <st_list ::= <st>{';'<st>}
    <st> ::= 'id'<H> | <statement_else>
    <H> ::= ':'<statement> | {('*'|'/')<factor>}{('+'|'-')<term>}'=>''id'{'=>''id'}
    <statement> ::= <assign> | <goto_st> | <if_st> | <write_st> | <block>
    <statement_else> ::= <assign_else> | <goto_st> | <if_st> | <write_st> | <block>
    <assign ::= <exp>'=>''id'{'=>''id'}
    <assign_else> ::= <exp_else>'=>''id'{'=>''id'}
    <goto_st>::= 'goto''id'
    <if_st> ::= 'if'<condition>'then'<st_list>['else'<st_list>]'fi'
    <write_st> ::= 'output''('<exp>{','<exp>}')'
    <condition> ::= <exp>('<'|'>'|'=')<exp>
    <exp> ::= <term>{('+'|'-')<term>}
    <exp_else> ::= ('input'|'number'|'('<exp>')'){('*'|'/')<factor>}{('+'|'-')<term>}
    <term> ::= <factor> {('*'|'/')<factor>}
    <factor> ::= 'input' | 'id' | 'number' | '('<exp>')'
