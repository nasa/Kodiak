#ifndef _BRANCHANDBOUNDDF_
#define _BRANCHANDBOUNDDF_

#include "types.hpp"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace kodiak {

    template <typename Expression, typename Answer, typename Environment> class BranchAndBoundDF {
    public:

#ifdef DEBUG
        struct Vertex { Answer ans; Environment env; };
        using Graph = typename boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, Vertex>;
        using Vertex_t = typename boost::graph_traits<Graph>::vertex_descriptor;
#endif

        nat splits() const {
            return splitCounter_;
        }

        nat depth() const {
            return currentDepth_;
        }

        nat maxdepth() const {
            return maximumDepth_;
        }

        nat debug() const {
            return debug_;
        }

        clock_t time() const {
            return timeInMls_;
        }

        void print_info(std::ostream &os = std::cout) const {
            os << "Splits: " << splits() << ", Depth: " << depth()
                    << ", Time: " << (time() / 1000.0) << "s";
        }

        void set_maxdepth(nat maxdepth) {
            maximumDepth_ = maxdepth;
        }

        void set_debug(const nat debug = 1) {
            debug_ = debug;
        }

        const Expression &theExpr() const {
            return initialExpression_;
        }

        const Environment &initialBox() const {
            return initialBox_;
        }

    protected:

        BranchAndBoundDF() : maximumDepth_(0), currentDepth_(0), splitCounter_(0), timeInMls_(0), debug_(0) {
#ifdef DEBUG
            this->root = boost::add_vertex(this->graph);
            this->current = this->root;
#endif
        }

        // Basic enclosure function
        virtual void evaluate(Answer &, Expression &, Environment &) = 0;

        // First answer was computed by first recursion.
        virtual void combine(Answer &, const DirVar &, const Answer &) = 0;
        // Second answer was computed by second recursion.
        virtual void combine(Answer &, const DirVar &, const Answer &, const Answer &) = 0;

        virtual void branch(Expression &, Environment &) {
        }

        virtual void unbranch(Expression &, Environment &) {
        }

        virtual void accumulate(const Answer &) {
        }

        virtual bool local_exit(const Answer &) {
            return false;
        }

        virtual bool global_exit(const Answer &) {
            return false;
        }

        virtual bool prune(const Answer &) {
            return false;
        }

        virtual bool isSound(const Answer &, const Expression &, const Environment &) {
            return true;
        }

        virtual void select(DirVar &dirvar, Expression &e, Environment &box) {
            round_robin(dirvar, e, box);
        }

        void round_robin(DirVar &dirvar, Expression &, Environment &box) {
            dirvar.init(box.size());
            nat next_var;
            if (selectionsStack_.empty())
                next_var = 0;
            else
                next_var = selectionsStack_.back().var + 1;
            for (nat round = 0; round < box.size(); round++) {
                if (next_var >= box.size())
                    next_var = 0;
                if (box[next_var].isPoint())
                    ++next_var;
                else {
                    dirvar.var = next_var;
                    dirvar.dir = (selectionsStack_.size() % 2) == 0;
                    break;
                }
            }
        }

        virtual void split(const DirVar &dirvar, Environment &box, const Interval &i, const real mid) {
            if (dirvar.dir)
                box[dirvar.var] = Interval(i.inf(), mid);
            else
                box[dirvar.var] = Interval(mid, i.sup());
        }

        // Branch and bound algorithm
        // e: expression
        // box: box of variables (variables are indexed by nat)

        void branchAndBound(Answer &answer, const Expression &expr, const Environment &box) {
            initialExpression_ = expr;
            initialBox_ = box;
            selectionsStack_.clear();
            splitCounter_ = 0;
            currentDepth_ = 0;
            globalExit_ = false;

            clock_ = clock();
            Expression nexpr = expr;
            Environment nbox = box;
            internalBranchAndBound(answer, nexpr, nbox);
            timeInMls_ = 1000 * (clock() - clock_) / CLOCKS_PER_SEC;
            assert(debug_ == 0 || isSound(answer, initialExpression_, initialBox_));
        }

        const DirVars& dirvars() const {
            return selectionsStack_;
        }

        DirVars& dirvars() {
            return selectionsStack_;
        }

    private:

        void internalBranchAndBound(Answer &answer, const Expression &expr, Environment &box) {

#ifdef DEBUG
            auto previousNode = this->current;
            auto thisNode = boost::add_vertex(this->graph);
            this->current = thisNode;
            boost::add_edge(previousNode, this->current, this->graph);
            this->graph[this->current].env = box;
#endif

            nat depth = selectionsStack_.size();
            if (depth > currentDepth_)
                currentDepth_ = depth;
            Expression e = expr;
            evaluate(answer, e, box);
            accumulate(answer);

#ifdef DEBUG
            this->graph[thisNode].ans = answer;
#endif

            ++splitCounter_;
            globalExit_ = global_exit(answer);
            if (box.empty() || (maximumDepth_ > 0 && depth + 1 >= maximumDepth_) ||
                    globalExit_ || local_exit(answer) || prune(answer))
                return;
            DirVar dirvar;
            select(dirvar, e, box);
            if (dirvar.var >= box.size())
                return;
            Interval i = box[dirvar.var];
            real mid = i.mid();
            split(dirvar, box, i, mid);
            selectionsStack_.push_back(dirvar);
            Answer ans1;
            branch(e, box);
            internalBranchAndBound(ans1, e, box);
            unbranch(e, box);
            if (globalExit_) dirvar.onlyone = true;
            if (dirvar.onlyone) {
                selectionsStack_.pop_back();
                box[dirvar.var] = i;
                combine(answer, dirvar, ans1);
                return;
            }
            dirvar.next();
            selectionsStack_.pop_back();
            split(dirvar, box, i, mid);
            selectionsStack_.push_back(dirvar);
            Answer ans2;
            branch(e, box);
#ifdef DEBUG
            this->current = thisNode;
#endif
            internalBranchAndBound(ans2, e, box);
            unbranch(e, box);
            selectionsStack_.pop_back();
            box[dirvar.var] = i;
            combine(answer, dirvar, ans1, ans2);

#ifdef DEBUG
            this->current = previousNode;
#endif
        }

        /* maximumDepth_
         * Maxdepth 0 means loop until one leaf satisfies global exit or until
         * all the leaves satisfy either prune or local exit
         */
        nat        maximumDepth_;

        nat        currentDepth_;
        nat        splitCounter_;
        DirVars    selectionsStack_;
        clock_t    clock_;
        clock_t    timeInMls_;
        Expression initialExpression_;
        Environment        initialBox_;
        nat        debug_; // If debug >= 0, assert sound predicate
        bool       globalExit_;

#ifdef DEBUG
    public:
        Vertex_t    current;
        Vertex_t    root;
        Graph       graph;
#endif
    };
}

#endif
