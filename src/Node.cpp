#include "Node.hpp"

using namespace kodiak;

std::string Node::toString() const {
    std::ostringstream strstream;
    print(strstream);
    return strstream.str();
}

// --- RealVal_Node ---

RealVal_Node::RealVal_Node(const Interval &i) : val_(i) {
    realExpressionType_ = ATOM;
}

Real RealVal_Node::deriv(const nat) const {
    return val(Interval::ZERO());
}

Interval RealVal_Node::eval(const Box &, NamedBox &, const bool) {
    return val_;
}

Real RealVal_Node::subs(const Substitution &, Names &) const {
    return val(val_);
}

void RealVal_Node::print(std::ostream &os) const {
    if (val_.isPoint()) {
        os << val_.inf();
    } else {
        os << val_;
    }
}

// --- RealVar_Node ---

RealVar_Node::RealVar_Node(const nat v, const std::string name) : var_(v), name_(name) {
    variableIndexes_.insert(v);
    realExpressionType_ = ATOM;
    numOfVariables = v + 1;
}

Real RealVar_Node::deriv(const nat v) const {
    if (var_ == v) {
        return val(Interval::ONE());
    }
    return val(Interval::ZERO());
}

Interval RealVar_Node::eval(const Box &box, NamedBox &, const bool) {
    if (var_ < box.size()) {
        return box[var_];
    }
    std::ostringstream os;
    os << "Kodiak (eval): variable \"" << name() << "\" is out of the box";
    throw Growl(os.str());
}

Real RealVar_Node::subs(const Substitution &, Names &) const {
    return var(var_, name_);
}

std::string RealVar_Node::name() const {
    if (Kodiak::debug() || name_ == "")
        return var_name(var_);
    return name_;
}

void RealVar_Node::print(std::ostream &os) const {
    os << name();
}

// --- RealName_Node ---

RealName_Node::RealName_Node(const std::string name, const bool local) :
name_(name), local_(local) {
    if (local) {
        localVariables_.insert(name);
        realExpressionType_ = REAL;
    } else {
        globalConstants_.insert(name);
        realExpressionType_ = ATOM;
    }
    db_ = 0;
}

Real RealName_Node::deriv(const nat v) const {
    if (local_) {
        std::ostringstream os;
        os << "d" << name_ << "/d" << var_name(v);
        return var(os.str());
    } else
        return val(Interval::ZERO());
}

Interval RealName_Node::eval(const Box &, NamedBox &constbox, const bool) {
    nat l = constbox.var(name_);
    if (l < constbox.size())
        return constbox.val(l);
    std::ostringstream os;
    os << "Kodiak (eval): " << (local_ ? "variable" : "constant") << " \"" << name_ << "\" not found";
    throw Growl(os.str());
}

Real RealName_Node::subs(const Substitution &sigma, Names &names) const {
    if (std::find(names.begin(), names.end(), name_) == names.end()) {
        Substitution::const_iterator it = sigma.find(name_);
        if (it != sigma.end()) {
            return it->second;
        }
    }
    if (local_) return var(name_);
    return cnst(name_);
}

void RealName_Node::print(std::ostream &os) const {
    std::string q = local_ ? "'" : "\"";
    os << q << name_ << q;
}

// --- RealUnary_Node ---

RealUnary_Node::RealUnary_Node(const OpType op, const Real &e) :
op_(op), ope_(e) {
    variableIndexes_ = e.vars();
    localVariables_ = e.locals();
    globalConstants_ = e.consts();
    realExpressionType_ = REAL;
    if (e.isMonomExpr() && op == NEG)
        realExpressionType_ = MONOMEXPR;
    else if ((op == NEG || op == SQ) && e.isPolyExpr())
        realExpressionType_ = POLYEXPR;
    numOfVariables = e.numberOfVariables();
}

Real RealUnary_Node::deriv(const nat v) const {
    switch (op_) {
        case NEG:
            return -ope_.deriv(v);
        case ABS:
            return ifnz(ope_, val(Interval(-1)), val(Interval::ONE())) * ope_.deriv(v);
        case SQ:
            return val(Interval(2)) * (ope_ * ope_.deriv(v));
        case SQRT:
            return ope_.deriv(v) / (val(Interval(2)) * Sqrt(ope_));
        case EXP:
            return Exp(ope_) * ope_.deriv(v);
        case LN:
            return ope_.deriv(v) / ope_;
        case SIN:
            return Cos(ope_) * ope_.deriv(v);
        case COS:
            return -Sin(ope_) * ope_.deriv(v);
        case TAN:
            return ope_.deriv(v) / Sq(Cos(ope_));
        case ASIN:
            return ope_.deriv(v) / Sqrt(val(Interval::ONE()) - Sq(ope_));
        case ACOS:
            return -ope_.deriv(v) / Sqrt(val(Interval::ONE()) - Sq(ope_));
        case ATAN:
            return ope_.deriv(v) / (val(Interval::ONE()) + Sq(ope_));
        case SINGLE_ULP:
        case DOUBLE_ULP:
            return Real(NotAReal); // TODO: NOTE: This is not the real derivative, but is only used for checking if the function is increasing or decreasing
        case DOUBLE_FEXP:
            return ope_.deriv(v); // TODO: NOTE: This is not the real derivative, but is only used for checking if the function is increasing or decreasing
        default:
            throw Growl("Kodiak (deriv): unary operator not supported");
    }
}

Interval RealUnary_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    switch (op_) {
        case NEG:
            return -ope_.eval(box, constbox, enclosure);
        case ABS:
            return abs(ope_.eval(box, constbox, enclosure));
        case SQ:
            return sqr(ope_.eval(box, constbox, enclosure));
        case SQRT:
        {
            Interval i = ope_.eval(box, constbox, enclosure);
            if (i.plt(0))
                throw Growl("Kodiak (eval): sqrt expects a nonnegative interval");
            return sqrt(i);
        }
        case EXP:
            return exp(ope_.eval(box, constbox, enclosure));
        case LN:
        {
            Interval i = ope_.eval(box, constbox, enclosure);
            if (i.ple(0))
                throw Growl("Kodiak (eval): ln expects a positive interval");
            return log(i);
        }
        case SIN:
            return sin(ope_.eval(box, constbox, enclosure));
        case COS:
            return cos(ope_.eval(box, constbox, enclosure));
        case TAN:
        {
            Interval i = ope_.eval(box, constbox, enclosure);
            if (!(i.cgt(-Interval::PI() / Interval(2)) && i.clt(Interval::PI() / Interval(2))))
                throw Growl("Kodiak (tan): tan expects a proper interval in [-pi/2,pi/2]");
            return tan(i);
        }
        case ASIN:
        {
            Interval i = ope_.eval(box, constbox, enclosure);
            if (!i.subset(Interval(-1, 1)))
                throw Growl("Kodiak (eval): asin expects an interval in [-1,1]");
            return asin(i);
        }
        case ACOS:
        {
            Interval i = ope_.eval(box, constbox, enclosure);
            if (!i.subset(Interval(-1, 1)))
                throw Growl("Kodiak (eval): acos expects an interval in [-1,1]");
            return acos(i);
        }
        case ATAN:
            return atan(ope_.eval(box, constbox, enclosure));
        case SINGLE_ULP:
            return SingleUlp(ope_.eval(box, constbox, enclosure));
        case DOUBLE_ULP:
            return DoubleUlp(ope_.eval(box, constbox, enclosure));
        case DOUBLE_FEXP:
            return DoubleFExp(ope_.eval(box, constbox, enclosure));
        default:
            throw Growl("Kodiak (eval): unary operator not supported");
    }
}

Real RealUnary_Node::subs(const Substitution &sigma, Names &names) const {
    switch (op_) {
        case NEG:
            return -ope_.subs(sigma, names);
        case ABS:
            return Abs(ope_.subs(sigma, names));
        case SQ:
            return Sq(ope_.subs(sigma, names));
        case SQRT:
            return Sqrt(ope_.subs(sigma, names));
        case EXP:
            return Exp(ope_.subs(sigma, names));
        case LN:
            return Ln(ope_.subs(sigma, names));
        case SIN:
            return Sin(ope_.subs(sigma, names));
        case COS:
            return Cos(ope_.subs(sigma, names));
        case TAN:
            return Tan(ope_.subs(sigma, names));
        case ASIN:
            return Asin(ope_.subs(sigma, names));
        case ACOS:
            return Acos(ope_.subs(sigma, names));
        case ATAN:
            return Atan(ope_.subs(sigma, names));
        case SINGLE_ULP:
            return SUlp(ope_.subs(sigma, names));
        case DOUBLE_ULP:
            return DUlp(ope_.subs(sigma, names));
        case DOUBLE_FEXP:
            return DFExp(ope_.subs(sigma, names));
        default:
            throw Growl("Kodiak (subs): unary operator not supported");
    }
}

