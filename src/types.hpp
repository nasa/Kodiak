#ifndef _TYPES_
#define _TYPES_

#define K_MAXFACT   13 // Not to be changed unless definition of nat is changed
#define K_MAXCHOOSE 35 // Not to be changed unless definition of nat is changed
#define K_PRECISION  8
#define K_PREFIX    "x"

#include <algorithm>
#include <assert.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <deque>
#include <exception>
#include <fstream>
#include <interval/interval.hpp>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

#include "Value.hpp"

namespace kodiak {

    typedef double real;
    typedef unsigned int nat;

    template <typename T>
    class Bag {
    public:

        Bag() : linear_(true) {
        }
        typedef typename std::map<T, nat>::const_iterator iterator;

        bool empty() const {
            return bag_.empty();
        }

        bool find(T t) const {
            return bag_.find(t) != bag_.end();
        }

        nat count(T t) const {
            iterator it = bag_.find(t);
            return it == bag_.end() ? 0 : it->second;
        }

        void insert(const T t, const nat count = 1) {
            typename std::map<T, nat>::iterator it = bag_.find(t);
            if (it == bag_.end()) {
                bag_[t] = count;
                linear_ = linear_ && count <= 1;
            } else {
                (it->second) += count;
                linear_ = linear_ && it->second <= 1;
            }
        }

        void insert(const Bag &b2) {
            for (iterator it = b2.bag_.begin(); it != b2.bag_.end(); ++it)
                insert(it->first, it->second);
        }

        bool linear() const {
            return linear_;
        }

        iterator begin() const {
            return bag_.begin();
        }

        iterator end() const {
            return bag_.end();
        }

        bool is_intersection_nonempty(const Bag &b2) const {
            bool result = false;
            for (iterator it = b2.bag_.begin(); it != b2.bag_.end(); ++it)
                if (it->second > 0 && count(it->first) > 0) {
                    result = true;
                    break;
                }
            return result;
        }
    private:
        std::map<T, nat> bag_;
        bool linear_;
    };

    typedef Bag<nat> VarBag;
    typedef std::vector<real> Point;
    typedef std::set<std::string> NameSet;
    typedef std::vector<std::string> Names;
    typedef std::vector<nat> Tuple;
    typedef std::vector<bool> Bools;
    typedef std::vector<int> Ints;

    extern const VarBag EmptyVarBag;
    extern const Names EmptyNames;
    extern const NameSet EmptyNameSet;

    // This class implments Steinhaus-Johnson-Trotter_algorithm for generating
    // permutations of alternating sign.
    // It closely follows the description in http://pastebin.com/fmuu3nGh.

    class Permutation {
    public:
        Permutation(const nat);
        bool next();

        nat size() const {
            return perm_.size();
        }

        int sign() {
            return (even_ ? 1 : -1);
        }

        const Tuple &perm() const {
            return perm_;
        }

        const nat &operator[](nat i) const {
            return perm_[i];
        }
        void print(std::ostream & = std::cout) const;
    private:
        bool isMobile(const nat);
        void nextIndex();
        void swap(const nat);
        void reverse(const nat);
        Tuple perm_;
        Bools left_;
        bool even_;
        bool first_;
        nat next_;
    };

    // Check that the input is an integer, as opposed to a floating point number.
    void check_input(const real);

    enum RelType {
        EQ, LE, LT, GE, GT
    };

    class Interval : public filib::interval<real>, public Value {
    public:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int) {
            ar &INF;
            ar &SUP;
        }

        Interval() : filib::interval<real>() {
        }

        Interval(const filib::interval<real> &i) : filib::interval<real>(i) {
        }

        Interval(const real val) : filib::interval<real>(val) {
            check_input(val);
        }

        Interval(const real lb, const real ub) : filib::interval<real>(lb, ub) {
        }

        void print(std::ostream &ostream) const override;

        Interval approximatedMidpoint() const {
            real mid = this->mid();
            return Interval(mid, mid);
        }

        Interval infimum() const {
            real inf = this->inf();
            return Interval(inf, inf);
        }

        Interval supremum() const {
            real sup = this-> sup();
            return Interval(sup, sup);
        }

        static Interval Empty() {
            return Interval(1, -1);
        }

        inline bool completelyEqualOrGreaterThan(interval<> const &i) const {
            return cge(i);
        }

        inline bool completelEqualOrLessThan(interval<> const &i) const {
            return cle(i);
        }

        bool empty() const {
            return !contains(inf());
        }
        // > 0 : relation wrt 0 is certainly true
        // = 0 : relation wrt 0 is certainly false
        // = -1 : relation wrt 0 is possibly true
        // = -2 : relation wrt 0 is possibly true within eps (a positive small number).
        int rel0(const RelType, const real = 0) const;
    };

    Interval ulp(const Interval&);
    std::pair<float,float> SingleEnclosure(real,real);
    Interval SingleUlp(const Interval &);
    Interval DoubleUlp(const Interval &);
    Interval DoubleFExp(const Interval &);

