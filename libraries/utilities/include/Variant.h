////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Exception.h"
#include "TypeName.h"

// stl
#include <memory>
#include <typeindex>
#include <string>
#include <type_traits>
#include <utility>

class ISerializable;

/// <summary> utilities namespace </summary>
namespace utilities
{
    //
    // VariantBase --- private implementation class used by Variant
    //
    class VariantBase
    {
    public:
        virtual ~VariantBase() = default;

    protected:
        VariantBase(std::type_index type);
        virtual std::unique_ptr<VariantBase> Clone() const = 0;
        virtual std::string ToString() const = 0;
        virtual std::string GetStoredTypeName() const = 0;
        virtual bool IsPrimitiveType() const = 0;
        virtual bool IsSerializable() const = 0;
        virtual bool IsPointer() const = 0;

    private:
        friend class Variant;

        template <typename ValueType>
        ValueType GetValue() const;

        std::type_index _type; // redundant with type in Variant class.
    };

    //
    // VariantDerived --- private implementation class used by Variant
    //
    template <typename ValueType>
    class VariantDerived : public VariantBase
    {
    public:
        VariantDerived(const ValueType& val);

    protected:
        const ValueType& GetValue() const { return _value; }
        virtual std::unique_ptr<VariantBase> Clone() const override;
        virtual std::string ToString() const override;
        virtual std::string GetStoredTypeName() const override;
        virtual bool IsPrimitiveType() const override { return std::is_fundamental<ValueType>::value; }
        virtual bool IsSerializable() const override { return !IsPrimitiveType(); }
        virtual bool IsPointer() const override { return std::is_pointer<ValueType>::value; }

    private:
        friend class Variant;
        friend class VariantBase;

        ValueType _value;
    };
    
    /// <summary> A class that can hold any kind of value and provide a type-safe way to access it </summary>
    class Variant
    {
    public:
        /// <summary> Copy constructor. </summary>
        Variant(const Variant& other);
        Variant(Variant&& other) = default;

        /// <summary> Copy assignment operator. </summary>
        Variant& operator=(const Variant& other);
        Variant& operator=(Variant&& other) = default;

        /// <summary> Assignment operator from basic (non-variant) types. </summary>
        template <typename ValueType>
        Variant& operator=(ValueType&& value);

        /// <summary> Get a type-safe value from the variant. </summary>
        ///
        /// <returns> The variant's current value. </returns>
        template <typename ValueType>
        ValueType GetValue() const;
    
        /// <summary> Checks the current type of the variant. </summary>
        ///
        /// <returns> True if the variant currently holds a value of type `ValueType`. </returns>
        template <typename ValueType>
        bool IsType() const;

        /// <summary> Checks if the variant is holding a primitive value. </summary>
        ///
        /// <returns> True if the variant currently holds a primitive value. </returns>
        bool IsPrimitiveType() const;

        /// <summary> Checks if the variant is holding a serializable object. </summary>
        ///
        /// <returns> True if the variant currently holds a serializable object. </returns>
        bool IsSerializable() const;

        /// <summary> Checks if the variant is holding a pointer. </summary>
        ///
        /// <returns> True if the variant currently holds a pointer. </returns>
        bool IsPointer() const;

        /// <summary> Gets a string representation of the value. </summary>
        ///
        /// <returns> A string representation of the value. </summary>
        std::string ToString() const;

        /// <summary> Gets the type name of the value stored in the variant. </summary>
        ///
        /// <returns> The type name of the value stored in the variant. </returns>
        std::string GetStoredTypeName() const;

    private:
        friend std::string to_string(const Variant& variant);

        template <typename ValueType, typename... Args>
        friend Variant MakeVariant(Args&&... args);

        Variant(std::type_index type, std::unique_ptr<VariantBase>&& variantValue);

        std::type_index _type;
        std::unique_ptr<VariantBase> _value;
    };

    /// <summary> Convenience function to create a Variant </summary>
    template <typename ValueType, typename... Args>
    Variant MakeVariant(Args&&... args);

    /// <summary> Get string representation of a Variant </summary>
    std::string to_string(const Variant& variant);
}

#include "../tcc/Variant.tcc"