void RealUnary_Node::print(std::ostream &os) const {
    bool bin = ope_.isBinary() || (op_ == NEG && ope_.isAtom());
    os << op_
            << (bin ? "" : "(") << ope_ << (bin ? "" : ")");
}

// --- Error_Node ---

Error_Node::Error_Node(const Real &value, const Real &error) : value_(value), error_(error) {

    variableIndexes_ = value.vars();
    variableIndexes_.insert(error.vars());
    realExpressionType_ = REAL;
    numOfVariables = std::max({value.numberOfVariables(), error.numberOfVariables()});
}

Real Error_Node::deriv(const nat) const {
    return NotAReal;
}

Interval Error_Node::eval(const Box &box, NamedBox &namedBox, const bool useBernstein) {

    Interval valueInterval = this->value_.eval(box,namedBox,useBernstein);
    Interval absoluteErrorInterval = this->error_.eval(box,namedBox,useBernstein);

    const real &maxAbsoluteError = absoluteErrorInterval.sup();
    Interval possibleErrorInterval = Interval(-maxAbsoluteError, maxAbsoluteError);

    return valueInterval + possibleErrorInterval;
}

Real Error_Node::subs(const Substitution &sigma, Names &names) const {
    const Real &valueSubs = this->value_.subs(sigma, names);
    const Real &errorSubs = this->error_.subs(sigma, names);
    return Error(valueSubs, errorSubs);
}

void Error_Node::print(std::ostream &os) const {
    os << "Error(" << this->value_  << "," << this->error_ << ")";
}

const Real &Error_Node::getValue() const {
    return this->value_;
}

const Real &Error_Node::getError() const {
    return this->error_;
}

bool Error_Node::isError() const {
    return true;
}

bool Error_Node::isEqual(Node const & other) const {
    Error_Node const & otherErrorNode = static_cast<Error_Node const &>(other);
    return this->value_ == otherErrorNode.value_ && this->error_ == otherErrorNode.error_;
}

// --- Floor_Node ---

Floor_Node::Floor_Node(const Real &expr) : operand_(expr) {
    variableIndexes_ = expr.vars();
    localVariables_ = expr.locals();
    globalConstants_ = expr.consts();
    realExpressionType_ = REAL;
    numOfVariables = expr.numberOfVariables();
}

Real Floor_Node::deriv(const nat) const {
    return NotAReal;
}

Interval Floor_Node::eval(const Box &box, NamedBox &namedBox, const bool useBernstein) {

    Interval operandInterval = this->operand_.eval(box,namedBox,useBernstein);

    real floorLower = std::floor(operandInterval.inf());
    real floorUpper = std::floor(operandInterval.sup());

    return Interval(floorLower,floorUpper);
}

Real Floor_Node::subs(const Substitution &sigma, Names &names) const {
    return Floor({this->operand_.subs(sigma,names)});
}

void Floor_Node::print(std::ostream &os) const {
    os << "Floor(" << this->operand_ << ")";
}

const Real &Floor_Node::getOperand() const {
    return this->operand_;
}

bool Floor_Node::isFloor() const {
    return true;
}

bool Floor_Node::isEqual(Node const & other) const {
    Floor_Node const & otherFloorNode = static_cast<Floor_Node const &>(other);
    return this->operand_ == otherFloorNode.operand_;
}


// --- Max_Node ---

Max_Node::Max_Node(std::vector<Real> &listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Max_Node[ctor]): a Max_Node cannot be created with an empty list of expressions");
    }
    for (auto r : listOfReals) {
        this->operands_.push_back(r);
    }
    for (auto r : this->operands_) {
        const VarBag &b2 = r.vars();
        variableIndexes_.insert(b2);
    }
    realExpressionType_ = REAL;
    for (auto r : this->operands_) {
        numOfVariables = std::max({numOfVariables, r.numberOfVariables()});
    }
}

Real Max_Node::deriv(const nat) const {
    return NotAReal;
}

Interval Max_Node::eval(const Box &box, NamedBox &namedBox, const bool useBernstein) {
    Interval maximum = this->operands_[0].eval(box,namedBox,useBernstein);
    real maximumInf = maximum.inf();
    real maximumSup = maximum.sup();
    // TODO: Avoid evaluating twice the first operand
    for (auto expr: this->operands_) {
        Interval result = expr.eval(box,namedBox,useBernstein);
        real resultInf = result.inf();
        real resultSup = result.sup();
        if (resultInf > maximumInf) {
            maximumInf = resultInf;
        }
        if (resultSup > maximumSup) {
            maximumSup = resultSup;
        }
    }
    return Interval(maximumInf,maximumSup);
}

Real Max_Node::subs(const Substitution &sigma, Names &names) const {
    std::vector<Real> subsOperands;
    for (auto expr : this->operands_) {
        subsOperands.push_back(expr.subs(sigma, names));
    }
    return Max(subsOperands);
}

void Max_Node::print(std::ostream &os) const {
    os << "Max(";
    bool firstElement = true;
    for (auto expr : this->operands_) {
        if (firstElement) {
            firstElement = false;
        } else {
            os << ",";
        }
        os << expr;
    }
    os << ")";
}

const std::vector<Real> &Max_Node::getOperands() const {
    return this->operands_;
}

bool Max_Node::isMax() const {
    return true;
}

bool Max_Node::isEqual(Node const & other) const {
    Max_Node const & otherMaxNode = static_cast<Max_Node const &>(other);
    return this->operands_ == otherMaxNode.operands_;
}


// --- Min_Node ---

Min_Node::Min_Node(std::initializer_list<const Real> listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Min_Node[ctor]): a Min_Node cannot be created with an empty list of expressions");
    }
    for (auto r : listOfReals) {
        this->operands_.push_back(r);
    }
    for (auto r : this->operands_) {
        variableIndexes_.insert(r.vars());
    }
    realExpressionType_ = REAL;
    for (auto r : this->operands_) {
        numOfVariables = std::max({numOfVariables, r.numberOfVariables()});
    }
}

Min_Node::Min_Node(std::vector<Real> &listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Min_Node[ctor]): a Min_Node cannot be created with an empty list of expressions");
    }
    for (auto r : listOfReals) {
        this->operands_.push_back(r);
    }
    for (auto r : this->operands_) {
        variableIndexes_.insert(r.vars());
    }
    realExpressionType_ = REAL;
    for (auto r : this->operands_) {
        numOfVariables = std::max({numOfVariables, r.numberOfVariables()});
    }
}

Real Min_Node::deriv(const nat) const {
    return NotAReal;
}

Interval Min_Node::eval(const Box &box, NamedBox &namedBox, const bool useBernstein) {
    Interval minimum = this->operands_[0].eval(box,namedBox,useBernstein);
    real minimumInf = minimum.inf();
    real minimumSup = minimum.sup();
    // TODO: Avoid evaluating twice the first operand
    for (auto expr: this->operands_) {
        Interval result = expr.eval(box,namedBox,useBernstein);
        real resultInf = result.inf();
        real resultSup = result.sup();
        if (resultInf < minimumInf) {
            minimumInf = resultInf;
        }
        if (resultSup < minimumSup) {
            minimumSup = resultSup;
        }
    }
    return Interval(minimumInf,minimumSup);
}

Real Min_Node::subs(const Substitution &sigma, Names &names) const {
    std::vector<Real> subsOperands;
    for (auto expr : this->operands_) {
        subsOperands.push_back(expr.subs(sigma, names));
    }
    return Min(subsOperands);
}

void Min_Node::print(std::ostream &os) const {
    os << "Min(";
    bool firstElement = true;
    for (auto expr : this->operands_) {
        if (firstElement) {
            firstElement = false;
        } else {
            os << ",";
        }
        os << expr;
    }
    os << ")";
}

const std::vector<Real> &Min_Node::getOperands() const {
    return this->operands_;
}

bool Min_Node::isMin() const {
    return true;
}

