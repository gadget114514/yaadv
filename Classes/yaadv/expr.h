#if !defined(EXPR_H_INCLUDED)
#define EXPR_H_INCLUDED

/*
The MIT License (MIT)

Copyright (c) 2014 Andrea Griffini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

class yaadv {
class Expr {

public:
    struct Error : std::runtime_error {
        int position;
        Error(const std::string& msg, int position = -1)
            : std::runtime_error(msg), position(position)
        {}
    };

    double eval() const;

    static void skipsp(const char *& s) {
        for(;;) {
            while (*s && isspace(*s)) s++;
            if (*s == ';') {
                while (*s && *s != '\n') s++;
            } else break;
        }
    }

    static Expr partialParse(const char *& s, std::map<std::string, double>& vars);

    static Expr parse(const char *s, std::map<std::string, double>& vars) {
        const char *s0 = s;
        Expr expr = partialParse(s, vars);
        if (*s) throw Error("Unexpected extra characters\n", s - s0);
        return expr;
    }

    Expr(double x = 0.0) {
        resreg = 0;
        wrk.push_back(x);
    }

    void swap(Expr& other) {
        std::swap(resreg, other.resreg);
        code.swap(other.code);
        wrk.swap(other.wrk);
        variables.swap(other.variables);
    }

    Expr(const char *s, std::map<std::string, double>& m) {
        Expr e = parse(s, m);
        swap(e);
    }

    operator double() const {
        return eval();
    }

    static void addFunction(const char *name, double (*f)()) {
        func0.push_back(f);
        functions[name] = std::make_pair(func0.size()-1, 0);
    }

    static void addFunction(const char *name, double (*f)(double)) {
        func1.push_back(f);
        functions[name] = std::make_pair(func1.size()-1, 1);
    }

    static void addFunction(const char *name, double (*f)(double, double)) {
        func2.push_back(f);
        functions[name] = std::make_pair(func2.size()-1, 2);
    }

    std::string disassemble() const;

private:
    enum { MOVE, LOAD,
           NEG, NOT,
           ADD, SUB, MUL, DIV, LT, LE, GT, GE, EQ, NE, AND, OR,
           B_SHL, B_SHR, B_AND, B_OR, B_XOR,
           FSIN, FCOS, FFLOOR, FABS, FSQRT, FTAN, FATAN, FLOG, FEXP, FATAN2, FPOW,
           FUNC0, FUNC1, FUNC2 };

    int resreg;
    std::vector<int> code;
    mutable std::vector<double> wrk;
    std::vector<double *> variables;

    struct Operator {
        const char *name;
        int level;
        int opcode;
    };

    static std::map<std::string, Operator> operators;
    static int max_level;
    static std::map<std::string, std::pair<int, int> > inlined;

    static std::map<std::string, std::pair<int, int> > functions;
    static std::vector<double (*)()> func0;
    static std::vector<double (*)(double)> func1;
    static std::vector<double (*)(double,double)> func2;

    class Init;
    friend class Init;

    enum { READONLY = 0x4000000 };

    int reg(std::vector<int>& regs) {
        if (regs.size() == 0) {
            wrk.resize(1 + wrk.size());
            regs.push_back(wrk.size()-1);
        }
        int r = regs.back();
        regs.pop_back();
        return r;
    }

    int compile(std::vector<int>& regs,
                const char *& s, std::map<std::string, double>& vars, int level);

}
}

#endif
