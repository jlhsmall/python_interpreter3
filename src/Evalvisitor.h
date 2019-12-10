#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"
#include<cstdio>
#include<iostream>
#include<string>
#include<cstring>
#include<cstdlib>
#include<map>
#include<vector>
#include<algorithm>
#include<stack>
namespace LEAF {
    enum typname {
        INT, FLOAT, STRING, BOOL, NONE,VAR,FLOW
    };
    enum flowname{
        BREAK,CONTINUE,RETURN
    };
    std::string Add(std::string s1, std::string s2) {
        if (s1.length() < s2.length())s1.swap(s2);
        int la = s1.length(), lb = s2.length();
        int *a = new int[la], *b = new int[la + 1];
        for (int i = 0; i < la; ++i)a[i] = s1[la-1 - i] - '0';
        for (int i = 0; i < lb; ++i)b[i] = s2[lb-1 - i] - '0';
        for (int i = lb; i <= la; ++i)b[i] = 0;
        for (int i = 0; i < la; ++i)if ((b[i] += a[i]) > 9)b[i] -= 10, ++b[i + 1];
        if (b[la])++la;
        std::string ret = "";
        for (int i = la - 1; i >= 0; --i)ret += (char) (b[i] + '0');
        delete[]a;
        delete[]b;
        return ret;
    }

    std::string Sub(std::string s1, std::string s2) {
        int la = s1.length(), lb = s2.length();
        int *a = new int[la], *b = new int[la];
        for (int i = 0; i < la; ++i)a[i] = s1[la-1 - i] - '0';
        for (int i = 0; i < lb; ++i)b[i] = s2[lb-1 - i] - '0';
        for (int i = lb; i < la; ++i)b[i] = 0;
        for (int i = 0; i < la; ++i)if ((a[i] -= b[i]) < 0)a[i] += 10, --a[i + 1];
        while (la>1 && !a[la - 1])--la;
        std::string ret = "";
        for (int i = la - 1; i >= 0; --i)ret += (char) (a[i] + '0');
        delete[]a;
        delete[]b;
        return ret;
    }

    std::string Mul(std::string s1, std::string s2) {
        if(s1=="0"||s2=="0")return "0";
        int la = s1.length(), lb = s2.length();
        int lc = la + lb;
        int *a = new int[la], *b = new int[lb], *c = new int[lc];
        for (int i = 0; i < la; ++i)a[i] = s1[la -1- i] - '0';
        for (int i = 0; i < lb; ++i)b[i] = s2[lb -1- i] - '0';
        for (int i = 0; i < lc; ++i)c[i] = 0;
        for (int i = 0; i < la; ++i)for (int j = 0; j < lb; ++j)c[i + j] += a[i] * b[j];
        for (int i = 0; i < lc - 1; ++i) {
            c[i + 1] += c[i] / 10;
            c[i] %= 10;
        }
        if (!c[lc - 1])--lc;
        std::string ret = "";
        for (int i = lc-1; i >= 0; --i)ret += (char) (c[i] + '0');
        delete[]a;
        delete[]b;
        delete[]c;
        return ret;
    }

    int Cmp(std::string s1, std::string s2) {
        if (s1.length() > s2.length())return 1;
        if (s1.length() < s2.length())return -1;
        int l = s1.length();
        for (int i = 0; i < l; ++i) {
            if (s1[i] < s2[i])return -1;
            if (s1[i] > s2[i])return 1;
        }
        return 0;
    }