bool Min_Node::isEqual(Node const & other) const {
    Min_Node const & otherMinNode = static_cast<Min_Node const &>(other);
    return this->operands_ == otherMinNode.operands_;
}

// --- RealBinary_Node ---

RealBinary_Node::RealBinary_Node(const OpType op, const Real &e1, const Real &e2) :
op_(op), ope1_(e1), ope2_(e2) {
    variableIndexes_ = e1.vars();
    variableIndexes_.insert(e2.vars());
    localVariables_ = e1.locals();
    localVariables_.insert(e2.locals().begin(), e2.locals().end());
    globalConstants_ = e1.consts();
    globalConstants_.insert(e2.consts().begin(), e2.consts().end());
    realExpressionType_ = REAL;
    if (op_ == MULT && e1.isMonomExpr() && e2.isMonomExpr())
        realExpressionType_ = MONOMEXPR;
    else if ((op_ == ADD || op_ == SUB || op_ == MULT) && e1.isPolyExpr() && e2.isPolyExpr())
        realExpressionType_ = POLYEXPR;
    NameSet interset;
    intersection(localVariables_, globalConstants_, interset);
    if (!interset.empty()) {
        std::ostringstream os;
        os << "Kodiak (binary): names in " << interset << " occur as variable and constant names";
        throw Growl(os.str());
    }
    if (e1.numberOfVariables() > e2.numberOfVariables())
        numOfVariables = e1.numberOfVariables();
    else
        numOfVariables = e2.numberOfVariables();
}

Real RealBinary_Node::deriv(const nat v) const {
    switch (op_) {
        case ADD:
            return ope1_.deriv(v) + ope2_.deriv(v);
        case SUB:
            return ope1_.deriv(v) - ope2_.deriv(v);
        case MULT:
            return ope1_.deriv(v) * ope2_ + ope2_.deriv(v) * ope1_;
        case DIV:
            return (ope1_.deriv(v) * ope2_ - ope2_.deriv(v) * ope1_) / Sq(ope2_);
        default:
            throw Growl("Kodiak (deriv): binary operator not supported");
    }
}

Interval RealBinary_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    switch (op_) {
        case ADD:
            return ope1_.eval(box, constbox, enclosure) + ope2_.eval(box, constbox, enclosure);
        case SUB:
            return ope1_.eval(box, constbox, enclosure) - ope2_.eval(box, constbox, enclosure);
        case MULT:
            return ope1_.eval(box, constbox, enclosure) * ope2_.eval(box, constbox, enclosure);
        case DIV:
        {
            Interval i = ope2_.eval(box, constbox, enclosure);
            if (i.contains(0)) {
                throw Growl("Kodiak (eval): division by an interval that contains zero");
            }
            return ope1_.eval(box, constbox, enclosure) / i;
        }
        default:
            throw Growl("Kodiak (eval): binary operator not supported");
    }
}

Real RealBinary_Node::subs(const Substitution &sigma, Names &names) const {
    switch (op_) {
        case ADD:
            return ope1_.subs(sigma, names) + ope2_.subs(sigma, names);
        case SUB:
            return ope1_.subs(sigma, names) - ope2_.subs(sigma, names);
        case MULT:
            return ope1_.subs(sigma, names) * ope2_.subs(sigma, names);
        case DIV:
            return ope1_.subs(sigma, names) / ope2_.subs(sigma, names);
        default:
            throw Growl("Kodiak (subs): binary operator not supported");
    }
}

void RealBinary_Node::print(std::ostream &os) const {
    os << "(" << ope1_
            << " " << op_ << " "
            << ope2_ << ")";
}

// --- RealPower_Node ---

RealPower_Node::RealPower_Node(const Real &e, const nat n) : ope_(e), n_(n) {
    variableIndexes_ = e.vars();
    localVariables_ = e.locals();
    globalConstants_ = e.consts();
    realExpressionType_ = REAL;
    if (e.type() == ATOM)
        realExpressionType_ = MONOMEXPR;
    else if (e.isPolyExpr())
        realExpressionType_ = POLYEXPR;
    numOfVariables = e.numberOfVariables();
}

Real RealPower_Node::deriv(const nat v) const {
    return val(Interval(n_))*(ope_^(n_ - 1)) * ope_.deriv(v);
}

Interval RealPower_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    return power(ope_.eval(box, constbox, enclosure), n_);
}

Real RealPower_Node::subs(const Substitution &sigma, Names &names) const {
    return ope_.subs(sigma, names)^n_;
}

void RealPower_Node::print(std::ostream &os) const {
    bool bin = ope_.isBinary() || ope_.isAtom();
    os << (bin ? "" : "(") << ope_ << (bin ? "" : ")")
            << POW << n_;
}

// --- RealLetin_Node ---

RealLetin_Node::RealLetin_Node(const std::string name, const Real &let, const Real &in) :
name_(name), let_(let), in_(in) {
    variableIndexes_ = in.vars();
    variableIndexes_.insert(let.vars());
    localVariables_ = in.locals();
    localVariables_.erase(name);
    localVariables_.insert(let.locals().begin(), let.locals().end());
    globalConstants_ = in.consts();
    globalConstants_.insert(let.consts().begin(), let.consts().end());
    NameSet interset;
    intersection(localVariables_, globalConstants_, interset);
    realExpressionType_ = REAL;
    if (!interset.empty()) {
        std::ostringstream os;
        os << "Kodiak (letin): names in " << interset << " occur as variable and constant names";
        throw Growl(os.str());
    }
    if (let.numberOfVariables() > in.numberOfVariables())
        numOfVariables = let.numberOfVariables();
    else
        numOfVariables = in.numberOfVariables();
}

Real RealLetin_Node::deriv(const nat v) const {
    std::ostringstream os;
    os << "d" << name_ << "/d" << var_name(v);
    return let(name_, let_, let(os.str(), let_.deriv(v), in_.deriv(v)));
}

Interval RealLetin_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    Interval let = let_.eval(box, constbox, enclosure);
    constbox.push(name_, let);
    try {
        Interval in = in_.eval(box, constbox, enclosure);
        constbox.pop();
        return in;
    } catch (Growl growl) {
        if (Kodiak::debug()) {
            std::cerr << "[GrowlException@RealLetin_Node::eval]" << growl.what() << std::endl;
        }
        constbox.pop();
        throw growl;
    }
}

Real RealLetin_Node::subs(const Substitution &sigma, Names &names) const {
    names.push_back(name_);
    Real in = in_.subs(sigma, names);
    names.pop_back();
    return let(name_, let_.subs(sigma, names), in);
}

void RealLetin_Node::print(std::ostream &os) const {
    os << "let " << name_ << " = " << let_
            << " in " << in_;
}

// --- RealIfnz_Node ---

RealIfnz_Node::RealIfnz_Node(const Real &cond, const Real &lt, const Real &gt) :
cond_(cond), lt_(lt), gt_(gt) {
    variableIndexes_ = cond.vars();
    variableIndexes_.insert(lt.vars());
    variableIndexes_.insert(gt.vars());
    localVariables_ = cond.locals();
    localVariables_.insert(lt.locals().begin(), lt.locals().end());
    localVariables_.insert(gt.locals().begin(), gt.locals().end());
    globalConstants_ = cond.consts();
    globalConstants_.insert(lt.consts().begin(), lt.consts().end());
    globalConstants_.insert(gt.consts().begin(), gt.consts().end());
    realExpressionType_ = REAL;
    NameSet interset;
    intersection(localVariables_, globalConstants_, interset);
    if (!interset.empty()) {
        std::ostringstream os;
        os << "Kodiak (ifnz): names in " << interset << " occur as variable and constant names";
        throw Growl(os.str());
    }
    numOfVariables = std::max(cond.numberOfVariables(), std::max(lt.numberOfVariables(),
                                                                       gt.numberOfVariables()));
}

Real RealIfnz_Node::deriv(const nat v) const {
    return ifnz(cond_, lt_.deriv(v), gt_.deriv(v));
}

Interval RealIfnz_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    Interval cond = cond_.eval(box, constbox, enclosure);
    if (cond.clt(0))
        return lt_.eval(box, constbox, enclosure);
    if (cond.cgt(0))
        return gt_.eval(box, constbox, enclosure);
    throw Growl("Kodiak (eval): ifnz cannot be evaluated when the first argument is an interval containing zero");
}

