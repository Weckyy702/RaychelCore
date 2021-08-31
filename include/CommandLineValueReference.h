/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHEL_CORE_COMMAND_LINE_VALUE_REFERENCE_H
#define RAYCHEL_CORE_COMMAND_LINE_VALUE_REFERENCE_H

#include <functional>
#include <string>

namespace Raychel {
    class CommandLineValueReference
    {

    public:
        enum class ValueType { bool_, int_, float_, string_ };

        explicit CommandLineValueReference(bool& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::bool_}, as_bool_{value}
        {}

        explicit CommandLineValueReference(int& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::int_}, as_int_{value}
        {}

        explicit CommandLineValueReference(float& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::float_}, as_float_{value}
        {}

        explicit CommandLineValueReference(std::string& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::string_}, as_string_{value}
        {}

        //NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
        CommandLineValueReference(const CommandLineValueReference& rhs) : type_{rhs.type_}
        {
            _assign_from(rhs);
        }

        //NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
        CommandLineValueReference(CommandLineValueReference&& rhs) noexcept : type_{rhs.type_}
        {
            _assign_from(rhs);
        }

        CommandLineValueReference& operator=(const CommandLineValueReference& rhs)
        {
            type_ = rhs.type_;
            _assign_from(rhs);

            return *this;
        }

        CommandLineValueReference& operator=(CommandLineValueReference&& rhs) noexcept
        {
            type_ = rhs.type_;
            _assign_from(rhs);

            return *this;
        }

        [[nodiscard]] ValueType type() const noexcept
        {
            return type_;
        }

        [[nodiscard]] bool& as_bool() const noexcept
        {
            return as_bool_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        [[nodiscard]] int& as_int() const noexcept
        {
            return as_int_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        [[nodiscard]] float& as_float() const noexcept
        {
            return as_float_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        [[nodiscard]] std::string& as_string() const noexcept
        {
            return as_string_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        //since std::reference_wrapper contains a non-owning pointer, this should be ok
        ~CommandLineValueReference() = default;

    private:
        void _assign_from(const CommandLineValueReference& rhs) noexcept
        {
            //This might cause UB. let's find out in testing ;)
            as_bool_ = rhs.as_bool_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        ValueType type_;
        union
        {
            std::reference_wrapper<bool> as_bool_;
            std::reference_wrapper<int> as_int_;
            std::reference_wrapper<float> as_float_;
            std::reference_wrapper<std::string> as_string_;
        };
    };

} // namespace Raychel
#endif //!RAYCHEL_CORE_COMMAND_LINE_VALUE_REFERENCE_H