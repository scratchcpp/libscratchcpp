// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <scratchcpp/veque.h>

#include "value.h"
#include "entity.h"

namespace libscratchcpp
{

class Target;
class Monitor;
class ListPrivate;

/*! \brief The List class represents a Scratch list. */
class LIBSCRATCHCPP_EXPORT List : public Entity
{
    public:
        List(const std::string &id, const std::string &name);
        List(const List &) = delete;

        ~List();

        const std::string &name();
        void setName(const std::string &name);

        Target *target() const;
        void setTarget(Target *target);

        Monitor *monitor() const;
        void setMonitor(Monitor *monitor);

        /*! Returns the list size. */
        inline size_t size() const { return m_dataPtr->size(); }

        /*! Returns true if the list is empty. */
        inline bool empty() const { return m_dataPtr->empty(); }

        /*! Returns the index of the given item. */
        inline size_t indexOf(const ValueData &value) const
        {
            for (size_t i = 0; i < m_dataPtr->size(); i++) {
                if (value_equals(&m_dataPtr->operator[](i), &value))
                    return i;
            }

            return -1;
        }

        /*! Returns the index of the given item. */
        inline size_t indexOf(const Value &value) const { return indexOf(value.data()); }

        /*! Returns true if the list contains the given item. */
        inline bool contains(const ValueData &value) const { return (indexOf(value) != -1); }

        /*! Returns true if the list contains the given item. */
        inline bool contains(const Value &value) const { return contains(value.data()); }

        /*! Clears the list. */
        inline void clear()
        {
            for (ValueData &v : *m_dataPtr)
                value_free(&v);

            m_dataPtr->clear();
        }

        /*! Appends an item. */
        inline void append(const ValueData &value)
        {
            m_dataPtr->push_back(ValueData());
            value_init(&m_dataPtr->back());
            value_assign_copy(&m_dataPtr->back(), &value);
        }

        /*! Appends an item. */
        inline void append(const Value &value) { append(value.data()); }

        /*! Appends an empty item and returns the reference to it. Can be used for custom initialization. */
        inline ValueData &appendEmpty()
        {
            m_dataPtr->push_back(ValueData());
            value_init(&m_dataPtr->back());
            return m_dataPtr->back();
        }

        /*! Removes the item at index. */
        inline void removeAt(size_t index)
        {
            assert(index >= 0 && index < size());
            value_free(&m_dataPtr->operator[](index));
            m_dataPtr->erase(m_dataPtr->begin() + index);
        }

        /*! Inserts an item at index. */
        inline void insert(size_t index, const ValueData &value)
        {
            assert(index >= 0 && index <= size());
            m_dataPtr->insert(m_dataPtr->begin() + index, ValueData());
            value_init(&m_dataPtr->operator[](index));
            value_assign_copy(&m_dataPtr->operator[](index), &value);
        }

        /*! Inserts an item at index. */
        inline void insert(size_t index, const Value &value) { insert(index, value.data()); }

        /*! Replaces the item at index. */
        inline void replace(size_t index, const ValueData &value)
        {
            assert(index >= 0 && index < size());
            value_assign_copy(&m_dataPtr->operator[](index), &value);
        }

        /*! Replaces the item at index. */
        inline void replace(size_t index, const Value &value) { replace(index, value.data()); }

        inline ValueData &operator[](size_t index)
        {
            assert(index >= 0 && index < size());
            return m_dataPtr->operator[](index);
        }

        /*! Joins the list items with spaces or without any separator if there are only digits and stores the result in dst. */
        inline void toString(std::string &dst) const
        {
            dst.clear();
            veque::veque<std::string> strings;
            strings.reserve(m_dataPtr->size());
            bool digits = true;

            for (const auto &item : *m_dataPtr) {
                strings.push_back(std::string());
                value_toString(&item, &strings.back());

                if (value_isValidNumber(&item) && !strings.back().empty()) {
                    double doubleNum = value_toDouble(&item);
                    long num = value_toLong(&item);

                    if (doubleNum != num) {
                        digits = false;
                        break;
                    }

                    if (num < 0 || num >= 10) {
                        digits = false;
                        break;
                    }
                } else {
                    digits = false;
                    break;
                }
            }

            size_t i;
            std::string s;

            if (digits) {
                for (i = 0; i < strings.size(); i++)
                    dst.append(strings[i]);

                for (; i < m_dataPtr->size(); i++) {
                    value_toString(&m_dataPtr->operator[](i), &s);
                    dst.append(s);
                }
            } else {
                for (i = 0; i < strings.size(); i++) {
                    dst.append(strings[i]);

                    if (i + 1 < m_dataPtr->size())
                        dst.push_back(' ');
                }

                for (; i < m_dataPtr->size(); i++) {
                    value_toString(&m_dataPtr->operator[](i), &s);
                    dst.append(s);

                    if (i + 1 < m_dataPtr->size())
                        dst.push_back(' ');
                }
            }
        }

        /*! Same as the other method, but returns the result directly. */
        inline std::string toString() const
        {
            std::string ret;
            toString(ret);
            return ret;
        }

        std::shared_ptr<List> clone();

    private:
        spimpl::unique_impl_ptr<ListPrivate> impl;
        veque::veque<ValueData> *m_dataPtr = nullptr; // NOTE: accessing through pointer is faster! (from benchmarks)
};

} // namespace libscratchcpp