Real RealIfnz_Node::subs(const Substitution &sigma, Names &names) const {
    return ifnz(cond_.subs(sigma, names),
            lt_.subs(sigma, names),
            gt_.subs(sigma, names));
}

void RealIfnz_Node::print(std::ostream &os) const {
    os << "( "
            << lt_ << ", if " << cond_ << " < 0; "
            << gt_ << ", if " << cond_ << " > 0 )";
}


// --- Polynomial_Node ---

Polynomial_Node::Polynomial_Node(const Monomials &monoms, const Names &names) {
    numOfVariables = 0;
    names_ = names;
    monoms_.resize(monoms.size());
    for (nat j = 0; j < nterms(); ++j) {
        monoms_[j] = monoms[j];
        variableIndexes_.insert(monoms[j].vars());
        localVariables_.insert(monoms[j].locals().begin(), monoms[j].locals().end());
        globalConstants_.insert(monoms[j].consts().begin(), monoms[j].consts().end());
        if (monoms[j].nvars() > numOfVariables)
            numOfVariables = monoms[j].nvars();
    }
    degs_.assign(numOfVariables, 0);
    for (nat j = 0; j < nterms(); ++j) {
        for (nat i = 0; i < numOfVariables; ++i) {
            if (monoms[j].expo(i) > degs_[i])
                degs_[i] = monoms[j].expo(i);
        }
    }
    realExpressionType_ = POLYNOMIAL;
}

Real Polynomial_Node::deriv(const nat v) const {
    Monomials monoms;
    for (nat j = 0; j < nterms(); ++j) {
        if (monoms_[j].expo(v) != 0) {
            Tuple expo = monoms_[j].exponents();
            expo[v] = expo[v] - 1;
            monoms.push_back(Monomial(monoms_[j].expo(v) * monoms_[j].coeff(), expo));
        }
    }
    return mk_poly(new Polynomial_Node(monoms, names_));
}

Interval Polynomial_Node::eval_poly(const Box &box, NamedBox &constbox) const {
    Interval X = Interval(0);
    for (nat j = 0; j < nterms(); ++j)
        X += monoms_[j].eval(box, constbox);
    return X;
}

bool Polynomial_Node::isMonotone(const nat v) {

    // sufficient test(s) for (non-strict) monotonicity

    if (m_[v] == UNK) {
        // Only apply these tests in the case of a single-orthant box
        // (for the time being, at least -> can potentially implement
        // weaker tests for non-single-orthant cases)
        // assert(s_ == 0);

        if (B_[v].isPoint()) {
            m_[v] = CNST;
            return true;
        }

        // ut: uniqueness test: 2 = constant, 1 = unique, 0- = 2 or more occ.
        int ut = 2;
        nat ct;
        for (nat j = 0; j < nterms(); ++j) { // for each term
            if (sne(b_[v][j][0], b_[v][j][degs_[v]])) {
                --ut;
                ct = j;
            }
            if (ut <= 0) break;
        }
        if (ut == 2) {
            m_[v] = CNST;
            return true;
        }
        if (inf(a_[ct]) < 0 && sup(a_[ct]) > 0) --ut;
        // cannot succeed if coefficient of unique term is both positive/negative
        if (ut == 1) {
            int parity = 1;
            for (nat i = 0; i < numberOfVariables(); ++i) {
                if (i == v) {
                    if (inf(b_[i][ct][0]) > inf(b_[i][ct][degs_[i]])) parity *= -1;
                } else if (inf(b_[i][ct][0]) < 0)
                    parity *= -1;
                else if (inf(b_[i][ct][degs_[i]]) < 0)
                    parity *= -1;
            }
            if (inf(a_[ct]) < 0) parity *= -1;
            if (parity == 1) {
                m_[v] = INC;
                return true;
            } else {
                m_[v] = DEC;
                return true;
            }
        }

        // Monotonicity test
        nat n_it, n_dt, n_bt, n_ct; // number of inc/dec/both/constant terms
        n_it = n_dt = n_bt = n_ct = 0;
        Ints behaviour; // 0 = constant, -1 = dec, 1 = inc, 2 = both/borked
        behaviour.assign(nterms(), 0);
        for (nat j = 0; j < nterms(); ++j) { // for each term
            if (sne(b_[v][j][0], b_[v][j][degs_[v]])) {
                behaviour[j] = 0;
                ++n_ct;
            } else if (inf(a_[j]) < 0 && sup(a_[j]) > 0) {
                behaviour[j] = 2;
                ++n_bt;
            } else {
                int parity = 1;
                for (nat i = 0; i < numberOfVariables(); ++i) {
                    if (i == v) {
                        if (inf(b_[i][j][0]) > inf(b_[i][j][degs_[i]])) parity *= -1;
                    } else if (inf(b_[i][j][0]) < 0)
                        parity *= -1;
                    else if (inf(b_[i][j][degs_[i]]) < 0)
                        parity *= -1;
                }
                if (inf(a_[j]) < 0) parity *= -1;
                behaviour[j] = parity;
                if (parity == 1) ++n_it;
                else ++n_dt;
            }
        }
        if ((n_dt == 0) && (n_bt == 0)) {
            m_[v] = INC;
            return true;
        } else if ((n_it == 0) && (n_bt == 0)) {
            m_[v] = DEC;
            return true;
        }

        // Dominance test
        Interval diffinc, diffdec, tmp1, tmp2;
        // check for inc >>> dec dominance
        diffinc = diffdec = 0;
        for (nat j = 0; j < nterms(); ++j) { // for each term
            if (behaviour[j] == 0) continue;
            if (behaviour[j] == 1) {
                tmp1 = hull(b_[v][j][0], b_[v][j][1]);
                tmp2 = hull(b_[v][j][degs_[v] - 1], b_[v][j][degs_[v]]);
                if (width(tmp2) > width(tmp1)) tmp1 = tmp2;
                tmp1 = tmp1 * a_[j];
                for (nat i = 0; i < numberOfVariables(); ++i) {
                    if (i == v) continue;
                    tmp2 = power(B_[i], monoms_[j].expo(i));
                    if (sup(tmp2) < 0) tmp1 = tmp1*-inf(tmp2);
                    else if (inf(tmp2) > 0) tmp1 = tmp1 * sup(tmp2);
                }
                diffinc = diffinc + tmp1;
            } else if ((behaviour[j] == -1) || (behaviour[j] == 2)) {
                tmp1 = hull(b_[v][j][0], b_[v][j][degs_[v]]);
                tmp1 = tmp1 * a_[j];
                for (nat i = 0; i < numberOfVariables(); ++i) {
                    if (i == v) continue;
                    tmp2 = power(B_[i], monoms_[j].expo(i));
                    if (sup(tmp2) < 0) tmp1 = tmp1*-inf(tmp2);
                    else if (inf(tmp2) > 0) tmp1 = tmp1 * sup(tmp2);
                }
                diffdec = diffdec + tmp1;
            }
        }
        if (width(diffinc) > width(diffdec)) {
            m_[v] = INC;
            return true;
        }

        // check for dec >>> inc dominance
        diffinc = diffdec = 0;
        for (nat j = 0; j < nterms(); ++j) { // for each term
            if (behaviour[j] == 0) continue;
            if (behaviour[j] == -1) {
                tmp1 = hull(b_[v][j][0], b_[v][j][1]);
                tmp2 = hull(b_[v][j][degs_[v] - 1], b_[v][j][degs_[v]]);
                if (width(tmp2) > width(tmp1)) tmp1 = tmp2;
                tmp1 = tmp1 * a_[j];
                for (nat i = 0; i < numberOfVariables(); ++i) {
                    if (i == v) continue;
                    tmp2 = power(B_[i], monoms_[j].expo(i));
                    if (sup(tmp2) < 0) tmp1 = tmp1*-inf(tmp2);
                    else if (inf(tmp2) > 0) tmp1 = tmp1 * sup(tmp2);
                }
                diffdec = diffdec + tmp1;
            } else if ((behaviour[j] == 1) || (behaviour[j] == 2)) {
                tmp1 = hull(b_[v][j][0], b_[v][j][degs_[v]]);
                tmp1 = tmp1 * a_[j];
                for (nat i = 0; i < numberOfVariables(); ++i) {
                    if (i == v) continue;
                    tmp2 = power(B_[i], monoms_[j].expo(i));
                    if (sup(tmp2) < 0) tmp1 = tmp1*-inf(tmp2);
                    else if (inf(tmp2) > 0) tmp1 = tmp1 * sup(tmp2);
                }
                diffinc = diffinc + tmp1;
            }
        }
        if (width(diffdec) > width(diffinc)) {
            m_[v] = DEC;
            return true;
        }

        return false;
    }
    return true;
}

