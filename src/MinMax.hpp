#ifndef KODIAK_MINMAX
#define KODIAK_MINMAX

#include "Real.hpp"

namespace kodiak {

    enum MinMaxType {
        MIN, MINMAX, MAX
    };

    class MinMax {
    public:
        friend class MinMaxSystem;
        friend class NewMinMaxSystem;
        MinMax();
        void init(const int);
        Interval min() const;
        Interval max() const;

        const Interval &outclosure() const {
            return mm_;
        }
        Interval inclosure() const;
        real lb_of_min() const;
        real ub_of_min() const;
        real lb_of_max() const;
        real ub_of_max() const;
        bool isSound() const;

        bool empty() const {
            return mm_.empty();
        }

        const Point& min_point() const {
            return min_point_;
        }

        const Point& max_point() const {
            return max_point_;
        }
        MinMaxType min_or_max() const;
        void print(std::ostream & = std::cout) const;

        Interval getEnclosure() const { return this->mm_; }

        MinMaxType getStrategy() const { return (MinMaxType) (this->morm_ + 1); }

        bool contains(MinMax &other) {
            if (this->morm_ != other.morm_) {
                return false;
            }

            bool result = true;
            MinMaxType strategy = this->getStrategy();
            if (strategy == MINMAX || strategy == MAX) {
                result = result && this->lb_of_max() <= other.lb_of_max() && this->ub_of_max() >= other.ub_of_max();
            }
            if (strategy == MINMAX || strategy == MIN) {
                result = result && this->lb_of_min() <= other.lb_of_min() && this->ub_of_min() >= other.ub_of_min();
            }

            return result;
        }

        bool operator==(const MinMax &other) const {
            return this->getEnclosure() == other.getEnclosure()
                && this->ub_of_min() == other.ub_of_min()
                && this->lb_of_max() == other.lb_of_max()
                && this->min_point() == other.min_point()
                && this->max_point() == other.max_point()
                && this->getStrategy() == other.getStrategy();
        }

        friend std::ostream &operator<<(std::ostream &os, const MinMax &mm) {
            MinMaxType maxType = mm.getStrategy();
            os.precision(20);
            if (maxType == MIN || maxType == MINMAX)
                if (mm.mm_.inf() > mm.ub_of_min_)
                    os << "Min: []";
                else
                    os << "Min:[" << mm.mm_.inf() << "," << mm.ub_of_min_ << "]";
            if (maxType == MAX || maxType == MINMAX)
                if (mm.mm_.sup() < mm.lb_of_max_)
                    os << "Max: []";
                else
                    os << "Max:[" << mm.lb_of_max_ << "," << mm.mm_.sup() << "]";
            return os;
        }
    protected:

        void resize(nat v) {
            min_point_.resize(v);
            max_point_.resize(v);
        }
        Interval mm_; // Enclosure of e(x) for all x in box  
        real ub_of_min_; // Upper bound of min e(x) for all x in box
        Point min_point_; // Point such that min() <= e(min_point_) <= ub_of_min_
        real lb_of_max_; // Lower bound of max e(x) for all x in box
        Point max_point_; // Point where  lb_of_max_ <= e(max_point_) <= max()
        int morm_; // 0: min & max, -1: min, 1: max
    };

}

#endif // KODIAK_MINMAX
