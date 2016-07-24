#pragma once

#include "Types.h"
#include "IntegerStack.h"
#include <memory>
#include <string>
#include <vector>

namespace emll
{
	namespace compiler
	{
		struct EmittedVar
		{
			bool isNew = false;
			uint64_t varIndex = 0;

			void Clear();
		};

		class EmittedVarAllocator
		{
		public:
			EmittedVar Alloc();
			void Free(EmittedVar& var);

		private:
			utilities::IntegerStack _varStack;
		};

		enum class VariableScope
		{
			Literal,
			Local,
			Global,
			Heap,
			Input,
			Output
		};

		enum class VariableType
		{
			Scalar,
			Vector
		};

		class VariableAllocator;

		class Variable
		{
		public:
			enum VariableFlags
			{
				none = 0,
				isMutable		= 0x00000001,
				hasInitValue		= 0x00000002,
				isVectorRef		= 0x00000004,
				isComputed		= 0x00000008
			};

		public:
			ValueType Type() const
			{
				return _type;
			}
			VariableScope Scope() const
			{
				return _scope;
			}
			virtual size_t Dimension() const
			{
				return 1;
			}
			bool IsVector() const
			{
				return (Dimension() > 1);
			}
			bool IsScalar() const
			{
				return (Dimension() == 1);
			}
			const std::string& EmittedName() const
			{
				return _emittedName;
			}
			bool HasEmittedName() const
			{
				return (_emittedName.length() > 0);
			}
			void SetEmittedName(std::string emittedName);

			bool IsLiteral() const
			{
				return (_scope == VariableScope::Literal);
			}
			bool IsMutable() const
			{
				return TestFlags(VariableFlags::isMutable);
			}
			bool IsConstant() const
			{
				return !IsMutable();
			}		
			bool IsVectorRef() const
			{
				return TestFlags(VariableFlags::isVectorRef);
			}
			bool HasInitValue() const
			{
				return TestFlags(VariableFlags::hasInitValue);
			}
			bool IsComputed() const
			{
				return TestFlags(VariableFlags::isComputed);
			}
			bool TestFlags(int flags) const
			{
				return ((_flags & flags) != 0);
			}

			void AssignVar(EmittedVar var) 
			{
				_emittedVar = var;
			}
			EmittedVar GetAssignedVar()
			{
				return _emittedVar;
			}

			virtual Variable* Combine(VariableAllocator& vAlloc, Variable& other, OperatorType op);
		
		protected:

			Variable(const ValueType type, const VariableScope scope, int flags = VariableFlags::none);
			void SetFlags(const VariableFlags flag)
			{
				_flags |= (int)flag;
			}
			void ClearFlags(const VariableFlags flag)
			{
				_flags &= (~((int)flag));
			}

		private:
			std::string _emittedName;
			ValueType _type;
			VariableScope _scope;
			int _flags;
			EmittedVar _emittedVar;
		};

		class VariableAllocator
		{
		public:
			template <typename VarType, typename... Args>
			VarType* AddVariable(Args&&... args);

			Variable* AddLocalScalarVariable(ValueType type);
			Variable* AddVectorElementVariable(ValueType type, Variable& src, int offset);

		private:
			std::vector<std::shared_ptr<Variable>> _variables;
		};


		template<typename T>
		class VectorVar : public Variable
		{
		public:
			VectorVar(const VariableScope scope, const size_t size, int flags = VariableFlags::isMutable);

			virtual size_t Dimension() const override
			{
				return _size;
			}
		private:
			int _size;
		};
		
		using VectorF = VectorVar<double>;
	}
}

#include "../tcc/Variable.tcc"

