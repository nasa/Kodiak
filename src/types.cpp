#include "Node.hpp"
#include "types.hpp"

using namespace kodiak;

// > 0 : relation wrt 0 is certainly true
// = 0 : relation wrt 0 is certainly false
// = -1 : relation wrt 0 is possibly true
// = -2 : relation wrt 0 is possibly true within eps (a positive small number). 
// Value of -2 is only possible for equality and non-strict inequalities

int Interval::rel0(const RelType rel, const real eps) const {
    switch (rel) {
        case EQ:
            if (contains(0)) {
                if (isPoint()) return 1;
                if (diam() <= eps) return -2;
                return -1;
            }
            return 0;
        case LE:
            if (cle(0)) return 1;
            if (ple(0)) {
                if (sup() <= eps) return -2;
                return -1;
            }
            return 0;
        case GE:
            if (cge(0)) return 1;
            if (pge(0)) {
                if (-inf() <= eps) return -2;
                return -1;
            }
            return 0;
        case LT:
            if (clt(0)) return 1;
            if (plt(0)) {
                if (sup() < eps) return -2;
                return -1;
            }
            return 0;
        case GT:
            if (cgt(0)) return 1;
            if (pgt(0)) {
                if (-inf() < eps) return -2;
                return -1;
            }
            return 0;
    }
    return 0;
}

void Interval::print(std::ostream &cout) const {
    cout << "[" << this->inf() << "," << this->sup() << "]";
}

nat NamedBox::push(const std::string name, const Interval &val) {
    nat v = var(name);
    if (v < size()) {
        std::ostringstream os;
        os << "Kodiak (push): name \"" << name << "\" already exists in named box";
        throw Growl(os.str());
    }
    names_.push_back(name);
    box_.push_back(val);
    return size() - 1;
}

nat NamedBox::push(const std::string name, const Interval &lb, const Interval &ub) {
    nat v = var(name);
    if (v < size()) {
        std::ostringstream os;
        os << "Kodiak (push): name \"" << name << "\" already exists in named box";
        throw Growl(os.str());
    }
    names_.push_back(name);
    box_.push_back(Interval(lb.inf(), ub.sup()));
    return size() - 1;
}

nat NamedBox::set(const nat v, const Interval &val) {
    if (v < size())
        box_[v] = val;
    else {
        std::ostringstream os;
        os << "Kodiak (set): variable index " << v << " doesn't exist in named box";
        throw Growl(os.str());
    }
    return v;
}

nat NamedBox::set(const nat v, const Interval &lb, const Interval &ub) {
    if (v < size())
        box_[v] = Interval(lb.inf(), ub.sup());
    else {
        std::ostringstream os;
        os << "Kodiak (set): variable index " << v << " doesn't exist in named box";
        throw Growl(os.str());
    }
    return v;
}

nat NamedBox::set(const std::string name, const Interval &val) {
    nat v = var(name);
    if (v < size())
        box_[v] = val;
    else {
        std::ostringstream os;
        os << "Kodiak (set): name \"" << name << "\" doesn't exist in named box";
        throw Growl(os.str());
    }
    return v;
}

nat NamedBox::set(const std::string name, const Interval &lb, const Interval &ub) {
    nat v = var(name);
    if (v < size())
        box_[v] = Interval(lb.inf(), ub.sup());
    else {
        std::ostringstream os;
        os << "Kodiak (set): name \"" << name << "\" doesn't exist in named box";
        throw Growl(os.str());
    }
    return v;
}

nat NamedBox::var(const std::string nm) const {
    if (nm == "") return size();
    nat v = size();
    while (v > 0) {
        if (names_[v - 1] == nm)
            break;
        v--;
    }
    if (v > 0)
        return v - 1;
    return size();
}

std::string NamedBox::name(const nat v) const {
    if (Kodiak::debug() || v >= names_.size() || names_[v] == "")
        return var_name(v);
    return names_[v];
}

