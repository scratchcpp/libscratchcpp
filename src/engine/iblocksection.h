// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "engine.h"
#include <string>

namespace libscratchcpp
{

/*!
 * \brief The IBlockSection class is an interface for block sections/categories.
 *
 * \see <a href="blockSections.html">Block sections</a>
 */
class IBlockSection
{
    public:
        virtual ~IBlockSection() { }

        /*! Returns the name of the block section. */
        virtual std::string name() const = 0;

        /*!
         * Returns true if the category in the block palette should be visible.\n
         * For example, there isn't a list category in the Scratch user interface.
         */
        virtual bool categoryVisible() const { return true; }

        /*!
         * Returns a pointer to the implementation function of the given block opcode.\n
         * Used internally by Engine.
         */
        virtual BlockImpl resolveBlock(const std::string &opcode) const final
        {
            if (m_blocks.count(opcode) == 1)
                return m_blocks.at(opcode);
            return nullptr;
        }

        /*!
         * Returns the ID of the input with the given name.\n
         * Used internally by Engine.
         */
        virtual int resolveInput(const std::string &name) const final
        {
            if (m_inputs.count(name) == 1)
                return m_inputs.at(name);
            return -1;
        }

        /*!
         * Returns the ID of the field with the given name.\n
         * Used internally by Engine.
         */
        virtual int resolveField(const std::string &name) const final
        {
            if (m_fields.count(name) == 1)
                return m_fields.at(name);
            return -1;
        }

    protected:
        /*!
         * Assigns a block implementation pointer to a block opcode.
         * \param[in] obj The instance of the IBlockSection subclass (`this`, if used from in the constructor)
         * \param[in] opcode The block opcode
         * \param[in] f A pointer to the block implementation function
         */
        template<class T, class F>
        void addBlock(T *obj, const std::string &opcode, F &&f)
        {
            m_blocks[opcode] = std::bind(f, obj, std::placeholders::_1);
        }

        /*! Assigns an input ID to an input name. */
        virtual void addInput(const std::string &name, int id) final { m_inputs[name] = id; }

        /*! Assigns a field ID to a field name. */
        virtual void addField(const std::string &name, int id) final { m_fields[name] = id; }

    private:
        std::map<std::string, BlockImpl> m_blocks;
        std::map<std::string, int> m_inputs;
        std::map<std::string, int> m_fields;
};

} // namespace libscratchcpp