/*!
 * ulpNumber - The minimum distance between a number and its neighbors, which is the distance between it and the next
 * number in the direction towards zero.
 *
 * @tparam T - A floating point type
 * @param fpNumber - A floating point number
 * @return The distance between `fpNumber` and its next number in the direction towards zero
 */
    template<class T>
    T ulp(T fpNumber) {

        using std::signbit;
        using std::nextafter;
        using std::numeric_limits;

        static const T infinity = numeric_limits<T>::infinity();
        static const T minusInfinity = -infinity;
        static const T zero = 0;

        if (signbit(fpNumber)) {
            const T nextNumberTowardsInfinity = nextafter(fpNumber, infinity);
            const T nextNumberTowardsMinusInfinity = nextafter(fpNumber, minusInfinity);
            return std::max({nextNumberTowardsInfinity - fpNumber, fpNumber - nextNumberTowardsMinusInfinity});
        } else {
            const T nextNumberTowardsInfinity = nextafter(fpNumber, infinity);
            const T nextNumberTowardsMinusInfinity = nextafter(fpNumber, minusInfinity);
            return std::max({nextNumberTowardsInfinity - fpNumber, fpNumber - nextNumberTowardsMinusInfinity});
        }
    }

    typedef std::vector<Interval> Box;
    extern const Box EmptyBox;

    typedef std::vector<Box> Boxes;

    class Growl : public std::exception {
    public:

        Growl(const std::string msg) : msg_(msg) {
        }

        ~Growl() throw () {
        }

        const char *what() const throw () {
            return const_cast<char *> (msg_.c_str());
        }
    private:
        std::string msg_;
    };

    class NamedBox {
    public:

        NamedBox() {
        };
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int) {
            ar &box_;
            ar &names_;
        }
        // Push a new variable

        nat push(const std::string id) {
            return push(id, 0);
        }
        nat push(const std::string, const Interval &);
        nat push(const std::string, const Interval &, const Interval &);

        nat push(const Interval &val) {
            return push("", val);
        };

        nat push(const Interval &lb, const Interval &ub) {
            return push("", lb, ub);
        };
        // Set a value to an exisiting variable
        nat set(const nat, const Interval &);
        nat set(const nat, const Interval &, const Interval &);
        nat set(const std::string, const Interval &);
        nat set(const std::string, const Interval &, const Interval &);

        void pop() {
            names_.pop_back();
            box_.pop_back();
        }

        void insert(const NamedBox &nbox) {
            box_.insert(box_.end(), nbox.box().begin(), nbox.box().end());
            names_.insert(names_.end(), nbox.names().begin(), nbox.names().end());
        }

        bool empty() const {
            return box_.empty();
        }

        void clear() {
            names_.clear();
            box_.clear();
        }

        nat size() const {
            return box_.size();
        }

        void resize(nat v) {
            box_.resize(v);
            names_.resize(v);
        }
        nat var(const std::string) const;

        Interval val(const nat v) const {
            return box_[v];
        }
        std::string name(const nat) const;

        const Box &box() const {
            return box_;
        }

        const Names &names() const {
            return names_;
        }

        Box &box() {
            return box_;
        }

        Names &names() {
            return names_;
        }
        void print(std::ostream & = std::cout, const nat = 0, const nat = 0) const;

        bool operator==(const NamedBox &another) const {
            return this->box() == another.box();
        }
    private:
        Box box_;
        Names names_;
    };

    extern const NamedBox EmptyNamedBox;

    class Kodiak {
    public:
        static void init();

        static nat precision() {
            return precision_;
        }
        static void set_precision(const nat = K_PRECISION);

        static std::string var_prefix() {
            return var_prefix_;
        }

        static void set_var_prefix(const std::string s = K_PREFIX) {
            var_prefix_ = s;
        }

        static bool safe_input() {
            return safe_input_;
        }

        static void set_safe_input(const bool b = true) {
            safe_input_ = b;
        }

        static bool debug() {
            return debug_;
        }

        static void set_debug(const bool b = true) {
            debug_ = b;
        }
        static nat factorial(nat);
        static nat choose(nat, nat);
    private:
        static nat precision_;
        static std::string var_prefix_;
        static bool safe_input_;
        static bool debug_;
        static nat factorial_[K_MAXFACT];
        static nat choose_[K_MAXCHOOSE][K_MAXCHOOSE];

        Kodiak() {
        }
    };

    struct DirVar {

        DirVar() : dir(true), var(0), onlyone(false), splitting(0), first_(true) {
        }
        bool dir; // Direction of splitting (true = left, false = right)
        nat var; // Variable to split
        bool onlyone; // Split only one side
        nat splitting; // Type of splitting. 0: Normal splitting. Other codes are user specific

        void init(nat v) {
            dir = true;
            var = v;
            onlyone = false;
            splitting = 0;
            first_ = true;
        }

        void next() {
            dir = !dir;
            first_ = !first_;
        }

        bool first() const {
            return first_;
        };
    private:
        bool first_; // True if DirVar object is used in the first recursive call.
    };

    typedef std::vector<DirVar> DirVars;

    enum OpType {
        NOP, ADD, SUB, MULT, DIV, NEG, ABS, SQ, SQRT, EXP, LN, SIN,
        COS, TAN, ASIN, ACOS, ATAN, POW, DOUBLE_ULP, SINGLE_ULP, DOUBLE_FEXP
    };

    // Returns string represenation of an operator
    std::string toString(const OpType);

    // Returns default variable name
    std::string var_name(const nat);

    // Returns default variable names from 0 to nat
    std::string var_names(const nat);

    // Rational number (exact): rat(n,d) = n/d
    Interval rat(const int, const nat = 1);

    // Decimal number (exact): dec(n,e) = n*10^e
    Interval dec(const int, const int = 0);

    Interval ival(const Interval &, const Interval &);

    // Poing interval close to number (recall that not all decimal numbers
    // can be written as floating point numbers), e.g, approx(n) = [f,f],
    // where f is a floating point number close to n
    Interval approx(const real);
    Interval approx(const real, const real);
    // Inject a hexidecimal precise floating point number from interval to
    // To kodiak library
    Interval hex_val(const real val);


    bool box_subset(const Box &, const Box &);

    bool overlap(const Box &, const Box &);
    void hull(Box &, const Box &);
    void pack(Boxes &, const Box &);
    void encluster(Boxes &);
    void save_boxes(std::ofstream &, const Boxes &, const Tuple &, const nat);
    void gnuplot_boxes(std::ofstream &, const Boxes &, const nat, const nat,
            const nat);
    void gnuplot_boxes(std::ofstream &, const Boxes &, const nat, const nat, const nat,
            const nat);

    template <typename T>
    void print(const std::vector<T> &v, std::ostream &os = std::cout,
            const std::string prefix = "(",
            const std::string midfix = ", ",
            const std::string postfix = ")") {
        os << prefix;
        if (!v.empty()) {
            os << v[0];
            for (nat i = 1; i < v.size(); ++i) {
                os << midfix;
                os << v[i];
            }
        }
        os << postfix;
    }

    template <typename T>
    void print(const std::set<T> &s, std::ostream &os = std::cout) {
        os << "{ ";
        if (!s.empty()) {
            typename std::set<T>::iterator it = s.begin();
            os << *it;
            ++it;
            for (; it != s.end(); ++it) {
                os << ", ";
                os << *it;
            }
        }
        os << " }";
    }

    template <typename T>
    void print(const Bag<T> &b, std::ostream &os = std::cout) {
        os << "{{ ";
        if (!b.empty()) {
            typename Bag<T>::iterator it = b.begin();
            os << it->first << " (" << it->second << ")";
            ++it;
            for (; it != b.end(); ++it) {
                os << ", ";
                os << it->first << " (" << it->second << ")";
            }
        }
        os << " }}";
    }

    template <typename T>
    void intersection(const std::set<T> &s1, const std::set<T> &s2, std::set<T> &intersect) {
        intersect.clear();
        std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                std::inserter(intersect, intersect.begin()));
    }

    void merge_names(Names &, const Names &);
    void print_box(const Box &, std::ostream & = std::cout);
    bool isZero(const Tuple &);

    enum Where {
        EXTERIOR, LEFT_INTERIOR, RIGHT_INTERIOR, INTERIOR
    };

    Where where(const DirVars &, const nat);

    template <typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector) {
        kodiak::print(vector, os);
        return os;
    }

    template <typename T>
    std::ostream &operator<<(std::ostream &os, const std::set<T> &set) {
        kodiak::print(set, os);
        return os;
    }

    template <typename T>
    std::ostream &operator<<(std::ostream &os, const kodiak::Bag<T> &bag) {
        kodiak::print(bag, os);
        return os;
    }

    std::ostream &operator<<(std::ostream &, const OpType);
    std::ostream &operator<<(std::ostream &, const DirVar &);
    std::ostream &operator<<(std::ostream &, const Permutation &);
    std::ostream &operator<<(std::ostream &, const NamedBox &);

    Box &operator<<(Box &, const Interval &);
}
#endif