void NamedBox::print(std::ostream &os, const nat from, const nat to) const {
    nat below = to;
    if (below == 0)
        below = size();
    os << "(";
    if (from < below) {
        os << name(from) << ": " << val(from);
    }
    for (nat v = from + 1; v < below; ++v) {
        os << ", " << name(v) << ": " << val(v);
    }
    os << ")";
}

std::ostream& kodiak::operator<<(std::ostream &os, const NamedBox &varbox) {
    varbox.print(os);
    return os;
}

void Kodiak::init() {
    filib::fp_traits<real>::setup();
    Interval::precision(precision_);
    factorial_[0] = 1;
    for (nat n = 1; n < K_MAXFACT; ++n) {
        factorial_[n] = factorial_[n - 1] * n;
    }
    for (nat n = 0; n < K_MAXCHOOSE; ++n)
        for (nat k = 0; k < K_MAXCHOOSE; ++k)
            choose_[n][k] = 0;
    for (nat n = 0; n < K_MAXCHOOSE; ++n) {
        choose_[n][0] = 1;
        choose_[n][n] = 1;
    }
    for (nat n = 1; n < K_MAXCHOOSE; ++n) {
        for (nat k = 1; k < n; ++k)
            if (k > n - k)
                choose_[n][k] = choose_[n][n - k];
            else
                choose_[n][k] = choose_[n - 1][k - 1] + choose_[n - 1][k];
    }
}

nat Kodiak::factorial_[K_MAXFACT];
nat Kodiak::choose_[K_MAXCHOOSE][K_MAXCHOOSE];

nat Kodiak::factorial(nat n) {
    if (n < K_MAXFACT)
        return factorial_[n];
    std::ostringstream os;
    os << "Kodiak (factorial): " << n << "! is out of bounds";
    throw Growl(os.str());
}

nat Kodiak::choose(nat n, nat k) {
    if (n < K_MAXCHOOSE && k < K_MAXCHOOSE)
        return choose_[n][k];
    std::ostringstream os;
    os << "Kodiak (choose): ( " << n << " " << k << " ) is out of bounds";
    throw Growl(os.str());
}

nat Kodiak::precision_ = K_PRECISION;
std::string Kodiak::var_prefix_ = K_PREFIX;
bool Kodiak::safe_input_ = true;
bool Kodiak::debug_ = false;

void Kodiak::set_precision(const nat precision) {
    precision_ = precision;
    Interval::precision(precision_);
}

std::string kodiak::toString(const OpType op) {
    switch (op) {
        case ADD:
            return "+";
        case SUB:
        case NEG:
            return "-";
        case MULT:
            return "*";
        case DIV:
            return "/";
        case ABS:
            return "abs";
        case SQ:
            return "sq";
        case SQRT:
            return "sqrt";
        case EXP:
            return "exp";
        case LN:
            return "ln";
        case SIN:
            return "sin";
        case COS:
            return "cos";
        case TAN:
            return "tan";
        case ASIN:
            return "asin";
        case ACOS:
            return "acos";
        case ATAN:
            return "atan";
        case POW:
            return "^";
        case DOUBLE_ULP:
            return "dulp";
        case SINGLE_ULP:
            return "sulp";
        case DOUBLE_FEXP:
            return "DFExp";
        default:
            return "NOP";
    }
}

Interval kodiak::rat(const int num, const nat denum) {
    if (denum == 1)
        return Interval(num);
    if (denum == 0)
        throw Growl("Kodiak (rat): denumerator of a rational expression cannot be zero");
    return Interval(num) / Interval(denum);
}

Interval kodiak::dec(const int num, const int exp) {
    if (exp == 0)
        return Interval(num);
    return Interval(num) * power(Interval(10), exp);
}

Interval kodiak::ival(const Interval &i1, const Interval &i2) {
    return Interval(i1.inf(), i2.sup());
}

Interval kodiak::approx(const real val) {
    if (Kodiak::safe_input()) {
        throw Growl("Kodiak (approx): approximated inputs are not allowed when safe_input is set to true");
    }
    return Interval(val, val);
}