bool Polynomial_Node::diffConstants(NamedBox &constbox) {
    if (localVariables_.empty() && globalConstants_.empty())
        return false;
    bool diffconsts = C_.empty();
    if (!diffconsts) {
        for (NameSet::iterator it = localVariables_.begin(); it != localVariables_.end(); ++it) {
            Interval iC = C_.val(C_.var(*it));
            Interval ic = constbox.val(constbox.var(*it));
            if (!iC.seq(ic)) {
                diffconsts = true;
                break;
            }
        }
    }
    if (!diffconsts) {
        for (NameSet::iterator it = globalConstants_.begin(); it != globalConstants_.end(); ++it) {
            Interval iC = C_.val(C_.var(*it));
            Interval ic = constbox.val(constbox.var(*it));
            if (!iC.seq(ic)) {
                diffconsts = true;
                break;
            }
        }
    }
    return diffconsts;
}

// diff[i] < 0  : box[i] == B_[i] 
// diff[i] == 0 : box[i] in B_[i]
// diff[i] > 0  : box[i] not in B[i] and box[i] != B_[i]

void Polynomial_Node::diffVariables(const Box &box, NamedBox &constbox) {
    diff_.resize(numberOfVariables());
    bool diffconsts = diffConstants(constbox);
    if (B_.empty() || diffconsts) {
        B_ = box;
        C_ = constbox;
        m_.assign(numberOfVariables(), UNK);
        diff_.assign(numberOfVariables(), 1);
        b_.resize(numberOfVariables());
        s_ = 0;
        for (nat i = 0; i < numberOfVariables(); ++i) {
            if (inf(B_[i]) < 0 && sup(B_[i]) > 0) ++s_;
            b_[i].resize(nterms());
            for (nat j = 0; j < nterms(); ++j)
                b_[i][j].resize(degree(i) + 1);
        }
        a_.resize(nterms());
        for (nat j = 0; j < nterms(); ++j) {
            a_[j] = monoms_[j].coeff().eval(box, constbox);
        }
    } else {
        for (nat i = 0; i < numberOfVariables(); ++i) {
            if (box[i].seq(B_[i]))
                diff_[i] = -1;
            else if (box[i].subset(B_[i]))
                diff_[i] = 0;
            else
                diff_[i] = 1;
            if (diff_[i] >= 0) { // B_[i] and box[i] are different 
                if (inf(B_[i]) < 0 && sup(B_[i]) > 0) {
                    if (inf(box[i]) >= 0 || sup(box[i]) <= 0) --s_;
                } else {
                    if (inf(box[i]) < 0 && sup(box[i]) > 0) ++s_;
                }
                if (diff_[i] > 0) // B_[i] and box[i] are different and box[i] is not included in B_[i]
                    // then reset monotonicity information, since it's not longer valid.
                    m_[i] = UNK;
                B_[i] = box[i];
            }
        }
    }
}

Interval Polynomial_Node::eval_bp(const Box &box, NamedBox &constbox) {
    diffVariables(box, constbox);
    Interval tmp;
    real wid;
    for (nat i = 0; i < numberOfVariables(); ++i) { // for each variable
        if (diff_[i] >= 0) {
            wid = width(box[i]);
            for (nat j = 0; j < nterms(); ++j) { // for each term
                nat kk = monoms_[j].expo(i);
                for (nat ii = 0; ii <= degs_[i]; ++ii) { // for each (univariate) BC
                    b_[i][j][ii] = Interval(0);
                    for (nat jj = 0; jj <= ii && jj <= kk; ++jj) {
                        tmp = Interval(Kodiak::choose(kk, jj)) * power(box[i].infimum(), kk - jj);
                        tmp *= Interval(Kodiak::choose(ii, jj)) * power(Interval(wid, wid), jj);
                        b_[i][j][ii] += tmp / (Interval(Kodiak::choose(degs_[i], jj)));
                    }
                }
            }
        }
    }

    // Bernstein enclosure with automatic choice of potential speedup (psu)
    bool psu = (s_ == 0 && numberOfVariables() >= 3);

    Tuple s_min, s_max; // multiindex search space for min BC
    Tuple t_min, t_max; // multiindex search space for max BC
    s_min.assign(numberOfVariables(), 0);
    s_max = degs_;
    t_min.assign(numberOfVariables(), 0);
    t_max = degs_;

    bool mono = false;

    if (psu) {
        for (nat i = 0; i < numberOfVariables(); ++i) {
            if (isMonotone(i)) {
                mono = true;
                if (m_[i] == INC) {
                    s_max[i] = 0;
                    t_min[i] = degs_[i];
                } else if (m_[i] == DEC) {
                    s_min[i] = degs_[i];
                    t_max[i] = 0;
                } else { // In this case m_[i] = CNST
                    s_max[i] = 0;
                    t_max[i] = 0;
                }
            }
        }
    }

    // Compute b_min_size, b_max_size
    nat b_min_size = 1, b_max_size = 1;
    for (nat i = 0; i < numberOfVariables(); ++i) {
        b_min_size *= (1 + s_max[i] - s_min[i]);
        b_max_size *= (1 + t_max[i] - t_min[i]);
    }

    Interval bmin, bmax;
    Box tmb;
    tmb.assign(nterms(), Interval(1));

    // Initialise bmin, bmax
    for (nat i = 0; i < numberOfVariables(); ++i) { // for each variable
        for (nat j = 0; j < nterms(); ++j) { // for each term
            tmb[j] *= b_[i][j][0]; // degree 0
        }
    }

    bmin = 0;
    for (nat j = 0; j < nterms(); ++j) bmin += tmb[j] * a_[j];
    bmax = bmin;

    // Compute necessary BCs
    // Optimised version where intermediate products are stored in tmb
    if (numberOfVariables() >= 3) { // only use this version for non-trivial degree

        Box tmb2;
        Tuple blocksize, tmbsize;
        nat tmbsizetotal = 1;
        // compute the sizes of the temporary vector of BCs, tmbsize
        tmbsize.assign(numberOfVariables(), 1);
        for (nat i = 0; i < numberOfVariables(); ++i) { // for each variable
            if ((s_min[i] != s_max[i]) || (t_min[i] != t_max[i])) tmbsize[i] = degs_[i] + 1;
            else if (s_min[i] != t_min[i]) tmbsize[i] = 2;
            tmbsizetotal *= tmbsize[i]; // total number of BCs to be computed
        }
        tmb.assign(tmbsizetotal, Interval(0));
        // tmb/tmb2 indexing:
        // tmb[i_0]...[i_(n-1)] = tmb[i_(n-1)*blocksize[n-1]+...+i_0]

        blocksize.resize(numberOfVariables());
        blocksize[0] = 1;
        for (nat i = 1; i < numberOfVariables(); ++i)
            blocksize[i] = blocksize[i - 1] * tmbsize[i - 1];

        for (nat j = 0; j < nterms(); ++j) { // for each term
            //      nat reduced = 0;   // does this term have zero exponents?
            tmb2.assign(tmbsizetotal, Interval(1));
            tmb2[0] = a_[j];
            for (nat i = 0; i < numberOfVariables(); ++i) { // for each variable
                //        if (monos_[j].expo(i)!=0 && tmbsize[i]!=1) {
                //        later to restore when optimising for zero-exponent terms
                if (tmbsize[i] != 1) {
                    if (tmbsize[i] == 2) {
                        Interval bt = b_[i][j][degs_[i]];
                        nat offset = blocksize[i];
                        for (nat iii = 0; iii < blocksize[i]; ++iii)
                            tmb2[iii + offset] = tmb2[iii] * bt;
                    } else {
                        for (nat ii = 1; ii < tmbsize[i]; ++ii) {
                            // for each univariate coeff except the first
                            Interval bt = b_[i][j][ii];
                            nat offset = blocksize[i] * ii;
                            for (nat iii = 0; iii < blocksize[i]; ++iii)
                                tmb2[iii + offset] = tmb2[iii] * bt;
                        }
                    }
                }
                for (nat iii = 0; iii < blocksize[i]; ++iii)
                    tmb2[iii] *= b_[i][j][0];
            }
            // iterate over tmbsizetotal, add tmb2 to tmb
            // basic version
            //      if (reduced==0)
            for (nat iii = 0; iii < tmbsizetotal; ++iii)
                tmb[iii] += tmb2[iii];
            // optimised version for terms with some zero exponents
            /*
            else {
              nat iiii = 0;
              for (nat iii=0; iii<tmbsizetotal; ++iii)
                tmb[iii] += tmb2[iiii];

            }
             */

            // can further optimise for zero-exponent terms
            // tricky; currently disabled
            // for example:   add if (reductionfactor>1)
            /*
            for (nat iii=0; iii<(tmbsizetotal/reductionfactor); ++iii)
              for (nat iiii=0; iiii<reductionfactor; ++iiii)
                tmb[iii*reductionfactor+iiii] += tmb2[iii];
             */
            // need to change above

        }
        for (nat iii = 0; iii < tmbsizetotal; ++iii) {
            if (inf(tmb[iii]) < inf(bmin)) bmin = tmb[iii];
            if (sup(tmb[iii]) > sup(bmax)) bmax = tmb[iii];
        }

    } else { // old version - should be used where numberOfVariables is <= 2?

        // Compute BCs for bmin
        Tuple idx_min, idx_max; //  Multiindex to compute bmin and bmax
        idx_min = s_min;
        idx_max = t_min;
        Box tmbmin, tmbmax;
        Interval bcmin, bcmax;
        for (nat i = 0; i < b_min_size || i < b_max_size; ++i) { // for each BC to compute
            tmbmin.assign(nterms(), Interval(1));
            tmbmax.assign(nterms(), Interval(1));
            for (nat ii = 0; ii < numberOfVariables(); ++ii) { // for each variable
                for (nat jj = 0; jj < nterms(); ++jj) { // for each term
                    if (i < b_min_size)
                        tmbmin[jj] *= b_[ii][jj][idx_min[ii]]; // degree idx_min[ii]
                    if (mono && i < b_max_size)
                        tmbmax[jj] *= b_[ii][jj][idx_max[ii]]; // degree idx_min[ii]
                }
            }
            bcmin = 0;
            bcmax = 0;
            for (nat j = 0; j < nterms(); ++j) {
                if (i < b_min_size)
                    bcmin += tmbmin[j] * a_[j];
                if (mono && i < b_max_size)
                    bcmax += tmbmax[j] * a_[j];
            }
            if (!mono) bcmax = bcmin;
            if (i < b_min_size) {
                if (inf(bcmin) < inf(bmin)) bmin = bcmin;
                nat k = 0;
                ++idx_min[k];
                while (idx_min[k] > s_max[k]) {
                    idx_min[k] = s_min[k];
                    ++k;
                    if (k >= numberOfVariables()) break;
                    ++idx_min[k];
                }
            }
            if (i < b_max_size) {
                if (sup(bcmax) > sup(bmax)) bmax = bcmax;
                if (mono) {
                    nat k = 0;
                    ++idx_max[k];
                    while (idx_max[k] > t_max[k]) {
                        idx_max[k] = t_min[k];
                        ++k;
                        if (k >= numberOfVariables()) break;
                        ++idx_max[k];
                    }
                }
            }
        }
    }

    return Interval(inf(bmin), sup(bmax));
}

