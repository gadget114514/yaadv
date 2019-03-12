#include "expr.h"
#include <math.h>
#include <stdio.h>
namespace yaadv {
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

std::map<std::string, std::pair<int, int> > Expr::functions;
std::vector<double (*)()> Expr::func0;
std::vector<double (*)(double)> Expr::func1;
std::vector<double (*)(double,double)> Expr::func2;

double Expr::eval() const {
    double *wp = &wrk[0];
    const int *cp = &code[0], *ce = cp+code.size();
    while (cp != ce) {
        switch(cp[0]) {
        case MOVE: wp[cp[1]] = wp[cp[2]]; cp+=3; break;
        case LOAD: wp[cp[1]] = *variables[cp[2]]; cp+=3; break;
        case NEG: wp[cp[1]] = -wp[cp[1]]; cp+=2; break;
        case NOT: wp[cp[1]] = !wp[cp[1]]; cp+=2; break;
        case ADD: wp[cp[1]] += wp[cp[2]]; cp+=3; break;
        case SUB: wp[cp[1]] -= wp[cp[2]]; cp+=3; break;
        case MUL: wp[cp[1]] *= wp[cp[2]]; cp+=3; break;
        case DIV: wp[cp[1]] /= wp[cp[2]]; cp+=3; break;
        case LT:  wp[cp[1]] = (wp[cp[1]] <  wp[cp[2]]); cp+=3; break;
        case LE:  wp[cp[1]] = (wp[cp[1]] <= wp[cp[2]]); cp+=3; break;
        case GT:  wp[cp[1]] = (wp[cp[1]] >  wp[cp[2]]); cp+=3; break;
        case GE:  wp[cp[1]] = (wp[cp[1]] >= wp[cp[2]]); cp+=3; break;
        case EQ:  wp[cp[1]] = (wp[cp[1]] == wp[cp[2]]); cp+=3; break;
        case NE:  wp[cp[1]] = (wp[cp[1]] != wp[cp[2]]); cp+=3; break;
        case AND: wp[cp[1]] = (wp[cp[1]] && wp[cp[2]]); cp+=3; break;
        case OR:  wp[cp[1]] = (wp[cp[1]] || wp[cp[2]]); cp+=3; break;
        case B_OR: wp[cp[1]] = (int(wp[cp[1]]) | int(wp[cp[2]])); cp+=3; break;
        case B_AND: wp[cp[1]] = (int(wp[cp[1]]) & int(wp[cp[2]])); cp+=3; break;
        case B_XOR: wp[cp[1]] = (int(wp[cp[1]]) ^ int(wp[cp[2]])); cp+=3; break;
        case B_SHL: wp[cp[1]] = (int(wp[cp[1]]) << int(wp[cp[2]])); cp+=3; break;
        case B_SHR: wp[cp[1]] = (int(wp[cp[1]]) >> int(wp[cp[2]])); cp+=3; break;
        case FFLOOR: wp[cp[1]] = floor(wp[cp[1]]); cp+=2; break;
        case FABS: wp[cp[1]] = fabs(wp[cp[1]]); cp+=2; break;
        case FSIN: wp[cp[1]] = sin(wp[cp[1]]); cp+=2; break;
        case FCOS: wp[cp[1]] = cos(wp[cp[1]]); cp+=2; break;
        case FSQRT: wp[cp[1]] = sqrt(wp[cp[1]]); cp+=2; break;
        case FTAN: wp[cp[1]] = tan(wp[cp[1]]); cp+=2; break;
        case FATAN: wp[cp[1]] = atan(wp[cp[1]]); cp+=2; break;
        case FLOG: wp[cp[1]] = log(wp[cp[1]]); cp+=2; break;
        case FEXP: wp[cp[1]] = exp(wp[cp[1]]); cp+=2; break;
        case FATAN2: wp[cp[1]] = atan2(wp[cp[1]], wp[cp[2]]); cp+=3; break;
        case FPOW: wp[cp[1]] = pow(wp[cp[1]], wp[cp[2]]); cp+=3; break;
        case FUNC0: wp[cp[2]] = func0[cp[1]](); cp+=3; break;
        case FUNC1: wp[cp[2]] = func1[cp[1]](wp[cp[2]]); cp+=3; break;
        case FUNC2: wp[cp[2]] = func2[cp[1]](wp[cp[2]], wp[cp[3]]); cp+=4; break;
        }
    }
    return wp[resreg];
}

Expr Expr::partialParse(const char *& s, std::map<std::string, double>& vars) {
    Expr result;
    std::vector<int> regs;
    const char *s0 = s;
    try {
        result.resreg = result.compile(regs, s, vars, -1)&~READONLY;
        skipsp(s);
    } catch (const Error& re) {
        throw Error(re.what(), s - s0);
    }
    //printf("s0 = \"%s\":\n%s\n\n", s0, result.disassemble().c_str());
    return result;
}

int Expr::compile(std::vector<int>& regs,
                  const char *& s, std::map<std::string, double>& vars, int level) {
    if (level == -1) level = max_level;
    if (level == 0) {
        skipsp(s);
        if (*s == '(') {
            s++;
            int res = compile(regs, s, vars, -1);
            skipsp(s);
            if (*s != ')') throw Error("')' expected");
            s++;
            return res;
        } else if (isdigit((unsigned char)*s) || (*s=='-' && isdigit((unsigned char)s[1]))) {
            char *ss = 0;
            double v = strtod(s, &ss);
            if (ss && ss!=s) {
                int x = wrk.size(); wrk.push_back(v);
                s = (const char *)ss;
                return x | READONLY;
            } else {
                throw Error("Invalid number");
            }
        } else if (*s == '-') {
            s++;
            int res = compile(regs, s, vars, 0);
            if (res & READONLY) {
                int r1 = reg(regs);
                code.push_back(MOVE); code.push_back(r1); code.push_back(res&~READONLY);
                res = r1;
            }
            code.push_back(NEG); code.push_back(res);
            return res;
        } else if (*s == '!') {
            s++;
            int res = compile(regs, s, vars, 0);
            if (res & READONLY) {
                int r1 = reg(regs);
                code.push_back(MOVE); code.push_back(r1); code.push_back(res&~READONLY);
                res = r1;
            }
            code.push_back(NOT); code.push_back(res);
            return res;
        } else if (*s && (*s == '_' || isalpha((unsigned char)*s))) {
            const char *s0 = s;
            while (*s && (isalpha((unsigned char)*s) || isdigit((unsigned char)*s) || *s == '_')) s++;
            std::string name(s0, s);
            if (*s == '(') {
                bool ii = false;
                std::map<std::string, std::pair<int, int> >::iterator it = functions.find(name);
                if (it == functions.end()) {
                    ii = true;
                    it = inlined.find(name);
                    if (it == inlined.end()) throw Error(std::string("Unknown function '" + name + "'"));
                }
                s++;
                std::vector<int> args;
                int id = it->second.first;
                int arity = it->second.second;
                for (int a=0; a<arity; a++) {
                    args.push_back(compile(regs, s, vars, -1));
                    if (a != arity-1) {
                        skipsp(s);
                        if (*s != ',') throw Error("',' expected");
                        s++;
                    }
                }
                skipsp(s);
                if (*s != ')') throw Error("')' expected");
                s++;
                int target = (arity == 0) ? reg(regs) : args[0];
                if (target & READONLY) {
                    int r1 = reg(regs);
                    code.push_back(MOVE); code.push_back(r1); code.push_back(target&~READONLY);
                    target = r1;
                }
                if (ii) {
                    code.push_back(id);
                } else {
                    code.push_back(FUNC0 + arity);
                    code.push_back(id);
                }
                code.push_back(target);
                for (int i=1; i<arity; i++) {
                    code.push_back(args[i]&~READONLY);
                    if (!(args[i] & READONLY)) {
                        regs.push_back(args[i]);
                    }
                }
                return target;
            } else {
                std::map<std::string, double>::iterator it = vars.find(name);
                if (it != vars.end()) {
                    int target = reg(regs);
                    variables.push_back(&it->second);
                    code.push_back(LOAD);
                    code.push_back(target);
                    code.push_back(variables.size()-1);
                    return target;
                } else {
                    throw Error(std::string("Unknown variable '" + name + "'"));
                }
            }
        } else {
            throw Error("Syntax error");
        }
    }
    int res = compile(regs, s, vars, level-1);
    while (skipsp(s), *s) {
        std::map<std::string, Operator>::iterator it = operators.find(std::string(s, s+2));
        if (it == operators.end()) it = operators.find(std::string(s, s+1));
        if (it == operators.end() || it->second.level != level) break;
        s += it->first.size();
        int x = compile(regs, s, vars, level-1);
        if (res & READONLY) {
            int r1 = reg(regs);
            code.push_back(MOVE); code.push_back(r1); code.push_back(res&~READONLY);
            res = r1;
        }
        code.push_back(it->second.opcode);
        code.push_back(res);
        code.push_back(x&~READONLY);
        if (!(x&READONLY)) {
            regs.push_back(x);
        }
    }
    return res;
}

std::string Expr::disassemble() const {
    const char *opnames[] = { "MOVE", "LOAD",
                              "NEG",
                              "ADD", "SUB", "MUL", "DIV", "LT", "LE", "GT", "GE", "EQ", "NE", "AND", "OR",
                              "B_SHL", "B_SHR", "B_AND", "B_OR", "B_XOR",
                              "FSIN", "FCOS", "FFLOOR", "FABS", "FSQRT", "FTAN", "FATAN", "FLOG", "FEXP",
                              "FATAN2", "FPOW",
                              "FUNC0", "FUNC1", "FUNC2" };
    std::string result;
    char buf[30];
    const char *fn = "?";
    for (int i=0,n=code.size(); i<n; i++) {
        sprintf(buf, "%i: ", i);
        result += buf;
        result += opnames[code[i]];
        switch(code[i]) {
        case MOVE:
            sprintf(buf, "(%i = %i) v=%0.3f\n", code[i+1], code[i+2], wrk[code[i+2]]);
            i += 2;
            break;
        case LOAD:
            sprintf(buf, "(%i = %p)\n", code[i+1], variables[code[i+2]]);
            i += 2;
            break;
        case NEG:
        case FSIN:
        case FCOS:
        case FFLOOR:
        case FABS:
        case FSQRT:
        case FTAN:
        case FATAN:
        case FLOG:
        case FEXP:
            sprintf(buf, "(%i)\n", code[i+1]);
            i += 1;
            break;
        case FATAN2:
        case FPOW:
            sprintf(buf, "(%i, %i) -> %i\n", code[i+1], code[i+2], code[i+1]);
            i += 2;
            break;
        case FUNC0:
        case FUNC1:
        case FUNC2:
            fn = "?";
            for (std::map<std::string, std::pair<int, int> >::iterator it=functions.begin();
                 it!=functions.end(); ++it) {
                if (it->second.second == code[i]-FUNC0 && it->second.first == code[i+1]) {
                    fn=it->first.c_str();
                }
            }
            switch(code[i]) {
            case FUNC0: sprintf(buf, " %p=%s() -> %i\n",
                                func0[code[i+1]], fn, code[i+2]);
                i+=2; break;
            case FUNC1: sprintf(buf, " %p=%s(%i) -> %i\n",
                                func1[code[i+1]], fn, code[i+2], code[i+2]);
                i+=2; break;
            case FUNC2: sprintf(buf, " %p=%s(%i, %i) -> %i\n",
                                func2[code[i+1]], fn, code[i+2], code[i+3], code[i+2]);
                i+=3; break;
            }
            break;
        default:
            sprintf(buf, "(%i, %i) -> %i\n", code[i+1], code[i+2], code[i+1]);
            i += 2;
            break;
        }
        result += buf;
    }
    return result;
}

std::map<std::string, std::pair<int, int> > Expr::inlined;

int Expr::max_level;
std::map<std::string, Expr::Operator> Expr::operators;

class Expr::Init {
    static double random() {
        return double(rand()) / RAND_MAX;
    }

public:
    Init() {
        const Expr::Operator ops[] =
            {{"*", 1, MUL}, {"/", 1, DIV},
             {"+", 2, ADD}, {"-", 2, SUB},

             {"<<", 3, B_SHL}, {">>", 3, B_SHR},
             {"&", 4, B_AND},
             {"|", 5, B_OR}, {"^", 5, B_XOR},

             {"<", 6, LT}, {">", 6, GT}, {"<=", 6, LE}, {">=", 6, GE}, {"==", 6, EQ}, {"!=", 6, NE},

             {"&&", 7, AND},

             {"||", 8, OR}};
        int n = sizeof(ops) / sizeof(ops[0]);
        for (int i=0; i<n; i++) {
            Expr::operators[ops[i].name] = ops[i];
        }
        Expr::max_level = ops[n-1].level;

        Expr::addFunction("random", random);

        Expr::inlined["floor"] = std::make_pair(Expr::FFLOOR, 1);
        Expr::inlined["abs"] = std::make_pair(Expr::FABS, 1);
        Expr::inlined["sqrt"] = std::make_pair(Expr::FSQRT, 1);
        Expr::inlined["sin"] = std::make_pair(Expr::FSIN, 1);
        Expr::inlined["cos"] = std::make_pair(Expr::FCOS, 1);
        Expr::inlined["tan"] = std::make_pair(Expr::FTAN, 1);
        Expr::inlined["atan"] = std::make_pair(Expr::FATAN, 1);
        Expr::inlined["log"] = std::make_pair(Expr::FLOG, 1);
        Expr::inlined["exp"] = std::make_pair(Expr::FEXP, 1);
        Expr::inlined["atan2"] = std::make_pair(Expr::FATAN2, 2);
        Expr::inlined["pow"] = std::make_pair(Expr::FPOW, 2);
    }
} init_instance;

}