Interval kodiak::approx(const real lb, const real ub) {
    if (Kodiak::safe_input()) {
        throw Growl("Kodiak (approx): approximated inputs are not allowed when safe_input is set to true");
    }
    return Interval(lb, ub);
}

Interval kodiak::hex_val(const real val) {
    return Interval(val, val);
}

void kodiak::check_input(const real val) {
    int i = val;
    if (i == val) return;
    std::ostringstream os;
    os << "Kodiak (check_input): expecting an interger <n>, a rational rat(<n>,<m>), a decimal dec(<n>,<e>), or an approximated approx(<f>) value. Got " << val;
    throw Growl(os.str());
}

std::string kodiak::var_name(const nat v) {
    std::ostringstream os;
    os << Kodiak::var_prefix() << v;
    return os.str();
}

std::string kodiak::var_names(const nat n) {
    std::ostringstream os;
    os << var_name(0);
    for (nat i = 1; i < n; ++i) {
        os << ",";
        os << var_name(i);
    }
    return os.str();
}

std::ostream& kodiak::operator<<(std::ostream &os, const OpType op) {
    os << toString(op);
    return os;
}

std::ostream& kodiak::operator<<(std::ostream &os, const DirVar &dirvar) {
    os << "<" << (dirvar.first() ? "1st" : "2nd") << ", "
            << (dirvar.dir ? (dirvar.onlyone ? "L" : "l") :
            (dirvar.onlyone ? "R" : "r"))
            << ", " << var_name(dirvar.var) << ", splitting: " << dirvar.splitting << ">";
    return os;
}

void kodiak::merge_names(Names &names1, const Names &names2) {
    if (names2.size() > names1.size()) {
        names1.insert(names1.end(), names2.size() - names1.size(), "");
    }
    for (nat i = 0; i < names2.size(); ++i) {
        if (names1[i] != "") {
            if (names2[i] != "" && names1[i] != names2[i]) {
                std::ostringstream os;
                os << "Kodiak (merge_names): variable " << var_name(i) << " has two different names, i.e, "
                        << names1[i] << " and " << names2[i] << std::endl;
                throw Growl(os.str());
            }
        } else
            names1[i] = names2[i];
    }
}

void kodiak::print_box(const Box &box, std::ostream &os) {
    os << "(";
    if (!box.empty()) {
        os << var_name(0) << ": " << box[0];
    }
    for (nat v = 1; v < box.size(); ++v) {
        os << ", " << var_name(v) << ": " << box[v];
    }
    os << ")";
}

bool kodiak::isZero(const Tuple &tuple) {
    for (nat v = 0; v < tuple.size(); ++v)
        if (tuple[v] != 0) return false;
    return true;
}

Where kodiak::where(const DirVars &dirvars, const nat v) {
    Where type = EXTERIOR;
    for (DirVars::const_iterator it = dirvars.begin(); it != dirvars.end(); ++it) {
        if ((*it).var == v) {
            if ((*it).dir) {
                if (type == LEFT_INTERIOR) return INTERIOR;
                if (type == EXTERIOR) type = RIGHT_INTERIOR;
            } else {
                if (type == RIGHT_INTERIOR) return INTERIOR;
                if (type == EXTERIOR) type = LEFT_INTERIOR;
            }
        }
    }
    return type;
}

Box& kodiak::operator<<(Box &box, const Interval &i) {
    box.push_back(i);
    return box;
}

Permutation::Permutation(const nat n) : even_(true), first_(true), next_(0) {
    //assert(n > 0);
    perm_.resize(n);
    left_.resize(n);
    for (nat i = 0; i < perm_.size(); ++i) {
        perm_[i] = i;
        left_[i] = true;
    }
}

bool Permutation::next() {
    if (next_ == size())
        return false;
    if (first_) {
        first_ = false;
        nextIndex();
        return true;
    }
    even_ = !even_;
    nat k = perm_[next_];
    //swap next_ and the adjacent integer it is looking at
    swap(next_);
    //reverse the direction of all integers larger than k
    reverse(k);
    nextIndex();
    return true;
}