    int cmp(int *a, int *b, int l) {
        for (int i = 0; i < l; ++i) {
            if (a[i] < b[i])return -1;
            if (a[i] > b[i])return 1;
        }
        return 0;
    }
    class variable {
        std::string val;
        double fval;
        bool bval, flg;
        flowname flow;
        typname type,rettype;
    public:
        std::string getval(){
            return val;
        }
        int gettype(){
            return type;
        }
        int getflow(){
            return flow;
        }
        void get(std::string s) {
            val=s;
            switch (s[0]) {
                case 'N':
                    type = NONE;
                    break;
                case 'T':
                    bval = 1;
                    type = BOOL;
                    break;
                case 'F':
                    bval = 0;
                    type = BOOL;
                    break;
                case '"':
                    val = val.substr(1, val.length() - 2);
                    type=STRING;
                    break;
                default:
                    if (s[0] == '-')flg = 1, val.erase(0, 1);
                    else flg = 0;
                    int i;
                    for (i = 0; i < val.length(); ++i)if (val[i] == '.')break;
                    if (i < val.length())
                        type = FLOAT, fval = atof(val.c_str());
                    else type = INT;
            }
        }
        variable getvar(std::string s){
            type=VAR;
            val=s;
            return *this;
        }
        variable getbreak(){
            type=FLOW;
            flow=BREAK;
            return *this;
        }
        variable getcontinue(){
            type=FLOW;
            flow=CONTINUE;
            return *this;
        }
        variable getreturn(){
            type=FLOW;
            flow=RETURN;
            return *this;
        }
        void mdf0(){
            if(val=="0")flg=0;
        }
        variable toint(){
            int i;
            switch(type){
                case STRING:
                    get(val);
                    toint();
                    break;
                case FLOAT:
                    for(i=0;i<val.length()&&val[i]!='.';++i);
                    get(val.substr(0,i));
                    break;
                case BOOL:
                    type=INT;
                    val=bval?"1":"0";
                    flg=0;
            }
            return *this;
        }
        variable tostring(){
            if(type==INT&&flg)val="-"+val;
            type=STRING;
            return *this;
        }
        variable tofloat(){
            int i;std::string s;
            switch(type){
                case INT:
                    tostring();
                    tofloat();
                    break;
                case STRING:
                    s=val;
                    for(i=0;i<s.length()&&s[i]!='.';++i);
                    if(i==s.length())s+=".0";
                    get(s);
                    break;
                case BOOL:
                    toint();tofloat();
            }
            return *this;
        }
        variable tobool(){
            switch(type){
                case INT:
                    bval=val!="0";
                    break;
                case FLOAT:
                    bval=fval!=0;
                    break;
                case STRING:
                    bval=val!="";
                    break;
                case NONE:
                    bval=0;
            }
            val=bval?"True":"False";
            type=BOOL;
            return *this;
        }
        variable operator+(variable x) {
            variable y;
            switch (type) {
                case INT:
                    if(x.type==FLOAT){
                        y=*this;y.tofloat();return y+x;
                    }
                    if(x.type==BOOL)x.toint();
                    if (flg == x.flg)x.val = Add(val, x.val);
                    else if (Cmp(val, x.val)>=0)x.flg = flg, x.val = Sub(val, x.val);
                    else x.val = Sub(x.val, val);
                    break;
                case FLOAT:
                    x.tofloat();
                    x.fval += fval;
                    x.val = std::to_string(x.fval);
                    break;
                case STRING:
                    x.val =val+x.val;
                    break;
                case BOOL:
                    y=*this;y.toint();return y+x;
                default:
                    puts("hapi");
            }
            x.mdf0();
            return x;
        }
        variable operator-(variable x) {
            variable y;
            switch (type) {
                case INT:
                    if(x.type==FLOAT){
                        y=*this;y.tofloat();return y-x;
                    }
                    if(x.type==BOOL)x.toint();
                    x.flg ^= 1;
                    return *this + x;
                case FLOAT:
                    x.tofloat();
                    x.fval -= fval;
                    x.val = std::to_string(x.fval);
                    break;
                case BOOL:
                    y=*this;y.toint();return y-x;
                default:
                    puts("hapi");
            }
            x.mdf0();
            return x;
        }

        variable operator*(variable x) {
            long long y;variable z;
            switch (type) {
                case INT:
                    if(x.type==FLOAT){
                        z=*this;z.tofloat();return z*x;
                    }
                    if(x.type==BOOL)x.toint();
                    x.flg ^= flg;
                    x.val = Mul(val, x.val);
                    break;
                case FLOAT:
                    x.tofloat();
                    x.fval *= fval;
                    x.val = std::to_string(x.fval);
                    break;
                case STRING:
                    x.toint();
                    x.type = STRING;
                    y = atol(x.val.c_str());
                    x.val = "";
                    while (y--)x.val += val;
                    return x;
                case BOOL:
                    z=*this;z.toint();return z*x;
                default:
                    puts("hapi");
            }
            x.mdf0();
            return x;
        }

        variable operator/(variable x) {
            x.tofloat();
            variable y=*this;y.tofloat();
            y.fval/=x.fval;
            y.val=std::to_string(y.fval);
            return y;
        }

        friend variable Div(variable x, variable y);

