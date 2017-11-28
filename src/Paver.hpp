#ifndef _PAVER_
#define _PAVER_

#include "LegacyBool.hpp"
#include "BranchAndBoundDF.hpp"
#include "System.hpp"
#include "Environment.hpp"

namespace kodiak {

    class PavingStyle {
    public:

        PavingStyle(const bool zoom = true, const bool filled = true) : zoom_(zoom), filled_(filled) {
        }
        void push_titleColor(const std::string, const std::string);
        void set_title(const nat, const std::string);
        void set_color(const nat, const std::string);

        void set_zoom(const bool zoom = true) {
            zoom_ = zoom;
        }

        void set_filled(const bool filled = true) {
            filled_ = filled;
        }

        bool zoom() const {
            return zoom_;
        }

        bool filled() const {
            return filled_;
        }

        std::string color(nat i) const {
            return color_[i];
        }

        std::string title(nat i) const {
            return title_[i];
        }

        const Names &titles() const {
            return title_;
        }

        const Names &colors() const {
            return color_;
        }

    private:
        Names title_;
        Names color_;
        bool zoom_;
        bool filled_;
    };

    // Special paving types:
    // -1: Only the points in the specified list of boxes belong to the paving
    // The other types are user-configurable. By default:
    // 0: All points certainly belong to the paving
    // 1: All points bossibly belong to the paving

    class PrePaving {
    public:

        PrePaving() : type_(-1) {
        }

        int type() const {
            return type_;
        }

        bool empty() const {
            return boxes_.size() == 0;
        }
        void clear_boxes();
        void push_box(const nat, const Box &);
        void move_box_to(const nat);
        void insert(const PrePaving &);

        nat ntypes() const {
            return boxes_.size();
        }

        nat size(const nat i) const {
            return i < boxes_.size() ? boxes_[i].size() : 0;
        }

        const Boxes &boxes(const nat i) const {
            return boxes_[i];
        }

        Boxes &boxes(const nat i) {
            return boxes_[i];
        }

        std::vector<Boxes> allBoxes() const {
            return this->boxes_;
        }

        bool operator==(PrePaving &other) const {
            return this->type() == other.type()
                    && this->boxes_ == other.allBoxes();
        }

        friend std::ostream& operator<< (std::ostream& stream, const PrePaving& paving) {
            stream << paving.boxes_; // "TODO:PrePaving";
        }

    protected:
        int type_;

        /* boxes_
         *
         * - The size of the vector boxes determines the different type of boxes
         *   that will be used, i.e., every index represents a type of box as in
         *   "Certainly", "Possibly", "Almost Certainly", "Certainly Not" types.
         *
         *  Index meaning: 
         *  [0] Certainly true
         *  [1] Possibly true
         *  [2] Possibly true within eps
         *  [3] Certainly not (for FULL_SEARCH) 
         */
        std::vector<Boxes> boxes_; // Collection of boxes
    };

    class Paving : public PrePaving {
    public:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int) {
            ar & type_;
            ar & varbox_;
            ar & boxes_;
        }

        void set_type(nat type) {
            type_ = type;
        }

        const NamedBox &varbox() const {
            return varbox_;
        }

        void set_varbox(const NamedBox &varbox) {
            varbox_ = varbox;
        }

        nat nvars() const {
            return varbox_.size();
        }
        void gnuplot(const std::string,
                const std::string, const std::string,
                const PavingStyle &);
        void gnuplot(const std::string,
                const std::string, const std::string, const std::string,
                const PavingStyle &);
        void gnuplot_2D(const std::string,
                const std::string, const std::string,
                const Names &, const Names &,
                const bool = false) const;
        void gnuplot_3D(const std::string,
                const std::string, const std::string, const std::string,
                const Names &, const Names &,
                const bool = false) const;
        void save(const std::string, const Names &, const Names & = EmptyNames) const;
        void write(const std::string);
        void read(const std::string);
        Paving projection(const Names &);
    private:
        NamedBox varbox_;
    };

    // STD: Search Certainly, AlmostCertainly, and Possibly boxes
    // FIRST: Search first box that is certainly or almost certainly (if tolerance is 0, search first that is certainly)
    // FULL: Seart standard + Certainly not

    enum SearchType {
        STD, FIRST, FULL
    };

    class Paver : public System, public BranchAndBoundDF<Ints, PrePaving, Environment> {
    public:
        Paver(const std::string = "");

        const Paving &answer() {
            return paving_;
        }
        void pave(const SearchType = STD);

        void first() {
            pave(FIRST);
        }

        void full() {
            pave(FULL);
        }

        void set_flags(nat n) {
            flags_ = n;
        }

        nat flags() const {
            return flags_;
        }

        virtual void print(std::ostream & = std::cout) const;

        virtual void gnuplot(const std::string var1, const std::string var2) {
            paving_.gnuplot(ID, var1, var2, style);
        }

        virtual void gnuplot(const std::string var1, const std::string var2, const std::string var3) {
            paving_.gnuplot(ID, var1, var2, var3, style);
        }

        static PavingStyle default_style(const bool = true, const bool = true);
        PavingStyle style;

    protected:
        void evaluate(PrePaving &, Ints &, Environment &);
        void combine(PrePaving &, const DirVar &, const PrePaving &);
        void combine(PrePaving &, const DirVar &, const PrePaving &, const PrePaving &);
        void select(DirVar &, Ints &, Environment &);
        real minimumDiameterConsideredForNthVar(nat n) const;

        virtual bool local_exit(const PrePaving &) {
            return currentBoxCertainty_ != -1;
        }

        virtual bool global_exit(const PrePaving &) {
            return search_ == FIRST && (currentBoxCertainty_ == -2 || currentBoxCertainty_ > 0);
        }
        virtual void sos_eval(Interval&, Ints &, const Environment &, const nat);
        Environment temp_; // Temporary box for internal computations
        nat flags_; // Extra-certainty flags
        SearchType search_; // Search type

        /*
         * currentBoxCertainty_
         *
         * > 0  : relation wrt 0 is certainly true
         * = 0  : relation wrt 0 is certainly false
         * = -1 : relation wrt 0 is possibly true
         * = -2 : relation wrt 0 is possibly true within eps (a positive small number).
         *
         * TODO: Refactor it into an enum type
         */
        int currentBoxCertainty_; // Certainty of current box

        Paving paving_; // Paving computed by branch and bound algorithm

    };

    class LegacyPaver : public Paver {
    public:
        using Paver::Paver;
        virtual void select(DirVar &dirvar, Ints &certainties, Environment &box) {
            round_robin(dirvar, certainties, box);
        }
    };
}

#endif