bool Permutation::isMobile(const nat i) {
    return !((i == 0 && left_[i]) ||
            (i == size() - 1 && !left_[i]) ||
            (left_[i] && perm_[i] < perm_[i - 1]) ||
            (!left_[i] && perm_[i] < perm_[i + 1]));
}

void Permutation::nextIndex() {
    next_ = size();
    nat i;
    for (i = 0; i < size(); ++i) {
        if (isMobile(i)) {
            next_ = i;
            break;
        }
    }
    if (i < size() - 1) {
        for (i = i + 1; i < size(); ++i) {
            if (isMobile(i) && perm_[i] > perm_[next_])
                next_ = i;
        }
    }
}

void Permutation::swap(const nat k) {
    if (left_[k]) {
        nat tempn = perm_[k];
        bool tempb = left_[k];
        perm_[k] = perm_[k - 1];
        left_[k] = left_[k - 1];
        perm_[k - 1] = tempn;
        left_[k - 1] = tempb;
    } else {
        nat tempn = perm_[k];
        bool tempb = left_[k];
        perm_[k] = perm_[k + 1];
        left_[k] = left_[k + 1];
        perm_[k + 1] = tempn;
        left_[k + 1] = tempb;
    }
}

void Permutation::reverse(const nat v) {
    for (nat i = 0; i < size(); ++i) {
        if (perm_[i] > v) {
            left_[i] = !left_[i];
        }
    }
}

void Permutation::print(std::ostream &os) const {
    kodiak::print(perm_, os);
    os << ", even_: " << even_ << ", next_: " << next_ << ", left_: " << left_;
}

std::ostream& kodiak::operator<<(std::ostream &os, const Permutation &perm) {
    perm.print(os);
    return os;
}

const Names kodiak::EmptyNames;
const Box kodiak::EmptyBox;
const NamedBox kodiak::EmptyNamedBox;
const VarBag kodiak::EmptyVarBag;
const NameSet kodiak::EmptyNameSet;

bool kodiak::overlap(const Box &bigbox, const Box &box) {
    nat points = 0;
    for (nat v = 0; v < bigbox.size(); ++v) {
        if (bigbox[v].sne(box[v])) {
            if (bigbox[v].inf() == box[v].sup() ||
                    bigbox[v].sup() == box[v].inf()) {
                if (points > 0)
                    return false;
                ++points;
            } else
                return false;
        }
    }
    return true;
}

// box subset function, is box B2 a (non-strict) subset of box B1?

bool kodiak::box_subset(const Box &B1, const Box &B2) {
    for (nat v = 0; v < B1.size(); ++v)
        if (!(B2[v].subset(B1[v]))) return false;
    return true;
}

void kodiak::save_boxes(std::ofstream &f, const Boxes &boxes,
        const Tuple &vs, const nat width) {
    for (nat i = 0; i < boxes.size(); ++i) {
        for (nat j = 0; j < vs.size(); ++j)
            f << std::setw(width) << boxes[i][vs[j]].inf();
        f << std::endl;
        for (nat j = 0; j < vs.size(); ++j)
            f << std::setw(width) << boxes[i][vs[j]].sup();
        f << std::endl << std::endl;
    }
}

void kodiak::gnuplot_boxes(std::ofstream &f, const Boxes &boxes,
        const nat var1, const nat var2, const nat width) {
    for (nat i = 0; i < boxes.size(); ++i) {
        f << std::setw(width) << boxes[i][var1].inf()
                << std::setw(width) << boxes[i][var2].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf()
                << std::setw(width) << boxes[i][var2].sup() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup()
                << std::setw(width) << boxes[i][var2].sup() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup()
                << std::setw(width) << boxes[i][var2].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf()
                << std::setw(width) << boxes[i][var2].inf() << std::endl;
        f << std::endl;
    }
}