        variable operator%(variable x) {
            switch (type) {
                case INT:
                    return *this - Div(*this, x) * x;
                default:
                    puts("hapi");
            }
            return x;
        }
        variable operator +=(variable x){
            return *this=*this+x;
        }
        variable operator -=(variable x){
            return *this=*this-x;
        }
        variable operator *=(variable x){
            return *this=*this*x;
        }
        variable operator /=(variable x){
            return *this=*this/x;
        }
        variable operator %=(variable x){
            return *this=*this%x;
        }
        variable operator +(){
            variable ret=*this;
            if(type==BOOL)ret.toint();
            return ret;
        }
        variable operator -(){
            variable x;
            x.get("0");
            return x-*this;
        }
        variable operator >(variable x){
            variable ret;ret.type=BOOL;
            variable y;
            switch(type){
                case INT:
                    if(x.type==FLOAT){
                        y=*this;y.tofloat();return y>x;
                    }
                    x.toint();
                    if(!flg&&!x.flg)ret.bval=Cmp(val,x.val)>0;
                    else if(!flg&&x.flg)ret.bval=1;
                    else if(flg&&!x.flg)ret.bval=0;
                    else ret.bval=Cmp(val,x.val)<0;
                    break;
                case FLOAT:
                    x.tofloat();
                    ret.bval=fval>x.fval;
                    break;
                case STRING:
                    ret.bval=val>x.val;
                    break;
                case BOOL:
                    y=*this;y.toint();return y>x;
                default:
                    puts("hapi");
            }
            ret.val=ret.bval?"True":"False";
            return ret;
        }
        variable operator >=(variable x){
            variable ret;ret.type=BOOL;
            variable y;
            switch(type){
                case INT:
                    if(x.type==FLOAT){
                        y=*this;y.tofloat();return y>=x;
                    }
                    x.toint();
                    if(!flg&&!x.flg)ret.bval=Cmp(val,x.val)>=0;
                    else if(!flg&&x.flg)ret.bval=1;
                    else if(flg&&!x.flg)ret.bval=0;
                    else ret.bval=Cmp(val,x.val)<=0;
                    break;
                case FLOAT:
                    x.tofloat();
                    ret.bval=fval>=x.fval;
                    break;
                case STRING:
                    ret.bval=val>=x.val;
                    break;
                case BOOL:
                    y=*this;y.toint();return y>=x;
                default:
                    puts("hapi");
            }
            ret.val=ret.bval?"True":"False";
            return ret;
        }
        variable operator <(variable x){
            variable ret;ret.type=BOOL;
            variable y;
            switch(type){
                case INT:
                    if(x.type==FLOAT){
                        y=*this;y.tofloat();return y<x;
                    }
                    x.toint();
                    if(!flg&&!x.flg)ret.bval=Cmp(val,x.val)<0;
                    else if(!flg&&x.flg)ret.bval=1;
                    else if(flg&&!x.flg)ret.bval=0;
                    else ret.bval=Cmp(val,x.val)>0;
                    break;
                case FLOAT:
                    x.tofloat();
                    ret.bval=fval<x.fval;
                    break;
                case STRING:
                    ret.bval=val<x.val;
                    break;
                case BOOL:
                    y=*this;y.toint();return y<x;
                default:
                    puts("hapi");
            }
            ret.val=ret.bval?"True":"False";
            return ret;
        }
        variable operator <=(variable x){
            variable ret;ret.type=BOOL;
            variable y;
            switch(type){
                case INT:
                    if(x.type==FLOAT){
                        y=*this;y.tofloat();return y<=x;
                    }
                    x.toint();
                    if(!flg&&!x.flg)ret.bval=Cmp(val,x.val)<=0;
                    else if(!flg&&x.flg)ret.bval=1;
                    else if(flg&&!x.flg)ret.bval=0;
                    else ret.bval=Cmp(val,x.val)>=0;
                    break;
                case FLOAT:
                    x.tofloat();
                    ret.bval=fval<=x.fval;
                    break;
                case STRING:
                    ret.bval=val<=x.val;
                    break;
                case BOOL:
                    y=*this;y.toint();return y<=x;
                default:
                    puts("hapi");
            }
            ret.val=ret.bval?"True":"False";
            return ret;
        }
        variable operator ==(variable x){
            variable ret;ret.type=BOOL;
            if(x.type==NONE||type==NONE)ret.bval=x.type==type;
            else{
                variable y;
                switch(type) {
                    case INT:
                        if (x.type == FLOAT) {
                            y = *this;
                            y.tofloat();
                            return y == x;
                        }
                        x.toint();
                        if (flg == x.flg)ret.bval = Cmp(val, x.val) == 0;
                        else ret.bval = 0;
                        break;
                    case FLOAT:
                        x.tofloat();
                        ret.bval = fval == x.fval;
                        break;
                    case STRING:
                        ret.bval = val == x.val;
                        break;
                    case BOOL:
                        y = *this;
                        y.toint();
                        return y == x;
                }
            }
            ret.val=ret.bval?"True":"False";
            return ret;
        }
        variable operator !=(variable x){
            variable ret;ret.type=BOOL;
            if(x.type==NONE||type==NONE){
                ret.bval=x.type!=type;
            }
            else {
                variable y;
                switch (type) {
                    case INT:
                        if (x.type == FLOAT) {
                            y = *this;
                            y.tofloat();
                            return y != x;
                        }
                        x.toint();
                        if (flg == x.flg)ret.bval = Cmp(val, x.val) != 0;
                        else ret.bval = 1;
                        break;
                    case FLOAT:
                        x.tofloat();
                        ret.bval = fval != x.fval;
                        break;
                    case STRING:
                        ret.bval = val != x.val;
                        break;
                    case BOOL:
                        y = *this;
                        y.toint();
                        return y != x;
                    default:
                        puts("hapi");
                }
            }
            ret.val=ret.bval?"True":"False";
            return ret;
        }
        variable flip(){
            if(type!=BOOL){
                tobool();return flip();
            }
            bval^=1;val=bval?"True":"False";
            return *this;
        }
        void out() {
            switch (type) {
                case FLOAT:
                    printf("%.6lf", fval);
                    break;
                case INT:
                    if(flg)putchar('-');
                default:
                    std::cout << val;
            }
        }
    };
    variable Div(variable x, variable y) {
        x.toint();y.toint();
        variable ret;
        ret.flg = x.flg ^ y.flg;ret.type=INT;
        if (Cmp(x.val, y.val) < 0) {
            ret.val = ret.flg ? "1" : "0";
            return ret;
        }
        int la = x.val.length(), lb = y.val.length();
        int lc = la - lb + 1;
        int *a = new int[la + 1], *b = new int[lb + 1], *c = new int[lc];
        for (int i = 0; i < la; ++i)
            a[i + 1] = x.val[i] - '0';
        a[0] = 0;
        for (int i = 0; i < lb; ++i)b[i + 1] = y.val[i] - '0';
        b[0] = 0;
        for (int i = 0; i < lc; ++i) {
            c[i] = 0;
            while (cmp(a + i, b, lb + 1)>=0) {
                for (int j = i + lb; j > i; --j)if ((a[j] -= b[j-i]) < 0)a[j] += 10, --a[j - 1];
                ++c[i];
            }
        }
        int i;
        if (ret.flg) {
            int i;
            for (i = 0; i < la && !a[i]; ++i);
            if (i < la) {
                ++c[lc-1];
                for (int j = lc - 1; j; --j)if (c[j] > 9)c[j] -= 10, ++c[j - 1];
            }
        }
        ret.type=INT;ret.val = "";
        for (int i = c[0] ? 0 : 1; i < lc; ++i)ret.val += (char) (c[i] + '0');
        ret.mdf0();
        return ret;
    }
    variable Anytovariable(antlrcpp::Any x) {
        variable ret;
        if (x.is<std::string>())ret.get(x.as<std::string>());
        return ret;
    }
}
std::map<std::string, std::pair<Python3Parser::SuiteContext*, Python3Parser::TypedargslistContext*> > funcctx;
std::stack<std::map < std::string, LEAF::variable> >   varkey;

