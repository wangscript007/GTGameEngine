// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ScriptVariable.hpp>

namespace GTEngine
{
    ScriptVariable::ScriptVariable(ScriptVariableType typeIn, const char* nameIn)
        : type(typeIn), name(nameIn)
    {
    }

    ScriptVariable::ScriptVariable(const ScriptVariable &other)
        : type(other.GetType()), name(other.GetName())
    {
    }


    ScriptVariable::~ScriptVariable()
    {
    }


    ScriptVariableType ScriptVariable::GetType() const
    {
        return this->type;
    }

    const char* ScriptVariable::GetName() const
    {
        return this->name.c_str();
    }



    /////////////////////////////////////
    // Number

    ScriptVariable_Number::ScriptVariable_Number(const char* name)
        : ScriptVariable(ScriptVariableType_Number, name)
    {
    }

    ScriptVariable_Number::ScriptVariable_Number(const ScriptVariable_Number &other)
        : ScriptVariable(other), value(other.GetValue())
    {
    }

    ScriptVariable_Number::~ScriptVariable_Number()
    {
    }









    /////////////////////////////////////
    // Statics

    ScriptVariable* ScriptVariable::CreateCopy(const ScriptVariable &variableToCopy)
    {
        switch (variableToCopy.GetType())
        {
        case ScriptVariableType_Number:
            {
                return new ScriptVariable_Number(static_cast<const ScriptVariable_Number &>(variableToCopy));
            }

        case ScriptVariableType_Vec2:
            {
                return nullptr;
            }

        case ScriptVariableType_Vec3:
            {
                return nullptr;
            }

        case ScriptVariableType_Vec4:
            {
                return nullptr;
            }

        case ScriptVariableType_String:
            {
                return nullptr;
            }
        
        case ScriptVariableType_Prefab:
            {
                return nullptr;
            }


        default: break;
        }


        return nullptr;
    }
}