/**************************************************************************
 * Flakkari Library v0.7.1
 *
 * Flakkari Library is a C++ Library for Network.
 * @file SparseArrays.hpp
 * @brief SparseArrays class for ECS (Entity Component System).
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.1
 * @date 2024-01-05
 **************************************************************************/

#ifndef FLAKKARI_SPARSEARRAYS_HPP_
#define FLAKKARI_SPARSEARRAYS_HPP_

#include <algorithm>
#include <optional>
#include <type_traits>
#include <vector>

namespace Flakkari::Engine::ECS {

template <typename Component> class SparseArrays {
public:
    using value_type = std::optional<Component>;
    using reference_type = value_type &;
    using const_reference_type = const value_type &;
    using container_type = std::vector<value_type>;
    using size_type = typename container_type::size_type;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

public:
    SparseArrays() = default;
    SparseArrays(const SparseArrays &other) : _data(other._data){};
    SparseArrays(SparseArrays &&other) noexcept : _data(std::move(other._data)){};
    ~SparseArrays() = default;

    /**
     * @brief Copy assignment operator for SparseArrays.
     *
     * @param other  The SparseArrays to copy.
     * @return SparseArrays&  The SparseArrays copied.
     */
    SparseArrays &operator=(SparseArrays const &other)
    {
        _data = std::move(other._data);
        return *this;
    }

    /**
     * @brief Move assignment operator for SparseArrays.
     *
     * @param other  The SparseArrays to move.
     * @return SparseArrays&  The SparseArrays moved.
     */
    SparseArrays &operator=(SparseArrays &&other) noexcept
    {
        if (this != &other)
            std::swap(_data, other._data);

        return *this;
    }

    /**
     * @brief Get the component at the index.
     *
     * @param idx  The index of the component.
     * @return reference_type  The component.
     */
    reference_type operator[](size_type idx)
    {
        if (idx >= _data.size())
            _data.resize(idx + 1, std::nullopt);

        return _data[idx];
    }

    /**
     * @brief Get the component at the index.
     *        Const version.
     *
     * @param idx  The index of the component.
     * @return const_reference_type  The component.
     */
    const_reference_type operator[](size_type idx) const
    {
        if (idx >= _data.size())
            return std::nullopt;

        return _data[idx];
    }

    iterator begin() { return _data.begin(); }

    const_iterator begin() const { return _data.begin(); }

    const_iterator cbegin() const { return _data.cbegin(); }

    iterator end() { return _data.end(); }

    const_iterator end() const { return _data.end(); }

    const_iterator cend() const { return _data.cend(); }

    size_type size() const { return _data.size(); }

    /**
     * @brief Insert a component at the end of the SparseArrays.
     *
     * @param pos  The position of the component.
     * @param component  The component to insert.
     * @return reference_type  The component inserted.
     */
    reference_type insert_at(size_type pos, const Component &component)
    {
        if (pos >= _data.size())
            _data.resize(pos + 1, std::nullopt);

        _data[pos].emplace(component);
        return _data[pos];
    }

    /**
     * @brief Insert a component at the end of the SparseArrays.
     *        Move version.
     *
     * @param pos  The position of the component.
     * @param component  The component to insert.
     * @return reference_type  The component inserted.
     */
    reference_type insert_at(size_type pos, Component &&component)
    {
        if (pos >= _data.size())
            _data.resize(pos + 1, std::nullopt);

        _data[pos] = std::move(component);
        return _data[pos];
    }

    /**
     * @brief Emplace a component at the end of the SparseArrays.
     *
     * @tparam Params  The parameters to construct the component.
     * @param pos  The position of the component.
     * @param params  The parameters to construct the component.
     * @return reference_type  The component inserted.
     */
    template <class... Params> reference_type emplace_at(size_type pos, Params &&...params)
    {
        if (pos >= _data.size())
            _data.resize(pos + 1, std::nullopt);

        _data[pos].emplace(std::forward<Params>(params)...);
        return _data[pos];
    }

    /**
     * @brief  Erase a component at the position.
     *
     * @param pos  The position of the component to erase.
     */
    void erase(size_type pos)
    {
        if (pos < _data.size())
            _data[pos].reset();
    }

    /**
     * @brief Get the index object from a component.
     *
     * @param component  The component to get the index from.
     * @return size_type  The index of the component.
     */
    size_type get_index(value_type const &component) const
    {
        auto it = std::find_if(_data.begin(), _data.end(),
                               [&component](auto const &opt) { return opt.has_value() && opt.value() == component; });

        if (it == _data.end())
            return _data.size();

        return std::distance(_data.begin(), it);
    }

private:
    container_type _data;
};

} // namespace Flakkari::Engine::ECS

#endif /* !FLAKKARI_SPARSEARRAYS_HPP_ */