class EvalVisitor: public Python3BaseVisitor {

//todo:override all methods of Python3BaseVisitor
    virtual antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        std::map< std::string, LEAF::variable> mp;
        std::vector<LEAF::variable>v;
        varkey.push(mp);
        for(int i=0;i<ctx->stmt().size();i++){
            v=visit(ctx->stmt(i)).as<std::vector<LEAF::variable> >();
            if(!v.empty()&&v[0].gettype()==LEAF::FLOW&&v[0].getflow()==LEAF::RETURN)
                break;
        }
        varkey.pop();
        return nullptr;
    }
    virtual antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        funcctx[ctx->NAME()->toString()]=std::make_pair(ctx->suite(),ctx->parameters()->typedargslist());
        std::vector<LEAF::variable>v;
        return v;
    }
    virtual antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override {
        return visit(ctx->typedargslist());
    }
    /*virtual antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
        return visitChildren(ctx);
    }
    virtual antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
        return visitChildren(ctx);
    }*/
    virtual antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override {
        if(ctx->simple_stmt())return visit(ctx->simple_stmt());
        if(ctx->compound_stmt())return visit(ctx->compound_stmt());
    }

    virtual antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
        return visit(ctx->small_stmt());
    }
    virtual antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
        if(ctx->expr_stmt()!=nullptr)return visit(ctx->expr_stmt());
        if(ctx->flow_stmt()!=nullptr)return visit(ctx->flow_stmt());
    }
    virtual antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
        std::vector<LEAF::variable>v1,v2;
        v2=visit(ctx->testlist(ctx->testlist().size()-1)).as<std::vector<LEAF::variable> >();
        if(ctx->testlist().size()==1)return v2;
        for(int j=0;j<v2.size();++j)if(v2[j].gettype()==LEAF::VAR)v2[j]=varkey.top()[v2[j].getval()];
        if(ctx->augassign()==nullptr) {
            for (int i = (int) ctx->testlist().size() - 2; i >= 0; --i) {
                v1 = visit(ctx->testlist(i)).as<std::vector<LEAF::variable> >();
                for (int j = 0; j < v2.size(); ++j)varkey.top()[v1[j].getval()] = v2[j];
            }
        }
        else if(ctx->augassign()->ADD_ASSIGN()!= nullptr){
            v1=visit(ctx->testlist(0)).as<std::vector<LEAF::variable> >();
            for(int j=0;j<v2.size();++j)varkey.top()[v1[j].getval()]+=v2[j];
        }
        else if(ctx->augassign()->SUB_ASSIGN()!=nullptr){
            v1=visit(ctx->testlist(0)).as<std::vector<LEAF::variable> >();
            for(int j=0;j<v2.size();++j)varkey.top()[v1[j].getval()]-=v2[j];
        }
        else if(ctx->augassign()->MULT_ASSIGN()!= nullptr){
            v1=visit(ctx->testlist(0)).as<std::vector<LEAF::variable> >();
            for(int j=0;j<v2.size();++j)varkey.top()[v1[j].getval()]*=v2[j];
        }
        else if(ctx->augassign()->DIV_ASSIGN()!= nullptr){
            v1=visit(ctx->testlist(0)).as<std::vector<LEAF::variable> >();
            for(int j=0;j<v2.size();++j)varkey.top()[v1[j].getval()]/=v2[j];
        }
        else if(ctx->augassign()->IDIV_ASSIGN()!= nullptr){
            v1=visit(ctx->testlist(0)).as<std::vector<LEAF::variable> >();
            for(int j=0;j<v2.size();++j)varkey.top()[v1[j].getval()]=LEAF::Div(varkey.top()[v1[j].getval()],v2[j]);
        }
        else if(ctx->augassign()->MOD_ASSIGN()!= nullptr){
            v1=visit(ctx->testlist(0)).as<std::vector<LEAF::variable> >();
            for(int j=0;j<v2.size();++j)varkey.top()[v1[j].getval()]%=v2[j];
        }


        for (auto ii = varkey.top().begin(); ii !=varkey.top().end(); ++ii) {
            if (ii->second.gettype() == LEAF::VAR)ii->second = varkey.top()[ii->second.getval()];
            std::cout<<ii->first<<" "<<ii->second.getval()<<std::endl;
        }
        v1.clear();
        return v1;
    }
    /*virtual antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {
        return visitChildren(ctx);
    }*/
    virtual antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
        std::vector<LEAF::variable>v;
        if(ctx->break_stmt()!= nullptr)
            return visit(ctx->break_stmt());
        if(ctx->continue_stmt()!=nullptr)
            return visit(ctx->continue_stmt());
        if(ctx->return_stmt()!= nullptr)
            return visit(ctx->return_stmt());
        return v;
    }
    virtual antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
        std::vector<LEAF::variable>v;LEAF::variable x;
        x.getbreak();v.push_back(x);
        return v;
    }

    virtual antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        std::vector<LEAF::variable>v;LEAF::variable x;
        x.getcontinue();v.push_back(x);
        return v;
    }

    virtual antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
        std::vector<LEAF::variable>v,ret;LEAF::variable x;
        x.getreturn();
        ret.push_back(x);
        if(ctx->testlist()!=nullptr) {
            v = visit(ctx->testlist()).as<std::vector<LEAF::variable> >();
            for(int i=0;i<v.size();++i)ret.push_back(v[i]);
        }
        return ret;
    }

    virtual antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
        if(ctx->if_stmt()!=nullptr)return visit(ctx->if_stmt());
        if(ctx->while_stmt()!=nullptr)return visit(ctx->while_stmt());
        if(ctx->funcdef()!= nullptr)return visit(ctx->funcdef());
    }
    virtual antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        std::vector<LEAF::variable> v;
        int i;
        for (i = 0; i < ctx->test().size(); ++i) {
            v = visit(ctx->test(i)).as<std::vector<LEAF::variable> >();
            if (v[0].gettype() == LEAF::VAR)v[0] = varkey.top()[v[0].getval()];
            v[0].tobool();
            if (v[0].getval() == "True") {
                v = visit(ctx->suite(i)).as<std::vector<LEAF::variable> >();
                break;
            }
        }
        if (i == ctx->test().size() && ctx->ELSE() != nullptr)
            v = visit(ctx->suite((int) ctx->suite().size() - 1)).as<std::vector<LEAF::variable> >();
        return v;
    }

    virtual antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        std::vector<LEAF::variable>v;
        while(1){
            v=visit(ctx->test()).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].tobool();
            if(v[0].getval()=="False")break;
            v = visit(ctx->suite()).as<std::vector<LEAF::variable> >();
            if (!v.empty()&&v[0].gettype() == LEAF::FLOW){
                if(v[0].getflow()==LEAF::BREAK)break;
                if(v[0].getflow()==LEAF::CONTINUE)continue;
                if(v[0].getflow()==LEAF::RETURN)return v;
            }
        }
        v.clear();return v;
    }
    virtual antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override {
        std::vector<LEAF::variable>v;
        if(ctx->simple_stmt()!=nullptr)return visit(ctx->simple_stmt());
        else{
            for(int i=0;i<ctx->stmt().size();++i) {
                v = visit(ctx->stmt(i)).as<std::vector<LEAF::variable> >();
                if (!v.empty()&&v[0].gettype() == LEAF::FLOW)return v;
            }
        }
        v.clear();
        return v;
    }
    virtual antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override {
        return visit(ctx->or_test());
    }

    virtual antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override {
        if(ctx->and_test().size()==1)
            return visit(ctx->and_test(0));
        std::vector<LEAF::variable>v;
        for(int i=0;i<ctx->and_test().size();++i){
            v=visit(ctx->and_test(i)).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].tobool();if(v[0].getval()=="True")return v;
        }
        return v;
    }

    virtual antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override {
        if(ctx->not_test().size()==1)
            return visit(ctx->not_test(0));
        std::vector<LEAF::variable>v;
        for(int i=0;i<ctx->not_test().size();++i){
            v=visit(ctx->not_test(i)).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].tobool();if(v[0].getval()=="False")return v;
        }
        return v;
    }

    virtual antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override {
        if(ctx->NOT()== nullptr)
            return visit(ctx->comparison());
        std::vector<LEAF::variable>v=visit(ctx->not_test()).as<std::vector<LEAF::variable> >();
        if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
        v[0].flip();return v;
    }

    virtual antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override {
        std::map<std::string, LEAF::variable>::iterator ii;
        /*      for (ii = varkey.top().begin(); ii !=varkey.top().end(); ++ii) {
                  if (ii->second.gettype() == LEAF::VAR)ii->second = varkey.top()[ii->second.getval()];
                  std::cout<<ii->first<<" "<<ii->second.getval()<<std::endl;
              }
          */    if(ctx->comp_op().empty())
            return visit(ctx->arith_expr(0));
        std::vector<LEAF::variable>v=visit(ctx->arith_expr(0)).as<std::vector<LEAF::variable> >();
        if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
        LEAF::variable x,y;
        for(int i=0;i<ctx->comp_op().size();++i) {
            x = v[0];
            v = visit(ctx->arith_expr(i + 1)).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            if (ctx->comp_op(i)->LESS_THAN() != nullptr)y = x<v[0];
            if (ctx->comp_op(i)->GREATER_THAN() != nullptr)y = x>v[0];
            if (ctx->comp_op(i)->LT_EQ() != nullptr)y = x<=v[0] ;
            if (ctx->comp_op(i)->GT_EQ() != nullptr)y = x>=v[0];
            if (ctx->comp_op(i)->EQUALS() != nullptr)y = x==v[0] ;
            if (ctx->comp_op(i)->NOT_EQ_2() != nullptr)y = x!=v[0];
            if (y.getval() == "False"){
                v.clear();v.push_back(y);return v;
            }
        }
        v.clear();v.push_back(y);return v;
    }
    /*virtual antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
        return visitChildren(ctx);
    }*/

    virtual antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
        std::vector<std::pair<int, int> > v;//second:1+2-3*4/5//6%
        int i;
        if (!ctx->ADD().empty()) {
            for (i = 0; i < ctx->ADD().size(); ++i)
                v.push_back(std::make_pair(ctx->ADD(i)->getSymbol()->getTokenIndex(), 1));
        }
        if (!ctx->MINUS().empty()) {
            for (i = 0; i < ctx->MINUS().size(); ++i)
                v.push_back(std::make_pair(ctx->MINUS(i)->getSymbol()->getTokenIndex(), 2));
        }
        std::vector<LEAF::variable> v2 = visit(ctx->term(0)).as<std::vector<LEAF::variable> >();
        if(v.empty())return v2;
        std::sort(v.begin(), v.end());
        LEAF::variable x;
        if(v2[0].gettype()==LEAF::VAR)v2[0]=varkey.top()[v2[0].getval()];
        x = v2[0];
        for (i = 0; i < v.size(); ++i) {
            v2 = visit(ctx->term(i + 1)).as<std::vector<LEAF::variable> >();
            if(v2[0].gettype()==LEAF::VAR)v2[0]=varkey.top()[v2[0].getval()];
            switch (v[i].second) {
                case 1:
                    x += v2[0];
                    break;
                case 2:
                    x -= v2[0];
                    break;
            }
        }
        v2.clear();
        v2.push_back(x);
        return v2;
    }

    virtual antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override {
        std::vector<std::pair<int, int> > v;//second:1+2-3*4/5//6%
        int i;
        if (!ctx->STAR().empty()) {
            for (i = 0; i < ctx->STAR().size(); ++i)
                v.push_back(std::make_pair(ctx->STAR(i)->getSymbol()->getTokenIndex(), 3));
        }
        if (!ctx->DIV().empty()) {
            for (i = 0; i < ctx->DIV().size(); ++i)
                v.push_back(std::make_pair(ctx->DIV(i)->getSymbol()->getTokenIndex(), 4));
        }
        if (!ctx->IDIV().empty()) {
            for (i = 0; i < ctx->IDIV().size(); ++i)
                v.push_back(std::make_pair(ctx->IDIV(i)->getSymbol()->getTokenIndex(), 5));
        }
        if (!ctx->MOD().empty()) {
            for (i = 0; i < ctx->MOD().size(); ++i)
                v.push_back(std::make_pair(ctx->MOD(i)->getSymbol()->getTokenIndex(), 6));
        }
        std::vector<LEAF::variable> v2 = visit(ctx->factor(0)).as<std::vector<LEAF::variable> >();
        if(v.empty())return v2;
        std::sort(v.begin(), v.end());
        LEAF::variable x;
        if(v2[0].gettype()==LEAF::VAR)v2[0]=varkey.top()[v2[0].getval()];
        x = v2[0];
        for (i = 0; i < v.size(); ++i) {
            v2 = visit(ctx->factor(i + 1)).as<std::vector<LEAF::variable> >();
            if(v2[0].gettype()==LEAF::VAR)v2[0]=varkey.top()[v2[0].getval()];
            switch (v[i].second) {
                case 3:
                    x *= v2[0];
                    break;
                case 4:
                    x /= v2[0];
                    break;
                case 5:
                    x = Div(x, v2[0]);
                    break;
                case 6:
                    x %= v2[0];
            }
        }
        v2.clear();
        v2.push_back(x);
        return v2;
    }

    virtual antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
        if (ctx->ADD() != nullptr)return visit(ctx->factor());
        if (ctx->MINUS() != nullptr) {
            std::vector<LEAF::variable> v = visit(ctx->factor()).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0] = -v[0];
            return v;
        }
        return visit(ctx->atom_expr());
    }

    virtual antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        if (ctx->trailer() == nullptr)return visit(ctx->atom());
        std::vector<LEAF::variable> v;
        std::string _name = ctx->atom()->NAME()->toString();
        if (_name == "print") {
            std::vector<LEAF::variable> v1;
            if(ctx->trailer()->arglist()!=nullptr) {
                for (int i = 0; i < ctx->trailer()->arglist()->argument().size(); ++i) {
                    v = visit(ctx->trailer()->arglist()->argument(i)->test()).as<std::vector<LEAF::variable> >();
                    for (int j = 0; j < v.size(); ++j)v1.push_back(v[j]);
                }
                for (int i = 0; i < v1.size(); ++i) {
                    if (v1[i].gettype() == LEAF::VAR)v1[i] = varkey.top()[v1[i].getval()];
                    if (i)putchar(' ');
                    v1[i].out();
                }
            }
            puts("");
            v.clear();
        }
        else if (_name == "int") {
            v=visit(ctx->trailer()->arglist()->argument(0)->test()).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].toint();
        }
        else if (_name == "float") {
            v=visit(ctx->trailer()->arglist()->argument(0)->test()).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].tofloat();
        }
        else if (_name == "str") {
            v=visit(ctx->trailer()->arglist()->argument(0)->test()).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].tostring();
        }
        else if (_name == "bool") {
            v=visit(ctx->trailer()->arglist()->argument(0)->test()).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            v[0].tobool();
        }
        else {
            LEAF::variable x;
            x.get("None");
            std::string s;
            std::vector<LEAF::variable> v1;
            std::map<std::string, LEAF::variable> mp;
            if(funcctx[_name].second!=nullptr) {
                int i, j, dt =
                        (int) funcctx[_name].second->tfpdef().size() - (int) funcctx[_name].second->test().size();
                for (i = 0; i < dt; ++i) {
                    s = funcctx[_name].second->tfpdef(i)->NAME()->toString();
                    mp[s] = x;
                }
                for (i = 0; i < funcctx[_name].second->test().size(); ++i) {
                    s = funcctx[_name].second->tfpdef(i + dt)->NAME()->toString();
                    v1 = visit(funcctx[_name].second->test(i)).as<std::vector<LEAF::variable> >();
                    mp[s] = v1[0];
                }
                for (i = 0; i < ctx->trailer()->arglist()->argument().size() &&
                            ctx->trailer()->arglist()->argument(i)->NAME() == nullptr; ++i) {
                    s = funcctx[_name].second->tfpdef(i)->NAME()->toString();
                    v1 = visit(ctx->trailer()->arglist()->argument(i)->test()).as<std::vector<LEAF::variable> >();
                    mp[s] = v1[0];
                }
                for (; i < ctx->trailer()->arglist()->argument().size(); ++i) {
                    s = ctx->trailer()->arglist()->argument(i)->NAME()->toString();
                    v1 = visit(ctx->trailer()->arglist()->argument(i)->test()).as<std::vector<LEAF::variable> >();
                    mp[s] = v1[0];
                }
                std::map<std::string, LEAF::variable>::iterator ii;
                //         std::cout<<_name<<std::endl;
                for (ii = mp.begin(); ii != mp.end(); ++ii) {
                    if (ii->second.gettype() == LEAF::VAR)ii->second = varkey.top()[ii->second.getval()];
                    //                std::cout<<ii->second.getval()<<" ";
                }
                //            std::cout<<std::endl;
            }
            varkey.push(mp);
            v = visit(funcctx[_name].first).as<std::vector<LEAF::variable> >();
            if(!v.empty())v.erase(v.begin());
            for(int i=0;i<v.size();++i)
                if(v[i].gettype()==LEAF::VAR)v[i]=varkey.top()[v[i].getval()];
            varkey.pop();
        }
        return v;
    }

    /*virtual antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override {
        return visit(ctx->arglist());
    }*/

    virtual antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override {
        std::vector<LEAF::variable> ret;
        LEAF::variable x, y;
        if (ctx->NAME() != nullptr) {
            x.getvar(ctx->NAME()->toString());
            ret.push_back(x);
            return ret;
        }
        if (ctx->NUMBER() != nullptr) {
            x.get(ctx->NUMBER()->toString());
        }
        if (!ctx->STRING().empty()) {
            x.get("\"\"");
            for (int i = 0; i < ctx->STRING().size(); ++i) {
                y.get(ctx->STRING(i)->toString());
                x += y;
            }
        }
        if(ctx->TRUE()!=nullptr){
            x.get("True");
        }
        if(ctx->FALSE()!=nullptr){
            x.get("False");
        }
        if(ctx->NONE()!=nullptr){
            x.get("None");
        }
        if(ctx->test()!= nullptr)
            return visit(ctx->test());
        ret.push_back(x);
        return ret;
    }

    virtual antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override {
        std::vector<LEAF::variable>v,ret;
        for(int i=0;i<ctx->test().size();++i){
            v=visit(ctx->test(i)).as<std::vector<LEAF::variable> >();
            if(!v.empty()){
                for(int j=0;j<v.size();++j)ret.push_back(v[j]);
            }
        }
        return ret;
    }
    /*virtual antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override {
        std::vector<LEAF::variable>v,ret;
        for(int i=0;i<ctx->argument().size();++i){
            v=visit(ctx->argument(i)).as<std::vector<LEAF::variable> >();
            if(v[0].gettype()==LEAF::VAR)v[0]=varkey.top()[v[0].getval()];
            ret.push_back(v[0]);
        }
        return ret;
    }

    virtual antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override {
        return visit(ctx->test());
    }*/
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
