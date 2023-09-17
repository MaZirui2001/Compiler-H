# LabLexer-3 report

## 实验内容

* 使用antlr，构建词法分析器

## 问题分析

* antlr的使用方法显然比flex更复杂，首先我们依然要对其规则进行学习。
* antlr是直接对一整个字符串进行处理，因此之前在flex中使用的getchar方法已经不再适用
* antlr可以用一个对象的成员来统计连续非关系运算符的个数，但这里我们也不能够再使用一个.来表示“其他字符”了

## 实验设计

* antlr的语法文件如下：

  ```
  lexer grammar relop;
  
  Equal:          '=';
  NonEqual:       '<>';
  Less:           '<';
  Greater:        '>';
  LessEqual:      '<=';
  GreaterEqual:   '>=';
  
  EndofInput:     [\n]|[\r\n];
  OtherChar:      (~([=]|[<>]|[<]|[>]|[<=]|[>=]|[\n]|[\r\n]))+;
  ```

  可以看出，antlr的语法规则显然比flex更加简洁。同时，这里也引入了~运算符，它可以用来表示“除某些字符以外的其他字符”

* 主函数设计

  ```c++
  for (auto token : tokens.getTokens()) {
      //std::cout << token->getText() << std::endl;
      std::string text = token->getText();
      int length = token->getStopIndex() - token->getStartIndex() + 1;
      //std::cout << length << std::endl;
      if(text == "=" || text == ">" || text == "<" || text == ">=" || text == "<=" || text == "<>"){
          std::cout << "(relop," << text << ")";
      }
      else {
          if(length == 0) break;
          std::cout << "(other," << length << ")";
      }
  }
  ```

  通过getStopIndex和getStartIndex方法，我们可以获得连续字符的长度，从而进行打印。而若要获得字符内容，需要调用getText方法。

## 遇到的问题

* antlr-runtime库没有链接：

  需要在本地编译antlr-runtime库，才能使得CMakeLists中对target的修改生效