Interval Polynomial_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    if (enclosure)
        return eval_bp(box, constbox);
    else
        return eval_poly(box, constbox);
}

Real Polynomial_Node::subs(const Substitution &sigma, Names &names) const {
    Monomials monoms;
    monoms.resize(monoms_.size());
    for (nat i = 0; i < nterms(); i++)
        monoms[i] = Monomial(monoms_[i].coeff().subs(sigma, names), monoms_[i].exponents());
    return mk_poly(new Polynomial_Node(monoms, names_));
}

void Polynomial_Node::print(std::ostream &os) const {
    if (Kodiak::debug())
        kodiak::print(monoms_, os, "<", " + ", ">");
    else {
        os << "<";
        if (monoms_.size() > 0)
            monoms_[0].print(os, names_);
        for (nat i = 1; i < monoms_.size(); ++i) {
            os << " + ";
            monoms_[i].print(os, names_);
        }
        os << ">";
    }
}

void Polynomial_Node::print_metadata(std::ostream &os) const {
    os << "*** B_ = " << B_ << std::endl;
    os << "*** s_: " << s_ << std::endl;
    os << "*** degs_: " << degs_ << std::endl;
    os << "*** m_: " << m_ << std::endl;
    for (nat j = 0; j < nterms(); ++j) {
        os << "Term " << j + 1 << ", coefficient: " << a_[j] << ":" << std::endl;
        for (nat i = 0; i < numberOfVariables(); ++i) {
            os << var_name(i) << ": " << b_[i][j] << std::endl;
        }
    }
    os << std::endl;
}

nat Polynomial_Node::degree(const nat v) const { // degree in variable v
    if (v > numberOfVariables())
        return 0;
    return degs_[v];
}

Rational_Node::Rational_Node(const Monomials &nummonoms, const Monomials &denmonoms, const Names &names) {
    numOfVariables = 0;
    names_ = names;
    num_monoms_.resize(nummonoms.size());
    den_monoms_.resize(denmonoms.size());

    for (nat j = 0; j < numnterms(); ++j) {
        num_monoms_[j] = nummonoms[j];
        variableIndexes_.insert(nummonoms[j].vars());
        localVariables_.insert(nummonoms[j].locals().begin(), nummonoms[j].locals().end());
        globalConstants_.insert(nummonoms[j].consts().begin(), nummonoms[j].consts().end());
        if (nummonoms[j].nvars() > numOfVariables)
            numOfVariables = nummonoms[j].nvars();
    }
    for (nat j = 0; j < dennterms(); ++j) {
        den_monoms_[j] = denmonoms[j];
        variableIndexes_.insert(denmonoms[j].vars());
        localVariables_.insert(denmonoms[j].locals().begin(), denmonoms[j].locals().end());
        globalConstants_.insert(denmonoms[j].consts().begin(), denmonoms[j].consts().end());
        if (denmonoms[j].nvars() > numOfVariables)
            numOfVariables = denmonoms[j].nvars();
    }
    degs_.assign(numOfVariables, 0);
    for (nat j = 0; j < numnterms(); ++j) {
        for (nat i = 0; i < numOfVariables; ++i) {
            if (nummonoms[j].expo(i) > degs_[i])
                degs_[i] = nummonoms[j].expo(i);
        }
    }
    for (nat j = 0; j < dennterms(); ++j) {
        for (nat i = 0; i < numOfVariables; ++i) {
            if (denmonoms[j].expo(i) > degs_[i])
                degs_[i] = denmonoms[j].expo(i);
        }
    }
    realExpressionType_ = RATIONAL;
}

Real Rational_Node::deriv(const nat v) const {

    // Compute monomials for derivative of numerator and denominator
    Monomials numdermonoms, dendermonoms;
    for (nat j = 0; j < numnterms(); ++j) {
        if (num_monoms_[j].expo(v) != 0) {
            Tuple expo = num_monoms_[j].exponents();
            expo[v] = expo[v] - 1;
            numdermonoms.push_back(Monomial(num_monoms_[j].expo(v) * num_monoms_[j].coeff(), expo));
        }
    }
    for (nat j = 0; j < dennterms(); ++j) {
        if (den_monoms_[j].expo(v) != 0) {
            Tuple expo = den_monoms_[j].exponents();
            expo[v] = expo[v] - 1;
            dendermonoms.push_back(Monomial(den_monoms_[j].expo(v) * den_monoms_[j].coeff(), expo));
        }
    }

    // Compute monomials for result numerator
    Monomials resnummonoms;
    for (nat i = 0; i < numdermonoms.size(); ++i)
        for (nat j = 0; j < den_monoms_.size(); ++j)
            push_monom(resnummonoms, mult_monom(numdermonoms[i], den_monoms_[j]));
    for (nat i = 0; i < dendermonoms.size(); ++i)
        for (nat j = 0; j < num_monoms_.size(); ++j)
            push_monom(resnummonoms, mult_monom(dendermonoms[i], num_monoms_[j]), -1);

    // Compute monomials for result denominator
    Monomials resdenmonoms;
    for (nat i = 0; i < den_monoms_.size(); ++i)
        for (nat j = 0; j < den_monoms_.size(); ++j)
            push_monom(resdenmonoms, mult_monom(den_monoms_[i], den_monoms_[j]));

    return poly_div(new Polynomial_Node(resnummonoms, names_), new Polynomial_Node(resdenmonoms, names_));

}

