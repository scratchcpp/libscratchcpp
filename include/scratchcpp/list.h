// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "value.h"
#include "entity.h"
#include "string_pool.h"
#include "string_functions.h"
#include "stringptr.h"
#include "veque.h"

namespace libscratchcpp
{

class Target;
class Monitor;
class ListPrivate;

/*!
 * \brief The List class represents a Scratch list.
 *
 * Due to the high optimization of the methods, indices out of range will result in undefined behavior!
 */
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

        /*! Returns a pointer to the raw list data. */
        inline ValueData *data() const { return m_dataPtr->data(); }

        /*!
         * Returns a pointer to the list size.
         * \note This is used internally by compiled code for various optimizations.
         */
        inline size_t *sizePtr() { return &m_size; }

        /*!
         * Returns a pointer to the allocated list size.
         * \note This is used internally by compiled code for various optimizations.
         */
        inline const size_t *allocatedSizePtr() const { return m_dataPtr->sizePtr(); }

        /*! Returns the list size. */
        inline size_t size() const { return m_size; }

        /*! Returns true if the list is empty. */
        inline bool empty() const { return m_size == 0; }

        /*! Returns the index of the given item. */
        inline size_t indexOf(const ValueData &value) const
        {
            for (size_t i = 0; i < m_size; i++) {
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
            // Keep at least 200,000 items allocated if the list has more
            constexpr size_t limit = 200000;
            m_size = 0;

            if (m_dataPtr->size() > limit)
                reserve(limit);
        }

        /*! Appends an empty item and returns the reference to it. Can be used for custom initialization. */
        inline ValueData &appendEmpty()
        {
            m_size++;
            reserve(getAllocSize(m_size));
            return m_dataPtr->operator[](m_size - 1);
        }

        /*! Appends an item. */
        inline void append(const ValueData &value) { value_assign_copy(&appendEmpty(), &value); }

        /*! Appends an item. */
        inline void append(const Value &value) { append(value.data()); }

        /*! Removes the item at index. */
        inline void removeAt(size_t index)
        {
            assert(index >= 0 && index < size());
            std::rotate(m_dataPtr->begin() + index, m_dataPtr->begin() + index + 1, m_dataPtr->begin() + m_size);
            m_size--;
        }

        /*! Inserts an empty item at index and returns the reference to it. Can be used for custom initialization. */
        inline ValueData &insertEmpty(size_t index)
        {
            assert(index >= 0 && index <= size());
            m_size++;
            reserve(getAllocSize(m_size));
            std::rotate(m_dataPtr->rbegin() + m_dataPtr->size() - m_size, m_dataPtr->rbegin() + m_dataPtr->size() - m_size + 1, m_dataPtr->rend() - index);
            return m_dataPtr->operator[](index);
        }

        /*! Inserts an item at index. */
        inline void insert(size_t index, const ValueData &value) { value_assign_copy(&insertEmpty(index), &value); }

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

        /*! Joins the list items with spaces or without any separator if there are only digits and returns the result as StringPtr. */
        inline StringPtr *toStringPtr() const
        {
            veque::veque<StringPtr *> strings;
            size_t size = 0;
            strings.reserve(m_size);
            bool digits = true;
            size_t i;

            for (i = 0; i < m_size; i++) {
                const ValueData *item = &m_dataPtr->operator[](i);
                strings.push_back(value_toStringPtr(item));
                size += strings.back()->size;

                if (value_isValidNumber(item) && !value_isBool(item) && strings.back()->size > 0) {
                    double doubleNum = value_toDouble(item);
                    long num = value_toLong(item);

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

            StringPtr *ret = string_pool_new();
            ret->size = 0;

            if (digits) {
                string_alloc(ret, size);

                for (i = 0; i < strings.size(); i++) {
                    memcpy(ret->data + ret->size, strings[i]->data, strings[i]->size * sizeof(char16_t));
                    ret->size += strings[i]->size;
                    string_pool_free(strings[i]);
                }

                for (; i < m_size; i++) {
                    StringPtr *item = value_toStringPtr(&m_dataPtr->operator[](i));
                    size += item->size + 1;
                    string_alloc(ret, size);
                    memcpy(ret->data + ret->size, item->data, item->size * sizeof(char16_t));
                    ret->size += item->size;
                    string_pool_free(item);
                }
            } else {
                size += strings.size() - 1;
                string_alloc(ret, size);

                for (i = 0; i < strings.size(); i++) {
                    memcpy(ret->data + ret->size, strings[i]->data, strings[i]->size * sizeof(char16_t));
                    ret->size += strings[i]->size;
                    string_pool_free(strings[i]);

                    if (i + 1 < m_size)
                        ret->data[ret->size++] = u' ';
                }

                for (; i < m_size; i++) {
                    StringPtr *item = value_toStringPtr(&m_dataPtr->operator[](i));
                    size += item->size + 1;
                    string_alloc(ret, size);
                    memcpy(ret->data + ret->size, item->data, item->size * sizeof(char16_t));
                    ret->size += item->size;
                    string_pool_free(item);

                    if (i + 1 < m_size)
                        ret->data[ret->size++] = u' ';
                }
            }

            ret->data[ret->size] = u'\0';
            return ret;
        }

        std::string toString() const;

        std::shared_ptr<List> clone();

    private:
        inline void reserve(size_t size)
        {
            assert(size >= m_size);

            while (size > m_dataPtr->size()) {
                m_dataPtr->push_back(ValueData());
                value_init(&m_dataPtr->back());
            }

            while (size < m_dataPtr->size()) {
                value_free(&m_dataPtr->back());
                m_dataPtr->erase(m_dataPtr->end());
            }
        }

        inline size_t getAllocSize(size_t x)
        {
            if (x == 0)
                return 0;

            size_t ret = 1;

            while (ret < x)
                ret *= 2;

            return ret;
        }

        spimpl::unique_impl_ptr<ListPrivate> impl;
        veque::veque<ValueData> *m_dataPtr = nullptr; // NOTE: accessing through pointer is faster! (from benchmarks)
        size_t m_size = 0;
};

} // namespace libscratchcpp