void kodiak::gnuplot_boxes(std::ofstream &f, const Boxes &boxes,
        const nat var1, const nat var2, const nat var3,
        const nat width) {
    for (nat i = 0; i < boxes.size(); ++i) {
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::endl;

        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::endl;

        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::endl;

        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].inf() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::endl;

        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].sup() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::endl;

        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].inf() << std::endl;
        f << std::setw(width) << boxes[i][var1].sup() << std::setw(width)
                << boxes[i][var2].inf() << std::setw(width) << boxes[i][var3].sup() << std::endl;
        f << std::endl;
    }
}

void kodiak::hull(Box &bigbox, const Box &box) {
    for (nat v = 0; v < bigbox.size(); ++v) {
        bigbox[v] = bigbox[v].hull(box[v]);
    }
}

void kodiak::pack(Boxes &boxes, const Box &box) {
    for (nat i = 0; i < boxes.size(); i++) {
        if (overlap(boxes[i], box)) {
            hull(boxes[i], box);
            return;
        }
    }
    boxes.push_back(box);
}

void kodiak::encluster(Boxes &boxes) {
    Boxes aux;
    for (nat i = 0; i < boxes.size(); ++i)
        pack(aux, boxes[i]);
    if (aux.size() < boxes.size()) {
        encluster(aux);
    }
    boxes = aux;
}

Interval kodiak::DoubleFExp(const Interval &interval) {

    real lb = interval.inf();
    real ub = interval.sup();

    int lbExponent = 0;
    std::frexp(lb, &lbExponent);

    int ubExponent = 0;
    std::frexp(ub, &ubExponent);

    const static Interval &zero = Interval(0);

    const bool intervalIsPositive = interval.cge(zero);
    if (intervalIsPositive) {
        return Interval(lbExponent, ubExponent);
    }

    const bool intervalIsNegative = interval.cle(zero);
    if (intervalIsNegative) {
        return Interval(ubExponent, lbExponent);
    }

    return Interval(0, std::max(ubExponent, lbExponent));
}

std::pair<float,float> kodiak::SingleEnclosure(real lower, real upper) {

    static const float infinity = ::std::numeric_limits<float>::infinity();

    float fLower = (float) lower;
    if (((double) fLower) > lower) {
        fLower = std::nextafter(fLower,-infinity);
    }

    float fUpper = (float) upper;
    if (((double) fUpper) < upper) {
        fUpper = std::nextafter(fUpper,infinity);
    }

    return std::make_pair(fLower,fUpper);
};

Interval kodiak::SingleUlp(const Interval &interval) {

    using std::abs;
    using std::signbit;

    static const float minulp = std::nextafter((float) 0,std::numeric_limits<float>::infinity()) - (float) 0;

    std::pair<float,float> floatRange = SingleEnclosure(interval.inf(), interval.sup());

    const float infimum = floatRange.first;
    const float supremum = floatRange.second;

    const float ulpSupremum = ulp(supremum);
    const float ulpInfimum = ulp(infimum);
    if (signbit(infimum) && signbit(supremum)) {
        return Interval(ulpSupremum, ulpInfimum);
    }
    if (!signbit(infimum) && !signbit(supremum)) {
        return Interval(ulpInfimum, ulpSupremum);
    }

    return Interval(minulp, std::max(ulpInfimum, ulpSupremum));
}

Interval kodiak::DoubleUlp(const Interval &interval) {

    using std::abs;
    using std::signbit;

    static const real minulp = std::nextafter(0,std::numeric_limits<double>::infinity()) - 0;

    const real &supremum = interval.sup();
    const real &infimum = interval.inf();

    if (signbit(infimum) && signbit(supremum)) {
        return Interval(ulp(supremum), ulp(infimum));
    }
    if (!signbit(infimum) && !signbit(supremum)) {
        return Interval(ulp(infimum), ulp(supremum));
    }

    return Interval(minulp, std::max(ulp(infimum), ulp(supremum)));
}