Interval Rational_Node::eval_rat(const Box &box, NamedBox &constbox) const {
    Interval X = Interval(0);
    for (nat j = 0; j < numnterms(); ++j)
        X += num_monoms_[j].eval(box, constbox);
    Interval Y = Interval(0);
    for (nat j = 0; j < dennterms(); ++j)
        Y += den_monoms_[j].eval(box, constbox);
    if (Y.contains(0)) {
        throw Growl("Kodiak (eval): division by an interval that contains zero");
    }
    return X / Y;

}

Interval Rational_Node::eval_bp(const Box &box, NamedBox &constbox) {

    diffVariables(box, constbox);
    Interval tmp;
    real wid;
    for (nat i = 0; i < numberOfVariables(); ++i) // for each variable
    {
        if (diff_[i] >= 0) {
            wid = width(box[i]);
            for (nat j = 0; j < numnterms(); ++j) // for each numerator term
            {
                nat kk = num_monoms_[j].expo(i);
                for (nat ii = 0; ii <= degs_[i]; ++ii) // for each (univariate) BC
                {
                    num_b_[i][j][ii] = Interval(0);
                    for (nat jj = 0; jj <= ii && jj <= kk; ++jj) {
                        tmp = Interval(Kodiak::choose(kk, jj)) * power(box[i].infimum(), kk - jj);
                        tmp *= Interval(Kodiak::choose(ii, jj)) * power(Interval(wid, wid), jj);
                        num_b_[i][j][ii] += tmp / (Interval(Kodiak::choose(degs_[i], jj)));
                    }
                }
            }
            for (nat j = 0; j < dennterms(); ++j) // for each denominator term
            {
                nat kk = den_monoms_[j].expo(i);
                for (nat ii = 0; ii <= degs_[i]; ++ii) // for each (univariate) BC
                {
                    den_b_[i][j][ii] = Interval(0);
                    for (nat jj = 0; jj <= ii && jj <= kk; ++jj) {
                        tmp = Interval(Kodiak::choose(kk, jj)) * power(box[i].infimum(), kk - jj);
                        tmp *= Interval(Kodiak::choose(ii, jj)) * power(Interval(wid, wid), jj);
                        den_b_[i][j][ii] += tmp / (Interval(Kodiak::choose(degs_[i], jj)));
                    }
                }
            }
        }
    }

    // Compute b_size
    nat b_size = 1;
    for (nat i = 0; i < numberOfVariables(); ++i) {
        b_size *= (1 + degs_[i]);
    }

    Interval bmin, bmax, btn, btd, bt;
    Box tmbn, tmbd;
    tmbn.assign(numnterms(), Interval(1));
    tmbd.assign(dennterms(), Interval(1));

    // Initialise bmin, bmax
    for (nat i = 0; i < numberOfVariables(); ++i) // for each variable
    {
        for (nat j = 0; j < numnterms(); ++j) // for each numerator term
        {
            tmbn[j] *= num_b_[i][j][0]; // degree 0
        }
        for (nat j = 0; j < dennterms(); ++j) // for each denominator term
        {
            tmbd[j] *= den_b_[i][j][0]; // degree 0
        }
    }

    int densign; // sign of denominator; +1 for positive, -1 for negative
    btn = 0;
    for (nat j = 0; j < numnterms(); ++j) btn += tmbn[j] * num_a_[j];
    btd = 0;
    for (nat j = 0; j < dennterms(); ++j) btd += tmbd[j] * den_a_[j];
    if (btd.contains(0)) {
        throw Growl("Kodiak (eval): division by an interval that contains zero");
    }
    if (btd.ple(0)) densign = -1;
    else densign = 1;
    bmax = bmin = btn / btd;

    // Compute BCs
    Tuple midx; //  Multiindex to compute bcs
    midx.assign(numberOfVariables(), 0);
    for (nat i = 0; i < b_size; ++i) // for each BC to compute
    {
        tmbn.assign(numnterms(), Interval(1));
        tmbd.assign(dennterms(), Interval(1));
        for (nat ii = 0; ii < numberOfVariables(); ++ii) // for each variable
        {
            for (nat jj = 0; jj < numnterms(); ++jj) // for each numerator term
            {
                if (i < b_size)
                    tmbn[jj] *= num_b_[ii][jj][midx[ii]]; // degree idx_min[ii]
            }
            for (nat jj = 0; jj < dennterms(); ++jj) // for each denominator term
            {
                if (i < b_size)
                    tmbd[jj] *= den_b_[ii][jj][midx[ii]]; // degree idx_min[ii]
            }
        }
        btn = 0;
        btd = 0;
        for (nat j = 0; j < numnterms(); ++j) {
            if (i < b_size)
                btn += tmbn[j] * num_a_[j];
        }
        for (nat j = 0; j < dennterms(); ++j) {
            if (i < b_size)
                btd += tmbd[j] * den_a_[j];
        }
        if (btd.contains(0)) {
            throw Growl("Kodiak (eval): division by an interval that contains zero");
        }
        if ((btd.ple(0) && (densign > 0)) || (btd.pge(0) && (densign < 0))) {
            throw Growl("Kodiak (eval): division by an interval that contains zero");
        }
        bt = btn / btd;
        if (i < b_size) {
            if (inf(bt) < inf(bmin)) bmin = bt;
            if (sup(bt) > sup(bmax)) bmax = bt;
            nat k = 0;
            ++midx[k];
            while (midx[k] > degs_[k]) {
                midx[k] = 0;
                ++k;
                if (k >= numberOfVariables()) break;
                ++midx[k];
            }
        }
    }

    return Interval(inf(bmin), sup(bmax));

}

Interval Rational_Node::eval(const Box &box, NamedBox &constbox, const bool enclosure) {
    if (enclosure)
        return eval_bp(box, constbox);
    else
        return eval_rat(box, constbox);
}

// diffConstants is identical to the Polynomial_Node version

bool Rational_Node::diffConstants(NamedBox &constbox) {
    if (localVariables_.empty() && globalConstants_.empty())
        return false;
    bool diffconsts = C_.empty();
    if (!diffconsts) {
        for (NameSet::iterator it = localVariables_.begin(); it != localVariables_.end(); ++it) {
            Interval iC = C_.val(C_.var(*it));
            Interval ic = constbox.val(constbox.var(*it));
            if (!iC.seq(ic)) {
                diffconsts = true;
                break;
            }
        }
    }
    if (!diffconsts) {
        for (NameSet::iterator it = globalConstants_.begin(); it != globalConstants_.end(); ++it) {
            Interval iC = C_.val(C_.var(*it));
            Interval ic = constbox.val(constbox.var(*it));
            if (!iC.seq(ic)) {
                diffconsts = true;
                break;
            }
        }
    }
    return diffconsts;
}

// diff[i] < 0  : box[i] == B_[i]
// diff[i] == 0 : box[i] in B_[i]
// diff[i] > 0  : box[i] not in B[i] and box[i] != B_[i]
// Monotonicity information (m_) and number of intervals containing
//  zero (s_) is not recorded for rational nodes

void Rational_Node::diffVariables(const Box &box, NamedBox &constbox) {
    diff_.resize(numberOfVariables());
    bool diffconsts = diffConstants(constbox);
    if (B_.empty() || diffconsts) {
        B_ = box;
        C_ = constbox;
        diff_.assign(numberOfVariables(), 1);
        // Numerator and denominator are required to have same (number of)
        // variables, same degrees in each, but may have different numbers of terms
        num_b_.resize(numberOfVariables());
        den_b_.resize(numberOfVariables());
        for (nat i = 0; i < numberOfVariables(); ++i) {
            num_b_[i].resize(numnterms());
            for (nat j = 0; j < numnterms(); ++j)
                num_b_[i][j].resize(degree(i) + 1);
            den_b_[i].resize(dennterms());
            for (nat j = 0; j < dennterms(); ++j)
                den_b_[i][j].resize(degree(i) + 1);
        }
        num_a_.resize(numnterms());
        for (nat j = 0; j < numnterms(); ++j) {
            num_a_[j] = num_monoms_[j].coeff().eval(box, constbox);
        }
        den_a_.resize(dennterms());
        for (nat j = 0; j < dennterms(); ++j) {
            den_a_[j] = den_monoms_[j].coeff().eval(box, constbox);
        }
    } else {
        for (nat i = 0; i < numberOfVariables(); ++i) {
            if (box[i].seq(B_[i]))
                diff_[i] = -1;
            else if (box[i].subset(B_[i]))
                diff_[i] = 0;
            else
                diff_[i] = 1;
            if (diff_[i] >= 0) // B_[i] and box[i] are different
                B_[i] = box[i];
        }
    }
}

