
#include "../Core/lexer.h"
#include "../Core/token.h"
#include "../Core/command_splitter.h"
#include "../Core/command.h"
#include "../Core/parse_result.h"
#include "../Core/parse_item.h"
#include "../Core/current_config.h"
#include "../Core/syntax_tree_builder.h"
#include "../Core/ref_value.h"
#include "../Core/syntax_tree_by_char_builder.h"

#include "../SYNTAX/syntax.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stack>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include "../Core/operators_tree_builder.h"

class TestError {
public:
    std::string message;
    int code;
    TestError(int code, std::string message) : code(code), message(message) {}
};

class StreamUtils {
    std::stringbuf sbuf;
    std::streambuf* oldbuf;

    void update_state() {
        this->sbuf = std::stringbuf(std::ios::out);
        this->oldbuf = std::cout.rdbuf(std::addressof(this->sbuf));
    }

public:
    void capture_stream() {
        this->update_state();
    }
    bool out_stream_to_be(std::string expect) {

        std::cout.rdbuf(this->oldbuf);

        std::string output = this->sbuf.str();

        return output == expect;
    }
};



class CompareUtils {
private:
    TestError ERROR = TestError(1, "Invalid type");
public:
    template<class C1>
    void are_equal(C1 c1, C1 c2){    
        try {
            bool equal = c1 == c2;
            if (!equal) throw ERROR;
            std::cout << "Успешно";
        }
        catch (TestError& error) {
            std::cout << "Ошибка: " << error.message << "\n";
            std::cout << "Фактический результат: \n" << c1 << "\nОжидаемый результат: " << "\n" << c2 << "\n";
        }
        catch (...) {
            std::cout << "Неизвестная ошибка: \n";
            std::cout << "Фактический результат: \n" << c1 << "\nОжидаемый результат: " << "\n" << c2 << "\n";
        }
    }
};

class TestDescriptionArgs {
public:
    StreamUtils s_u = StreamUtils();
    CompareUtils c_u = CompareUtils();
};

class TestUtils {
public:
    void it(std::string testName, std::function<void(TestDescriptionArgs utils)> description) {
        std::cout << testName << ": ";
        description(TestDescriptionArgs());
        std::cout << "\n";
    }
};

class Test {
public:
    void describe(std::string testName, std::function<void(TestUtils utils)> description) {
        std::cout << "------------------ start " << testName << "------------------\n";
        description(TestUtils());
        std::cout << "------------------ end " << testName << "------------------\n";
        std::cout << "\n";
    }
};

void run_test(std::string line) {
    Parser lexer;
    auto parseResult = lexer.parse(line);
    auto baseResult = lexer.parse(line);
    std::cout << "Ожидаемый:" << std::endl;
    for (auto& item : parseResult.items) {
        std::cout << item << std::endl;
    }

    std::cout << "Фактический:" << std::endl;
    for (auto& item : baseResult.items) {
        std::cout << item << std::endl;
    }

    if (baseResult == parseResult) {
        std::cout << "Результат: УСПЕХ" << "\n\n";
    }
    else {
        std::cout << "Результат: НЕУДАЧА" << "\n\n";
    }
    
}

void run_simp_test(std::string line) {
    Parser lexer;
    auto parseResult = lexer.parse(line);
    auto baseResult = lexer.parse(line);

    std::cout << "Ожидаемый:" << std::endl;
    std::cout << line << std::endl;
    

    std::cout << "Фактический:" << std::endl;
    std::cout << line << std::endl;
    
    if (baseResult == parseResult) {
        std::cout << "Результат: УСПЕХ" << "\n\n";
    }
    else {
        std::cout << "Результат: НЕУДАЧА" << "\n\n";
    }
}


int main() {
    setlocale(LC_ALL, "");

    run_test("A:= 6.1;");
    run_test("A:= 0;");
    run_test("A:= 4.55;");
    run_test("A:=10.5e-3 < 10.5e+3;");
    run_test("A:= 65.358e+10;");
    run_test("if A > B then A:= B;");
    run_test("if A < B then A:= B else B:= A");
    run_test("A:=25.36e-9;");
    run_test("A:=0.e+8;");
    run_test("A:=5.6;"
        "// comment ");
    run_test("A:=5.6; /* comment */ ");


}
