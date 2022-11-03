#pragma once

namespace Dash
{
    namespace FMath
    {
        // Non-member Function

        // --Declaration-- //

        template<typename Scalar> Scalar Width(const TAABB<Scalar, 2>& b) noexcept;

        template<typename Scalar> Scalar Height(const TAABB<Scalar, 2>& b) noexcept;

        template<typename Scalar> Scalar Area(const TAABB<Scalar, 2>& b) noexcept;

        template<typename Scalar> std::size_t MaximumExtent(const TAABB<Scalar, 2>& b) noexcept;





        // Non-member Function

        // --Implementation-- //

        template<typename Scalar>
        FORCEINLINE Scalar Width(const TAABB<Scalar, 2>& b) noexcept
        {
            return b.Upper.X - b.Lower.X;
        }

        template<typename Scalar>
        FORCEINLINE Scalar Height(const TAABB<Scalar, 2>& b) noexcept
        {
            return b.Upper.Y - b.Lower.Y;
        }

        template<typename Scalar>
        FORCEINLINE Scalar Area(const TAABB<Scalar, 2>& b) noexcept
        {
            TScalarArray<Scalar, 2> d = Diagonal(b);
            return (d.X * d.Y);
        }

        template<typename Scalar>
        FORCEINLINE std::size_t MaximumExtent(const TAABB<Scalar, 2>& b) noexcept
        {
            TScalarArray<Scalar, 2> diag = Diagonal(b);
            if (diag.X > diag.Y)
                return 0;
            else
                return 1;
        }
    }



    class AABB2iIterator : public std::forward_iterator_tag {
    public:
        AABB2iIterator(const TAABB<int, 2>& b, const TScalarArray<int, 2>& pt)
            : p(pt), TAABB(&b) {}
        AABB2iIterator operator++() {
            Advance();
            return *this;
        }
        AABB2iIterator operator++(int) {
            AABB2iIterator old = *this;
            Advance();
            return old;
        }
        bool operator==(const AABB2iIterator& bi) const {
            return p == bi.p && TAABB == bi.TAABB;
        }
        bool operator!=(const AABB2iIterator& bi) const {
            return p != bi.p || TAABB != bi.TAABB;
        }

        TScalarArray<int, 2> operator*() const { return p; }

    private:
        void Advance() {
            ++p.X;
            if (p.X == TAABB->Upper.X) {
                p.X = TAABB->Lower.X;
                ++p.Y;
            }
        }
        TScalarArray<int, 2> p;
        const TAABB<int, 2>* TAABB;
    };
}