Real Rational_Node::subs(const Substitution &sigma, Names &names) const {
    Monomials nummonoms;
    nummonoms.resize(num_monoms_.size());
    for (nat i = 0; i < numnterms(); i++)
        nummonoms[i] = Monomial(num_monoms_[i].coeff().subs(sigma, names), num_monoms_[i].exponents());
    Monomials denmonoms;
    denmonoms.resize(den_monoms_.size());
    for (nat i = 0; i < dennterms(); i++)
        denmonoms[i] = Monomial(den_monoms_[i].coeff().subs(sigma, names), den_monoms_[i].exponents());
    return rational(new Rational_Node(nummonoms, denmonoms, names_));
}

void Rational_Node::print(std::ostream &os) const {
    if (Kodiak::debug()) {
        kodiak::print(num_monoms_, os, "numerator: <", " + ", ">");
        kodiak::print(den_monoms_, os, "denominator: <", " + ", ">");
    } else {
        os << "<";
        if (num_monoms_.size() > 0)
            num_monoms_[0].print(os, names_);
        for (nat i = 1; i < num_monoms_.size(); ++i) {
            os << " + ";
            num_monoms_[i].print(os, names_);
        }
        os << ">/<";
        if (den_monoms_.size() > 0)
            den_monoms_[0].print(os, names_);
        for (nat i = 1; i < den_monoms_.size(); ++i) {
            os << " + ";
            den_monoms_[i].print(os, names_);
        }
        os << ">";
    }
}

void Rational_Node::print_metadata(std::ostream &os) const {
    os << "*** B_ = " << B_ << std::endl;
    os << "*** degs_: " << degs_ << std::endl;
    os << "numerator: ";
    for (nat j = 0; j < numnterms(); ++j) {
        os << "Term " << j + 1 << ", coefficient: " << num_a_[j] << ":" << std::endl;
        for (nat i = 0; i < numberOfVariables(); ++i) {
            os << var_name(i) << ": " << num_b_[i][j] << std::endl;
        }
    }
    os << std::endl;
    os << "denominator: ";
    for (nat j = 0; j < dennterms(); ++j) {
        os << "Term " << j + 1 << ", coefficient: " << den_a_[j] << ":" << std::endl;
        for (nat i = 0; i < numberOfVariables(); ++i) {
            os << var_name(i) << ": " << den_b_[i][j] << std::endl;
        }
    }
    os << std::endl;
}

nat Rational_Node::degree(const nat v) const // degree in variable v
{
    if (v > numberOfVariables())
        return 0;
    return degs_[v];
}

// e is PolyExpr. Returns POLYNOMIAL

Real kodiak::poly_polyexpr(const Real &e) {
    if (e.isPolynomial())
        return e;
    else if (e.isMonomExpr())
        return poly_monomexpr(e);
    else if (e.isOperator(NEG))
        return poly_neg(poly_polyexpr(e.ope1()));
    else if (e.isOperator(SQ))
        return poly_pow(poly_polyexpr(e.ope1()), 2);
    else if (e.isOperator(ADD))
        return poly_add(poly_polyexpr(e.ope1()), poly_polyexpr(e.ope2()));
    else if (e.isOperator(SUB))
        return poly_add(poly_polyexpr(e.ope1()), poly_polyexpr(e.ope2()), -1);
    else if (e.isOperator(MULT))
        return poly_mult(poly_polyexpr(e.ope1()), poly_polyexpr(e.ope2()));
    else if (e.isPower())
        return poly_pow(poly_polyexpr(e.ope1()), e.expn());
    return e;
}

// e is a MonomExpr. Integer is sign of monomial. Returns POLYNOMIAL

Real kodiak::poly_monomexpr(const Real &e, const int sign) {
    Monomials monoms;
    Names names;
    names.assign(e.numberOfVariables(), "");
    push_monom(monoms, mk_monom(names, e), sign);
    return Real(new Polynomial_Node(monoms, names));
}

// p1 and p2 are POLYNOMIALs. Returns POLYNOMIAL p1 + sign*p2

Real kodiak::poly_add(const Real &p1, const Real &p2, const int sign) {
    Monomials monoms = p1.monoms();
    Names names = p1.names();
    add_monoms(monoms, p2.monoms(), sign);
    merge_names(names, p2.names());
    return Real(new Polynomial_Node(monoms, names));
}

Real kodiak::poly_mult(const Real &p1, const Real &p2) {
    Monomials monoms;
    Names names = p1.names();
    merge_names(names, p2.names());
    for (nat i = 0; i < p1.monoms().size(); ++i)
        for (nat j = 0; j < p2.monoms().size(); ++j)
            push_monom(monoms, mult_monom(p1.monoms()[i], p2.monoms()[j]));
    return Real(new Polynomial_Node(monoms, names));
}

Real kodiak::poly_neg(const Real &p) {
    Monomials monoms = p.monoms();
    Names names = p.names();
    for (nat i = 0; i < monoms.size(); ++i)
        monoms[i].neg();
    return Real(new Polynomial_Node(monoms, names));
}

Real kodiak::poly_pow(const Real &p, const nat n) {
    if (n == 0) return val(1);
    else if (n == 1) return p;
    else return poly_mult(p, poly_pow(p, n - 1));
}

Real kodiak::rational(const Real &e) {
    if (e.vars().linear() || e.isPolynomial() || e.isRational())
        return e;
    else if (e.isPolyExpr())
        return mk_poly(poly_polyexpr(e));
    else if (e.isOperator(NEG))
        return -rational(e.ope1());
    else if (e.isOperator(ABS))
        return Abs(rational(e.ope1()));
    else if (e.isOperator(SQ))
        return Sq(rational(e.ope1()));
    else if (e.isOperator(SQRT))
        return Sqrt(rational(e.ope1()));
    else if (e.isOperator(EXP))
        return Exp(rational(e.ope1()));
    else if (e.isOperator(LN))
        return Ln(rational(e.ope1()));
    else if (e.isOperator(SIN))
        return Sin(rational(e.ope1()));
    else if (e.isOperator(COS))
        return Cos(rational(e.ope1()));
    else if (e.isOperator(TAN))
        return Tan(rational(e.ope1()));
    else if (e.isOperator(ASIN))
        return Asin(rational(e.ope1()));
    else if (e.isOperator(ACOS))
        return Acos(rational(e.ope1()));
    else if (e.isOperator(ATAN))
        return Atan(rational(e.ope1()));
    else if (e.isOperator(ADD))
        return rational(e.ope1()) + rational(e.ope2());
    else if (e.isOperator(SUB))
        return rational(e.ope1()) - rational(e.ope2());
    else if (e.isOperator(MULT))
        return rational(e.ope1()) * rational(e.ope2());
    else if (e.isOperator(DIV))
        return rational(e.ope1()) / rational(e.ope2());
    else if (e.isPower())
        return rational(e.ope1())^e.expn();
    else if (e.isLetin())
        return let(e.name(), rational(e.ope1()), rational(e.ope2()));
    else if (e.isIfnz())
        return ifnz(rational(e.ope1()),
            rational(e.ope2()), rational(e.ope3()));
    return e;
}

// Parameters are POLYNOMIALs p1 and p2. Returns RATIONAL p1/p2

Real kodiak::poly_div(const Real &p1, const Real &p2) {
    Monomials nummonoms = p1.monoms();
    Monomials denmonoms = p2.monoms();
    Names names = p1.names();
    merge_names(names, p2.names());
    return Real(new Rational_Node(nummonoms, denmonoms, names));
}

std::ostream &kodiak::operator<<(std::ostream &os, const MonoInfo monoinfo) {
    switch (monoinfo) {
        case UNK: os << "?";
            break;
        case INC: os << "<";
            break;
        case DEC: os << ">";
            break;
        case CNST: os << "_";
            break;
    }
    return os;
}


